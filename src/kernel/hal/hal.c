#include "hal.h"
#include <stdio.h>
#if defined(TARGET) && TARGET == i686
    #include <arch/i686/gdt.h>
    #include <arch/i686/idt.h>
#endif

void HAL_Initialize() {
// Arch is i686
#if defined(TARGET) && TARGET == i686
    i686_GDT_Initialize();
    printf("GDT initialized.\n");
    i686_IDT_Initialize();
    printf("IDT initialized.\n");
#endif
}
