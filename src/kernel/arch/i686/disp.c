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

/**
 * Draws a bitmap at (x, y) with width w and height h.
 * The bitmap must be in row-major order with a pitch of w pixels per row.
 * Each pixel in the bitmap must be RGB565 (16-bit).
 */
void i686_DISP_DrawBitmap(int x, int y, int w, int h, const uint16_t* bitmap) {
    if (!g_modeInfo || !bitmap) {
        return; // Invalid mode or bitmap
    }

    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (32-bit color)

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            uint16_t pixel565 = bitmap[row * w + col];

            // Extract RGB565 components
            uint8_t r = (pixel565 >> 11) & 0x1F;
            uint8_t g = (pixel565 >> 5)  & 0x3F;
            uint8_t b =  pixel565        & 0x1F;

            // Scale to 8-bit
            r = (r * 255) / 31;
            g = (g * 255) / 63;
            b = (b * 255) / 31;

            // Convert to ARGB8888 (alpha=0xFF)
            uint32_t color = 0xFF000000 | (r << 16) | (g << 8) | b;

            framebuffer[(y + row) * pitch + (x + col)] = color;
        }
    }
}
