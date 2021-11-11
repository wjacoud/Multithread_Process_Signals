#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void) { 

  int i;
  int status;

  for(i = 0; i<4; i++){

    //Se for filho 
    if(fork()==0){
      sleep(1);
      
    }
  }

  printf("This is the end. \n");

    return 0;
  } 

//a) São criados 16 processos. Ver ficheiro png nesta pasta.

// b) Para ter exatamente 4 processos filhos, bastaria dar um exit
// após o sleep. Dessa forma, o processo que teria sido criado pelo fork (o filho)
// terminava antes de ser incrementado o ciclo for nesse processo filho.

//c) 

int main(void) { 

  int i;
  int status;
  int v[4]; //Vetor que vai guardar o PID dos processos filhos
  for(i = 0; i<4; i++){

    
    v[i] = fork(); //Cria o processo filho e guarda o pid do processo filho
    
    //Se estivermos dentro do processo filho
    if(v[i] == 0){
      sleep(1);
      exit(1);  //exit(i + 1) para concluir a alínea d)
      //faz o que tem a fazer para os filhos
    }
  }

  //Tratando agora do processo pai
  for(i = 0; i<4; i++){

    if(v[i] % 2 == 0){ //Se PID do processo filho, guardado no array, for par

      waitpid(v[i],&status,0); //O pai vai esperar pelo processo com o pid[i], par.
      
    }

  }


  printf("This is the end. \n");

    return 0;
  } 
