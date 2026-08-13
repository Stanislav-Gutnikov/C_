; два прохода по заданию,отдельно для len и uppercase
section .data
    msg db 'Hello, World!', 0
    len dq 0

section .text
    global _start

_start:
    mov rsi, msg
    mov rax, 0
    mov rcx, 0

count_msg_len:
    mov al, [rsi]
    cmp al, 0
    jz write_msg_len
    add rcx, 1
    add rsi, 1
    jmp count_msg_len
    
write_msg_len:
    mov [len], rcx
    sub rsi, 1 ; сдвигаем указатель с 0 обратно на восклицательный знак
    jmp transform_msg

transform_msg:
    cmp rcx, 0
    je do_exit
    mov al, [rsi]
    cmp al, 97
    jc skip
    cmp al, 122
    ja skip
    sub al, 32
    mov [rsi], al

    sub rcx, 1
    sub rsi, 1
    jmp transform_msg

skip:
    sub rcx, 1
    sub rsi, 1
    jmp transform_msg

do_exit:    
    mov rax, 60
    mov rdi, 0
    syscall