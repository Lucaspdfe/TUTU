#include "stdio.h"
// #if defined(TARGET) && TARGET == i686
#include <arch/i686/io.h>
#include <arch/i686/pit.h>
#include <arch/i686/key.h>
#include <arch/i686/vbe.h>
#include <arch/i686/disp.h>
// #endif
#include <stdarg.h>
#include <stdbool.h>

unsigned SCREEN_WIDTH = 80;
unsigned SCREEN_HEIGHT = 25;

int g_ScreenX = 0, g_ScreenY = 0;

void STDIO_Initialize(VbeModeInfo* modeInfo)
{
    i686_DISP_Initialize(modeInfo);

    int width, height;
    i686_DISP_GetSize(&width, &height);
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}

void clrscr()
{
    i686_DISP_ClearScreen();
    g_ScreenX = 0;
    g_ScreenY = 0;
}

void putc(char c)
{
    switch (c)
    {
        case '\n':
            g_ScreenX = 0;
            g_ScreenY += 16;
            break;
    
        case '\t':
            g_ScreenX = (g_ScreenX + 16) & ~7;
            break;

        case '\r':
            g_ScreenX = 0;
            break;

        case '\b':
            if (g_ScreenX > 0)
            {
                g_ScreenX -= 16;
                i686_DISP_PutChar('\0', g_ScreenX, g_ScreenY);
            }
            break;
        default:
            i686_DISP_PutChar(c, g_ScreenX, g_ScreenY);
            g_ScreenX += 16;
            break;
    }

    if (g_ScreenX >= SCREEN_WIDTH)
    {
        g_ScreenY += 16;
        g_ScreenX = 0;
    }
}

void puts(const char* str)
{
    while(*str)
    {
        putc(*str);
        str++;
    }
}

const char g_HexChars[] = "0123456789abcdef";

void printf_unsigned(unsigned long long number, int radix)
{
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do 
    {
        unsigned long long rem = number % radix;
        number /= radix;
        buffer[pos++] = g_HexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        putc(buffer[pos]);
}

void printf_signed(long long number, int radix)
{
    if (number < 0)
    {
        putc('-');
        printf_unsigned(-number, radix);
    }
    else printf_unsigned(number, radix);
}

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4

void printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    bool sign = false;
    bool number = false;

    while (*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':   state = PRINTF_STATE_LENGTH;
                                break;
                    default:    putc(*fmt);
                                break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_SHORT;
                                state = PRINTF_STATE_LENGTH_SHORT;
                                break;
                    case 'l':   length = PRINTF_LENGTH_LONG;
                                state = PRINTF_STATE_LENGTH_LONG;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   putc((char)va_arg(args, int));
                                break;

                    case 's':   
                                puts(va_arg(args, const char*));
                                break;

                    case '%':   putc('%');
                                break;

                    case 'd':
                    case 'i':   radix = 10; sign = true; number = true;
                                break;

                    case 'u':   radix = 10; sign = false; number = true;
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = false; number = true;
                                break;

                    case 'o':   radix = 8; sign = false; number = true;
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                if (number)
                {
                    if (sign)
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     printf_signed(va_arg(args, int), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG:        printf_signed(va_arg(args, long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   printf_signed(va_arg(args, long long), radix);
                                                        break;
                        }
                    }
                    else
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     printf_unsigned(va_arg(args, unsigned int), radix);
                                                        break;
                                                        
                        case PRINTF_LENGTH_LONG:        printf_unsigned(va_arg(args, unsigned  long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   printf_unsigned(va_arg(args, unsigned  long long), radix);
                                                        break;
                        }
                    }
                }

                // reset state
                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix = 10;
                sign = false;
                number = false;
                break;
        }

        fmt++;
    }

    va_end(args);
}

void print_buffer(const char* msg, const void* buffer, uint32_t count)
{
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    puts(msg);
    for (uint16_t i = 0; i < count; i++)
    {
        putc(g_HexChars[u8Buffer[i] >> 4]);
        putc(g_HexChars[u8Buffer[i] & 0xF]);
    }
    puts("\n");
}

void sleep(int seconds) {
// #if defined(TARGET) && TARGET == i686
    i686_PIT_Sleep(seconds * 1000);
// #endif
}

