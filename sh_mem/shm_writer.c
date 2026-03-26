#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>


void shm_write() {
    int fd = shm_open("/mycounter", O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open");
        _exit(1);
    }
    if (ftruncate(fd, sizeof(int)) < 0) {
        perror("ftruncate");
        _exit(1);
    }
    int *counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (counter == MAP_FAILED) {
        perror("mmap");
        _exit(1);
    }
    *counter = 0;
    while ((*counter) < 10) {
        sleep(1);
        (*counter)++;
        printf("Current num: %d\n", *counter);
    }
    shm_unlink("/mycounter");
    munmap(counter,  sizeof(int));
    return;
}

int main() {
    shm_write();
    return 0;
}