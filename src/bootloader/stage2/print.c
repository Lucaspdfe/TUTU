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

void puts_int(int __i) {
    char buffer[5]; // max 5 digits for 16-bit int (32767)
    int i = 0;

    // Handle negative numbers
    if (__i < 0) {
        putc('-');
        __i = -__i;
    }

    // Special case: zero
    if (__i == 0) {
        putc('0');
        return;
    }

    // Convert digits in reverse
    while (__i > 0) {
        buffer[i++] = (__i % 10) + '0';
        __i /= 10;
    }

    // Print in correct order
    while (--i >= 0) {
        putc(buffer[i]);
    }
}

void puts_uint(unsigned int __u) {
    char buffer[5]; // max 5 digits for 16-bit unsigned int (65535)
    int i = 0;

    if (__u == 0) {
        putc('0');
        return;
    }

    while (__u > 0) {
        buffer[i++] = (__u % 10) + '0';
        __u /= 10;
    }

    while (--i >= 0) {
        putc(buffer[i]);
    }
}

void puts_hex(unsigned int __u) {
    char hex_chars[] = "0123456789abcdef";
    char buffer[4]; // 4 hex digits max for 16-bit
    int i = 0;

    if (__u == 0) {
        putc('0');
        return;
    }

    while (__u > 0) {
        buffer[i++] = hex_chars[__u & 0xF];
        __u >>= 4;
    }

    while (--i >= 0) {
        putc(buffer[i]);
    }
}

void puts_hexu(unsigned int __u) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[4]; // 4 hex digits max for 16-bit
    int i = 0;

    if (__u == 0) {
        putc('0');
        return;
    }

    while (__u > 0) {
        buffer[i++] = hex_chars[__u & 0xF];
        __u >>= 4;
    }

    while (--i >= 0) {
        putc(buffer[i]);
    }
}
