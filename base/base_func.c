//#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>


void my_strcpy(char *dest, const char *src) {
    while (*src != 0)
    {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = 0;
}

int my_strlen(const char *s) {
    int i = 0;
    while (s[i] != 0)
    {
        i++;
    }
    return i;
    
}

void int_to_str(int num) {
    char buf[16];
    char *ptr = buf + sizeof(buf) - 1; //15-й элемент массива
    while (num > 0)
    {
        ptr--; //передвигаем указатель налево на 1 эл-т
        *ptr = (num % 10) + '0'; //кладем туда последнюю цифру из num c преобразованием ASCII (в char)
        num /= 10; //отбрасываем эту цифру из num
    }
    write(1, ptr, sizeof(buf) - (ptr-buf));
}

void my_malloc(int count) {
    void *before = sbrk(0);
    printf("Current break: %p \n", before); 
    int need = count * sizeof(int);
    void *ptr = sbrk(need);
    printf("Allocated at: %p \n", ptr);
    /*Приведение к int:
    ptr объявлен как void* — "сырой указатель", может указывать на что угодно, компилятор не знает, как интерпретировать данные по этому адресу.
    (int*) говорит компилятору: "считай, что по этому адресу лежит массив/переменная типа int
    */
    int *arr = (int*)ptr;
    int i = 0;
    int c = 0;
    int sum = 0;
    while (c < count)
    {
        arr[i] = c;
        c++;
        i++;
    }
    for (int j = 0; j < count; j++)
    {
        sum += arr[j];
    }
    printf("%d\n", sum);
    brk(before);
}

void my_mmap(int count) {
    int bytes = count * 4;
    void *ptr = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 8192);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        return;
    }
    
    int *arr = (int*)ptr;
    int i = 0;
    int c = 0;
    int sum = 0;
    while (c < count)
    {
        arr[i] = c;
        sum+=c;
        c++;
        i++;
    }
    printf("%d\n", sum);
    munmap(ptr, bytes);
}

void my_read(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        {
            write(STDERR_FILENO, "Open file error!\n", 17);
            return;
        }
    void *buf = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (buf == MAP_FAILED)
    {
        write(STDERR_FILENO, "Mapping error!\n", 15);
        return;
    }
    size_t read_bytes;
    while ((read_bytes = read(fd, buf, 1024)) > 0)
    {
        if (read_bytes < 0)
        {
            write(STDERR_FILENO, "Read file error!\n", 18);
            return;
        }
        
        write(STDOUT_FILENO, buf, read_bytes);
    }
    close(fd);
    munmap(buf, 4096);
}
