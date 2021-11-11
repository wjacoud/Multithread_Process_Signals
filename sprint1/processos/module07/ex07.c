#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define ARRAY_SIZE 1000
int main(void) { 

  int numbers[ARRAY_SIZE];
  int n, status;
  time_t t;
  int i;

  int total = 0; //contador

  srand((unsigned) time (&t));

  for( i=0; i<ARRAY_SIZE; i++){
    numbers[i] = rand() % 10000;
  }
  
  n = rand() % 10000;


  pid_t p = fork();

  if(p==0){

    for(i = 0; i<ARRAY_SIZE/2; i++){
      if(numbers[i]==n){
        total++;
      }
    }
    exit(total);
  }

  total = WEXITSTATUS(status);
  for(i = ARRAY_SIZE/2; i<ARRAY_SIZE; i++){
    if(numbers[i]==n){
      total++;
    }
  }

  printf("O valor de ocorrências do número %d foi %d \n", n, total);

  return 0;
} 

