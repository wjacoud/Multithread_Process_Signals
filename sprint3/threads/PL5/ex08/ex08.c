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

#define THREADS 5
#define PARCIAL 200
#define TAMANHO 1000


int data[TAMANHO]; 
int result[TAMANHO]; 

pthread_mutex_t mutex[THREADS];

void* threadFunction (void *arg) {

   int* argumento = (int*)arg;
   int ordem = *argumento;

   int min = ordem*PARCIAL;
   int max = (ordem+1)*PARCIAL;
   int i;

   //calculos realizados em paralelo
   for(i=min; i<max; i++){
      result[i] = data[i] * 10 + 2;
   }

   //valida se pode entrar, se consegue desbloquear o "semaforo" == sem_wait
   if(pthread_mutex_lock(&mutex[ordem])){
         printf("Error while locking the mutex thread \n");
         pthread_exit((void *)1);
   }

   for(i=min; i<max; i++){
   printf("result[%d] = %d\n", i, result[i]);
   }
   
   if(ordem<THREADS-1){ 
      if(pthread_mutex_unlock(&mutex[ordem+1])){
         printf("Error while unlocking the mutex thread \n");
         pthread_exit((void *)1);
      }
   }

   pthread_exit((void *) NULL); 

}

void preencherArrayData(){
   int i;
   time_t t;

   srand((unsigned)time(&t));

   for (i = 0; i < TAMANHO; i++)
   {
      data[i] = 1 + rand() % TAMANHO;
   }

}

int main(int argc, char *agrv[]) {

   preencherArrayData();
   int i;
   //preencher array result  a 0
   for(i = 0 ; i<TAMANHO; i++){
      result[i] = 0;
   }

   //cria "semaforos" de forma a garantir a ordem, thread1 desbloqueia thread2, thread2 desbloqueia a thread3...
   for (i = 0; i < THREADS; i++)
    {
        if (pthread_mutex_init(&mutex[i], NULL))
        {
        printf("Error while initializing the mutex thread \n");
            exit(1);
        }
        //só não bloqueia a primeira thread
        if (i > 0)
        {
            if (pthread_mutex_lock(&mutex[i]))
            {
               printf("Error while locking the mutex thread \n");
                exit(1);
            }
        }
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


   //detroi todos os "semaforos" das threads
    for (i = 0; i < THREADS; i++)
    {

      if(pthread_mutex_unlock(&mutex[i])){
         printf("Error while unlocking the mutex thread \n");
         exit(1);
      }

      if (pthread_mutex_destroy(&mutex[i]) != 0)
      {
         printf("Error while destroying the mutex thread \n");
         exit(1);
      }

    }

   printf("All threads terminated\n");

   return 0;
}


