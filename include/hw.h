#pragma once
#include <stdint.h>

struct HW
{
    const int display_w = 64;
    const int display_h = 32;
    const bool original_shift = false;
    const int memory_size = 4096;
    const int render_scaler = 24;
    uint8_t framebuffer[64 * 32] = {0};
    uint8_t framebuffer_rendered[64 * 32] = {0};
    uint8_t key_press[16] = {0};
    uint8_t memory[4096] = {0}; 
};

extern HW *m_hw;