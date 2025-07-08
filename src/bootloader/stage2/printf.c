#include "printf.h"
#include "print.h"

void _cdecl printf(const char *format, ...) {
    int* argp = (int*)&format;
    argp += sizeof(format) / sizeof(int);
    while (*format) {
        if (*format != '%') {           // Default state
            putc(*format);
            format++;
        } else {
            format++;                   // Ignore the '%'
            switch(*format) {
                case 'd':
                case 'i':
                    puts_int(*argp);
                    argp++;
                    break;
                case 'u':
                    puts_uint((unsigned int)*argp);
                    argp++;
                    break;
                case 'X':
                case 'x':
                    puts_hexu((unsigned int)*argp);
                    argp++;
                    break;
                case 's':
                    puts((const char*)*argp);
                    argp += sizeof((const char*)*argp) / sizeof(int);
                    break;
                case 'c':
                    putc((char)*argp);
                    argp++;
                    break;
                case '%':
                    putc('%');
                    break;
            }
            format++;                   // Ignore the parsed character
        }
    }
}
