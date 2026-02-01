.data
public g_ssn
public g_addr
g_ssn DWORD 0
g_addr QWORD 0

_safetext SEGMENT READ EXECUTE

ggh_sys proc
    mov r10, rcx
    mov eax, g_ssn
    jmp g_addr
ggh_sys endp

ic_handler proc
    jmp r10
ic_handler endp

spf_invoke proc
    pop rax
    mov r11, rcx
    push rax
    push rdx
    mov rcx, r8
    mov rdx, r9
    jmp r11
spf_invoke endp

spf_invoke_5 proc
    pop rax
    mov r10, rcx
    mov r11, rdx
    mov rcx, r8
    mov rdx, r9
    mov r8, [rsp+32]
    mov r9, [rsp+40]
    push rax
    mov rax, [rsp+56]
    mov [rsp+32], rax
    push r11
    jmp r10
spf_invoke_5 endp

spf_invoke_7 proc
    pop rax
    mov r10, rcx
    mov r11, rdx
    mov rcx, r8
    mov rdx, r9
    mov r8, [rsp+32]
    mov r9, [rsp+40]
    push rax
    mov rax, [rsp+56]
    mov [rsp+32], rax
    mov rax, [rsp+64]
    mov [rsp+40], rax
    mov rax, [rsp+72]
    mov [rsp+48], rax
    push r11
    jmp r10
spf_invoke_7 endp

_safetext ENDS

end
