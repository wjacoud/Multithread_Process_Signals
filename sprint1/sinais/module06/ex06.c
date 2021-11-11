#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>

volatile sig_atomic_t ucv = 1;

void handle_SIG(int signo, siginfo_t *sinfo, void *context)
{
    int j,i = 0;
    int uc = ucv;
    char con[5], noc[5];
    while(uc > 0 || i == 0) {
        con[i] = '0'+ uc%10;    // con[i] fica com o digito das unidades
        uc = uc/10;               // Descarta digito das unidades, "empurra" digitos para a direita
        ++i;
    }
    for (j = 0; j < i ; j++) { // Inversao de correcao do array
        noc[j] = con[i - 1 - j];
    }
    noc[i] = '\n';
    write(STDOUT_FILENO, " SIGUSR1 signal captured, USR1_COUNTER = ", 41);
    write(STDOUT_FILENO, noc, 5);
    ucv++;

    write(STDOUT_FILENO, "Sig Handler Working\n", 21); // d)
    sleep(10); //aumentamos os segundos para perceber melhor o funcionamento do exercicio
    write(STDOUT_FILENO, "Done\n", 5);
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    //sigfillset(&act.sa_mask); /* All signals blocked */ // b)
    sigemptyset(&act.sa_mask); /* No signals blocked */ // d)
    act.sa_sigaction = handle_SIG;
    act.sa_flags = SA_SIGINFO //| SA_NODEFER; com a flag, nem o proprio sinal, com no siganls blocked, fica "bloqueado"
    sigaction(SIGUSR1, &act, NULL);

    pid_t pid = fork();
    int i = 0;
    if (pid == 0){ // c)
        pid_t ppid = getppid();
        printf("Hello, Father!\n");
        for (i = 0; i < 12; i++){
            printf("Take signal %d!\n", i + 1);
            kill(ppid, SIGUSR1);
            nanosleep((const struct timespec[]){{0, 10000000L}}, NULL);
            //sleep(1);  //tempo para o Handler terminar
        }
        printf("Goodbye, Father!\n");
        exit(1);
    }

    for (;;)
    {
        printf("I'm working \n");
        sleep(2);
    }

    return 0;
}
