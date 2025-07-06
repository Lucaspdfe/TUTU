org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

main:
    ; Setup data segments
    mov ax, 0
    mov ds, ax
    mov es, ax

    ; Setup stack
    mov ss, ax
    mov sp, 0x7C00

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

msg_hello: db 'Hello, world!', ENDL, 0

times 510-($-$$) db 0
dw 0AA55h