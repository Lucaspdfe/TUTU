#include "print.h"
#include "x86.h"

void putc(char __c) {
    x86_VideoWriteCharTeletype(__c, 0);
}

void puts(const char* __s) {
    while (*__s) {
        putc(*__s);
        __s++;
    }
}

