#include "disp.h"
#include <util/8x8font.h>

VbeModeInfo* g_modeInfo;

void i686_DISP_PassVBEInfo(VbeModeInfo* modeinfo);
void i686_DISP_Initialize(VbeModeInfo* modeInfo) {
    i686_DISP_PassVBEInfo(modeInfo);
}

void i686_DISP_PassVBEInfo(VbeModeInfo* modeinfo) {
    g_modeInfo = modeinfo;
}

void i686_DISP_PutChar(char c, int x, int y, uint32_t color, int sizeMultiplier) {
    if (!g_modeInfo || c > 0x7F) { // Only handle ASCII range (0x00 - 0x7F)
        return; // Invalid mode or unsupported character
    }

    uint8_t* fontData = font8x8_basic[c & 0x7F]; // Get font data for the character
    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (divide by 4 for 32-bit color)

    for (int row = 0; row < 8; row++) {
        uint8_t rowData = fontData[row];
        for (int col = 0; col < 8; col++) {
            if (rowData & (1 << col)) {  // Only draw if the bit is set
                // Draw pixel with size multiplier
                for (int dy = 0; dy < sizeMultiplier; dy++) {
                    for (int dx = 0; dx < sizeMultiplier; dx++) {
                        int drawX = x + col * sizeMultiplier + dx;
                        int drawY = y + row * sizeMultiplier + dy;
                        if (drawX >= 0 && drawX < g_modeInfo->width && drawY >= 0 && drawY < g_modeInfo->height) {
                            framebuffer[drawY * pitch + drawX] = color;
                        }
                    }
                }
            }
        }
    }
}

void i686_DISP_PutPixel(int x, int y, uint32_t color) {
    if (!g_modeInfo) {
        return; // Invalid mode
    }

    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (divide by 4 for 32-bit color)

    framebuffer[y * pitch + x] = color;
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
 * Each pixel in the bitmap must be RGB888 (32-bit).
 */
void i686_DISP_DrawBitmap(int x, int y, int w, int h, const unsigned long* bitmap) {
    if (!g_modeInfo) {
        return; // Invalid mode
    }

    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (32-bit color)

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            framebuffer[(y + row) * pitch + (x + col)] = bitmap[row * w + col];
        }
    }
}

void i686_DISP_DrawBitmapScaled(int x, int y, int w, int h, const unsigned long* bitmap, int scale) {
    if (!g_modeInfo || scale <= 0) {
        return; // Invalid mode or scale
    }

    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (32-bit color)

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            uint32_t pixel = bitmap[row * w + col];

            // Draw this pixel as a scale x scale block
            for (int dy = 0; dy < scale; dy++) {
                for (int dx = 0; dx < scale; dx++) {
                    int drawX = x + col * scale + dx;
                    int drawY = y + row * scale + dy;

                    if (drawX >= 0 && drawX < g_modeInfo->width &&
                        drawY >= 0 && drawY < g_modeInfo->height) {
                        framebuffer[drawY * pitch + drawX] = pixel;
                    }
                }
            }
        }
    }
}

void i686_DISP_DrawBitmapTransparent(int x, int y, int w, int h, const unsigned long* bitmap, uint32_t transparentColor) {
    if (!g_modeInfo) {
        return; // Invalid mode
    }

    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (32-bit color)

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            uint32_t pixel = bitmap[row * w + col];
            if (pixel != transparentColor) { // Only draw if not the transparent color
                framebuffer[(y + row) * pitch + (x + col)] = pixel;
            }
        }
    }
}

void i686_DISP_DrawBitmapScaledTransparent(int x, int y, int w, int h, const unsigned long* bitmap, int scale, uint32_t transparentColor) {
    if (!g_modeInfo || scale <= 0) {
        return; // Invalid mode or scale
    }

    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (32-bit color)

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            uint32_t pixel = bitmap[row * w + col];
            if (pixel == transparentColor) continue;

            // Draw this pixel as a scale x scale block
            for (int dy = 0; dy < scale; dy++) {
                for (int dx = 0; dx < scale; dx++) {
                    int drawX = x + col * scale + dx;
                    int drawY = y + row * scale + dy;

                    if (drawX >= 0 && drawX < g_modeInfo->width &&
                        drawY >= 0 && drawY < g_modeInfo->height) {
                        framebuffer[drawY * pitch + drawX] = pixel;
                    }
                }
            }
        }
    }
}


void i686_DISP_DrawRect(int x, int y, int w, int h, uint32_t color) {
    if (!g_modeInfo) {
        return; // Invalid mode
    }

    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (32-bit color)

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            framebuffer[(y + row) * pitch + (x + col)] = color;
        }
    }
}

#include <stdint.h>
#include <stdbool.h>

void i686_DISP_DrawRoundedRect(int x, int y, int w, int h, int radius, uint32_t color) {
    if (!g_modeInfo) {
        return; // Invalid mode
    }

    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (32-bit color)

    int r2 = radius * radius; // radius squared for circle checks

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            int px = col;
            int py = row;

            bool draw = true;

            // Top-left corner check
            if (px < radius && py < radius) {
                int dx = radius - px;
                int dy = radius - py;
                if (dx*dx + dy*dy > r2) draw = false;
            }

            // Top-right corner check
            if (px >= w - radius && py < radius) {
                int dx = px - (w - radius - 1);
                int dy = radius - py;
                if (dx*dx + dy*dy > r2) draw = false;
            }

            // Bottom-left corner check
            if (px < radius && py >= h - radius) {
                int dx = radius - px;
                int dy = py - (h - radius - 1);
                if (dx*dx + dy*dy > r2) draw = false;
            }

            // Bottom-right corner check
            if (px >= w - radius && py >= h - radius) {
                int dx = px - (w - radius - 1);
                int dy = py - (h - radius - 1);
                if (dx*dx + dy*dy > r2) draw = false;
            }

            if (draw) {
                framebuffer[(y + row) * pitch + (x + col)] = color;
            }
        }
    }
}

uint32_t i686_DISP_GetPixel(int x, int y) {
    if (!g_modeInfo) {
        return 0; // Invalid mode
    }

    uint32_t* framebuffer = (uint32_t*)g_modeInfo->framebuffer;
    int pitch = g_modeInfo->pitch / 4; // Pitch in pixels (32-bit color)

    return framebuffer[y * pitch + x];
}

