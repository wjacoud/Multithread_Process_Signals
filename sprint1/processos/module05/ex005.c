#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void) { 

  pid_t p = fork();
  
  if(p>0){
    pid_t p1 = fork();
    if(p1==0){
      sleep(2);
      exit(2);
    }
  }else{
    sleep(1);
    exit(1);
  }
  int status,status1;
  wait(&status);
  printf("%d \n",WEXITSTATUS(status));
  wait(&status1);
  printf("%d \n",WEXITSTATUS(status1));
  return 0;
  } 

// int i;
// for(i = 1; i <= 2; i++){

//   pid_t pid = fork();

//   if(pid == 0){
//     sleep(i);
//     exit(i);
//   }  
// }
