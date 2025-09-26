#include "hal.h"
#include <stdio.h>
#if defined(TARGET) && TARGET == i686
    #include <arch/i686/gdt.h>
    #include <arch/i686/idt.h>
    #include <arch/i686/isr.h>
    #include <arch/i686/irq.h>
    #include <arch/i686/pit.h>
    #include <arch/i686/key.h>
    #include <arch/i686/scheduler.h>
    #include <arch/i686/mouse.h>
    #include <arch/i686/disp.h>
#endif

void HAL_Initialize() {
#if defined(TARGET) && TARGET == i686
    i686_GDT_Initialize();
    printf("GDT initialized.\n");
    i686_IDT_Initialize();
    printf("IDT initialized.\n");
    i686_ISR_Initialize();
    printf("ISR initialized.\n");
    i686_IRQ_Initialize();
    printf("IRQ initialized.\n");
    i686_PIT_Initialize();
    printf("PIT initialized.\n");
    i686_KEY_Initialize();
    printf("Keyboard initialized.\n");
    i686_MOUSE_Initialize();
    printf("PS/2 Mouse initialized.\n");
#endif
}

// Keyboard
bool HAL_KEY_HasChar() {
#if defined(TARGET) && TARGET == i686
    return i686_KEY_HasChar();
#endif
}
char HAL_KEY_GetChar() {
#if defined(TARGET) && TARGET == i686
    return i686_KEY_GetChar();
#endif
}

char HAL_KEY_ReadChar() {
#if defined(TARGET) && TARGET == i686
    return i686_KEY_ReadKey();
#endif
}

// Timer
void HAL_TIMER_Sleep(uint32_t milliseconds) {
#if defined(TARGET) && TARGET == i686
    i686_PIT_Sleep(milliseconds);
#endif
}

// Mouse
void HAL_MOUSE_WaitForInput(int* x, int* y, bool *leftButton, bool *rightButton) {
#if defined(TARGET) && TARGET == i686
    i686_MOUSE_WaitForInput(x, y, leftButton, rightButton);
#endif
}
void HAL_MOUSE_GetPosition(int* x, int* y, bool* leftButton, bool* rightButton) {
#if defined(TARGET) && TARGET == i686
    i686_MOUSE_GetPosition(x, y, leftButton, rightButton);
#endif
}

// Display
void HAL_DISP_PutChar(char c, int x, int y, uint32_t color, int sizeMultiplier) {
#if defined(TARGET) && TARGET == i686
    i686_DISP_PutChar(c, x, y, color, sizeMultiplier);
#endif
}

void HAL_DISP_PassVBEInfo(VbeModeInfo* modeinfo) {
#if defined(TARGET) && TARGET == i686
    i686_DISP_PassVBEInfo(modeinfo);
#endif
}

void HAL_DISP_PutPixel(int x, int y, uint32_t color) {
#if defined(TARGET) && TARGET == i686
    i686_DISP_PutPixel(x, y, color);
#endif
}

void HAL_DISP_ClearScreen() {
#if defined(TARGET) && TARGET == i686
    i686_DISP_ClearScreen();
#endif
}

void HAL_DISP_GetSize(int* width, int* height) {
#if defined(TARGET) && TARGET == i686
    i686_DISP_GetSize(width, height);
#endif
}

void HAL_DISP_DrawBitmap(int x, int y, int w, int h, const unsigned long* bitmap) {
#if defined(TARGET) && TARGET == i686
    i686_DISP_DrawBitmap(x, y, w, h, bitmap);
#endif
}

void HAL_DISP_DrawBitmapScaledTransparent(int x, int y, int w, int h, const unsigned long* bitmap, int scale, uint32_t transparentColor) {
#if defined(TARGET) && TARGET == i686
    i686_DISP_DrawBitmapScaledTransparent(x, y, w, h, bitmap, scale, transparentColor);
#endif
}

void HAL_DISP_DrawRect(int x, int y, int w, int h, uint32_t color) {
#if defined(TARGET) && TARGET == i686
    i686_DISP_DrawRect(x, y, w, h, color);
#endif
}

void HAL_DISP_DrawRoundedRect(int x, int y, int w, int h, int radius, uint32_t color) {
#if defined(TARGET) && TARGET == i686
    i686_DISP_DrawRoundedRect(x, y, w, h, radius, color);
#endif
}

void HAL_DISP_DrawBitmapTransparent(int x, int y, int w, int h, const unsigned long* bitmap, uint32_t transparentColor) {
#if defined(TARGET) && TARGET == i686
    i686_DISP_DrawBitmapTransparent(x, y, w, h, bitmap, transparentColor);
#endif
}

void HAL_DISP_DrawBitmapScaled(int x, int y, int w, int h, const unsigned long* bitmap, int scale) {
#if defined(TARGET) && TARGET == i686
    i686_DISP_DrawBitmapScaled(x, y, w, h, bitmap, scale);
#endif
}

uint32_t HAL_DISP_GetPixel(int x, int y) {
#if defined(TARGET) && TARGET == i686
    return i686_DISP_GetPixel(x, y);
#endif
}

void HAL_Scheduler_Init(uint32_t tick_hz, uint32_t default_timeslice_ticks) {
#if defined(TARGET) && TARGET == i686
    Scheduler_Init(tick_hz, default_timeslice_ticks);
#endif
}
Task* HAL_Scheduler_AddTask(void (*entry)(void*), void *arg, const char *name) {
#if defined(TARGET) && TARGET == i686
    return Scheduler_AddTask(entry, arg, name);
#endif
}
__attribute__((noreturn)) void HAL_Scheduler_Start(void) {
#if defined(TARGET) && TARGET == i686
    Scheduler_Start();
#endif
}

void HAL_Scheduler_Yield(void) {
#if defined(TARGET) && TARGET == i686
    Scheduler_Yield();
#endif
}

void HAL_Scheduler_Sleep(uint32_t ms) {
#if defined(TARGET) && TARGET == i686
    Scheduler_Sleep(ms);
#endif
}

void HAL_Scheduler_Exit(void) {
#if defined(TARGET) && TARGET == i686
    Scheduler_Exit();
#endif
}
