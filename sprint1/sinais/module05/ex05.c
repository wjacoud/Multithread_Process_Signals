#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void handle_SIG(int signo, siginfo_t *sinfo, void *context)
{
    int s = sinfo->si_signo;
    if (s == 2)
    {
        write(STDOUT_FILENO, " I won’t let the process end with CTRL-C!\n", 44); //b)
    }
    if (s == 3)
    {
        write(STDOUT_FILENO, " I won’t let the process end by pressing CTRL-\\!\n", 52); //d) if + este write
    }
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask); /* No signals blocked */
    act.sa_sigaction = handle_SIG;
    act.sa_flags = SA_SIGINFO;
    
    sigaction(2, &act, NULL); //b)
    sigaction(3, &act, NULL); //d)

    for (;;)
    {
        printf("I Love SCOMP! \n");
        sleep(5);
    }

    return 0;
}

//a) Quando pressionadas as teclas Ctrl+c no teclado a execução do programa é interrompida.
//Isso acontece pois o processo recebe o sinal SIGINT, que por defeito interrompe o processo em execução.

//e) tratou da mesma forma, capturou os sinais e indicou que não os deixariam ser "efetuados"

//f)Usamos o comando kill mas o processo ,manteve-se na lista
// ao usar o comando kill %1 significa que estamos a "matar" o job 1, que neste caso, era o processo que estaria interrompido
//Este comando funciona no terminal onde foi executado o processo
//%1 refere-se ao primeiro jobs na lista devolvida pelo comando "jobs"

//g) printf não é uma função async safe. Funções async safe significa que suportam a forma assíncrona. Como o envio de sinais pode acontecer
//a qualquer altura, o programa tem que ser capaz de lidar com todos os que possa receber e esteja configurado para tratar, a qualquer altura.
//Dessa forma, se for utilizado o printf, como é uma função reentrante (que re-entra), pode acontecer de o sinal que fora enviado para o programa, 
//não ser caputrado e tratado corretamente, prejudicando assim o efeito do tratamento do sinal.
//Sinais são asíncronos e funções como printf não são aptas para assincronismo.