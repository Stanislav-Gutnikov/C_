section .data
    a dq 10
    b dq 5
    result dq 0

section .text
    global _start

_start:
    mov rax, [a]
    mov rbx, [b]
    cmp rax, rbx
    jl not_equal ;jl / jnge: проверяет условие SF != OF (флаги SF и OF не должны быть равны) и выполняет переход, если первый операнд меньше второго. Оба операнда со знаком.
    mov rax, 0
    mov [result], rax
    mov rax, 60
    mov rdi, 0
    syscall

not_equal:
    mov rax, 1 
    mov [result], rax
    mov rax, 60
    mov rdi, 0
    syscall