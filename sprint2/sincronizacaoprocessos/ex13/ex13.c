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

#define SHM_NAME "/SHM_EEX13"
#define SEMAFORO_INITIAL_VALUE 0
#define NUMERO_DE_SEMAFOROS 3
#define BUFFER 10

typedef struct {
    int numbers[BUFFER];
    int curr;
    int wrID;
} shared_data;

int main() {

    sem_t *semaforo[NUMERO_DE_SEMAFOROS];
    char semName[20];
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;
    int i, fd, r, rdID = 0;
    time_t t;
    srand((unsigned) time(&t));

    // Semaforos
    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) { // Cria os semaforos
        sprintf(semName, "Eex13sema%d", i + 1);
        if ((semaforo[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) {
            if ((semaforo[i] = sem_open(semName, 0)) == SEM_FAILED) {
                perror("Erro ao abrir o semaforo");
                exit(1);
            }
        }
    }

    for (i = 0; i < BUFFER; i++) {
        sem_post(semaforo[0]); // semaforo permite o numero de escritas ate o tail do buffer
    }
    sem_post(semaforo[2]); //semaforo de controlo da escrita

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

    dataPointer->wrID = 0;

    pid_t p;

    for (int i = 0; i < 2; i++) {

        p = fork();                                //Cria filhos

        if (p == 0) {
            for (int i = 0; i < 15; i++) {

                sem_wait(semaforo[0]);                                           //semaforo de controlo da escrita
                sem_wait(semaforo[2]);                                           //semaforo de acesso ao id do buffer
                if (dataPointer->wrID == BUFFER) {
                    dataPointer->wrID = 0;                                       //volta ao inicio do buffer
                }
                dataPointer->numbers[dataPointer->wrID] = dataPointer->curr;     //coloca o nr atual a no sitio correto da memoria
                dataPointer->curr = rand() % 30;                                        //atribui novo numero atual
                dataPointer->wrID++;

                sem_post(semaforo[2]);                                           //semaforo de acesso ao id do buffer
                sem_post(semaforo[1]);                                           //semaforo de controlo da leitura
            }
            exit(0);
        }

    }

    for (int i = 0; i < 30; i++) {
        if (rdID == BUFFER) {
            rdID = 0;                                                   //volta ao inicio do buffer
            for (int j = 0; j < BUFFER; j++) {                          //semaforo de controlo da escrita
                sem_post(semaforo[0]);
            }
        }

        sem_wait(semaforo[1]);                                          //semaforo de controlo da leitura
        printf("%iº nr: %i\n", i + 1, dataPointer->numbers[rdID]);
        rdID++;
    }

    wait(0);
    wait(0);

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) {
        sprintf(semName, "Eex13sema%d", i + 1);
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