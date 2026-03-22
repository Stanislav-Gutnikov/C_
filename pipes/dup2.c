#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

const int buf_len = 256;

void redirect() {
    int cpid = 0;
    int fd[2];
    int ppipe = pipe(fd);
    if (ppipe < 0) {
        perror("Creating pipe error!");
        _exit(1);
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork error!");
        _exit(1);
    }
    else if (pid == 0) {
        cpid = getpid();
        close(fd[0]);
        int dup = dup2(fd[1], STDOUT_FILENO); // перенаправляем stdout ребенка в канал
        if (dup < 0) {
            perror("dup2 error!");
            _exit(1);
        }
        execlp("ls", "ls", "-l", NULL);
        perror("execlp failed!");
        _exit(1);
        close(fd[1]);
    }
    close(fd[1]);
    char buf[buf_len];
    size_t read_bytes;
    while ((read_bytes = read(fd[0], buf, 128)) > 0) {
        if (read_bytes < 0) {
            perror("Read error!");
            _exit(1);
        }
        write(STDOUT_FILENO, buf, read_bytes);
    }
    int status;
    wait(&status);
    if (WIFEXITED(status)) {
        write(STDOUT_FILENO, "\n\nChild finished!\n", 18);
    }
    close(fd[0]);
    return;
}

int main() {
    redirect();
    return 0;
}