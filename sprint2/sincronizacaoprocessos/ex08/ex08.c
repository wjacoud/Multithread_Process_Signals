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

#define CHILD 2
#define NUMERO_DE_SEMAFOROS 2
#define SEMAFORO_INITIAL_VALUE 1

int cria_filhos(int n) {
    pid_t pid;
    int i;
    for (i = 0; i < n; i++) {
        pid = fork();
        if (pid < 0) {
            return -1;
        } else if (pid == 0) {
            return i + 1;
        }
    }
    return 0;
}
void printOk(char *message) {
    printf("%s", message);
    fflush(stdout);
}
int main(int argc, char *agrv[]) {
    sem_t *semaforo[NUMERO_DE_SEMAFOROS];
    char semName[20];
    int i = 0;

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) { // Cria os semaforos
        sprintf(semName, "Eeex8sem%d", i+1);
        if ((semaforo[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) {
            if ((semaforo[i] = sem_open(semName, 0)) == SEM_FAILED){
                perror("Erro ao abrir o semaforo");
                exit(1);
            }
        }
        sem_post(semaforo[i]);
    }

    int p = cria_filhos(CHILD);
    if(p<0){
        perror("Erro ao criar filhos");
        exit(2);
    }

    if (p == 1) {
        while (1) {
            sem_wait(semaforo[0]);
            printOk("S");
            sem_post(semaforo[1]);
        }
        exit(0);
    }

    if (p == 2) {
        while (1) {
            sem_wait(semaforo[1]);
            printOk("C");
            sem_post(semaforo[0]);
        }
        exit(0);
    }

    for (i = 0; i < CHILD; i++) {  // espera os filhos para iniciar
        wait(NULL);
    }

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) {
        sprintf(semName, "Eeex8sem%d", i + 1);
        if (sem_close(semaforo[i]) == -1) { // Fecha Semaforo
            perror("Erro no sem_close()\n");
            exit(3);
        }

        if (sem_unlink(semName) == -1) {  // Remove Semaforo
            perror("Erro no sem_unlink()\n");
            exit(4);
        }
    }

    return 0;
}