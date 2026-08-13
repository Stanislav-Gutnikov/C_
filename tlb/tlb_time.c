#include <unistd.h>
#include <stdio.h>
#include <time.h>


#define ARRAY_SIZE 10000
#define STRIDE_1 1
#define STRIDE_2 128


void tlb_test() {
    struct timespec start, end;
    int arr[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE-1; i++)
    {
        arr[i] = i;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);


    
}


int main() {
    return 0;
}