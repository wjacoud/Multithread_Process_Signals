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

struct estrutura
{
    int number;
    char string[MAX_STRING_SIZE];
};

int main(int argc, char *argv[])
{

    int p[NUM_CHILDREN];
    int estado = 0;
    int fd[2];
    int i = 0;
    struct estrutura input_pai; //estrutura onde o pai vai escrever
    struct estrutura vencedora_filho; //estrutua onde o filh vai guardar a informacao vinda do pipe

   

    if (pipe(fd) == -1)
    {
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }

    for (i = 0; i < NUM_CHILDREN; i++) //percorre as rondas do jogo
    {
        p[i] = fork();  //cria 10 filhos e guarda o PID dos filhos no vetor p

        if (p[i] == -1)
        {
            printf("An error ocurred with fork\n");
            return 2;
        }

        if (p[i] > 0) //PAI
        {

            sleep(2);   //dorme 2 segundos

            input_pai.number = i + 1; //atribui o valor da ronda à estrutura
            strcpy(input_pai.string, "Win");    //copia o "WIN" para a estrutura a ser enviada para o pipe

            if (write(fd[ESCRITA], &input_pai, sizeof(input_pai)) == -1) //Se conseguir escrever na estrutura, escreve
            {
                printf("An error ocurred with writing to the pipe\n");
                return 3;
            }

            //NÃO SE FECHOU A ESCRITA NO PIPE PORQUE COMO ESTAMOS NO CICLO FOR, VAMOS 
            //PRECISAR QUE O PAI ESCREVA NOVAMENTE ATRAVÉS DO PIPE PARA AS RONDAS SEGUINTES
        }

        else if (p[i] == 0) //FILHO
        {
            //o filho lê os dados enviado pelo pai, atraves do pipe
            if (read(fd[LEITURA], &vencedora_filho, sizeof(vencedora_filho)) == -1)
            {
                printf("An error ocurred with reading to the pipe\n");
                return 3;
            }
            //O filho diz que ganhou aquela ronda 
            printf("Filho: I %s round %d\n", vencedora_filho.string, vencedora_filho.number);
            //printf("my pid is %d\n", getpid());   //comentado apenas paraver se bate certo
            
            
            if (i == NUM_CHILDREN - 1)
            {
                //Quando chegarmos a 10x que estamos a percorrer o for, ou seja a i=9 pois for é até i<10
                //como o pai nao precisa de escrever mais nada para o pipe, nem o filho vai precisar de ler mais nada
                //antes de dar saida do processo filho, vamos fechar o pipe
                close(fd[LEITURA]);
                close(fd[ESCRITA]);
            }
           
           
            //retorna o valor da ronda que venceu (que recebeu esse mesmo valor pelo pipe)
            exit(vencedora_filho.number);
        }
    }

    for (i = 0; i < NUM_CHILDREN; i++)
    {
        //o pai espera que os 10 filhos terminem e indica quemm ganhou qual ronda
        if (wait(&estado) > 0 && WIFEXITED(estado))
        {
            printf("Pai: o pid do meu filho é: %d e ele ganhou a ronda %d\n", p[i], WEXITSTATUS(estado));
        }
    }

    return 0;
}
