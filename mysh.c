#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>


void write_msg(char *msg) {
    int len = strlen(msg); 
    write(STDERR_FILENO, msg, len);
    return;
}

void start_sh() {
    write_msg("Starting shell...\n");
    char buf[1024];
    write_msg("$ ");
    size_t read_bytes;
    const char *exit_command = "exit\n";
    while ((read_bytes = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        buf[read_bytes-1] = '\0';
        if (strcmp(buf, exit_command)==0) {
            write_msg("Bye...\n");
            return;
        }
        char *argv[100];
        int i = 0;
        int k = 1;
        argv[i] = &buf[0];
        while (i < strlen(buf))
        {
            if (buf[i]==' ') {
                buf[i] = '\0';
                if (buf[i+1] != '\0') {
                    argv[k] = &buf[i+1];
                    k++;
                }
            }
            i++;
        }
        argv[k]=NULL;
        pid_t pid = fork();
        if (pid < 0) {
            write_msg("Fork failed!\n");
            _exit(1);
        }
        else if (pid == 0) {
            execvp(argv[0], argv);
            write_msg("Command not found\n");
            _exit(1);
        }
        int status;
        wait(&status);
        write_msg("$ ");
    }
    return;
}

int main() {
    start_sh();
    return 0;
}
