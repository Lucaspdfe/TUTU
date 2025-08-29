#pragma once
#include <stdint.h>

void clrscr();
void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);
void sleep(int seconds);
static int kgetc(void);
static void kungetc(int c);
int kgets(char* buf, int maxlen);
#define scanf kscanf
int kscanf(const char* fmt, ...);
