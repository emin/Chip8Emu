#include "raylib.h"
#include <stdint.h>
#include "CPU.h"
#include "gpu.h"
#include "hw.h"
#include <chrono>
#include <thread>
#include "input.h"



HW *m_hw;

int main()
{
 
    HW hw;
    m_hw = &hw;
    gpu_init();
    cpu_init();
    
   
    const double frequency = 60.0;
    const auto period = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(1.0 / frequency));

    auto next_call = std::chrono::high_resolution_clock::now();

    while (!WindowShouldClose())
    {
        input_process();
        cpu_step();
        
        // 60Hz
        const auto now = std::chrono::high_resolution_clock::now();
        if (now >= next_call) {
            cpu_update_timer();
            gpu_render();
            next_call += std::chrono::duration_cast<std::chrono::steady_clock::duration>(period);
        }

    }

   
    gpu_close();

    return 0;
}
