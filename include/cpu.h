#pragma once
#include "hw.h"
#include <vector>



void cpu_step();
void cpu_update_timer();
void cpu_init();


void print_memory_region(int start, int len);

void load_rom_from_file(const char* filename);
void load_rom_from_bytes(std::vector<uint8_t> data);



