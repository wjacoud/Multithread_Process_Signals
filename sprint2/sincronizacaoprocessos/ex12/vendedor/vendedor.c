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
    int i, fd, r;
    time_t t;
    srand((unsigned) time(&t));

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) { // Cria os semaforos
        sprintf(semName, "Eex12sema%d", i + 1);
        if ((semaforo[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) {
            if ((semaforo[i] = sem_open(semName, 0)) == SEM_FAILED) {
                perror("Erro ao abrir o semaforo");
                exit(1);
            }
        }
    }

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

                /* atendimento VENDEDOR */

    dataPointer->clienteFila = 0; // controla a fila de atendimento (senha)
    dataPointer->clienteProx = 0; // controla os clientes atendidos e sua ordem
    dataPointer->bilhetes = 100;  // numero do bilhete inicial
    sem_post(semaforo[0]); // abre o semaforo da retirada de numero de atendimento (senha)
    sem_post(semaforo[1]); // abre o semaforo para o primeiro cliente ser atendido

    printf("VENDEDOR - Bom dia!\n");

    do { // vendedor executa os atendimentos
        sem_wait(semaforo[2]); // Esse semaforo so é aberto depois da validacao da ordem de chegada e do termino do atendimento do cliente anterior
        dataPointer->bilhetes++;
        int n =  1 + (rand() % 9);
        printf("VENDEDOR - %d\n", n);
        sleep(n);
        printf("VENDEDOR - Próximo cliente!!!\n");
        sem_post(semaforo[1]); // abre o semaforo para o proximo cliente ser atendido
    } while (1);
    printf("VENDEDOR - Todos os clientes atendidos\n");

                /* ----------------------*/

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) {
        sprintf(semName, "Eex12sema%d", i + 1);
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