#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


const int MSG_LEN = 5; // msgs: ping, pong

void my_write(int fd, char *str, int str_len) {
    int w = write(fd, str, str_len);
    if (w < 0) {
        perror("Write error!");
        _exit(1);
    }
    return;
}

void duplex() {
    int pc_fd[2];
    int cp_fd[2];
    int pc_pipe = pipe(pc_fd);
    int cp_pipe = pipe(cp_fd);
    if (pc_pipe < 0 || cp_pipe < 0) {
        perror("Pipe creating error!");
        _exit(1);
    }
    write(pc_fd[1], "ping\n", MSG_LEN); // parent write to parent->child pipe
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork error!");
        _exit(1);
    }
    else if (pid == 0) {
        close(cp_fd[0]); // close read fd in child->parent pipe
        close(pc_fd[1]); // close write fd in parent->child pipe
        char c_buf[16];
        read(pc_fd[0], c_buf, MSG_LEN); // child read from parent->child pipe
        close(pc_fd[0]); // close read fd in parent->child pipe
        write(STDOUT_FILENO, "Recived from parent: ", 21);
        write(STDOUT_FILENO, c_buf, MSG_LEN);
        write(cp_fd[1], "pong\n", MSG_LEN); // child write to child->parent pipe
        close(cp_fd[1]); // close write fd in child->parent pipe
        _exit(0);
    }
    int status;
    wait(&status);
    if (WIFEXITED(status)) {
        char p_buf[16];
        read(cp_fd[0], p_buf, MSG_LEN); // parent read from child->parent pipe
        close(cp_fd[0]); // close read fd in child->parent pipe
        write(STDOUT_FILENO, "Recived from child: ", 21);
        write(STDOUT_FILENO, p_buf, MSG_LEN);
    }
    return;
}

int main() {
    duplex();
    return 0;
}
