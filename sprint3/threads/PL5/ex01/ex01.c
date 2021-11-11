#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


int main() {

 pid_t pid = fork();
 
    if(pid == 0) {
    fork();
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, thread_func, NULL);
    pthread_join(thread_id, NULL);
    }

 fork();
 …
 }

 /** 

 a) 5 processos + pai = 6 processos

 b) 2 threads


 c) justificacao ver imagem

 */
