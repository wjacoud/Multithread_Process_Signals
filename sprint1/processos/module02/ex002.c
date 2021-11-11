#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void) {

printf("I'm...\n");
pid_t p = fork(); //fork retorna 0 para o filho

if(p==0){
    printf("I'll never join you!\n");
    exit(EXIT_SUCCESS);
}
else {
    printf("the...\n");
    p = fork();
    if(p==0){
         printf("I'll never join you!\n");
         exit(EXIT_SUCCESS);
    } 
    else {
        printf("father...\n");
        p = fork();
        if(p==0){
         printf("I'll never join you!\n");
         exit(EXIT_SUCCESS); //opcional neste exercício, serve para terminar processo
        }
    }
}
return 0;
}