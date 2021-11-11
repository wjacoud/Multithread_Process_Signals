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

#define THREADS 3
#define TAMANHO 1000

int minimo;
int maximo;
int media;

void* procuraMinimo (void *arg) {

    int* argumento = (int*)arg;
    int i, valor;
    minimo = 0;
    for(i=0; i<TAMANHO; i++){       //Calcula o minimo
        valor = argumento[i];
        if(valor<minimo){
            minimo = valor;
        }
    }
    
    pthread_exit((void *) NULL); 
}

void* procuraMaximo (void *arg) {

    int* argumento = (int*)arg;
    int i, valor;
    maximo = 0;
    for(i=0; i<TAMANHO; i++){       //Calcula o maximo
        valor = argumento[i];
        if(valor>maximo){
            maximo = valor;
        }
    }
    
    pthread_exit((void *) NULL); 

}

void* procuraMedia (void *arg) {

    int* argumento = (int*)arg;
    int i, soma = 0;
    media = 0;

    for(i=0; i<TAMANHO; i++){           //Soma todos os elementos do array
        soma = soma + argumento[i];
    }

    media = soma/TAMANHO;           //Calcula a media
    
    pthread_exit((void *) NULL); 
}

int main(int argc, char *agrv[]) {

   
    time_t t;
    srand((unsigned) time (&t));
    int i;

    int balances[TAMANHO];

    for(i = 0; i < TAMANHO; i++) {
        balances[i] = -300 + rand() % 10301;  //preenche o array de valores das contas:  (min) + rand() %d (max+1 - min)
    }
    

    pthread_t threads[THREADS];

    if (pthread_create(&threads[0], NULL, procuraMinimo, (void *)&balances)) {
        printf("Error while creating threads \n");
        exit(1);
    }
    if (pthread_create(&threads[1], NULL, procuraMaximo, (void *)&balances)) {
        printf("Error while creating threads \n");
        exit(1);
    }
    if (pthread_create(&threads[2], NULL, procuraMedia, (void *)&balances)) {
        printf("Error while creating threads \n");
        exit(1);
    }

    for (i = 0; i< THREADS; i++) {
        if (pthread_join(threads[i], NULL)) {
            printf("Error while waiting for thread %d \n", i);
            exit(1);
        }
    }
    printf("All threads terminated\n");


    printf("Valor maximo: %d\n", maximo);
    printf("Valor minimo: %d\n", minimo);
    printf("Media: %d\n", media);

    return 0;
}


