#pragma once
#include <stdint.h>
#include <arch/i686/vbe.h>

typedef enum {
    DISP_COLOR_BLACK = 0x000000,
    DISP_COLOR_WHITE = 0xFFFFFF,
    DISP_COLOR_RED   = 0xFF0000,
    DISP_COLOR_GREEN = 0x00FF00,
    DISP_COLOR_BLUE  = 0x0000FF,
    DISP_COLOR_YELLOW= 0xFFFF00,
    DISP_COLOR_CYAN  = 0x00FFFF,
    DISP_COLOR_MAGENTA=0xFF00FF,
    DISP_COLOR_GRAY  = 0x808080,
    DISP_COLOR_LGRAY = 0xC0C0C0,
    DISP_COLOR_DGRAY = 0x404040,
} DispColor;

void i686_DISP_Initialize(VbeModeInfo* modeInfo);
void i686_DISP_PutChar(char c, int x, int y, uint32_t color);
void i686_DISP_ClearScreen();
void i686_DISP_GetSize(int* width, int* height);
void i686_DISP_DrawBitmap(int x, int y, int w, int h, const uint16_t* bitmap);
void i686_DISP_DrawRect(int x, int y, int w, int h, uint32_t color);
void i686_DISP_DrawRoundedRect(int x, int y, int w, int h, int radius, uint32_t color);
