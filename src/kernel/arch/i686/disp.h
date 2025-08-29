#pragma once
#include <stdint.h>
#include <arch/i686/vbe.h>

void i686_DISP_Initialize(VbeModeInfo* modeInfo);
void i686_DISP_PutChar(char c, int x, int y);
void i686_DISP_ClearScreen();
void i686_DISP_GetSize(int* width, int* height);
void i686_DISP_DrawBitmap(int x, int y, int w, int h, const uint16_t* bitmap);