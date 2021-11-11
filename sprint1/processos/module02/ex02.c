#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void) {
    printf("I'm..\n");
    pid_t p1 = fork();
    if (p1 == 0){
        printf("I'll never join you!\n");
    } else {
        wait(NULL);
        printf("the..\n");
        p1 = fork();
        if (p1 == 0){
            printf("I'll never join you!\n");
        } else {
            wait(NULL);
            printf("father..\n");
            p1 = fork();
            if (p1 == 0){
                printf("I'll never join you!\n");
            }
        }
    }
    exit(EXIT_SUCCESS);
}
