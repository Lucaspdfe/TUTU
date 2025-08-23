#include "printf.h"
#include "print.h"

// convert signed 16-bit int to decimal string
char* int16_to_str(short value, char *buf) {
    char tmp[6]; // max -32768 + '\0'
    int i = 0, j = 0;
    unsigned short v;

    if (value < 0) {
        buf[j++] = '-';
        v = (unsigned short)(-value);
    } else {
        v = (unsigned short)value;
    }

    do {
        tmp[i++] = '0' + (v % 10);
        v /= 10;
    } while (v);

    while (i--)
        buf[j++] = tmp[i];
    buf[j] = '\0';
    return buf;
}

// convert unsigned 16-bit int to decimal string
char* uint16_to_str(unsigned short value, char *buf) {
    char tmp[5]; // max 65535 + '\0'
    int i = 0, j = 0;

    do {
        tmp[i++] = '0' + (value % 10);
        value /= 10;
    } while (value);

    while (i--)
        buf[j++] = tmp[i];
    buf[j] = '\0';
    return buf;
}

// convert signed 16-bit int to hex string
char* int16_to_hex(short value, char *buf) {
    unsigned short v = (unsigned short)value;
    char tmp[4]; // max FFFF
    int i = 0, j = 0;

    do {
        int digit = v & 0xF;
        tmp[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        v >>= 4;
    } while (v);

    while (i--)
        buf[j++] = tmp[i];
    buf[j] = '\0';
    return buf;
}

// convert unsigned 16-bit int to hex string
char* uint16_to_hex(unsigned short value, char *buf, int with_prefix) {
    char tmp[4]; // max FFFF
    int i = 0, j = 0;

    if (with_prefix) {
        buf[j++] = '0';
        buf[j++] = 'x';
    }

    do {
        int digit = value & 0xF;
        tmp[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        value >>= 4;
    } while (value);

    while (i--)
        buf[j++] = tmp[i];
    buf[j] = '\0';
    return buf;
}

void _cdecl printf(const char *fmt, ...)
{
    int* argp;
    char buf[16];
    int pad_zero;
    int width;
    int i;
    int len;
    char *str;
    short s_val;
    unsigned short u_val;

    argp = (int*)&fmt;
    argp += sizeof(fmt) / sizeof(int);

    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;

            pad_zero = 0;
            width = 0;

            if (*fmt == '0')
            {
                pad_zero = 1;
                fmt++;
            }

            while (*fmt >= '0' && *fmt <= '9')
            {
                width = width * 10 + (*fmt - '0');
                fmt++;
            }

            switch (*fmt)
            {
                case 'c':
                    putc(*(char*)argp);
                    argp++;
                    break;

                case 's':
                    str = (char*)*argp;
                    argp++;
                    for (i = 0; str[i] != '\0'; i++)
                        ; // compute length
                    for (i = 0; i < width - i; i++)
                        putc(' ');
                    puts(str);
                    break;

                case 'd':
                    s_val = (short)*argp;
                    argp++;
                    int16_to_str(s_val, buf);

                    for (len = 0; buf[len] != '\0'; len++)
                        ; // compute length

                    for (i = 0; i < width - len; i++)
                        putc(pad_zero ? '0' : ' ');

                    puts(buf);
                    break;

                case 'u':
                    u_val = (unsigned short)*argp;
                    argp++;
                    uint16_to_str(u_val, buf);

                    for (len = 0; buf[len] != '\0'; len++)
                        ; // compute length

                    for (i = 0; i < width - len; i++)
                        putc(pad_zero ? '0' : ' ');

                    puts(buf);
                    break;

                case 'x':
                    s_val = (short)*argp;
                    argp++;
                    int16_to_hex(s_val, buf);

                    for (len = 0; buf[len] != '\0'; len++)
                        ;

                    for (i = 0; i < width - len; i++)
                        putc(pad_zero ? '0' : ' ');

                    puts(buf);
                    break;

            case 'X':
                u_val = (unsigned short)*argp;
                argp++;
                        
                // convert number to hex digits in temporary buffer
                int16_to_hex(u_val, buf); // buf contains "F" for 15
                        
                // count length of hex digits
                for (len = 0; buf[len] != '\0'; len++)
                    ;
                        
                // print "0x" prefix first
                putc('0');
                putc('x');
                        
                // print zero padding if needed
                for (i = 0; i < width - len; i++)
                    putc('0');
                        
                // print the hex digits
                puts(buf);
                break;

                case '%':
                    putc('%');
                    break;

                default:
                    break;
            }
            fmt++;
        }
        else
        {
            putc(*fmt);
            fmt++;
        }
    }
}

