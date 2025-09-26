#pragma once
#include <stdint.h>
#include <hal/hal.h>

void i686_DISP_Initialize(VbeModeInfo* modeInfo);
void i686_DISP_PassVBEInfo(VbeModeInfo* modeinfo);
void i686_DISP_PutChar(char c, int x, int y, uint32_t color, int sizeMultiplier);
void i686_DISP_PutPixel(int x, int y, uint32_t color);
void i686_DISP_ClearScreen();
void i686_DISP_GetSize(int* width, int* height);
void i686_DISP_DrawBitmap(int x, int y, int w, int h, const unsigned long* bitmap);
void i686_DISP_DrawBitmapScaled(int x, int y, int w, int h, const unsigned long* bitmap, int scale);
void i686_DISP_DrawRect(int x, int y, int w, int h, uint32_t color);
void i686_DISP_DrawRoundedRect(int x, int y, int w, int h, int radius, uint32_t color);
void i686_DISP_DrawBitmapTransparent(int x, int y, int w, int h, const unsigned long* bitmap, uint32_t transparentColor);
void i686_DISP_DrawBitmapScaledTransparent(int x, int y, int w, int h, const unsigned long* bitmap, int scale, uint32_t transparentColor);
uint32_t i686_DISP_GetPixel(int x, int y);
