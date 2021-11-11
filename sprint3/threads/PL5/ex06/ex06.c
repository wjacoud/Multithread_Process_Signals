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



void* threadFunction (void *arg) {

    unsigned int limite = *(unsigned int *)arg;


    int i, flag;
    

    //inspirado em https://www.programiz.com/c-programming/examples/prime-number-intervals

    while (limite) {
      flag = 0;

      // se o numbero for menor ou igual a 1, ele vai "ignorar" e decrementar até que chegue ao 0 para sair do ciclo
      if (limite <= 1) {
         --limite;
         continue;
      }

      // se limite é não primo, a flag é 1
      for (i = 2; i <= limite / 2; ++i) {

         if (limite % i == 0) {
            flag = 1;
            break;
         }
      }

      if (flag == 0){ //se é primo, a flag é 0.
         printf("%d ", limite);
      }

      // para ver o proximo numero
      // decrementa o limite a 1
      --limite;
   }

    pthread_exit((void *) NULL); 
}

int main(int argc, char *agrv[]) {

    unsigned int limite;

    pthread_t thread;


    printf("Qual o numero? \n");
    scanf("%d", &limite);
    printf("Iremos indicar os numeros primos abaixo desse: \n");
    fflush(stdout); //limpa o buffer se saida
   
  
    if (pthread_create(&thread, NULL, threadFunction, (void *)&limite)) {
        printf("Error while creating threads \n");
        exit(1);
    }


    if (pthread_join(thread, NULL)) {
        printf("Error while waiting for thread \n");
        exit(1);
    }
    
    printf("All threads terminated\n");


    return 0;
}


