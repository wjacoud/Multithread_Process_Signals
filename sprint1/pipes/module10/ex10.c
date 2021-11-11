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

#define NUM_CHILDREN 10

#define MAX_STRING_SIZE 100

int main(int argc, char *argv[])
{
    pid_t p;
    int fd1[2], fd2[2], credit = 20, makeBet = 1, apostaFilho, valorSaida;

    //criar primeiro pipe (pai escreve, filho lê)
    pipe(fd1);
    if (pipe(fd1) == -1)
    {
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }

    //criar segundo pipe (filho escreve, pai lê)
    pipe(fd2);
    if (pipe(fd2) == -1)
    {
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }

    //cria processo filho
    p = fork();
    if (p == -1)
    {
        printf("An error ocurred with fork\n");
        return 2;
    }

    if (p == 0) //PROCESSO FILHO
    {
        //assegura que os numeros gerados aleatoriiamente são sempre diferentes todas as vezes que corre o programa
        srand((unsigned)time(NULL) * getpid());
        //FD1 - PAI ESCREVE FILHO LÊ, ENTÃO FILHO NÃO ESCREVE NESTE PIPE
        close(fd1[ESCRITA]);
        //FD2 - FILHO ESCREVE E PAI LÊ, ENTÃO FILHO NÃO LÊ ESTE PIPE
        close(fd2[LEITURA]);

        //enquanto o crédito não acabar, o filho vai tentar jogar
        while (credit > 0)
        {
            //vai saber através do pai se tem crédito suficiente para fazer uma aposta
            if (read(fd1[LEITURA], &makeBet, sizeof(int)) == -1)
            {
                printf("An error ocurred with reading to the pipe\n");
                return EXIT_FAILURE;
            }
            

            // se makeBet=1 então o filho pode continuar a jogar
            if (makeBet == 1)
            {
                apostaFilho = 1 + rand() % 5; //gera número aleatório de 1 a 5

                //escreve a aposta e envia para o pai

                if (write(fd2[ESCRITA], &apostaFilho, sizeof(int)) == -1) 
                {
                    printf("An error ocurred with writing to the pipe\n");
                    return EXIT_FAILURE;
                }

                
                //lê o valor de créditos do pai (o pai avalia se o filho ganhou, se sim aumenta creditos, se não diminui)
                if (read(fd1[LEITURA], &credit, sizeof(int)) == -1)
                {
                    printf("An error ocurred with reading to the pipe\n");
                    return EXIT_FAILURE;
                }

                //filho passa a saber o valor que tem para continuar a jogar
                printf("Credito atual: %d\n\n", credit);
            }
            else
            {
                //Se chega aqui, makeBet = 0, significa que não tem mais creditos para jogar, terminando o jogo

                //FD1 - PAI ESCREVE FILHO LÊ, Filho já não precisa de ler mais nada do pai
                close(fd1[LEITURA]);
                //FD2 - FILHO ESCREVE E PAI LÊ, filho já não precisa de escrever mais para o pai
                close(fd2[ESCRITA]);
                exit(0); //Processo filho termina.
            }
        }
    }
    else //PROCESSO PAI
    {

        //FD1 - PAI ESCREVE FILHO LÊ, ENTÃO PAI NÃO LÊ NESTE PIPE
        close(fd1[LEITURA]);
        //FD2 - FILHO ESCREVE E PAI LÊ, ENTÃO PAI NÃO ESCREVE NESTE PIPE
        close(fd2[ESCRITA]);

        //enquanto houver credito há jogo
        while (credit > 0)
        {
            //
            valorSaida = 1 + rand() % 5; //gera número aleatório de 1 a 5 

            if (credit > 0)
            {
                makeBet = 1; 
            }
            else
            {
                makeBet = 0;
            }

            //escreve para com o filho a dizer se pode apostar
            if (write(fd1[ESCRITA], &makeBet, sizeof(int)) == -1)
            {
                printf("An error ocurred with writing to the pipe\n");
                return EXIT_FAILURE;
            }

            //Se chegar aqui, o filho não terminou, então pôde apostar, então o pai tem que ler a aposta que o filho fez
            if (read(fd2[LEITURA], &apostaFilho, sizeof(int)) == -1)
            {
                printf("An error ocurred with reading to the pipe\n");
                return EXIT_FAILURE;
            }

            printf("Aposta Filho: %d | Resultado Pai: %d\n", apostaFilho, valorSaida);
           
           //Se coincidir incrementa 10 no credito, se for diferente decrementa 5 no crédito
            if (apostaFilho == valorSaida)
            {
                credit = credit + 10; 
            }
            else
            {
                credit = credit - 5; 
            }


            //escreve o credito que existe e envia para o filho, para que o filho possa imprimir o crédito que tem
            if (write(fd1[ESCRITA], &credit, sizeof(int)) == -1) 
            {
                printf("An error ocurred with writing to the pipe\n");
                return EXIT_FAILURE;
            }

        }
        
        close(fd1[ESCRITA]);
       
        close(fd2[LEITURA]);
    }
    return 0;
}