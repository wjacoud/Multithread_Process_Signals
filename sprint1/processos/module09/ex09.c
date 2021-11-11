#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>


int main(void) { 

  int v[10]; //Vetor que armazena o PID dos processos filhos
  int i = 0;
  int j = 0;
  int status;

  for(i=0; i<10; i++){  //Ciclo for para criar os 10 processos filhos e realizar operações em cada processo filho
    
    v[i]=fork(); //Cria o processo filho
    

    if(v[i]==0){   //Se estivermos "dentro" do processo filho criado

      //Fazer print dos números de 1..100 para filho 1
      //Fazer print dos numeros de 101...200 para filho 2
      //...

      for(j = (i*100) + 1; j <= (i+1)*100; j++){
        printf("%d; ",j);
      }
      printf("\n");
      exit(1);
    }

  } //Ciclo FOR termina para garantir que são realmente criados os 10 processos filhos


  //Novo ciclo for para conseguirmos realizar operações no processo pai
	for (i = 0; i < 10; i++)
	{
    //O pai vai ter a função waitpid (aguardar pelo processo filho), para todos os 10 filhos criados
		waitpid(v[i], &status, 0); 
	}

  //Isto é apenas impresso se de facto o pai bloqueou - código 0 - e esperou pelo término dos processos filhos
  printf("Todos os numeros foram impressos.\n"); 
  


  return 0;
} 

//O output não vem ordenado, 
//uma vez que são criados 10 filhos no processo pai e, 
//só após a criação de todos os filhos é que o pai aguarda que todos terminem.
//Conseguiamos "garantir" a ordem, se o pai, por cada processo filho criado, aguardava que o mesmo terminasse
//Ou seja, criava 1 processo filho, aguardava pelo término dele, o filho imprimia de 1 a 100
//O pai posteriormente criava outro filho, aguardava pelo término dele, o segundo filho imprimia de 2 a 200...
//Portanto, para criar 10 processos filhos "simultâneamente", não é possível garantir a ordem pelos quais
//são processados.