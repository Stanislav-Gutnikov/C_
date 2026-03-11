#include <unistd.h>
#include <sys/wait.h>


int my_strlen(char *s) {
    int i = 0;
    while (s[i] != 0)
    {
        i++;
    }
    return i;
}

void error_msg(char *msg) {
    int len = my_strlen(msg); 
    write(STDERR_FILENO, msg, len);
    return;
}

void int_to_str(int num) {
    if (num == 0) {
        write(1, "0", 1);
        return;
    }
    char buf[16];
    char *ptr = buf + sizeof(buf) - 1; //15-й элемент массива
    while (num > 0)
    {
        ptr--; //передвигаем указатель налево на 1 эл-т
        *ptr = (num % 10) + '0'; //кладем туда последнюю цифру из num c преобразованием ASCII (в char)
        num /= 10; //отбрасываем эту цифру из num
    }
    write(1, ptr, sizeof(buf) - (ptr-buf));
    return;
}

void my_fork() {
    int ppid = getpid();
    error_msg("This is parent! Pid: ");
    int_to_str(ppid);
    error_msg("\n");
    pid_t pid = fork();
    if (pid<0) {
        error_msg("Fork failed!\n");
        return;
    } else if (pid == 0) {
        int cpid = getpid();
        error_msg("This is child! Pid: ");
        int_to_str(cpid);
        error_msg("\n");
        execl("./copy_file", "copy_file", "file.txt", "file1.txt", NULL);
        _exit(0);
    }
    int status;
    wait(&status);
    if (WIFEXITED(status)) {
        int wexit = WEXITSTATUS(status);
        error_msg("Ребенок завершился с кодом: ");
        int_to_str(wexit);
        error_msg("\n");
    } else if (WIFSIGNALED(status)){
        int wterm = WTERMSIG(status);
        error_msg("Ребенок завершился с кодом: ");
        int_to_str(wterm);
        error_msg("\n");
    }
    return;
}

int main () {
    my_fork();
    return 0;
}
