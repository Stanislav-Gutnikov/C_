section .data
    message db 'Hello, World!', 0
    len dq 0

section .text
    global _start

_start:
    mov rsi, message
    mov rax, 0
    mov rcx, 0

do_while:
    mov al, [rsi]
    cmp al, 0
    jz end_while
    add rcx, 1
    add rsi, 1
    jmp do_while
    
end_while:
    mov [len], rcx
    mov rax, 60
    mov rdi, 0
    syscall