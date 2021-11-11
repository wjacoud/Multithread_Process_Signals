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

#define THREADS 16
#define PARCIAL 500
#define BILHETES 8000
#define CHAVES 5
#define MAXIMO 49

int database[BILHETES][CHAVES]; //base de dados que contem todos os bilhetes com as 5 chaves
int numero_escolhas[MAXIMO];  //array para colocar os valores estatisticos de quantas vezes cada numero foi escolhido

pthread_mutex_t mutex;

void* threadFunction (void *arg) {

   int* argumento = (int*)arg;
   int ordem = *argumento;

   int min = ordem*PARCIAL;
   int max = (ordem+1)*PARCIAL;
   int i,j;
   int valor = 0;

   for(i=min; i<max; i++){

      if(pthread_mutex_lock(&mutex)){
         printf("Error while locking the mutex thread \n");
         exit(1);
      }

      for(j=0; j<CHAVES; j++){
         valor = database[i][j];
         numero_escolhas[valor]++;
      }

      if(pthread_mutex_unlock(&mutex)){
         printf("Error while locking the mutex thread \n");
         exit(1);
      }

   }

   pthread_exit((void *) NULL); 

}

void preencherBaseDeDados(){
   int i,k;
   time_t t;

   srand((unsigned)time(&t));

   for (k = 0; k < BILHETES; k++)
   {
      for (i = 0; i < CHAVES; i++)
      {
         database[k][i] = 1 + rand() % MAXIMO;
      }
    }
}

int main(int argc, char *agrv[]) {

   preencherBaseDeDados();
   int i;
   //preencher array das estatisticas a 0
   for(i = 0 ; i<MAXIMO; i++){
      numero_escolhas[i] = 0;
   }

   //inicia a thread mutex
   if (pthread_mutex_init(&mutex, NULL)) {
        printf("Error while initializing the mutex thread \n");
        exit(1);
   }


   pthread_t threads[THREADS];
   int ordem[THREADS]; //array para guardar o "numero" da thread, de forma a que possa criar threads e indicar as posicoes a procurar sem perder o valor armazenado no endereco de memoria
  
   for (i = 0; i < THREADS; i++){
      ordem[i] = i;
      if (pthread_create(&threads[i], NULL, threadFunction, (void *)&ordem[i])) {
      printf("Error while creating the thread \n");
      exit(1);
      }
   }

   //cria as threads para procurar e contar os valores das escolhas
   for (i = 0; i < THREADS; i++){
      if (pthread_join(threads[i],NULL)) {
      printf("Error while waiting for thread \n");
      exit(1);
      }
   }

   printf("All threads terminated\n");

   //destroi a thread que serve de mutex
   if (pthread_mutex_destroy(&mutex)) {
      printf("Error while destroying the mutex thread \n");
      exit(1);
   }

   for(i = 1; i<=MAXIMO; i++){
      printf("O numero %d foi escolhido %d vezes \n", i, numero_escolhas[i]);
   }

   return 0;
}


