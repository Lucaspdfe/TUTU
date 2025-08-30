#pragma once
#include <stdint.h>
#include <arch/i686/vbe.h>

void STDIO_Initialize(VbeModeInfo* modeInfo);
void clrscr();
void drawBitmap(int width, int height, const uint16_t* bitmap);
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
