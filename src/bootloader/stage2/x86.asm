bits 16

section _TEXT class=CODE

global _x86_VideoWriteCharTeletype
_x86_VideoWriteCharTeletype:
    push bp
    mov bp, sp

    push bx

    mov ah, 0x0e
    mov al, [bp + 4]
    mov bh, [bp + 6]
    int 10h

    pop bx

    mov sp, bp
    pop bp
    ret
