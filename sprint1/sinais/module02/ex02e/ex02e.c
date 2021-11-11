#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>



int main(void) {

    pid_t p;
    int s;

    printf("Please give me the PID: \n");
    scanf("%i",&p);
    

    printf("Please tell me what signal do you want to send? \n");
    scanf("%i",&s);

    kill(p,s);


    return 0;


}

//b)
//Usou-se o comando ps -ef para saber quais os processos em execução
//Seguido de grep ex02 para descobrirmos qual seria o PID do processo do executavel
//De seguida fizemos Kill -SIGSTOP (19) + PID para parar o processo
//c)
//Para continuar utilizamos o kill -18 (SIGCONT) PID
//d)
//kill -9 PID para matar o processo


