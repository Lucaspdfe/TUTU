#include "stdint.h"
#include "stdio.h"

void _cdecl cstart_(/*uint16_t bootDrive*/) {
    short a = -1234;
    unsigned short b = 5678;
    short c = 15;
    unsigned short d = 15;
    clrscr();

    printf("Character: %c\n", 'A');
    printf("String: %s\n", "Hello, world!");
    printf("Signed decimal: %d\n", a);
    printf("Unsigned decimal: %u\n", b);
    printf("2-digit hex (lowercase): %02x\n", c);
    printf("2-digit hex (uppercase with 0x): %02X\n", d);
    printf("Percent sign: %%\n");
    for (;;);
}
