#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>


const char sigterm_msg[] = "Terminated\n";
const char sigint_msg[] = "SIGINT ignored!\n";
const char sigusr1_msg[] = "Received SIGUSR1\n";
const char sigusr2_msg[] = "Received SIGUSR2\n";
const char reciever_msg[] = "Reciever working...\n";


struct sigaction sa_sigterm;
struct sigaction sa_sigint;
struct sigaction sa_sigusr1;
struct sigaction sa_sigusr2;


void sigterm() {
    write(STDERR_FILENO, sigterm_msg, sizeof(sigterm_msg)-1);
    _exit(1);
    return;
}

void sigint() {
    write(STDERR_FILENO, sigint_msg, sizeof(sigint_msg)-1);
    return;
}

void sigusr1() {
    write(STDOUT_FILENO, sigusr1_msg, sizeof(sigusr1_msg)-1);
    return;
}

void sigusr2() {
    write(STDOUT_FILENO, sigusr2_msg, sizeof(sigusr2_msg)-1);
    return;
}

void sa_attrs_conf(struct sigaction *sa, int sigs[], int count) {
    sigemptyset(&sa->sa_mask);
    sa->sa_flags = 0;
    for (int i = 0; i < count; i++)
    {
        sigaddset(&sa->sa_mask, sigs[i]);
    }
    return;
}

void init_sa() {
    int term_mask[] = {SIGUSR1, SIGUSR2, SIGINT};
    sa_sigterm.sa_handler = sigterm;
    sa_attrs_conf(&sa_sigterm, term_mask, 3);
    sigaction(SIGTERM, &sa_sigterm, NULL);

    int int_mask[] = {SIGUSR1, SIGUSR2};
    sa_sigint.sa_handler = sigint;
    sa_attrs_conf(&sa_sigint, int_mask, 2);
    sigaction(SIGINT, &sa_sigint, NULL);

    int usr1_mask[] = {SIGUSR2};
    sa_sigusr1.sa_handler = sigusr1;
    sa_attrs_conf(&sa_sigusr1, usr1_mask, 1);
    sigaction(SIGUSR1, &sa_sigusr1, NULL);

    int usr2_mask[] = {SIGUSR1};
    sa_sigusr2.sa_handler = sigusr2;
    sa_attrs_conf(&sa_sigusr2, usr2_mask, 1);
    sigaction(SIGUSR2, &sa_sigusr2, NULL);

    return;
}

int main() {
    int pid = getpid();
    char str[20];
    int len = sprintf(str, "PID: %d\n", pid);
    write(STDOUT_FILENO, str, len);
    init_sa();
    while (1) {
        write(STDOUT_FILENO, reciever_msg, sizeof(reciever_msg)-1);
        sleep(10);
    }
    return 0;
}