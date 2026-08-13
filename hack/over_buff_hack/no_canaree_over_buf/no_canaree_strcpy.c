#include <stdio.h>
#include <unistd.h>
#include <string.h>


void hack() {
    printf("You are hacked!");
}


void copy_str(char *str) {
    char buf[64];
    printf("buf:  %p\n", buf);
    strcpy(buf, str);
    printf("massive over\n");
}

int main(int argc, char *argv[]) {
    printf("hack: %p\n", hack);
    copy_str(argv[1]);
    return 0;
}
