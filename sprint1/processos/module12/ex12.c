#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>


int spawn_childs(int n){
  int i;
  pid_t p;

  for(i=0;i<n;i++){

    p = fork();

    if(p==0){   //ESTÁ DENTRO DO PROCESSO FILHO
      printf("Sou o filho nº %d. \n",i+1);
      return i+1;   //Return do valor index do filho (processo filho 1, return 1)
    }    
  }

  return 0; //Retorna 0 pois obrigatoriamente, se chega aqui, é o PAI
}


int main(void) { 

  int x=0,i,status;

  x=spawn_childs(6);  //Cria os 6 processos filhos
  
  if(x==0){    //Se x = 0 significa que a estamos no pai, pois a função retorna 0 para o pai
   
   for(i=0;i<6;i++){   //Para os 6 processos filhos, o pai vai esperar que todos terminem
    wait(&status);
 
  }
  printf("Sou o pai, esperei por todos! \n"); 

  }else{   //Se não (se x for diferente de 0), estamos no filho

    printf("Index do processo filho * 2 : %d. \n", x*2);
    exit(x*2);

  }


  return 0;
} 

