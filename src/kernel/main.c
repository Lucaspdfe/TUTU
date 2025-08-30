#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <arch/i686/vbe.h>
#include <arch/i686/disp.h>
#include <arch/i686/key.h>   // for i686_KEY_WaitArrow() + KEY_UP/DOWN/LEFT/RIGHT

extern uint8_t __bss_start;
extern uint8_t __end;

#define MAX_INPUT_LEN 128

static void drawWindow(VbeModeInfo* modeInfo, int x, int y, int w, int h, const char* inputText) {
    // Window body
    i686_DISP_DrawRect(x, y, w, h, DISP_COLOR_DGRAY);

    // Title bar
    i686_DISP_DrawRect(x, y, w, 30, DISP_COLOR_LGRAY);

    // Title text
    const char* title = "VERSION";
    int titleX = x + 10;
    int titleY = y + 8;
    for (const char* p = title; *p; p++) {
        i686_DISP_PutChar(*p, titleX, titleY, DISP_COLOR_BLACK);
        titleX += 16;
    }

    // Version text
    const char* version = "Version 1.4.5";
    int versionX = x + 10;
    int versionY = y + 50;
    for (const char* p = version; *p; p++) {
        i686_DISP_PutChar(*p, versionX, versionY, DISP_COLOR_WHITE);
        versionX += 16;
    }

    // Draw input text inside the window
    int inputX = x + 10;
    int inputY = versionY + 30;
    for (const char* p = inputText; *p; p++) {
        i686_DISP_PutChar(*p, inputX, inputY, DISP_COLOR_WHITE);
        inputX += 16;
        if (inputX + 16 > x + w) { // wrap to next line if needed
            inputX = x + 10;
            inputY += 16;
        }
    }
}


void __attribute__((section(".entry"))) start(VbeModeInfo* modeInfo)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));
    STDIO_Initialize(modeInfo);
    HAL_Initialize();

    clrscr();

    // Background + dock
    i686_DISP_DrawRect(0, 0, modeInfo->width, modeInfo->height, DISP_COLOR_GRAY);
    i686_DISP_DrawRoundedRect(20, modeInfo->height - 50 - 20,
                              modeInfo->width - 40, 50, 10, DISP_COLOR_LGRAY);

    int winWidth  = modeInfo->width / 2;
    int winHeight = modeInfo->height / 2;
    int winX = (modeInfo->width - winWidth) / 2 - 40;
    int winY = (modeInfo->height - winHeight) / 2 - 40;

    char inputText[MAX_INPUT_LEN] = {0};
    int inputLen = 0;

    drawWindow(modeInfo, winX, winY, winWidth, winHeight, inputText);

    for (;;) {
        int oldX = winX, oldY = winY;

        // Poll arrow key (non-blocking)
        uint8_t arrow = i686_KEY_GetArrow();
        switch (arrow) {
            case KEY_UP:    winY -= 10; break;
            case KEY_DOWN:  winY += 10; break;
            case KEY_LEFT:  winX -= 10; break;
            case KEY_RIGHT: winX += 10; break;
        }

        // Clamp window position
        if (winX < 0) winX = 0;
        if (winY < 0) winY = 0;
        if (winX + winWidth > modeInfo->width)  winX = modeInfo->width - winWidth;
        if (winY + winHeight > modeInfo->height) winY = modeInfo->height - winHeight;

        // Poll character key (non-blocking)
        char ch = i686_KEY_GetChar();
        if (ch) {
            if (ch == '\b') {
                if (inputLen > 0) inputText[--inputLen] = '\0';
            } else if (ch == '\n') {
                inputLen = 0;
                inputText[0] = '\0';
            } else if (ch >= 32 && ch <= 126 && inputLen < MAX_INPUT_LEN - 1) {
                inputText[inputLen++] = ch;
                inputText[inputLen] = '\0';
            }
        }

        // Redraw window only if moved or typed
        if (oldX != winX || oldY != winY || ch) {
            // Erase old window
            i686_DISP_DrawRect(oldX, oldY, winWidth, winHeight, DISP_COLOR_GRAY);
            // Restore dock
            i686_DISP_DrawRoundedRect(20, modeInfo->height - 50 - 20,
                                      modeInfo->width - 40, 50, 20, DISP_COLOR_LGRAY);

            // Draw new window
            drawWindow(modeInfo, winX, winY, winWidth, winHeight, inputText);
        }
    }
}
