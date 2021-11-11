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

#define SHM_NAME "/SHM_EEX12"
#define SEMAFORO_INITIAL_VALUE 0
#define NUMERO_DE_SEMAFOROS 3

typedef struct {
    int clienteFila;
    int clienteProx;
    int bilhetes;
} shared_data;

int main() {

    sem_t *semaforo[NUMERO_DE_SEMAFOROS];
    char semName[20];
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;
    int i, fd, senha = 0;

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) { // Cria os semaforos
        sprintf(semName, "Eex12sema%d", i + 1);
        if ((semaforo[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) {
            if ((semaforo[i] = sem_open(semName, 0)) == SEM_FAILED) {
                perror("Erro ao abrir o semaforo");
                exit(1);
            }
        }
    }

    fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("In shm_open()");
        exit(1);
    }

    ftruncate(fd, dataSize); /*tamanho da shm*/

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                       0); /* cria o mapeamento */

    /* atendimento CLIENTE */

    sem_wait(semaforo[0]); // retirada de numero de atendimento (senha)
    senha = dataPointer->clienteFila; // numero de atendimento (senha)
    dataPointer->clienteFila++;
    printf("Novo cliente - número %d\n", senha + 1);
    sem_post(semaforo[0]);

    do {
        sem_wait(semaforo[1]); // semaforo para entrada de cliente a ser atendido
        if (dataPointer->clienteProx == senha) { // Verificacao da ordem de chegada
            sleep(2);
            printf("Sou o cliente %d e meu bilhete é o %d\n", senha + 1, dataPointer->bilhetes);
            dataPointer->clienteProx++;
            sem_post(semaforo[2]); // semaforo para indicar fim de atendimento
        } else {
            printf("Sou o cliente %d e ainda não é a minha vez\n", senha);
            sem_post(semaforo[1]);
        }
    } while (dataPointer->clienteProx != senha + 1); // Se o cliente não era o proximo ele continua a esperar.

    /* ----------------------*/

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) {
        sprintf(semName, "Eex12sema%d", i + 1);
        if (sem_close(semaforo[i]) == -1) { // Fecha Semaforo
            perror("Erro no sem_close()\n");
            exit(1);
        }
    }

    return 0;
}