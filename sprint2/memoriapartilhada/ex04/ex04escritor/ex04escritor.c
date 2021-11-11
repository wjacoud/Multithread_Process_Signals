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

#define ARRAY_SIZE 100

#define SHM_NAME "/shmcharray"

typedef struct
{
    char array[100];
    int NewData;
} shared_data;

int main(void) {
    time_t t;
    srand((unsigned) time (&t));
    int i;
    char charray[ARRAY_SIZE];

    for(i = 0; i < ARRAY_SIZE; i++) {
        charray[i] = 65 + rand() % 26;
    }

    int fd;
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

    //para não aceder a shm ao mesmo tempo
    dataPointer->NewData = 0;

    for(i = 0; i < ARRAY_SIZE; i++) {
        dataPointer->array[i] = charray[i];
    }

    dataPointer->NewData = 1;

    printf("<escritor end>\n");

    return 0;
}
