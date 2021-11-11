#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SEMAFORO_INITIAL_VALUE 0
#define NUMERO_DE_SEMAFOROS 3

int cria_filhos(int n) {
    pid_t p;
    int i = 0;
    for (i = 0; i < n; ++i) {
        p = fork();

        if (p < 0)
            return -1;
        else if (p == 0)
            return i + 1;
    }
    return 0;
}

void printOk(char *message) {
    printf("%s", message);
    fflush(stdout);  // limpar buffer de output
}

int main(int argc, char *agrv[]) {
    sem_t *semaforo[NUMERO_DE_SEMAFOROS];
    char semName[20];
    int i = 0;

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) { // Cria os semaforos
        sprintf(semName, "Eeex7sem%d", i+1);
        if ((semaforo[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) {
            perror("Erro ao abrir o semaforo");
            exit(1);
        }
    }
    sem_post(semaforo[0]); // Incrementa sem do processo filho

    int p = cria_filhos(3);
    if(p<0){
        printf("Erro ao criar filhos");
        exit(2);
    }


        if (p == 1) {  // Processo 1
            sem_wait(semaforo[0]);
                printOk("Sistemas ");
            sem_post(semaforo[1]);

            sem_wait(semaforo[0]);
                printOk("a ");
            sem_post(semaforo[1]);
            exit(1);
        }

        if (p == 2) {  // Processo 2
            sem_wait(semaforo[1]);
                printOk("de ");
            sem_post(semaforo[2]);

            sem_wait(semaforo[1]);
                printOk("melhor ");
            sem_post(semaforo[2]);
            exit(1);
        }

        if (p == 3) {  // Processo 3
            sem_wait(semaforo[2]);
                printOk("Computadores - ");
            sem_post(semaforo[0]);
            sem_wait(semaforo[2]);
                printOk("disciplina!\n");
            exit(1);
        }

    for (i = 0; i< 3; i++) { // espera os filhos para iniciar
        wait(NULL);
    }

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) {
        sprintf(semName, "Eeex7sem%d", i + 1);
        if (sem_close(semaforo[i]) == -1) { // Fecha Semaforo
            perror("Erro no sem_close()\n");
            exit(1);
        }

        if (sem_unlink(semName) == -1) {  // Remove Semaforo
            perror("Erro no sem_unlink()\n");
            exit(1);
        }
    }

    return 0;
}