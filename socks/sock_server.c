#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*
Чтобы не писать отдельную функцию для каждого типа, разработчики сделали общий тип struct sockaddr.
Все специфические структуры начинаются одинаково (с поля sa_family, приводить можно, если первое поле у структур одинаковое),
поэтому ядро может понять, какой тип адреса передан.

Приведение типов (struct sockaddr*)&addr говорит компилятору:

"Я знаю, что addr — это struct sockaddr_un, но функция bind ждёт struct sockaddr*.
Это безопасно, потому что первые байты структуры совместимы."
*/

void server() {
    int s_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s_socket < 0) {
        perror("socket");
        _exit(1);
    }

    //struct сокета-сервера:
    struct sockaddr_un s_addr;
    s_addr.sun_family = AF_UNIX;
    strcpy(s_addr.sun_path, "/tmp/mysocket");

    //struct сокета-клиента куда будем класть входящие соединения:
    struct sockaddr_un c_addr;
    socklen_t c_addr_len = sizeof(c_addr);

    unlink("/tmp/mysocket");
    if (bind(s_socket, (struct sockaddr*)&s_addr, sizeof(s_addr)) < 0) {
        perror("bind");
        _exit(1);
    }
    write(STDOUT_FILENO, "Create server socket\n", 21);

    if (listen(s_socket, 3) < 0) {
        perror("listen");
        _exit(1);
    }
    write(STDOUT_FILENO, "Listening...\n", 13);

    int c_socket = accept(s_socket, (struct sockaddr*)&c_addr, &c_addr_len);
    if (c_socket < 0) {
        perror("accept");
        _exit(1);
    }
    write(STDOUT_FILENO, "Accepted connection\n", 20);
    
    char recv_msg[32]; 
    ssize_t n = recv(c_socket, recv_msg, sizeof(recv_msg)-1, 0);
    if (n < 0) {
        perror("recv");
        _exit(1);
    }
    recv_msg[n] = '\0';
    write(STDOUT_FILENO, "Msg from client: ", 17);
    write(STDOUT_FILENO, recv_msg, n);

    if (send(c_socket, "Hello from server!\n", 19, 0) < 0) {
        perror("send");
        _exit(1);
    }
    close(c_socket);

    close(s_socket);
    return;
}

int main() {
    server();
}