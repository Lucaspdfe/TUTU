#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void i686_PIT_Initialize();
void i686_PIT_Sleep(uint32_t milliseconds);