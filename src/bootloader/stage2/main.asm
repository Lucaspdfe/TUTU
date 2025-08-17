org 0x0
bits 16

start:
    ; Clear the screen
    mov ah, 0
    mov al, 0x03
    int 10h

    ; Print "Hello, World from KERNEL!!"
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
    pop cx
    pop bx
    pop ax

    ret

msg: db 'Hello, World from STAGE2!!', 0
