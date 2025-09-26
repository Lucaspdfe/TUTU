#pragma once
#include <stdint.h>
#include <stdbool.h>

// Structs
typedef struct {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed)) VbeModeInfo;

typedef enum {
    DISP_COLOR_BLACK    = 0x000000,
    DISP_COLOR_WHITE    = 0xFFFFFF,
    DISP_COLOR_RED      = 0xFF0000,
    DISP_COLOR_GREEN    = 0x00FF00,
    DISP_COLOR_BLUE     = 0x0000FF,
    DISP_COLOR_YELLOW   = 0xFFFF00,
    DISP_COLOR_CYAN     = 0x00FFFF,
    DISP_COLOR_MAGENTA  = 0xFF00FF,
    DISP_COLOR_GRAY     = 0x808080,
    DISP_COLOR_LGRAY    = 0xC0C0C0,
    DISP_COLOR_DGRAY    = 0x404040,
} DispColor;

typedef enum {
    TASK_UNUSED = 0,
    TASK_RUNNABLE,
    TASK_SLEEPING,
    TASK_ZOMBIE
} TaskState;

typedef struct Task {
    // Saved stack pointer for context switch
    uint32_t *sp;

    // Metadata
    TaskState state;
    const char *name;

    // Entry
    void (*entry)(void *);
    void *arg;

    // Timing / scheduling
    uint64_t wake_tick;
    uint32_t time_slice;      // remaining ticks in current slice
    uint32_t base_timeslice;  // initial timeslice length (ticks)
    uint32_t id;
} Task;

// Init
void HAL_Initialize();

// Keyboard
bool HAL_KEY_HasChar();
char HAL_KEY_GetChar();
char HAL_KEY_ReadChar();

// Timer
void HAL_TIMER_Sleep(uint32_t milliseconds);

// Mouse
void HAL_MOUSE_WaitForInput(int* x, int* y, bool *leftButton, bool *rightButton);
void HAL_MOUSE_GetPosition(int* x, int* y, bool* leftButton, bool* rightButton);

// Display
void HAL_DISP_PassVBEInfo(VbeModeInfo* modeinfo);
void HAL_DISP_PutChar(char c, int x, int y, uint32_t color, int sizeMultiplier);
void HAL_DISP_PutPixel(int x, int y, uint32_t color);
void HAL_DISP_ClearScreen();
void HAL_DISP_GetSize(int* width, int* height);
void HAL_DISP_DrawBitmap(int x, int y, int w, int h, const unsigned long* bitmap);
void HAL_DISP_DrawBitmapScaledTransparent(int x, int y, int w, int h, const unsigned long* bitmap, int scale, uint32_t transparentColor);
void HAL_DISP_DrawBitmapScaled(int x, int y, int w, int h, const unsigned long* bitmap, int scale);
void HAL_DISP_DrawRect(int x, int y, int w, int h, uint32_t color);
void HAL_DISP_DrawRoundedRect(int x, int y, int w, int h, int radius, uint32_t color);
void HAL_DISP_DrawBitmapTransparent(int x, int y, int w, int h, const unsigned long* bitmap, uint32_t transparentColor);
uint32_t HAL_DISP_GetPixel(int x, int y);

// Scheduler
void HAL_Scheduler_Init(uint32_t tick_hz, uint32_t default_timeslice_ticks);
Task* HAL_Scheduler_AddTask(void (*entry)(void*), void *arg, const char *name);
__attribute__((noreturn)) void HAL_Scheduler_Start(void);
void HAL_Scheduler_Yield(void);
void HAL_Scheduler_Sleep(uint32_t ms);
void HAL_Scheduler_Exit(void);
