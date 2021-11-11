#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#include <sys/types.h> /*tipo de dados usados pela API*/
#include <fcntl.h>     /*opções de controlo de ficheiros*/
#include <sys/stat.h>  /*contantes utilizadas para modo de abertura*/
#include <sys/mman.h>  /*declarações relacionadas com a gestão de memória partilhada: shm_* e mmap()*/

#define SHM_NAME "/shmintegers"

typedef struct
{
    int x;
    int y;
    int NewData;
} shared_data;

int main(void) {
    int i, fd, r;
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;

    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); /*abrir canal, criar*/
    if (fd < 0)
    {
        perror("In shm_open()");
        exit(1);
    }

    ftruncate(fd, dataSize); /*tamanho da shm*/

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */

    // Inicializa a flag a 0 e os inteiros pedidos no enunciado
    dataPointer->NewData = 0;
    dataPointer->x = 8000;
    dataPointer->y = 200;

    pid_t p = fork();
    if (p == -1) {
        printf("An error ocurred with fork\n");
        return 2;
    }

    for(i = 0; i < 1000000; i++) { // executa 1000000 vezes as operacoes em cada processo
        if (p > 0) {
            while (!dataPointer->NewData) // Nao ecede a shm quando flag = 0
                ;
            dataPointer->x++;
            dataPointer->y--;

            dataPointer->NewData = 0; // flag = 0 para o outro processo aceder
        } else {
            while (dataPointer->NewData) // Nao ecede a shm quando flag = 1
                ;
            dataPointer->x--;
            dataPointer->y++;

            dataPointer->NewData = 1; // flag = 1 para o outro processo aceder
        }
    }
    if(p == 0) exit(EXIT_SUCCESS);

    printf("Valor final de x: %d\tValor final de y: %d\n", dataPointer->x, dataPointer->y);

    r = munmap(dataPointer, dataSize); //fecha mapeamento
    if (r < 0)
        return 1; //verifica erro

    close(fd); //fecha a shm
    if (r < 0)
        return 1; //verifica erro

    r = shm_unlink(SHM_NAME);
    if (r < 0)
        return 1; //verifica erro

    return 0;
}