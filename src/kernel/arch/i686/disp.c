#include "disp.h"
#include <util/8x8font.h>

VbeModeInfo* g_modeInfo;

void i686_DISP_Initialize(VbeModeInfo* modeInfo) {
    g_modeInfo = modeInfo;
}

void i686_DISP_PutChar(char c, int x, int y) {
    if (!g_modeInfo || c > 0x7F) { // Only handle ASCII range (0x00 - 0x7F)
        return; // Invalid mode or unsupported character
    }

    uint8_t* fontData = font8x8_basic[c & 0x7F]; // Get font data for the character
    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (divide by 4 for 32-bit color)

    for (int row = 0; row < 8; row++) {
        uint8_t rowData = fontData[row];
        for (int col = 0; col < 8; col++) {
            uint32_t color = (rowData & (1 << col)) ? 0xFFFFFF : 0x000000; // White or black pixel
            // Draw a 2x2 block for each pixel to double the size
            framebuffer[(y + row * 2) * pitch + (x + col * 2)] = color;
            framebuffer[(y + row * 2) * pitch + (x + col * 2 + 1)] = color;
            framebuffer[(y + row * 2 + 1) * pitch + (x + col * 2)] = color;
            framebuffer[(y + row * 2 + 1) * pitch + (x + col * 2 + 1)] = color;
        }
    }
}

void i686_DISP_ClearScreen() {
    if (!g_modeInfo) {
        return; // Invalid mode
    }

    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int totalPixels = (g_modeInfo->width * g_modeInfo->height);

    for (int i = 0; i < totalPixels; i++) {
        framebuffer[i] = 0x000000; // Clear to black
    }
}

void i686_DISP_GetSize(int* width, int* height) {
    if (!g_modeInfo) {
        *width = 0;
        *height = 0;
        return; // Invalid mode
    }

    *width = g_modeInfo->width;
    *height = g_modeInfo->height;
}