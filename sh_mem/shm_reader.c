#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>


void shm_read() {
    int fd = -1;
    while (fd < 0) {
        sleep(1);
        fd = shm_open("/mycounter", O_RDONLY, 0666);
        printf("Waiting...\n");
    }
    int *counter = mmap(NULL, sizeof(int), PROT_READ, MAP_SHARED, fd, 0);
    if (counter == MAP_FAILED) {
        perror("mmap");
        _exit(1);
    }
    close(fd);
    int i = 0;
    while (i < 10)
    {
        sleep(1);
        printf("Shmem num: %d\n", *counter);
        i++;
    }
    munmap(counter,  sizeof(int));
    return;
}

int main() {
    shm_read();
    return 0;
}