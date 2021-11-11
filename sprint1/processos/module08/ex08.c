#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>


int main(void) { 

  pid_t p;

  if(fork()==0){
    printf("1-PID = %d\n",getpid());
    exit(0);
  }

  if((p=fork())==0){
    printf("2-PID = %d\n",getpid());
    exit(0);
  }

  printf("Parent PID = %d\n",getpid());
  waitpid(p, NULL, 0);

  printf("Enter Loop...1n");
  while(1); 

  return 0;
} 

//a) Não estão todos os processos criados. Está apenas
//o processo pai e o processo 1 em modo zombie. O processo 2
//não surge porque terminou e o processo pai (de ambos), estava
//à espera dele para prosseguir com a sua própria execução. 
//O processo 1 surge, em modo zombie, porque o processo pai não captou a sua informação de
//"final" de execução quando o processo em si (filho), terminou antes do pai.

