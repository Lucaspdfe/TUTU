#include "mouse.h"
#include "irq.h"
#include "io.h"
#include "scheduler.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// PS/2 ports
#define MOUSE_DATA      0x60
#define MOUSE_STATUS    0x64
#define MOUSE_CMD       0x64

// Commands
#define MOUSE_ENABLE    0xF4
#define MOUSE_ACK       0xFA

static int mouse_x = 0;
static int mouse_y = 0;
static bool left_btn = false;
static bool right_btn = false;

static uint8_t packet[3];
static uint8_t packet_index = 0;
static bool packet_ready = false;

static void mouse_write(uint8_t val) {
    // Tell controller we’re sending to mouse
    outb(MOUSE_CMD, 0xD4);
    outb(MOUSE_DATA, val);
}

static uint8_t mouse_read(void) {
    return inb(MOUSE_DATA);
}

void i686_MOUSE_Handler(Registers* regs);

void i686_MOUSE_Initialize() {
    // Enable auxiliary device (mouse)
    outb(MOUSE_CMD, 0xA8);

    // Enable interrupts
    outb(MOUSE_CMD, 0x20);
    uint8_t status = inb(MOUSE_DATA) | 2;
    outb(MOUSE_CMD, 0x60);
    outb(MOUSE_DATA, status);

    // Tell mouse to use default settings
    mouse_write(0xF6);
    mouse_read(); // ACK

    // Enable data reporting
    mouse_write(MOUSE_ENABLE);
    mouse_read(); // ACK

    // Register IRQ handler
    i686_IRQ_RegisterHandler(12, i686_MOUSE_Handler);
}

void i686_MOUSE_Handler(Registers* regs) {
    Scheduler_OnTickISR();
    uint8_t data = inb(MOUSE_DATA);

    packet[packet_index++] = data;

    if (packet_index == 3) {
        packet_index = 0;
        packet_ready = true;

        uint8_t status = packet[0];
        int dx = (int8_t)packet[1];
        int dy = (int8_t)packet[2];

        // Apply sign bits explicitly (defensive)
        if (status & 0x10) dx |= 0xFFFFFF00;
        if (status & 0x20) dy |= 0xFFFFFF00;

        mouse_x += dx;
        mouse_y -= dy; // PS/2 Y is inverted

        if (mouse_x < 0) mouse_x = 0;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_x > 639) mouse_x = 639;
        if (mouse_y > 479) mouse_y = 479;

        left_btn  = status & 0x1;
        right_btn = status & 0x2;
    }
}


void i686_MOUSE_WaitForInput(int* x, int* y, bool *leftButton, bool *rightButton) {
    // Spin until new packet arrives
    while (!packet_ready) {
        __asm__ volatile("hlt");
    }
    packet_ready = false;
    i686_MOUSE_GetPosition(x, y, leftButton, rightButton);
}

void i686_MOUSE_GetPosition(int* x, int* y, bool* leftButton, bool* rightButton) {
    if (x) *x = mouse_x;
    if (y) *y = mouse_y;
    if (leftButton) *leftButton = left_btn;
    if (rightButton) *rightButton = right_btn;
}
