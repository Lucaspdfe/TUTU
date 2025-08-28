#include "key.h"
#include "irq.h"
#include "io.h"
#include <stddef.h>
#include <stdio.h>

#define KEYBOARD_DATA 0x60
#define KEYBOARD_STATUS 0x64

char last_key_pressed;
bool is_key_modified;

static bool shift_pressed = false;

void i686_KEY_Handler(Registers* regs);

void i686_KEY_Initialize() {
    i686_IRQ_RegisterHandler(1, i686_KEY_Handler);
}

// Simple US QWERTY keymap for scan codes 0x01-0x3A
static const char keymap[59][2] = {
    {0, 0},    // 0x00
    {27, 27},  // 0x01 ESC
    {'1', '!'},// 0x02
    {'2', '@'},// 0x03
    {'3', '#'},// 0x04
    {'4', '$'},// 0x05
    {'5', '%'},// 0x06
    {'6', '^'},// 0x07
    {'7', '&'},// 0x08
    {'8', '*'},// 0x09
    {'9', '('},// 0x0A
    {'0', ')'},// 0x0B
    {'-', '_'},// 0x0C
    {'=', '+'},// 0x0D
    {'\b','\b'},// 0x0E Backspace
    {'\t','\t'},// 0x0F Tab
    {'q', 'Q'},// 0x10
    {'w', 'W'},// 0x11
    {'e', 'E'},// 0x12
    {'r', 'R'},// 0x13
    {'t', 'T'},// 0x14
    {'y', 'Y'},// 0x15
    {'u', 'U'},// 0x16
    {'i', 'I'},// 0x17
    {'o', 'O'},// 0x18
    {'p', 'P'},// 0x19
    {'[', '{'},// 0x1A
    {']', '}'},// 0x1B
    {'\n','\n'},// 0x1C Enter
    {0, 0},    // 0x1D Ctrl
    {'a', 'A'},// 0x1E
    {'s', 'S'},// 0x1F
    {'d', 'D'},// 0x20
    {'f', 'F'},// 0x21
    {'g', 'G'},// 0x22
    {'h', 'H'},// 0x23
    {'j', 'J'},// 0x24
    {'k', 'K'},// 0x25
    {'l', 'L'},// 0x26
    {';', ':'},// 0x27
    {'\'','"'},// 0x28
    {'`', '~'},// 0x29
    {0, 0},    // 0x2A Left Shift
    {'\\','|'},// 0x2B
    {'z', 'Z'},// 0x2C
    {'x', 'X'},// 0x2D
    {'c', 'C'},// 0x2E
    {'v', 'V'},// 0x2F
    {'b', 'B'},// 0x30
    {'n', 'N'},// 0x31
    {'m', 'M'},// 0x32
    {',', '<'},// 0x33
    {'.', '>'},// 0x34
    {'/', '?'},// 0x35
    {0, 0},    // 0x36 Right Shift
    {'*', '*'},// 0x37 Keypad *
    {0, 0},    // 0x38 Alt
    {' ', ' '},// 0x39 Space
    {0, 0},    // 0x3A CapsLock
};

void i686_KEY_Handler(Registers* regs) {
    unsigned char status = inb(KEYBOARD_STATUS);
    if (status & 0x01) {
        unsigned char scancode = inb(KEYBOARD_DATA);

        // Key released: high bit set
        bool released = scancode & 0x80;
        unsigned char keycode = scancode & 0x7F;

        // Shift key handling (left shift: 0x2A, right shift: 0x36)
        if (keycode == 0x2A || keycode == 0x36) {
            shift_pressed = !released;
            return;
        }

        if (!released) {
            char ch = 0;
            if (keycode < sizeof(keymap)/sizeof(keymap[0])) {
                ch = keymap[keycode][shift_pressed ? 1 : 0];
            }
            last_key_pressed = ch;
            is_key_modified = true;
        }
    }
}

char i686_KEY_ReadKey() {
    is_key_modified = false;
    while (!is_key_modified);
    return last_key_pressed;
}

const char* i686_KEY_ReadString() {
    static char buffer[256];
    size_t index = 0;
    while (index < sizeof(buffer) - 1) {
        char ch = i686_KEY_ReadKey();
        if (ch == '\n') {
            putc('\n');
            break;
        } else if (ch == '\b') {
            if (index > 0) {
                index--;
                putc('\b');
            }
        } else if (ch != 0) {
            buffer[index++] = ch;
            putc(ch);
        }
    }
    buffer[index] = '\0';
    return buffer;
}