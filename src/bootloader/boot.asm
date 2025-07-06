org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

;
; FAT12 header
;
jmp short main
nop

bdb_oem:                        db "MSWIN4.1"
bdb_bytes_per_sector:           dw 0x0200
bdb_sectors_per_cluster:        db 0x01
bdb_reserved_sectors:           dw 0x0001
bdb_fat_count:                  db 0x02
bdb_dir_entries_count:          dw 0x00E0
bdb_total_sectors:              dw 0x0B40
bdb_media_descriptor_type:      db 0xF0
bdb_sectors_per_fat:            dw 0x0009
bdb_sectors_per_track:          dw 0x0012
bdb_heads:                      dw 0x0002
bdb_hidden_sectors:             dd 0x00000000
bdb_large_sector_count:         dd 0x00000000

; Extended boot record (ebr)
ebr_drive_number:               db 0x00
                                db 0x00
ebr_signature:                  db 0x29
ebr_volume_id:                  db 0x26, 0x12, 0x20, 0x13
ebr_volume_label:               db 'TUTU OS    '
ebr_system_id:                  db 'FAT12   '

main:
    ; Setup data segments
    mov ax, 0
    mov ds, ax
    mov es, ax

    ; Setup stack
    mov ss, ax
    mov sp, 0x7C00

    mov [ebr_drive_number], dl

    mov ax, 1
    mov cl, 1
    mov bx, 0x7E00
    call disk_read

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

;
; Error handlers
;
floppy_error:
    mov si, msg_read_failed
    call puts
    jmp wait_key_and_reboot

wait_key_and_reboot:
    mov ah, 0
    int 16h
    jmp 0xFFFF:0
.halt:
    cli
    hlt

;
; Disk Routines
;

;
; Converts an LBA address to a CHS address
; Params
;   - ax: LBA address
; Returns
;   - cx [bits 0-5]: sector number
;   - cx [bits 6-15]: cylinder
;   - dh: head
;
lba2chs:
    push ax
    push dx

    xor dx, dx
    div word [bdb_sectors_per_track]
    inc dx
    mov cx, dx
    xor dx, dx
    div word [bdb_heads]
    mov dh, dl
    mov ch, al
    shl ah, 6
    or cl, ah

    pop ax
    mov dl, al
    pop ax
    ret

;
; Reads sectors from a disk
; Params:
;   - ax: LBA address
;   - cl: number of sectors to read
;   - dl: drive number
;   - es:bx: memory location where to store read data
;
disk_read:

    push ax
    push bx
    push cx
    push dx
    push di

    push cx
    call lba2chs
    pop ax
    mov ah, 0x02
    mov di, 3

.retry:
    pusha
    stc
    int 13h
    jnc .done

    popa
    call disk_reset

    dec di
    test di, di
    jnz .retry

.fail:
    jmp floppy_error

.done:
    popa

    pop di
    pop dx
    pop cx
    pop bx
    pop ax

    ret   

;
; Resets disk controller
; Params
;   - dl: drive number
;
disk_reset: 
    pusha
    mov ah, 0
    stc
    int 13h
    jc floppy_error
    popa
    ret

msg_read_failed:    db 'Read failed!', ENDL, 0
msg_hello:          db 'Hello, world!', ENDL, 0

times 510-($-$$) db 0
dw 0AA55h

