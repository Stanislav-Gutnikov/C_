#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>


const char EXIT_MSG[] = "Timeout!\n";
struct sigaction sa_alarm;


void alarm_handler() {
    write(STDOUT_FILENO, EXIT_MSG, sizeof(EXIT_MSG)-1);
    _exit(0);
}

int main() {
    void *buf = mmap(NULL, 256, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (buf == MAP_FAILED) {
        write(STDERR_FILENO, "Mapping error!\n", 14);
        _exit(1);
    }
    sa_alarm.sa_handler = alarm_handler;
    sigemptyset(&sa_alarm.sa_mask);
    sa_alarm.sa_flags = 0;
    sigaction(SIGALRM, &sa_alarm, NULL);
    while (1) {
        alarm(5);
        size_t read_bytes = read(STDIN_FILENO, buf, 256);
        if (read_bytes < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                write(STDERR_FILENO, "Read error!\n", 12);
                _exit(1);
            }
        }
    }
    return 0;
}