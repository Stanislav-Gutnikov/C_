#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


/*
Запуск:
 parallel -j 20 ./client ::: msg{1..20}
*/

void check(int res, char *msg) {
    if (res < 0) {
        perror(msg);
        _exit(1);
    }
    return;
}


void client(char *str) {
    size_t len = strlen(str);
    str[len-1] = '\n';
    int c_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    check(c_socket, "socket");

    struct sockaddr_un s_sock_addr;
    s_sock_addr.sun_family = AF_UNIX;
    strcpy(s_sock_addr.sun_path, "/tmp/echo_socket");
    socklen_t s_addr_len = sizeof(s_sock_addr);


    check(connect(c_socket, (struct sockaddr*)&s_sock_addr, s_addr_len), "connect");
    check(send(c_socket, str, strlen(str), 0), "send");
    char buf[32];
    ssize_t n = recv(c_socket, buf, sizeof(buf)-1, 0);
    check(n, "recv");
    buf[n] = '\0';
    write(STDOUT_FILENO, "Msg from server: ", 17);
    write(STDOUT_FILENO, buf, n);

    return;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        write(STDERR_FILENO, "Too few arguments!\n", 19);
        _exit(1);
    }
    client(argv[1]);
}