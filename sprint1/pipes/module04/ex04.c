#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LEITURA 0
#define ESCRITA 1
#define MAX_STRING_SIZE 100000

int main(int argc, char *argv[])
{

    int fd[2];
    char *ficheiro = "test.txt";
    FILE *fp;
    char f[MAX_STRING_SIZE];
    int c, i=0;

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

        fp = fopen(ficheiro, "r");
        if (fp == NULL)
        {
            printf("Nao foi possivel abrir o ficheiro \n");
            exit(EXIT_FAILURE);
        }

        //Lê o ficheiro e guarda-o na variavel f
        while (1)
        {
          c = fgetc(fp);
          if(feof(fp)){
              break;
          }
          f[i]=c;
          i++;
        }

        fclose(fp);

          if (write(fd[ESCRITA], f,sizeof(f)+1) == -1)
            {
                printf("An error ocurred with writing to the pipe\n");
                return 3;
            }
        
        close(fd[ESCRITA]);
        
    }
    else
    {
        char result[MAX_STRING_SIZE];
        close(fd[ESCRITA]);

        if (read(fd[LEITURA], result, sizeof(result)) == -1)
        {
            printf("An error ocurred with reading from the pipe\n");
            return 4;
        }

        printf("%s \n", result);
        close(fd[LEITURA]);

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
