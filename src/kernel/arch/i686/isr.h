#pragma once
#include <stdint.h>

typedef struct {
    uint32_t ds;                  // Data segment selector
    uint32_t edi, esi, ebp, kern_esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, user_esp, ss; // Pushed by the processor automatically.
} __attribute__((packed)) Registers;

typedef void (*ISRHandler)(Registers* regs);

void i686_ISR_Initialize();
void i686_ISR_RegisterHandler(int interrupt, ISRHandler handler);