org 0x7C00
bits 16

boot:
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    jmp start
start:
    mov si, msg
    call puts
    cli
    hlt

;
; puts: prints a string to the screen
; Parameters:
;   ds:si: pointer to the string to print
;
puts:
    push ax
    push bx
    push cx
    push dx
    push si
    push di

    mov ah, 0x0E
    mov bh, 0x00
    mov bl, 0x07

    mov cx, 0xFFFF
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    loop .loop
.done:
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax

    ret

msg: db 'Hello, World!', 0

times 510-($-$$) db 0
db 0x55, 0xAA
