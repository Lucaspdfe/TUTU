org 0x0
bits 16

%define ENDL 0x0D, 0x0A

main:
    ; Write "Hello, world!" to the screen
    mov si, msg_hello
    call puts

    hlt

.halt:
    jmp .halt

;
; Prints a string to the screen
; Params
;   - ds:si points to the string
;
puts:
    ; Save registers
    push si
    push ax

    ; Write character in TTY mode (by setting ah to 0Eh)
    mov ah, 0Eh
    mov bh, 0
.loop:
    lodsb
    or al, al
    jz .done
    int 10h
    jmp .loop
.done:
    pop ax
    pop si
    ret

msg_hello: db 'Hello, world from Stage2 bootloader!', ENDL, 0