#define IS_SPACE(ch) ((ch)==' ' || (ch)=='\t' || (ch)=='\r' || (ch)=='\n')
#define IS_DIGIT(ch) ((ch) >= '0' && (ch) <= '9')

/* --- line reader (until Enter) --- */
static int readline(char *buf, int maxlen) {
    int len = 0;
    for (;;) {
        int c = i686_KEY_ReadKey();
        if (c == '\r') c = '\n';

        if (c == '\n') {
            putc('\n');
            buf[len] = '\0';
            return len;
        }

        if (c == 8 || c == 127) { /* backspace */
            if (len > 0) {
                len--;
                putc('\b'); putc(' '); putc('\b');
            }
            continue;
        }

        if (len+1 < maxlen) {
            buf[len++] = (char)c;
            putc((char)c);
        }
    }
}

/* --- integer parsing --- */
static int parse_uint(const char *s, unsigned long *out, int base) {
    unsigned long v=0; int d, any=0;
    if (base==0) {
        if (s[0]=='0' && (s[1]=='x'||s[1]=='X')) { base=16; s+=2; }
        else if (s[0]=='0' && s[1]) { base=8; s++; }
        else base=10;
    }
    for (; *s; s++) {
        if (*s>='0'&&*s<='9') d=*s-'0';
        else if (*s>='a'&&*s<='f') d=*s-'a'+10;
        else if (*s>='A'&&*s<='F') d=*s-'A'+10;
        else return 0;
        if (d>=base) return 0;
        v = v*base+d; any=1;
    }
    if (!any) return 0;
    *out=v; return 1;
}

static int parse_sint(const char *s, long *out) {
    int neg=0; if (*s=='-'||*s=='+'){neg=(*s=='-');s++;}
    unsigned long u;
    if (!parse_uint(s,&u,10)) return 0;
    *out=neg?-(long)u:(long)u; return 1;
}

/* --- token splitter --- */
static int next_token(const char **p, char *out, int maxlen) {
    const char *s = *p;
    while (*s && IS_SPACE(*s)) s++;
    if (!*s) return 0;

    int len=0;
    while (*s && !IS_SPACE(*s)) {
        if (len+1 < maxlen) out[len++] = *s;
        s++;
    }
    out[len] = '\0';
    *p = s;
    return 1;
}

/* --- main scanf-like --- */
int kscanf(const char *fmt, ...) {
    va_list ap;
    int assigned=0;

    va_start(ap, fmt);

    while (*fmt) {
        if (*fmt != '%') {
            /* print literal text as prompt */
            putc(*fmt);
            fmt++;
            continue;
        }

        fmt++; /* skip % */

        /* read a fresh line for each specifier */
        char line[256];
        readline(line, sizeof line);
        const char *lp = line;

        if (*fmt == 's') {
            char *dst = va_arg(ap, char *);
            int i = 0;

            /* Copy everything until newline or end of buffer */
            while (*lp && *lp != '\n' && i < 255) {
                dst[i++] = *lp++;
            }
            dst[i] = '\0';
        
            assigned++;
            fmt++;
            continue;
        }

        if (*fmt == 'c') {
            char *dst = va_arg(ap,char*);
            if (*lp) {
                *dst = *lp;
                assigned++;
            }
            fmt++; continue;
        }

        if (*fmt == 'd' || *fmt == 'i') {
            char tok[64]; long v;
            if (!next_token(&lp, tok, sizeof tok)) break;
            if (!parse_sint(tok,&v)) break;
            *va_arg(ap,int*)=(int)v;
            assigned++; fmt++; continue;
        }

        if (*fmt=='u' || *fmt=='x' || *fmt=='X' || *fmt=='o') {
            char tok[64]; unsigned long v;
            int base=(*fmt=='u'?10:(*fmt=='o'?8:16));
            if (!next_token(&lp, tok, sizeof tok)) break;
            if (!parse_uint(tok,&v,base)) break;
            *va_arg(ap,unsigned int*)=(unsigned int)v;
            assigned++; fmt++; continue;
        }
    }

    va_end(ap);
    return assigned;
}