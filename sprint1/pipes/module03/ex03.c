#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_STRING_SIZE 100
#define LEITURA 0
#define ESCRITA 1

int main(int argc, char *argv[])
{
    int fd[2];
    char str1[] = "Hello World";
    char str2[] = "Goodbye!";

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
        close(fd[LEITURA]);

        if (write(fd[ESCRITA], str1, sizeof(str1)) == -1)
        {
            printf("An error ocurred with writing to the pipe\n");
            return 3;
        }

        if (write(fd[ESCRITA], str2, sizeof(str2)) == -1)
        {
            printf("An error ocurred with writing to the pipe\n");
            return 3;
        }

        close(fd[ESCRITA]);
    }
    else
    {
        close(fd[ESCRITA]);

        if (read(fd[LEITURA], str1, sizeof(str1)) == -1)
        {
            printf("An error ocurred with reading from the pipe\n");
            return 4;
        }

        if (read(fd[LEITURA], str2, sizeof(str2)) == -1)
        {
            printf("An error ocurred with reading from the pipe\n");
            return 4;
        }

        close(fd[LEITURA]);

        printf("Filho: %s e %s \n", str1, str2);

        exit(1);
    }

    //Espera do pai
    int estado = 0;
    if (wait(&estado) > 0 && WIFEXITED(estado))
    {
        printf("Pai: o pid do meu filho é: %d e ele terminou com o valor %d\n", p, WEXITSTATUS(estado));
    }

    return 0;
}
