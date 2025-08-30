#include "key.h"
#include "irq.h"
#include "pit.h"
#include "io.h"
#include <stddef.h>
#include <stdio.h>

#define KEYBOARD_DATA 0x60
#define KEYBOARD_STATUS 0x64

char last_key_pressed;
bool is_key_modified;

static bool shift_pressed = false;
static unsigned char last_scancode = 0;
static size_t key_hold_time = 0;
static const size_t KEY_REPEAT_DELAY = 500; // Delay in milliseconds
static const size_t KEY_REPEAT_RATE = 50;  // Repeat rate in milliseconds

static bool extended_code = false;  // track 0xE0 prefix
static uint8_t last_arrow_scancode = 0;

void i686_KEY_Handler(Registers* regs);

void i686_KEY_Initialize() {
    i686_IRQ_RegisterHandler(1, i686_KEY_Handler);
}

bool i686_KEY_HasChar() {
    return is_key_modified;
}

char i686_KEY_WaitChar() {
    is_key_modified = false;
    while (!is_key_modified) {
        // spin until a normal character is pressed
        // arrow keys and 0s will be ignored
    }
    return last_key_pressed;
}

// Returns 0 if no arrow key pressed
uint8_t i686_KEY_GetArrow() {
    uint8_t arrow = last_arrow_scancode;
    last_arrow_scancode = 0;  // reset after reading
    return arrow;
}

// Returns 0 if no printable key pressed
char i686_KEY_GetChar() {
    if (!is_key_modified) return 0;
    is_key_modified = false;
    return last_key_pressed;
}


static const char ext_keymap[128] = {
    [KEY_UP]    = 0x80, // assign custom codes above ASCII
    [KEY_DOWN]  = 0x81,
    [KEY_LEFT]  = 0x82,
    [KEY_RIGHT] = 0x83,
};


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

// Wait for one arrow key, return its scancode (0x48, 0x50, 0x4B, 0x4D)
uint8_t i686_KEY_WaitArrow() {
    last_arrow_scancode = 0;
    while (last_arrow_scancode == 0) {
        // spin until an arrow key is captured
    }
    return last_arrow_scancode;
}

void i686_KEY_Handler(Registers* regs) {
    uint8_t status = inb(KEYBOARD_STATUS);
    if (status & 0x01) {
        uint8_t scancode = inb(KEYBOARD_DATA);

        // Check for extended prefix (0xE0)
        if (scancode == 0xE0) {
            extended_code = true;
            return;
        }

        bool key_released = scancode & 0x80;
        scancode &= 0x7F;

        if (extended_code) {
            extended_code = false;
            if (!key_released) {
                // Arrow keys only
                switch (scancode) {
                    case KEY_UP:
                    case KEY_DOWN:
                    case KEY_LEFT:
                    case KEY_RIGHT:
                        last_arrow_scancode = scancode; // store scancode
                        break;
                }
            }
            return; // don’t process further as normal key
        }

        // --- normal keys (same as your old code) ---
        if (scancode == 0x2A || scancode == 0x36) { // Shift keys
            shift_pressed = !key_released;
        } else if (!key_released && scancode < sizeof(keymap)/sizeof(keymap[0])) {
            char key = shift_pressed ? keymap[scancode][1] : keymap[scancode][0];
            if (key) {
                last_key_pressed = key;
                is_key_modified = true;
                last_scancode = scancode;
                key_hold_time = 0;
            }
        }

        // Handle key repeat
        if (!key_released && scancode == last_scancode) {
            key_hold_time += 10;
            if (key_hold_time >= KEY_REPEAT_DELAY) {
                size_t repeat_intervals = (key_hold_time - KEY_REPEAT_DELAY) / KEY_REPEAT_RATE;
                if (repeat_intervals > 0) {
                    last_key_pressed = shift_pressed ? keymap[scancode][1] : keymap[scancode][0];
                    is_key_modified = true;
                }
            }
        } else if (key_released && scancode == last_scancode) {
            last_scancode = 0;
            key_hold_time = 0;
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