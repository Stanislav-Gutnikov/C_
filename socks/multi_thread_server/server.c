#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>


int thread_counter = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER; // инициализация мьютекса
int server_socket;
struct sigaction sa_sigint;



void check(int res, char *msg) {
    if (res < 0) {
        perror(msg);
        _exit(1);
    }
    return;
}


void print_connects (int counter) {
    char str[8];
    sprintf(str, "%d", counter);
    printf("Accepted connection. Current connects: %s\n", str);
    return;
}


void sigint(int sig) {
    close(server_socket);
    unlink("/tmp/echo_socket");
    char msg_sigint[] = "\nServer stopped. Bye!\n";
    write(STDERR_FILENO, msg_sigint, sizeof(msg_sigint)-1);
    _exit(0);
    return;
}

/*
Функция потока обязана иметь сигнатуру void* Name(void* arg).
Аргумент arg — это тот самый адрес, который мы передали в pthread_create.
*/
void *response(void *arg) {
    /*
    Шаг А: У нас есть "безымянный" адрес (void*). 
    Нам нужно сказать компилятору: "Там лежит целое число".
    Для этого приводим (void*) обратно к (int*).
    int *ptr = (int*)arg;
    Шаг Б: Теперь извлекаем само значение (разыменовываем).
    int c_socket = *ptr;
    Итого в одну строку: 
    */
    int c_socket = *(int*)arg;
    char buf[128];
    ssize_t r = recv(c_socket, buf, sizeof(buf)-1, 0);
    if (r < 0) {
        perror("recv");
        close(c_socket);
        pthread_mutex_lock(&m);
        thread_counter--;
        pthread_mutex_unlock(&m);
        free(arg);
        return NULL;
    }
    buf[r] = '\0';
    write(STDOUT_FILENO, "Msg from client: ", 17);
    write(STDOUT_FILENO, buf, r);
    ssize_t s = send(c_socket, "Hello from server!\n", 19, 0);
    if (s < 0) {
        perror("send");
    }
    close(c_socket);
    //меняем через мьютекс, чтобы другой поток не изменил пока меняем:
    write(STDOUT_FILENO, "Processing...\n", 14);

    sleep(5); // ИСКУССТВЕННАЯ ЗАДЕРЖКА!!! УБРАТЬ!
    
    pthread_mutex_lock(&m);
    thread_counter--;
    pthread_mutex_unlock(&m);
    free(arg);
    return NULL;
}

void server() {
    sa_sigint.sa_handler = sigint;
    sigemptyset(&sa_sigint.sa_mask);
    sa_sigint.sa_flags = 0;

    int s_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    check(s_socket, "socket");
    server_socket = s_socket;

    struct sockaddr_un s_sock_addr;
    s_sock_addr.sun_family = AF_UNIX;
    strcpy(s_sock_addr.sun_path, "/tmp/echo_socket");
    socklen_t s_addr_len = sizeof(s_sock_addr);

    struct sockaddr_un c_sock_addr;
    socklen_t c_sock_len = sizeof(c_sock_addr);

    unlink("/tmp/echo_socket");
    check(bind(s_socket, (struct sockaddr*)&s_sock_addr, s_addr_len), "bind");
    sigaction(SIGINT, &sa_sigint, NULL);
    write(STDOUT_FILENO, "Create server socket\n", 21);
    check(listen(s_socket, 10), "listen");
    write(STDOUT_FILENO, "Listening...\n", 13);

    while (1) {
        int c_socket = accept(s_socket, (struct sockaddr*)&s_sock_addr, &s_addr_len);
        pthread_mutex_lock(&m);
        int check_counter = thread_counter;
        pthread_mutex_unlock(&m);
        if (check_counter >= 10) {
            close(c_socket);
            write(STDOUT_FILENO, "Limit reached, closing...\n", 26);
            continue;
        }
        //меняем через мьютекс, чтобы другой поток не изменил пока меняем:
        pthread_mutex_lock(&m);
        thread_counter++;
        int current_counter = thread_counter;
        pthread_mutex_unlock(&m);

        print_connects(current_counter);
        /*
        У тебя есть переменная c_socket (это int). Тебе нужно передать её в поток.
        Если передать адрес локальной переменной (&c_socket), она может измениться на следующей итерации цикла accept. 
        Поэтому мы копируем её в отдельное место в памяти.
        */
        int *client_fd = malloc(sizeof(int));// 1. Выделяем память (адрес) под один int.
        check(client_fd == NULL ? -1 : 0, "malloc"); // if clientfd == NULL, то передаем -1, else передаем 0
        *client_fd = c_socket; // 2. Кладем значение сокета по этому адресу.
        pthread_t thread;
        // 3. Передаем этот адрес в поток.
        // pthread_create принимает void*, поэтому мы приводим (int*) к (void*).
        // По сути, мы просто говорим: "Вот какой-то адрес, поток сам разберется".
        check(pthread_create(&thread, NULL, response, (void*)client_fd), "pth_create");
        check(pthread_detach(thread), "pth_detach");

    }
    return ;
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    server();
    return 0;
}