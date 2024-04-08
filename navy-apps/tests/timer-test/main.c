#include <stdio.h>
#include <sys/time.h>
#include <NDL.h>

void delay(int seconds) {
    struct timeval start, current;
    gettimeofday(&start, NULL);
    
    do {
        gettimeofday(&current, NULL);
    } while ((current.tv_sec - start.tv_sec) < seconds);
}

int main() {
    int ms = 0;
    while (1) {
        int total_us = NDL_GetTicks();

        printf("current us : %d\n", total_us);

        delay(1);
    }

    return 0;
}
