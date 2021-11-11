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
#define NUMERO_DE_SEMAFOROS 2

int main(int argc, char *agrv[]) {
    sem_t *semaforo[NUMERO_DE_SEMAFOROS];
    char semName[20];
    int i = 0;

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) { // Cria os semaforos
        sprintf(semName, "Ex6sem%d", i+1);
        if ((semaforo[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) {
            perror("Erro ao abrir o semaforo");
            exit(1);
        }
    }
    sem_post(semaforo[1]); // Incrementa sem do processo filho

    pid_t pid = fork();
    if(pid == -1){
        perror("Erro ao criar o processo filho\n");
        exit(1);
    }

    for(i = 0; i < 15; i++){
        if (pid > 0) {  // Processo Pai
            sem_wait(semaforo[0]);
            printf("I'm the father!\n");
            sem_post(semaforo[1]);
        }

        if (pid == 0) {  // Processo Filho
            sem_wait(semaforo[1]);
            printf("I'm the child!\n");
            sem_post(semaforo[0]);
        }
    }
    if (pid == 0) exit(0);


    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) {
        sprintf(semName, "Ex6sem%d", i + 1);
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