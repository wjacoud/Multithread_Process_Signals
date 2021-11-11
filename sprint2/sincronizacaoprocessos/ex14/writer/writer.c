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
#define SEMAFORO_INITIAL_VALUE 0
#define NUMERO_DE_SEMAFOROS 4
#define SIZE 255

typedef struct {
    char string[SIZE];
} shared_data;

int main() {

    sem_t *semaforo[NUMERO_DE_SEMAFOROS];
    char semName[20];
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;
    int i, fd, leitores, escritores;
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
    sem_post(semaforo[1]); //coloca total de escritores a 0;

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


    for(i = 0; i<5; i++){
        pid_t p = fork();
        if(p==0){
            sem_post(semaforo[3]); //aumenta o total de escritores para indicar quantos houveram
            sem_post(semaforo[2]); //aumenta o total de escritores para controlo da possivel leitura dos leitores (Se escritores > 0 nao pode ler)
            sem_wait(semaforo[1]); //decrementa a possibilidade para outros escritores escrevem, so 1 escreve de cada vez na shm


            //sprintf(dataPointer->string, "PID: %d Hora: %s\n", getpid(), time_str);


            printf("Acabei de escrever \n");
            sem_getvalue(semaforo[0], &leitores);
            printf("Estao %d leitores\n", leitores);
            sem_getvalue(semaforo[3], &escritores);
            printf("Estao %d escritores\n", escritores);	
            sleep(2); 
            sem_post(semaforo[1]);
            sem_wait(semaforo[2]);
            exit(1);
     }else{
            wait(NULL);
        }
    }


    return 0;

}