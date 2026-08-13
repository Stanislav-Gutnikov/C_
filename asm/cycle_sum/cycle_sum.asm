section .data
    arr dq 1, 2, 3, 4, 5
    arr_len dq 5
    sum dq 0

section .text ;Кстати, у любой программы 3 области памяти: код(.text), данные (.data) и куча (heap). Куча никак не обозначается как область, так как растет динамически по ходу выполнения? 
    global _start

_start:
    mov rsi, arr
    mov rcx, [arr_len]
    mov rax, 0

do_while:
    add rax, [rsi]
    add rsi, 8 ; +8 бит (сдвигаем адрес на следующий эл-т)
    dec rcx ; уменьшаем счетчик (-=1)
    cmp rcx, 0 ; сравниваем счетчик с 0
    jnz do_while ; если не ноль - условие срабатывает - новая итерация с начала цикла
    mov [sum], rax ; если не срабатывает - цикл завершен , запысываем sum, exit
    mov rax, 60
    mov rdi, 0
    syscall
