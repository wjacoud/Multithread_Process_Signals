#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
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

#define ELEMENTOS 5


typedef struct {
    int num;
    char nome[255];
    int nota;
} info;


void* threadFunction(void* arg){

    info* argumento = (info*)arg;

    printf("Numero : %d\n", argumento->num);
    printf("Nome: %s\n", argumento->nome);
    printf("Nota: %d\n", argumento->nota);
    printf("\n");

    fflush(stdout); 

    pthread_exit((void*)NULL); //termina a thread


}

int main(int argc, char* agrv[]) {

    info arrayInfo[ELEMENTOS];
    pthread_t threads[ELEMENTOS];

    int i;

    for(i=0; i < ELEMENTOS; i++){
        arrayInfo[i].num = i;
        strcpy(arrayInfo[i].nome, "Teste");
        arrayInfo[i].nota = i+10;
    }

    for(i=0; i<5; i++){

        if (pthread_create(&threads[i], NULL, threadFunction, (void*)&arrayInfo[i])) {
            printf("Error while creating threads \n");
            exit(1);
        }    
    }

    for(i=0; i<5; i++){

        if (pthread_join(threads[i], NULL)) {
            printf("Error while waiting for thread %d \n", i);
            exit(1);
        }    
    }

    printf("All threads terminated\n");


    return 0;
}


