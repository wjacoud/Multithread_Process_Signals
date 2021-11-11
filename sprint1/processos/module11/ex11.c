#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define ARRAY_SIZE 1000

int main(void) { 

  int i,j,max,status;
  int numbers[ARRAY_SIZE];
  int max_value = 0;
  int v[5]; //Vetor que guarda os 5 PID dos processos filhos
  int result[ARRAY_SIZE/2]; //Vetor que guarda as 500 posiçoes no pai e as 500 posições no filho

  /* intializes RNG (srand():stdlib.h; time(): time.h) */
  srand(time(NULL));

  /* initialize array with random numbers (rand(): stdlib.h) */ 
  for(i=0; i<ARRAY_SIZE; i++){

    numbers[i]= rand () % 255; 
    
  }

  //Cria 5 processos filhos
  for(i = 0; i<5; i++){

    v[i]=fork();

    if(v[i]==0){
      
      //Se estivermos dentro do processo filho (1a vez no primeiro, 2a vez no segundo,...)
      for(j = i*200; j<200*(i+1); j++){
        //Verifica 1/5 do array total  no filho em questão e descobre o seu máximo
        if(numbers[j]>max){
          max = numbers[j];
        }

      }
      //Sai do processo filho, capturando o valor máximo
      exit(max);

    }

  }
  //Isto acontece no processo pai, 5x
  for(i=0;i<5;i++){
    //Espera pelos 5 filhos
    wait(&status);
    //Verifica qual o valor dos 5 filhos de exit é o maior, para ser efetivamente o maior do array total
    if(WEXITSTATUS(status)>max_value){
      max_value = WEXITSTATUS(status);
    }    
  }

  //Cria um novo processo filho
  pid_t p = fork();

//Se estivermos dentro do filho
  if(p==0){
    //Faz o calculo e dá print do valor para os primeiros 500 do array numbers e guarda o resultado no array result
    //
    for(i = 0; i<ARRAY_SIZE/2; i++){
      result[i]=((int)numbers[i]/max_value)*100;
      //Se o enunciado não indicasse para calcular e print, poderia o calculo estar dentro do print
      printf("%d; ", result[i]);
    }
    printf("\n Processo Filho Terminado \n");
    //Termina o processo filho, dando print dos primeiros 500 números calculados
    exit(1);
  }

  //Estando agora no processo pai
  for(i=ARRAY_SIZE/2; i<ARRAY_SIZE;i++){
    //Calcula o valor dos últimos 500 números
    result[i]=((int)numbers[i]/max_value)*100;
  }
  //Espera que o filho termine, de forma a conseguirmos garantir que o result é printed na ordem de index do array numbers
  wait(&status);
  for(i=ARRAY_SIZE/2; i<ARRAY_SIZE;i++){
    printf("%d; ", result[i]);
  }

  return 0;
} 

