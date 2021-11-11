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

int make_children(int n)
{
    pid_t pid = 0;

    int i = 0;
    for (i = 0; i < n; ++i)
    {
        pid = fork();
        if (pid < 0)
        {
            return -1;
        }
        else if (pid == 0)
        {
            return i + 1; //se pid = 0 estamos dentro do filho, então a função retorna o nº de ordem de criação do filho, ou seja, 1º filho a ser criado, retorna 1, por aí fora.
        }
    }
    return 0; //se chega aqui, estamos no pai, e vai retornar 0.
}

int main(int argc, char *argv[])
{
    int vec1[ARRAY_SIZE];
    int vec2[ARRAY_SIZE];
    unsigned long long tmp;
    int i;

    int fd[2];

    time_t t; //for srand

    ///* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned)time(&t));

    /* initialize array with random numbers (rand(): stdlib.h) */

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        vec1[i] = rand() % 4;
    }
    for (i = 0; i < ARRAY_SIZE; i++)
    {
        vec2[i] = rand() % 4;
    }

    //cria o pipe

    if (pipe(fd) == -1)
    {
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }

    int p = make_children(NUM_CHILDREN);

    if (p == -1)
    {
        printf("An error ocurred with fork\n");
        return 2;
    }

    if (p == 0) //PAI
    {
        close(fd[ESCRITA]); //o pai apenas vai ler o que os filhos lhe enviaram e calcular o resultado
        int estado = 0;
        unsigned long long final = 0;
        for (i = 0; i < 5; i++)
        { //espera por todos os filhos , pois espera por qualquer um dos filhos, 5x
            if (wait(&estado) > 0 && WIFEXITED(estado))
            {
                if (read(fd[LEITURA], &tmp, sizeof(tmp)) == -1)
                {
                    printf("An error ocurred with writing to the pipe\n");
                    return 3;
                }
                final = final + tmp;
            }
        }
        close(fd[LEITURA]);
        printf("O resultado final é %llu\n", final);
    }
    if (p > 0) //FILHO
    {
        //o filho vai escrever para o pai apenas
        close(fd[LEITURA]);
        unsigned long long semi_result = 0;
        int INICIO = p * NUM_ELEMENTS;
        int FIM = INICIO + NUM_ELEMENTS;
        for (i = INICIO; i < FIM; i++)
        {
            tmp = vec1[i] + vec2[i];
            semi_result = semi_result + tmp;
        }

        if (write(fd[ESCRITA], &semi_result, sizeof(&semi_result)) == -1)
        {
            printf("An error ocurred with writing to the pipe\n");
            return 3;
        }
        //o filho já não precisa de enviar mais nada para o pai
        close(fd[ESCRITA]);

        exit(1);
    }

    return 0;
}
