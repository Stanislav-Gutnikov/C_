section .data
    counter dq 10

section .text
    global _start
_start:
    mov rax, [counter] ; в скобочках - значение переменной по адресу, без скобочек - адрес переменной
    add rax, 1
    mov [counter], rax
    add rax, 1
    mov [counter], rax
    mov rax, 60
    mov rdi, 0
    syscall