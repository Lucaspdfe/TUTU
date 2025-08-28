#pragma once
#include <arch/i686/io.h>
#include <stdint.h>

void i686_PIC_Remap(int offset1, int offset2);
void i686_PIC_EOI(uint8_t irq);
void i686_PIC_Mask(uint8_t IRQline);
void i686_PIC_Unmask(uint8_t IRQline);
void i686_PIC_Disable();
uint16_t i686_PIC_ReadIrqRequestRegister();
uint16_t i686_PIC_ReadInServiceRegister();