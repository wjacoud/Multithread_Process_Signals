#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>

volatile sig_atomic_t procA = 1;

void handle_SIG(int signo, siginfo_t *sinfo, void *context)
{
    write(STDOUT_FILENO, "Sinal SIGUSR1 handled!\n", 24); //b)
    procA--;
}

int main(int argc, char *argv[])
{
    time_t t;
    srand((unsigned) time (&t));

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask); /* No signals blocked */
    act.sa_sigaction = handle_SIG;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    pid_t p = fork();
    if (p > 0) {
        printf("Pai executa processo A\n");
        sleep(3);
        kill(p, SIGUSR1);
    } else {
        int n = (rand() % 5) + 1;
        printf("Filho executa processo B\n");
        sleep(n);
        while(procA > 0){
            printf("A espera do procA terminar\n");
        }
        printf("Filho executa processo C\n");
        exit(1);
    }
    wait();
    return 0;
}