; void ctx_switch(uint32_t **old_sp, uint32_t *new_sp)
; cdecl: args on stack: [esp+4]=&old_sp, [esp+8]=new_sp
global ctx_switch
section .text
ctx_switch:
    ; Save caller's ESP into *old_sp
    mov     eax, [esp+4]     ; &old_sp
    mov     edx, [esp+8]     ; new_sp
    mov     [eax], esp       ; *old_sp = current ESP
    mov     esp, edx         ; switch to new stack
    ret                      ; continue on new stack
