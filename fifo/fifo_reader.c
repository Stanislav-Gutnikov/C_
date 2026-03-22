#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

void fifo_read() {
    int fd = open("myfifo", O_RDONLY);
    if (fd < 0) {
        perror("open");
        _exit(1);
    }
    write(STDOUT_FILENO, "From writer:\n\n", 14);
    char buf[16];
    while (1) {
        size_t read_bytes = read(fd, buf, 16);
        if (read_bytes < 0) {
            perror("read");
            _exit(1);
        }
        if (read_bytes == 0) {
            break;
        }
        else if (read_bytes > 0) {
            write(STDOUT_FILENO, buf, read_bytes);
        }
    }
    close(fd);
    _exit(0);
    return;
}

int main() {
    fifo_read();
    return 0;
}