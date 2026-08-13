#include <stdio.h>
#include <stdlib.h>


int main() {
    char buf[64];
    printf("buf:  %p\n", buf);
    gets(buf);
    return 0;
}
