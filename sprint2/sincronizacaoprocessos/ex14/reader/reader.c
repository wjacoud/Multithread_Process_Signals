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

#define SHM_NAME "/SHM_EX14"
#define SEMAFORO_INITIAL_VALUE 1
#define NUMERO_DE_SEMAFOROS 3
#define SIZE 255

typedef struct {
    char string[SIZE];
}shared_data;

int main() {

    sem_t *semaforo[NUMERO_DE_SEMAFOROS];
    char semName[20];
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;
    int i, fd, r, leitores, escritores = 0;
    time_t t;
    srand((unsigned) time(&t));

    // Semaforos
    // sem0 - total de readers
    // sem1 - pode escrever
    //sem2 - total de escritores para os leitores poderem ler
    //sem3 total de escritores sem decrementar
    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) { // Cria os semaforos
        sprintf(semName, "Ex14sem%d", i + 1);
        if ((semaforo[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) {
            if ((semaforo[i] = sem_open(semName, 0)) == SEM_FAILED) {
                perror("Erro ao abrir o semaforo");
                exit(1);
            }
        }
    }

    // Shmem
    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); /*abrir canal, criar*/
    if (fd < 0) {
        fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
        if (fd < 0) {
            perror("In shm_open()");
            exit(1);
        }
    }

    ftruncate(fd, dataSize); /*tamanho da shm*/

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */


    for(i = 0; i<20; i++){
        pid_t p = fork();

        if(p==0){
            
            do{
                 sem_getvalue(semaforo[2], &escritores); //verifica se pode ler, nao pode enquanto houver escritores a querer escrever
                
            }while(escritores>0);
            sem_post(semaforo[0]); //aumenta o nmr de leitores que querem ler
            sleep(1); //simula 1sg como se estivessemos a abrir varios terminais a correr, com 1 segundo de diferenca
            sem_getvalue(semaforo[0], &leitores); //imprime quantos leitores ainda estao a tentar ler 


             //deveria aceder a memoria partilhada para imprimir de la


            printf("Sou 1 dos %d leitores \n", leitores); //(podem ter sido impedidos de ler por haver mais escritores pois tem prioridade para os escritores)
            sem_wait(semaforo[0]); //decrementa o numero de leitores a espera de ler, pois ja leu
            exit(1);
            
    }
    }


    for(i = 0; i<20; i++){
        wait(NULL);
    }

   
    
    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) {
        sprintf(semName, "Ex14sem%d", i + 1);
        if (sem_close(semaforo[i]) == -1) { // Fecha Semaforo
            perror("Erro no sem_close()\n");
            exit(1);
        }

        if (sem_unlink(semName) == -1) {  // Remove Semaforo
            perror("Erro no sem_unlink()\n");
            exit(1);
        }
    }

    r = munmap(dataPointer, dataSize); //fecha mapeamento
    if (r < 0) exit(1); //verifica erro

    close(fd); //fecha a shm
    if (r < 0) exit(1); //verifica erro

    r = shm_unlink(SHM_NAME);
    if (r < 0) exit(1); //verifica erro


    return 0;

}