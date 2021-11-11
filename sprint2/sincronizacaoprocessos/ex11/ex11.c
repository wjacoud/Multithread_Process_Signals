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

#define LUGARES 200
#define PORTAS 3
#define SHM_NAME "/SHM_EEX11"
#define SEMAFORO_INITIAL_VALUE 1

typedef struct {
    int lugaresLivres;
    int lugaresOcupados;
} shared_data;

void gerarEntradaDePessoas(shared_data *dataPointer);

void gerarSaidaDePessoas(shared_data *dataPointer);

int main(int argc, char *agrv[]) {

    sem_t *semaforo[PORTAS];
    char semName[20];
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;
    int i, fd, r;
    pid_t pids[PORTAS];

    // Semaforos
    for (i = 0; i < PORTAS; i++) { // Cria os semaforos
        sprintf(semName, "Eex11sema%d", i + 1);
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

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                       0); /* cria o mapeamento */

    dataPointer->lugaresOcupados = 100;
    dataPointer->lugaresLivres = LUGARES - 100;

    for (i = 0; i < PORTAS; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("Erro ao criar filhos\n");
            exit(2);
        }

        if (pids[i] == 0) {
            sem_trywait(semaforo[0]);
            printf("A sair pessoas pela porta 1!\n");
            gerarSaidaDePessoas(dataPointer);
            sem_post(semaforo[0]);

            sem_trywait(semaforo[1]);
            printf("A sair pessoas pela porta 2!\n");
            gerarSaidaDePessoas(dataPointer);
            sem_post(semaforo[1]);

            sem_trywait(semaforo[2]);
            printf("A sair pessoas pela porta 3!\n");
            gerarSaidaDePessoas(dataPointer);
            sem_post(semaforo[2]);

            exit(0);
        } else {
            sem_trywait(semaforo[0]);
            printf("A entrar pessoas pela porta 1!\n");
            gerarEntradaDePessoas(dataPointer);
            sem_post(semaforo[0]);

            sem_trywait(semaforo[1]);
            printf("A entrar pessoas pela porta 2!\n");
            gerarEntradaDePessoas(dataPointer);
            sem_post(semaforo[1]);

            sem_trywait(semaforo[2]);
            printf("A entrar pessoas pela porta 3!\n");
            gerarEntradaDePessoas(dataPointer);
            sem_post(semaforo[2]);
        }
    }

    for (i = 0; i < PORTAS; i++) {
        sprintf(semName, "Eex11sema%d", i + 1);
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

void gerarEntradaDePessoas(shared_data *dataPointer) {
    time_t t;
    srand((unsigned) time(&t));
    int total;
    total = rand() % (dataPointer->lugaresLivres);
    if (total > 30) {
        total /= 2;
    }
    dataPointer->lugaresOcupados += total;
    dataPointer->lugaresLivres -= total;
    if (total > 0) {
        printf("%d pessoas a entrar!\n", total);
    } else {
        printf("Comboio cheio!\n");
    }
    printf("TOTAL DE PESSOAS NO COMBOIO: %d\n\n", dataPointer->lugaresOcupados);
}

void gerarSaidaDePessoas(shared_data *dataPointer) {
    time_t t;
    srand((unsigned) time(&t));
    int total;
    total = rand() % (dataPointer->lugaresOcupados);
    if (total > 30) {
        total /= 2;
    }
    dataPointer->lugaresOcupados -= total;
    dataPointer->lugaresLivres += total;
    if (total > 0) {
        printf("%d pessoas a sair!\n", total);
    } else {
        printf("Comboio vazio!\n");
    }
    printf("TOTAL DE PESSOAS NO COMBOIO: %d\n\n", dataPointer->lugaresOcupados);
}
