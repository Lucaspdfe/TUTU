#pragma once

#include <stdint.h>
#include <stdbool.h>

// Extended scancodes (0xE0 prefix)
enum {
    KEY_UP    = 0x48,
    KEY_LEFT  = 0x4B,
    KEY_RIGHT = 0x4D,
    KEY_DOWN  = 0x50,
};

void i686_KEY_Initialize();
bool i686_KEY_HasChar();      // check if a normal char key was pressed
char i686_KEY_WaitChar();    // wait for a normal char key, return ASCII
char i686_KEY_GetChar();     // get last char key pressed, or 0 if none
uint8_t i686_KEY_GetArrow(); // get last arrow key scancode, or
uint8_t i686_KEY_WaitArrow(); // wait for arrow key, return scancode
char i686_KEY_ReadKey();
const char* i686_KEY_ReadString();