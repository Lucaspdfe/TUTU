#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <arch/i686/key.h>

extern uint8_t __bss_start;
extern uint8_t __end;

void crash_me();

void __attribute__((section(".entry"))) start(uint16_t bootDrive)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    HAL_Initialize();

    // Make this a comment if you want to keep HAL Debug output.
    clrscr();

    printf("Welcome to TUTU OS!\n");
    printf("Write anything you want.\n");
    i686_KEY_ReadString();
    printf("Waiting 1 second...\n");
    sleep(1);
    printf("Waited 1 second.\n");

    //crash_me();

end:
    for (;;);
}