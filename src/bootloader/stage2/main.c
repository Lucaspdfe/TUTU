#include "stdint.h"
#include "printf.h"

void _cdecl cstart_(/* uint16_t bootDrive */) {
    printf("Test \"%s\" '%c' <0x%x> %i u%u", "Test String", 'H', 16, -15, -15);
    for(;;);
}
