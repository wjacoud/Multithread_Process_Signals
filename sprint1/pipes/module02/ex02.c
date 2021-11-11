#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define MAX_STRING_SIZE 100

struct estrutura
{
    int integer;
    char string[MAX_STRING_SIZE];
};

int main(int argc, char *argv[])
{
    int num;
    char str[MAX_STRING_SIZE];
    int num1;
    char str1[MAX_STRING_SIZE];
    int fd[2];

    struct estrutura input;
    struct estrutura output;

    //fd[0] - read | fd[1] - write;
    if (pipe(fd) == -1)
    {
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }
    int p = fork();
    if (p == -1)
    {
        printf("An error ocurred with fork\n");
        return 2;
    }
    if (p > 0)
    {
        close(fd[0]);

        printf("Pai: por favor insira um número: \n");
        scanf("%d", &num);
        printf("Pai: por favor insira uma string: \n");
        scanf("%s", str);

        // if (write(fd[1], &num, sizeof(num)) == -1)
        // {
        //     printf("An error ocurred with writing to the pipe\n");
        //     return 3;
        // }
        // if (write(fd[1], str, sizeof(str)) == -1)
        // {
        //     printf("An error ocurred with writing to the pipe\n");
        //     return 3;
        // }

        input.integer = num;
        strcpy(input.string, str);
        if (write(fd[1], &input, sizeof(input)) == -1)
        {
            printf("An error ocurred with writing to the pipe\n");
            return 3;
        }

        close(fd[1]);
    }
    else
    {
        close(fd[1]);
        // if (read(fd[0], &num1, sizeof(num1)) == -1)
        // {
        //     printf("An error ocurred with reading from the pipe\n");
        //     return 4;
        // }

        // if (read(fd[0], str1, sizeof(str1)) == -1)
        // {
        //     printf("An error ocurred with reading from the pipe\n");
        //     return 4;
        // }

        if (read(fd[0], &output, sizeof(output)) == -1)
        {
            printf("An error ocurred with reading from the pipe\n");
            return 4;
        }

        close(fd[0]);

        printf("Filho: O meu pai recebeu o inteiro %d e a string %s \n", output.integer, output.string);

        // printf("Filho: O meu pai recebeu o inteiro %d e a string %s\n", num1, str1);
        exit(1);
    }

    return 0;
}
