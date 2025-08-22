#include "print.h"
#include "x86.h"

void putc(char c) {
    if (c == '\n')
    {
        x86_Video_WriteCharTeletype('\r', 0);
        x86_Video_WriteCharTeletype('\n', 0);
        return;
    }
    x86_Video_WriteCharTeletype(c, 0);
}

void puts(const char* str) {
    while(*str)
    {
        putc(*str);
        str++;
    }
}

void clrscr(void) {
    x86_Video_ClearScreen();
}
