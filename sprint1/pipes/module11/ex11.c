#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define ESCRITA 1
#define LEITURA 0
#define MAX_SIZE 1000
#define NUM_FILHOS 5
#define NUM_PIPES 6

int maiorNum(int num1, int num2)
{
    if (num1 >= num2)
    {
        return num1;
    }
    else
        return num2;
}

int main()
{

    pid_t p;
    int i, status, numPai, aleatorio, n, maior, num;
    int fd[NUM_PIPES][2];
    srand((unsigned)time(NULL) * getpid());

    /* criar pipes */
    for (i = 0; i < NUM_PIPES; i++)
    {
        if (pipe(fd[i]) == -1)
        {
            printf("An error ocurred with opening the pipe\n");
            return 1;
        }
    }

    //Pai gera numer aleatoroo
    numPai = rand() % 500 + 1;
    printf("PID: %d = %d\n", getpid(), numPai);

    //O pai manda para o primeiro pipe o valor aleatorio dele
    if (write(fd[0][ESCRITA], &numPai, sizeof(numPai)) == -1)
    {
        printf("An error ocurred with writing to the pipe\n");
        return EXIT_FAILURE;
    }
    //Já não se vai escrever mais nesse pipe (no primeiro)
    close(fd[0][ESCRITA]);

    for (i = 0; i < NUM_FILHOS; i++)
    {
        aleatorio = rand() % 500 + 1;
        p = fork();
        if (p == -1)
        {
            printf("An error ocurred with fork\n");
            return 2;
        }

        if (p == 0)
        { //Filho
            printf("Filho n. %d | PID: %d | Random Number: %d\n", i + 1, getpid(), aleatorio);

            //Vai ler o que veio do anterior
            if (read(fd[i][LEITURA], &n, sizeof(n)) == -1)
            {
                printf("An error ocurred with reading to the pipe\n");
                return EXIT_FAILURE;
            }
            //Já não vai ser preciso ler mais do pipe que antecede
            close(fd[i][LEITURA]);

            //compara os numeros
            num = maiorNum(aleatorio, n);
            //Vai escrever o número maior para o pipe seguinte
            if (write(fd[i + 1][ESCRITA], &num, sizeof(num)) == -1)
            {
                printf("An error ocurred with writing to the pipe\n");
                return EXIT_FAILURE;
            }
            //Já não precisa de escrever para o pipe seguinte
            close(fd[i + 1][ESCRITA]); /* fechar extremidade de leitura */
            exit(0);
        }
        if (p > 0)
        {
            wait(&status); //espera 5x para que todos os filhos terminem
                           //mesmo sem esta condicao, isto funciona corretamente
        }
    } //ciclo for termina, filhos ja terminaram a execucao

    //Aqui já é so o pai que executa

    if (read(fd[5][LEITURA], &maior, sizeof(maior)) == -1)
    {
        printf("An error ocurred with reading to the pipe\n");
        return EXIT_FAILURE;
    }

    close(fd[5][LEITURA]); /* fechar extremidade de leitura */

    printf("Maior num = %d\n", maior);

    return 0;
}