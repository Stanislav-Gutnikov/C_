section .data
    n dq 5
    result dq 0
section .text
    global _start
_start:
    mov rdi, [n]
    call factorial
    mov [result], rax
    mov rax, 60
    mov rdi, 0
    syscall


factorial:
    cmp rdi, 1
    je return

    push rdi
    sub rdi, 1
    call factorial

    pop rdi
    mul rdi
    ret

return:
    mov rax, 1
    ret