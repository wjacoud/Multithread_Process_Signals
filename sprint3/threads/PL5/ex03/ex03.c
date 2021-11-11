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

#define THREADS 10
#define ELEMENTOS 1000
#define PROCURA 100

typedef struct {
    int num;
    int indexThread;
    int* array;
} info;


void* threadFunction(void* arg){

    info* argumento = (info*)arg;

   int numero = argumento->num;
   int indexThread = argumento->indexThread;
   int* array = argumento->array;

   int inicio = indexThread * PROCURA;
   int fim = (indexThread + 1) * PROCURA;

   int i;

   for(i=inicio; i<fim; i++){
       if(array[i] == numero){
           printf("O número foi encontrado na posicao %d \n", i);
           fflush(stdout);
           pthread_exit((void*)indexThread+1);

       }
   }

    pthread_exit((void*)NULL); //termina a thread a NULL (as que nao encontram)

}

int main(int argc, char* agrv[]) {

    int i, numero;

    int arrayInicial[ELEMENTOS];

    info arrayInfo[THREADS];

    pthread_t threads[THREADS];

    //preenche o array com numeros unicos
    for (i = 0; i < ELEMENTOS; i++) {
        arrayInicial[i] = i;
    }




    printf("Qual o numero a procurar no array? \n");
    scanf("%d", &numero);
    printf("Obrigada! Vamos procurar... \n");
    fflush(stdout); //limpa o buffer se saida

    //preenche a informacao a que cada thread vai usar
    for(i=0; i < THREADS; i++){
        arrayInfo[i].array = arrayInicial;
        arrayInfo[i].indexThread = i;
        arrayInfo[i].num = numero;
    }

    //cria as threads
    for(i=0; i<THREADS; i++){

        if (pthread_create(&threads[i], NULL, threadFunction, (void*)&arrayInfo[i])) {
            printf("Error while creating threads \n");
            exit(1);
        }    
    }

    int retorno, index=0;
    for(i=0; i<THREADS; i++){

        if (pthread_join(threads[i], (void*)&retorno)) {
            printf("Error while waiting for thread %d \n", i);
            exit(1);
        }
        if(retorno > 0){
            index = retorno;
        }

    }
    
    printf("All threads terminated\n");

    if(index>0){
        printf("A thread que encontrou o numero foi : %d\n", index);
    }else{
        printf("O numero não foi encontrado... \n");
    }
    


    return 0;
}


