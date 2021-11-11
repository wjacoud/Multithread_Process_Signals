#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
void handle_SIG(int signo, siginfo_t *sinfo, void *context)
{
    sigset_t mask; // Vai ser preenchido com o atual valor da mascara do struct sigaction
    sigprocmask(SIG_BLOCK, NULL, &mask); //como o segundo parametro esta a NULL, o primeiro parametro nao é significante.
                                        // Serve apenas para guardar no terceiro paramatro o set de sinais bloqueados anteriormente(main).
                                        // Maneira de aceder a mascara atual na função handler.

    int i;
    for(i = 1; i <= 64; i++) {
        if(sigismember(&mask, i)) { // O primeiro parametro é um set de sinais e o segundo o sinal a ser procurado.
            printf("Sinal %d bloqueado\n", i);
        }
    }
    //desta forma conseguimos saber os sinais bloqueados a quando a execução deste handler.
}

int main()
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigfillset(&act.sa_mask); // Bloqueia todos os sinais
    act.sa_sigaction = handle_SIG;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    raise(SIGUSR1); // Envia um SIGUSR1 a si proprio p entrar no handler
    return 0;
}

// Como se diz na man signal, os sinais SIGKILL e SIGSTOP não podem ser ignorados, apanhados ou bloqueados.

