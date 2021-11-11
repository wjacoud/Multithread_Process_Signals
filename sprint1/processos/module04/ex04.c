#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void) { 
    int a=0, b, c, d; 
    b = (int) fork(); 
    c = (int) getpid(); /* getpid(), getppid(): unistd.h*/ 
    d = (int) getppid(); 
    a = a + 5; 
    printf("\na=%d, b=%d, c=%d, d=%d\n",a,b,c,d);
    return 0;
  } 

  //A) A única variável que o valor é igual para ambos os processos é a.
  // b vai ser 0 se filho, se pai tem outro valor (ou seja, pid do processo filho)
  // c vai ter o valor do pid conforme o processo em que está a ser executada a linha de código
  // d vai ter o valor do ppid do (pid do processo pai) conforme o processo em que está a ser executada essa linha de código
  
  //b) a desenhar