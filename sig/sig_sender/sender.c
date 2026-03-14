#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>

int main(int argc, char *argv[]) { //Enter ./sender <reciver PID> <SIGNAL>
    if (argc != 3) {
        write(STDERR_FILENO, "Must be 2 args!\n", 16);
        _exit(1);
    }
    
    int pid = atoi(argv[1]);
    int sig = atoi(argv[2]);
    int send = kill(pid, sig);
    if (send == 0) {
        write(STDOUT_FILENO, "Signal sent.\n", 13);
    }
    else {
        perror("Kill error");
        _exit(1);
    }

    return 0;
}