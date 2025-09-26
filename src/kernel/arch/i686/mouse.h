#pragma once
#include <stdint.h>
#include <stdbool.h>

void i686_MOUSE_Initialize();
void i686_MOUSE_WaitForInput(int* x, int* y, bool *leftButton, bool *rightButton);
void i686_MOUSE_GetPosition(int* x, int* y, bool* leftButton, bool* rightButton);