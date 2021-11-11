#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void) { 
    fork(); fork(); fork(); 
    printf("SCOMP!\n"); 
    return 0;
  } 

  //a) 8, o nº de processos é 2^nº de forks
  //b) DESENHAR
  //c) 8x