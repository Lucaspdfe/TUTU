#pragma once
#include <stdint.h>
#include "hal/hal.h"

void STDIO_Initialize();
void clrscr();
void drawBitmap(int width, int height, const unsigned long* bitmap);
void putc(char c);
void getXY(int* x, int* y);
void setNewline(int x);
void setMaxSize(int px);
void setTextScale(int mult);
void puts(const char* str);
void gotoXY(int x, int y);
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);
void sleep(int seconds);
static int kgetc(void);
static void kungetc(int c);
int kgets(char* buf, int maxlen);
#define scanf kscanf
int kscanf(const char* fmt, ...);
