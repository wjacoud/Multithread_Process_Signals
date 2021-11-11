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

#define ARRAY_SIZE 10

#define SHM_NAME "/shmex07"

typedef struct
{
    int array[ARRAY_SIZE];
    int NewData;
} shared_data;

int main(void) {
    int fd, r, i, sum = 0;
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;

    fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR); /*abrir canal*/
    if (fd < 0)
    {
        perror("In shm_open()");
        exit(1);
    }

    ftruncate(fd, dataSize); /*indica o tamanho da shm*/

    dataPointer = (shared_data *)mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */

    while (!dataPointer->NewData) //para não aceder a shm ao mesmo tempo
        ;

    for (i = 0; i < ARRAY_SIZE; i++) {
        sum += dataPointer->array[i];
    }
    printf("\nMedia do valores = %i\n", sum / ARRAY_SIZE);


    r = munmap(dataPointer, dataSize); //fecha mapeamento
    if (r < 0)
        return 1; //verifica erro

    close(fd); //fecha a shm
    if (r < 0)
        return 1; //verifica erro

    r = shm_unlink(SHM_NAME);
    if (r < 0)
        return 1; //verifica erro

    printf("<leitor termina>\n");

    return 0;
}
