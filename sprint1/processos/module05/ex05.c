#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void) { 

  pid_t p = fork();
  pid_t p1;

  if(p>0){
    p1 = fork();
  }
  if(p==0){
    sleep(1);
    exit(1);
  }
  if(p1==0){
    sleep(2);
    exit(2);
  }
    return 0;
  } 

