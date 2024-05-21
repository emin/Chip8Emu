#include "CPU.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <assert.h>
#include <random>
#include <chrono>
#include <thread>
#include <stdint.h>
#include <stack>


uint16_t m_PC = 0x200; // program counter
uint16_t m_I = 0; // index register
std::stack<uint16_t> m_stack;
uint8_t m_delay_timer = 0;
uint8_t m_sound_timer = 0;
uint8_t m_registers[16] = { 0 };

std::chrono::high_resolution_clock::time_point m_last_time = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point m_next_call = std::chrono::high_resolution_clock::now();


void cpu_init()
{
	uint8_t font_data[80]{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80 // F
	};

	// interpreter assumed to be between 000 - 1FF
	// because we're emulating, leaving that space empty
	
	// putting font data into interpreter space
	memcpy(&m_hw->memory[0x050], &font_data, 80 * sizeof(uint8_t));
	

	size_t  rom_size = load_rom();

	print_memory_region(0x200, 0x200 + rom_size);
}






size_t load_rom()
{
	const char* filename = "C:\\Users\\emink\\Downloads\\IBMLogo.ch8";
	// const char* filename = "C:\\Users\\emink\\Downloads\\chip8-roms-master\\demos\\Stars [Sergey Naydenov, 2010].ch8";

	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file) {
		std::cerr << "Failed to open the file: " << filename << std::endl;
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> buffer(size);

	if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
		std::cerr << "Failed to read the file: " << filename << std::endl;
	}

	file.close();

	for (int i = 0; i < buffer.size(); ++i) 
	{
		m_hw->memory[i + 0x200] = buffer[i];
	}

	std::cout << "ROM loaded successfully. Size: " << size << " bytes." << std::endl;

	return size;
}


void print_memory_region(int start, int len)
{
	int c = 1;
	for (int i = start; i < len; ++i)
	{
		printf("%x ", m_hw->memory[i]);
		if (c % 5 == 0)
			printf("\n");
		c++;
	}
	printf("\n");
}

