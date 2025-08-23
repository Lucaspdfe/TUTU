#include "stdint.h"
#include "stdio.h"

void _cdecl cstart_(/*uint16_t bootDrive*/)
{
    clrscr();
    puts("Hello world from C!\n");
    for (;;);
}
