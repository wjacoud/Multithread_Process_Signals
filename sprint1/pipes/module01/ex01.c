#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    int fd[2];
    //fd[0] - read | fd[1] - write;
    if (pipe(fd) == -1) {
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }
    int p = fork();
    if (p == -1) {
        printf("An error ocurred with fork\n");
        return 2;
    }
    if (p > 0) {
        close(fd[0]);
        int x = getpid();
        printf("I am the father, and my pid is %d\n", x);
        if (write(fd[1], &x, sizeof(int)) == -1) {
            printf("An error ocurred with writing to the pipe\n");
            return 3;
        }
        close(fd[1]);
    } else {
        close(fd[1]);
        int y;
        if (read(fd[0], &y, sizeof(int)) == -1) {
            printf("An error ocurred with reading from the pipe\n");
            return 4;
        }
        close(fd[0]);
        printf("I am the child, and my father's pid is %d\n", y);
        exit(1);
    }
}
