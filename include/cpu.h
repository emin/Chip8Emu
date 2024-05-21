#pragma once
#include "hw.h"




void cpu_step();
void cpu_update_timer();
void cpu_init();

size_t load_rom();

void print_memory_region(int start, int len);



