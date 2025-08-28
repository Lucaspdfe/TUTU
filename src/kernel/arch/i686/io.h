#pragma once
#include <stdint.h>

#define outb(port, value) i686_outb(port, value)
#define inb(port) i686_inb(port)
#define iowait() i686_iowait()
void __attribute__((cdecl)) i686_outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) i686_inb(uint16_t port);
void i686_iowait();
void __attribute__((cdecl)) i686_EnableInterrupts();
void __attribute__((cdecl)) i686_DisableInterrupts();

void __attribute__((cdecl)) i686_Panic();