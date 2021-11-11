#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>


int main(void) {

    for(;;){
        printf("I Love SCOMP! \n");
        sleep(1);
    }



}

//b)
//Usou-se o comando ps -ef para saber quais os processos em execução
//Seguido de grep ex02 para descobrirmos qual seria o PID do processo do executavel
//De seguida fizemos Kill -SIGSTOP (19) + PID para parar o processo
//c)
//Para continuar utilizamos o kill -18 (SIGCONT) PID
//d)
//kill -9 PID para matar o processo


