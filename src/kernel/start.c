#include "stdio.h"
#include <string.h>
#include <stdint.h>
#include "hal/hal.h"

// --- Extern Main function defined at main.c ---
extern void main();

// --- BSS definitions ---
extern uint8_t __bss_start; 
extern uint8_t __end;

// --- Main ---
void __attribute__((section(".entry"))) start(VbeModeInfo* modeInfo) {
    memset((void*)&__bss_start, 0, (&__end - &__bss_start));
    HAL_Initialize();
    HAL_DISP_PassVBEInfo(modeInfo);
    STDIO_Initialize();
    clrscr();

    main();
}
