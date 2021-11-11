#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define ARRAY_SIZE 2000

int main(void) { 

  int i,j,n,status,saida,index;
  int numbers[ARRAY_SIZE];
  int v[10]; //Vetor que guarda os 10 PID dos processos filhos


  /* intializes RNG (srand():stdlib.h; time(): time.h) */
  srand(time(NULL));

  /* initialize array with random numbers (rand(): stdlib.h) */ 
  for(i=0; i<ARRAY_SIZE; i++){
    numbers[i]= rand () % 10000; 
    //numbers[i]=9;
  }

  /* initialize n */
  //n = rand() % 10000;
  n = 9;
  printf("Número a encontrar: %d \n", n);

  for(i = 0; i<10; i++){

    v[i]=fork();

    if(v[i]==0){
      
      for(j = i*200; j<(i+1)*200; j++){

        if(numbers[j]==n){
          
          exit(j-(i*200));
        }
      }

      exit(255);

    }

  }

  for(i=0;i<10;i++){

    waitpid(v[i],&status,0);
    saida = WEXITSTATUS(status);
    if(saida == 255){
      printf("Não houve ocorrências do número %d no processo filho com o PID %d. \n", n, v[i]);
    }else{
      index = saida+(i*200);
      printf("Foi encontrado o número %d, no processo filho com PID %d, cujo index real é %d, mas o valor de saída é %d. \n",n,v[i],index,saida);
    }
  }

  return 0;
} 

