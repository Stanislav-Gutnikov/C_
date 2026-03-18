#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/mman.h>

const char CHILD_MSG[] = "Hello from child!\n";
const int CHILD_MSG_LEN = 18;

void create_pipe() {
    int fd[2];
    int ppipe = pipe(fd);
    if (ppipe<0) {
        perror("Creating pipe error!");
        _exit(1);
    }
    pid_t pid = fork();
    if (pid<0) {
        perror("Fork error!");
        _exit(1);
    }
    else if (pid==0) {
        close(fd[0]);
        write(fd[1], CHILD_MSG, CHILD_MSG_LEN);
        write(STDOUT_FILENO, "Child wrote to pipe\n", 20);
        close(fd[1]);
        _exit(0);
    }
    int status;
    wait(&status);
    if (WIFEXITED(status)) {
        write(STDOUT_FILENO, "Child exited\n", 13);
    }
    close(fd[1]);
    char buf[32];
    read(fd[0], buf, CHILD_MSG_LEN);
    close(fd[0]);
    write(STDERR_FILENO, buf, CHILD_MSG_LEN);
    return;
}

int main() {
    create_pipe();
    return 0;
}