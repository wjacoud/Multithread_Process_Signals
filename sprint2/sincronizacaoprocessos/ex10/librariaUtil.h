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


#define TOTAL_REGISTOS 100
#define SEMAFORO_INITIAL_VALUE 1
#define SEMAFORO_FILE_NAME "Ex10sem"
#define SHM_NAME "/SHM_EEEEEX10"

typedef struct {
    int numero;
    char nome[100];
    char endereco[150];
} Registo;

typedef struct {
    Registo registos[TOTAL_REGISTOS];
} shared_data;


