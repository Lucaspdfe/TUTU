#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <arch/i686/vbe.h>
#include <arch/i686/disp.h>
#include "cat.h"

extern uint8_t __bss_start;
extern uint8_t __end;

void crash_me();

void __attribute__((section(".entry"))) start(VbeModeInfo* modeInfo)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    STDIO_Initialize(modeInfo);

    HAL_Initialize();

    // Make this a comment if you want to keep HAL Debug output.
    clrscr();

    const char* str;

    printf("Gato");

    i686_DISP_DrawBitmap(0, 18, 144, 144, bitmapcat);

    // printf("Welcome to TUTU OS!\n");
    // scanf("Write anything you want\n%s", str);
    // printf("You wrote: %s\n", str);
    // printf("Waiting 1 second...\n");
    // sleep(1);
    // printf("Waited 1 second.\n");

    //crash_me();

end:
    for (;;);
}