void cpu_step()
{
	const double frequency = 700.0;
	const auto period = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(1.0 / frequency)) ;



	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsedTime = now - m_last_time;


	// https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set

	if (now >= m_next_call) {
		// processor tick
		uint16_t ins = 0x0000;
		ins = ((m_hw->memory[m_PC] << 8) & 0xff00) | (m_hw->memory[m_PC + 1] & 0x00ff);
		m_PC += 2;
		
		// n represent nibble ( ins = n1 n2 n3 n4 )
		uint16_t n1 = ins & 0xf000;
		uint16_t loc, reg, val, x, y, n, xloc, yloc, regx, regy, n4;
		uint8_t b, kidx, pressed, r;
		bool changed = false, block;
		switch (n1)
		{
			case 0x0000: 
				if (ins == 0x00E0) 
				{
					// clear screen
					const int len = m_hw->display_w * m_hw->display_h;
					for (int i = 0; i < len; ++i)
					{
						m_hw->framebuffer[i] = 0;
					}
				}
				else if (ins == 0x00EE)
				{
					// return from subroutine
					m_PC = m_stack.top();
					m_stack.pop();
				}
				break;

			

			case 0x1000: // jump to NNN
				loc = ins & 0x0fff;
				m_PC = loc;
				break;

			case 0x2000: // subroutine 2NNN (at NNN)

				m_stack.push(m_PC);
				loc = ins & 0x0fff;
				m_PC = loc;

				break;

			case 0x3000: // 3XNN skip if VX == NN
				reg = (ins & 0x0f00) >> 8;
				val = ins & 0x00ff;

				if (m_registers[reg] == val)
				{
					m_PC += 2;
				}

				break;

			case 0x4000: // 4XNN skip if VX != NN
				reg = (ins & 0x0f00) >> 8;
				val = ins & 0x00ff;

				if (m_registers[reg] != val)
				{
					m_PC += 2;
				}

				break;

			case 0x5000: // 5XY0 skip if VX = VY
				regx = (ins & 0x0f00) >> 8;
				regy = (ins & 0x00f0) >> 4;
				
				if (m_registers[regx] == m_registers[regy])
				{
					m_PC += 2;
				}

				break;

			case 0x6000: // set register VX (6XNN)
				reg = (ins & 0x0f00) >> 8;
				val = ins & 0x00ff;
				m_registers[reg] = val;
				break;
			case 0x7000: // ad value to register VX (7XNN)
				reg = (ins & 0x0f00) >> 8;
				val = ins & 0x00ff;
				m_registers[reg] += val;
				break;

			case 0x8000:

				n4 = (ins & 0x000f);
				regx = (ins & 0x0f00) >> 8;
				regy = (ins & 0x00f0) >> 4;

				switch (n4)
				{
					case 0: // 8XY0
						m_registers[regx] = m_registers[regy];
						break;

					case 1: // 8XY1
						m_registers[regx] = m_registers[regx] | m_registers[regy];
						break;

					case 2: // 8XY2
						m_registers[regx] = m_registers[regx] & m_registers[regy];
						break;

					case 3: // 8XY3
						m_registers[regx] = m_registers[regx] ^ m_registers[regy];
						break;

					case 4: // 8XY4
						// carry check (overflow)
						m_registers[0xF] = ((int)(m_registers[regx]) + (int)(m_registers[regy])) >= 0xff ? 1 : 0;
						m_registers[regx] += m_registers[regy];
						break;
					
					case 5: // 8XY5
						// carry check (underflow)
						m_registers[0xF] = m_registers[regx] < m_registers[regy] ? 1 : 0;
						m_registers[regx] -= m_registers[regy];
						break;

					case 6: // 8XY6

					#if ORIGINAL_SHIFT
						m_registers[regx] = m_registers[regy] >> 1;
						m_registers[0xF] = m_registers[regy] & 0b00000001;
					#else
						m_registers[0xF] = m_registers[regx] & 0b00000001;
						m_registers[regx] = m_registers[regx] >> 1;
					#endif

						break;

					case 7: // 8XY7

						// carry check (underflow)
						m_registers[0xF] = m_registers[regy] < m_registers[regx] ? 1 : 0;
						m_registers[regx] = m_registers[regy] - m_registers[regx];


						break;

					case 0xE: // 8XYE

				#if ORIGINAL_SHIFT
						m_registers[regx] = m_registers[regy] << 1;
						m_registers[0xF] = m_registers[regy] & 0b10000000;
				#else
						m_registers[0xF] = m_registers[regx] & 0b10000000;
						m_registers[regx] = m_registers[regx] << 1;
				#endif

						break;
				}


				break;

			case 0x9000: // 9XY skip if VX != VY

				regx = (ins & 0x0f00) >> 8;
				regy = (ins & 0x00f0) >> 4;

				if (m_registers[regx] != m_registers[regy])
				{
					m_PC += 2;
				}

				break;

			case 0xA000: // store NNN to index register I
				loc = ins & 0x0fff;
				m_I = loc;
				break;

			case 0xB000: // BNNN jump to V0 + NNN
				loc = ins & 0x0fff;
				m_PC = m_registers[0] + loc;
				break;

			case 0xC000: // CXNN
				reg = (ins & 0x0f00) >> 8;
				val = ins & 0x00ff;
				r = static_cast<uint8_t>(std::rand() % 256);
				m_registers[reg] = r & val;

				break;

			case 0xD000: // DXYN Draw at VX, VY with N bytes of sprite data at the address stored in I
				// Set VF to 01 if any set pixels are changed to unset, and 00 otherwise

				xloc = (ins & 0x0f00) >> 8;
				yloc = (ins & 0x00f0) >> 4;
				n = (ins & 0x000f);

				x = m_registers[xloc] % m_hw->display_w;
				y = m_registers[yloc] % m_hw->display_h;

				m_registers[0xF] = 0;

				for (int i = 0; i < n; ++i)
				{
					// vertical (n = height of the sprite)
					if ((y + i) >= m_hw->display_h)
						continue;

					b = m_hw->memory[m_I + i];
					for (int k = 0; k < 8; ++k)
					{
						// horizontal (1 bit per pixel)

						if ((x + k) >= m_hw->display_w)
							continue;

						kidx = 7 - k;

						if (((0x1 << kidx) & b) > 0 &&
							m_hw->framebuffer[(x + k) + (y + i) * m_hw->display_w] == 1)
						{
							m_hw->framebuffer[(x + k) + (y + i) * m_hw->display_w] = 0;
							m_registers[0xF] = 1;
						}
						else if (((0x1 << kidx) & b) > 0 && m_hw->framebuffer[(x + k) + (y + i) * m_hw->display_w] == 0)
						{
							m_hw->framebuffer[(x + k) + (y + i) * m_hw->display_w] = (uint8_t)1;
						}


					}

				}
				break;

			case 0xE000: // EX9E and EXA1 (input check)

				n4 = ins & 0x00ff;
				regx = (ins & 0x0f00) >> 8;

				switch (n4)
				{
					case 0x9E:
						if (m_hw->key_press[m_registers[regx]] == 1)
						{
							m_PC += 2;
						}
						break;

					case 0xA1:
						if (m_hw->key_press[m_registers[regx]] == 0)
						{
							m_PC += 2;
						}
						break;
				}
				break;

			case 0xF000: 

				n4 = ins & 0x00ff;
				regx = (ins & 0x0f00) >> 8;

				switch (n4)
				{
					case 0x07: // FX07
						m_registers[regx] = m_delay_timer;
						break;

					case 0x15: // FX15
						m_delay_timer = m_registers[regx];
						break;

					case 0x18: // FX18
						m_sound_timer = m_registers[regx];
						break;

					case 0x1E: // FX1E
						m_I += m_registers[regx];
						break;

					case 0x0A: // FX0A

						block = true;
						pressed = 0;
						for (int i = 0; i < 16; i++) 
						{
							if (m_hw->key_press[i] > 0)
							{
								pressed = m_hw->key_press[i];
								block = false;
								break;
							}
						}
						if (block) 
						{
							m_PC -= 2;
						}
						else
						{
							m_registers[regx] = pressed;
						}


						break;

					case 0x29: // FX29 point to the font loc for char
						m_I = 0x050 + m_registers[regx] * 5;
						break;

					case 0x33: // FX33
						val = m_registers[regx];

						m_hw->memory[m_I] = val / 100;
						m_hw->memory[m_I + 1] = (val % 100) / 10;
						m_hw->memory[m_I + 2] = val % 10;

						break;

					case 0x55: // FX55 store to memory

						for (int i = 0; i <= regx; ++i)
						{
							m_hw->memory[m_I + i] = m_registers[i];
						}

						break;

					case 0x65: // FX65 load from memory

						for (int i = 0; i <= regx; ++i)
						{
							m_registers[i] = m_hw->memory[m_I + i];
						}

						break;
				}
		}


		if (m_PC >= m_hw->memory_size)
		{
			m_PC = 0x200;
		}
		

		m_next_call += std::chrono::duration_cast<std::chrono::steady_clock::duration>(period);
	}

	

	if (elapsedTime.count() >= 1.0)
	{
		//std::cout << m_PC << " ops per second" << std::endl;
		m_last_time = std::chrono::high_resolution_clock::now();
	}


}


void cpu_update_timer()
{
	if (m_delay_timer > 0)
	{
		--m_delay_timer;
	}

	if (m_sound_timer > 0)
	{
		--m_sound_timer;
	}
}

