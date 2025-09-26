#include "scheduler.h"
#include "pit.h"
#include "irq.h"
#include "io.h"

uint64_t pit_ticks = 0;

void i686_PIT_Handler(Registers* regs) {
    Scheduler_OnTickISR();
    pit_ticks++;
}

void i686_PIT_SetFrequency(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36); // Command byte
    outb(0x40, divisor & 0xFF); // Low byte
    outb(0x40, (divisor >> 8) & 0xFF); // High byte
}

void i686_PIT_Initialize() {
    i686_IRQ_RegisterHandler(0, i686_PIT_Handler);
    i686_PIT_SetFrequency(1000); // Set default frequency to 1000Hz
}

void i686_PIT_Sleep(uint32_t milliseconds) {
    uint64_t target_ticks = pit_ticks + (milliseconds); // 1000Hz = 1ms per tick
    while (pit_ticks < target_ticks) {
    }
}