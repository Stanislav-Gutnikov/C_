#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <stdlib.h>

int CMD_LEN = 128;

void pid_info () {
    int pid = getpid();
    char str_pid[16];
    sprintf(str_pid, "PID: %d\n", pid);
    write(STDOUT_FILENO, str_pid, sizeof(str_pid));
    return;
}

void cmd_info () {
    int cmdline_fd = open("/proc/self/cmdline", O_RDONLY, 0666);
    if (cmdline_fd < 0) {
        perror("open");
        _exit(1);
    }
    char buf[CMD_LEN];
    size_t read_bytes = read(cmdline_fd, buf, CMD_LEN);
    int i = 0;
    while (i < read_bytes-1) {
        if (buf[i] == '\0') {
            buf[i] = ' ';
        }
        i++;
    }
    buf[i] = '\n';
    write(STDOUT_FILENO, "Cmdline: ", 9);
    write(STDOUT_FILENO, buf, read_bytes+1);
    close(cmdline_fd);
    return;
}

void find_vm()
void mem_info() {
    int mem_fd = open("/proc/self/status", O_RDONLY, 0666);
    if (mem_fd < 0) {
        perror("open");
        _exit(1);
    }
    char buf[4096];
    size_t read_bytes;
    read(mem_fd, buf, 4096);
    char *rss = strstr(buf, "VmRSS:");
    int i = atoi(rss);
    int k = i;
    while (1)
    {
        if (rss[i] == '\n') {
            break;
        }
        i++;
    }
    
    write(STDOUT_FILENO, rss, i-k+1);
    return;
}


int main() {
    pid_info();
    cmd_info();
    mem_info();
    return 0;
}