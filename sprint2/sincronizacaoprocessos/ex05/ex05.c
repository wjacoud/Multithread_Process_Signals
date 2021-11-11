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

#define SEMAFORO_FILE_NAME "ex05"
#define SEMAFORO_INITIAL_VALUE 0

int main(int argc, char *agrv[]) {
    sem_t *semaforo;

    if ((semaforo = sem_open(SEMAFORO_FILE_NAME, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) { // Abre o semaforo
        perror("Erro ao abrir o semaforo\n");
        exit(1);
    }

    pid_t pid = fork();
    if(pid == -1){
        perror("Erro ao criar o processo filho\n");
        exit(1);
    }

    if (pid > 0) {  // Processo Pai
        sem_wait(semaforo);
        printf("I'm the father!\n");
    }

    if (pid == 0) {  // Processo Filho
        printf("I'm the child!\n");
        sem_post(semaforo);
        exit(0);
    }

    if (sem_close(semaforo) == -1) { // Fecha Semaforo
        perror("Error at sem_close()!\n");
        exit(1);
    }

    if (sem_unlink(SEMAFORO_FILE_NAME) == -1) {  // Remove Semaforo
        perror("Error at sem_unlink()!\n");
        exit(1);
    }

    return 0;
}