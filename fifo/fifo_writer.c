#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>


void fifo_write() {
    if (mkfifo("myfifo", 0666) < 0) {
        perror("mkfifo");
        _exit(1);
    }
    write(STDOUT_FILENO, "FIFO created\nEnter your string:\n", 32);
    char buf[16];
    int fd = open("myfifo", O_WRONLY);
    if (fd < 0) {
        perror("open");
        _exit(1);
    }
    while (1) {
        size_t read_bytes = read(STDIN_FILENO, buf, 16);
        if (read_bytes < 0) {
            perror("read");
            _exit(1);
        }
        if (read_bytes == 5 && strncmp(buf, "exit\n", 5) == 0) {
            break;
        }
        else if (read_bytes > 0) {
            write(fd, buf, read_bytes);
        }
    }
    close(fd);
    unlink("myfifo");

    _exit(0);
    return;
}

int main() {
    fifo_write();
    return 0;
}