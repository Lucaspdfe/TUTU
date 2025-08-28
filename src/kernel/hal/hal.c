#include "hal.h"
#include <stdio.h>
#if defined(TARGET) && TARGET == i686
    #include <arch/i686/gdt.h>
    #include <arch/i686/idt.h>
    #include <arch/i686/isr.h>
    #include <arch/i686/irq.h>
    #include <arch/i686/pit.h>
    #include <arch/i686/key.h>
#endif

void HAL_Initialize() {
// Arch is i686
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
#endif
}
