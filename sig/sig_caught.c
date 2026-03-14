#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

const char msg_sigint[] = "Signal caught, but I won't die!\n";
const char msg_sigterm[] = "Received SIGTERM, exiting.\n"; 

struct sigaction sa_sigint; // Инициализация sigaction
struct sigaction sa_sigterm;

void sigterm() {
    write(STDERR_FILENO, msg_sigterm, sizeof(msg_sigterm)-1);
    _exit(1);
    return;
}

void sigint() {
    write(STDERR_FILENO, msg_sigint, sizeof(msg_sigint)-1);
    return;
}

int main() {
    int pid = getpid();
    char str[10];
    sprintf(str, "%d", pid);
    printf("PID: %s\n", str);
    char *msg = "Working...\n";
    size_t len = strlen(msg);

    sa_sigint.sa_handler = sigint; // Добавлям обработчик в структуру
    sigemptyset(&sa_sigint.sa_mask); // Очищаем маску (в ней нули)
    sigaddset(&sa_sigint.sa_mask, SIGTERM); // Блокируем SIGTERM на время работы обработчика (будет ждать завершения)
    sa_sigint.sa_flags = 0; // Обнуляем флаги
    sigaction(SIGINT, &sa_sigint, NULL); //Включаем обработчик
    
    sa_sigterm.sa_handler = sigterm;
    sigemptyset(&sa_sigterm.sa_mask);
    sigaddset(&sa_sigterm.sa_mask, SIGINT);
    sa_sigterm.sa_flags = 0;
    sigaction(SIGTERM, &sa_sigterm, NULL);

    while (1)
    {
        write(STDOUT_FILENO, msg, len);
        sleep(1);
    }
    return 0;
}
