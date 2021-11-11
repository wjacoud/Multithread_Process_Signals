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

#define LEITURA 0
#define ESCRITA 1
#define MAX_STRING_SIZE 100000

void clientSide(char *str) //client recebe a mensagem a alterar
{
    printf("Inserir Mensagem:\n");
    scanf("%s", str);
}

void serverSide(char *string) //processa a mensagem a processar
{
    while (*string)
    {
        if (isupper(*string))
        {
            *string = tolower(*string);
        }
        else if (islower(*string))
        {
            *string = toupper(*string);
        }
        string++;
    }
}

int main(int argc, char *argv[])
{

    int up[2];
    int down[2];
    char strPorTratar[MAX_STRING_SIZE];
    char stdin[MAX_STRING_SIZE];

    if (pipe(up) == -1)
    {
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }
    if (pipe(down) == -1)
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

    if (p > 0) //PAI
    {
        close(up[ESCRITA]); //o pai só envia (escreve) para o filho pelo down
        close(down[LEITURA]); //o pai só lê do filho (recebe o que o filho escreveu) pelo up

        //o pai vai ler o que o filho lhe enviou pelo UP, que será a string por tratar (ainda não convertida)
        if (read(up[LEITURA], strPorTratar, sizeof(strPorTratar)) == -1)
        {
            printf("An error ocurred with reading to the pipe\n");
            return 3;
        }

        //o pai já não precisa de ler do filho mais nada
        close(up[LEITURA]);
        //converte a string
        serverSide(strPorTratar);
        //escreve o resultado da string, guardado na strPorTratar , no pipe down, de forma a enviar a string tratada para o filho
        if (write(down[ESCRITA], strPorTratar, sizeof(strPorTratar)) == -1)
        {
            printf("An error ocurred with writing to the pipe\n");
            return 3;
        }
        //o pai já não precisa de enviar mais nada para o filho
        close(down[ESCRITA]);
    }
    else //FILHO
    {
        //o filho só lê do pai, que vem atraves do down
        close(up[LEITURA]);
        //o filho só escreve para o pai, atraves do up
        close(down[ESCRITA]);

        //o filho precisa de saber qual a string introduzida pelo user (input) que vai enviar para o pai
        clientSide(stdin);

        // o filho escreve no pipe up, para enviar para o pai, a string que quer tratar
        if (write(up[ESCRITA], stdin, sizeof(stdin)) == -1)
        {
            printf("An error ocurred with writing to the pipe\n");
            return 3;
        }
        //o filho já não precisa de enviar mais nada para o pai
        close(up[ESCRITA]);

        //o filho lê o resultado (a string tratada pelo pai) atraves do pipe down
        if (read(down[LEITURA], stdin, sizeof(stdin)) == -1)
        {
            printf("An error ocurred with reading to the pipe\n");
            return 3;
        }
        //o filho ja nao precisa de ler mais nada vindo do pai
        close(down[LEITURA]);
        //imprime o resultado
        printf("%s\n", stdin);

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
