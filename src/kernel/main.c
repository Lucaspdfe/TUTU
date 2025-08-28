#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <arch/i686/key.h>

extern uint8_t __bss_start;
extern uint8_t __end;

void crash_me();

void timer(Registers* regs)
{
    // printf(".");
}

void __attribute__((section(".entry"))) start(uint16_t bootDrive)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

    HAL_Initialize();

    // Make this a comment if you want to keep HAL Debug output.
    clrscr();

    i686_IRQ_RegisterHandler(0, timer);

    printf("%s", i686_KEY_ReadString());

    //crash_me();

end:
    for (;;);
}