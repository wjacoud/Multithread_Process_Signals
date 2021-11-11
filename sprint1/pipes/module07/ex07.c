#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <time.h>
#define LEITURA 0
#define ESCRITA 1
#define ARRAY_SIZE 100
#define NUM_ELEMENTS 20
#define NUM_CHILDREN 5
int main(int argc, char *argv[])
{
    int vec1[ARRAY_SIZE];
    int vec2[ARRAY_SIZE];
    int array_final[ARRAY_SIZE];
    int tmp;
    int i, j;
    int estado = 0;
    int semi_result = 0;
    pid_t pid;
    int fd[5][2];
  //  // time_t t; //for srand
    // ///* intializes RNG (srand():stdlib.h; time(): time.h) */
    // srand((unsigned)time(&t));
    // /* initialize array with random numbers (rand(): stdlib.h) */
    // for (i = 0; i < ARRAY_SIZE; i++)
    // {
    //     vec1[i] = rand() % 4;
    // }
    // for (i = 0; i < ARRAY_SIZE; i++)
    // {
    //     vec2[i] = rand() % 4;
    // }
    for (j = 0; j < ARRAY_SIZE; ++j)
    {
        vec1[j] = 1;
        vec2[j] = j;
    }
    //cria o pipe
    for (i = 0; i < 5; i++) //corre isto 5 vezes
    {
        if (pipe(fd[i]) == -1) //cria o pipe 0.funcoes ou 1.funcoes ou 2.funcoes ou 3.funcoes ou 4,funcoes
        {
            printf("An error ocurred with opening the pipe\n");
            return 1;
        }
        pid = fork(); //cria 5 filhos /pois esta dentro do ciclo for
        if (pid == -1)
        {
            printf("An error ocurred with fork\n");
            return 2;
        }
        if (pid == 0) //FILHO
        {
            //o filho vai escrever para o pai apenas
            close(fd[i][LEITURA]);

            int INICIO = i * NUM_ELEMENTS;
            int FIM = INICIO + NUM_ELEMENTS;

            for (j = INICIO; j < FIM; j++)
            {
                semi_result = vec1[j] + vec2[j];
                if (write(fd[i][ESCRITA], &semi_result, sizeof(int)) == -1)
                {
                    printf("An error ocurred with writing to the pipe\n");
                    return 3;
                }
            }
            //o filho já não precisa de enviar mais nada para o pai
            close(fd[i][ESCRITA]);
            exit(1);
        }
        if (pid > 0)
        {
            close(fd[i][ESCRITA]); //o pai apenas vai ler o que os filhos lhe enviaram e calcular o resultado
            //espera por todos os filhos , pois espera por qualquer um dos filhos, 5x
            if (wait(&estado) > 0 && WIFEXITED(estado))
            {
                int INICIO = i * NUM_ELEMENTS;
                int FIM = INICIO + NUM_ELEMENTS;
                for (j = INICIO; j < FIM; j++)
                {
                    if (read(fd[i][LEITURA], &tmp, sizeof(int)) == -1)
                    {
                        printf("An error ocurred with reading to the pipe\n");
                        return 3;
                    }
                    array_final[i] = tmp;
                    printf("%d\n", array_final[i]);
                }
                printf("----------------------------------------\n");
            }
        }
        close(fd[i][LEITURA]);
    }

    return 0;
}