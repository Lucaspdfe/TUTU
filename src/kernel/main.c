#include "stdio.h"
#include "hal/hal.h"
#include "icons.h"
#include <stdint.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define BG_COLOR (uint32_t)0x001F1F1F
#define DOCK_COLOR (uint32_t)0x000F0F0F
#define DOCK_HEIGHT 40

int dockApps = 0;

typedef struct {
    const char* name;
    const unsigned long* bitmap;
    uint32_t color;
    bool isUsed;
} App;

App apps[10];

void DrawDock() {
    int scrW, scrH;
    HAL_DISP_GetSize(&scrW, &scrH);
    HAL_DISP_DrawRect(scrW, scrH - DOCK_HEIGHT, scrW, DOCK_HEIGHT, DOCK_COLOR);
}

void DrawDockApps(const unsigned long* bitmap) {
    int scrW, scrH;
    HAL_DISP_GetSize(&scrW, &scrH);
    HAL_DISP_DrawBitmapScaled(3+(35*dockApps),scrH-40+5,16,16,bitmap,2);
    dockApps++;
}

void AddApp(const char* name,
            const unsigned long* bitmap,
            uint32_t color) 
{
    
}

void DrawBg() {
    int scrW, scrH;
    HAL_DISP_GetSize(&scrW, &scrH);
    HAL_DISP_DrawRect(0, 0, scrW, scrH, 0x001F1F1F);
}

void main() {
    DrawBg();

    // Draw Apps
    HAL_DISP_DrawBitmapScaled(3,SCREEN_HEIGHT-40+5,16,16,bitmapterminal,2);
    HAL_DISP_DrawBitmapScaled(3 + 35,SCREEN_HEIGHT-40+5,16,16,bitmapnotepad,2);

    // Draw Windows
    int windowTermX = 100;
    int windowTermY = 100;
    uint32_t windowTermColor = 0x002F002F;
    HAL_DISP_DrawRect(windowTermX, windowTermY, 200, 150, 0x000F0F0F);
    HAL_DISP_DrawRect(windowTermX + 1, windowTermY + 20 + 1, 200 - 2, 130 - 2, windowTermColor);
    HAL_DISP_DrawBitmap(windowTermX + 3, windowTermY + 3, 16, 16, bitmapterminal);
    gotoXY(windowTermX + 3 + 20, windowTermY + 3);
    setTextScale(2);
    const char* termName = "Terminal";
    printf(termName);
    setTextScale(1);
    gotoXY(windowTermX, windowTermY + 20);
    setMaxSize((windowTermX + 200) - windowTermX);
    setNewline(windowTermX);
    printf("This is only a visual representation. This is not a working example.");
    setMaxSize(SCREEN_WIDTH);

    int windowNoteX = 300;
    int windowNoteY = 300;
    uint32_t windowNoteColor = 0x00F0F0F0;
    HAL_DISP_DrawRect(windowNoteX, windowNoteY, 200, 150, 0x000F0F0F);
    HAL_DISP_DrawRect(windowNoteX + 1, windowNoteY + 20 + 1, 200 - 2, 130 - 2, windowNoteColor);
    HAL_DISP_DrawBitmap(windowNoteX + 3, windowNoteY + 3, 16, 16, bitmapnotepad);
    gotoXY(windowNoteX + 20, windowNoteY);
    setTextScale(2);
    const char* noteName = "Notepad";
    printf(noteName);
}