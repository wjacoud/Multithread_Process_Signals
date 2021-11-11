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
    pid_t p;

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) { // Cria os semaforos
        sprintf(semName, "Ex9sem%d", i+1);
        if ((semaforo[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) {
            if ((semaforo[i] = sem_open(semName, 0)) == SEM_FAILED){
                perror("Erro ao abrir o semaforo");
                exit(1);
            }
        }
    }

    p = fork();

    if(p==0){
        //filho

        printf("filho: COMPREI AS BATATAS FRITAS! \n");
       
        sem_post(semaforo[1]); //incrementa
        
        printf("filho: estou a espera\n");
        sem_wait(semaforo[0]); //decrementa

        printf("filho: JÁ PODEMOS COMER E BEBER! \n");

        exit(1);
    
    
    }
    //pai

    // sleep(2);
    printf("pai: COMPREI A CERVEJA! \n");

    sem_post(semaforo[0]); //incrementa
    printf("pai: estou a espera\n");
    sem_wait(semaforo[1]); //decrementa
    
    printf("pai: JÁ PODEMOS COMER E BEBER! \n");


    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) {
        sprintf(semName, "Ex9sem%d", i + 1);
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