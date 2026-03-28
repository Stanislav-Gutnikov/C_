#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


void client() {
    int c_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (c_socket < 0) {
        perror("socket");
        _exit(1);
    }

    //struct сокета-клиента:
    struct sockaddr_un s_addr;
    s_addr.sun_family = AF_UNIX;
    strcpy(s_addr.sun_path, "/tmp/mysocket");

    if (connect(c_socket, (struct sockaddr*)&s_addr, sizeof(s_addr)) < 0) {
        perror("connect");
        _exit(1);
    }
    write(STDOUT_FILENO, "Established\n", 12);

    if (send(c_socket, "Hello from client!\n", 19, 0) < 0) {
        perror("send");
        _exit(1);
    }

    char recv_msg[32]; 
    ssize_t n = recv(c_socket, recv_msg, sizeof(recv_msg)-1, 0);
    if (n < 0) {
        perror("recv");
        _exit(1);
    }
    recv_msg[n] = '\0';
    write(STDOUT_FILENO, "Msg from server: ", 17);
    write(STDOUT_FILENO, recv_msg, n);

    close(c_socket);
    unlink("/tmp/mysocket");
    return;
}

int main() {
    client();
}