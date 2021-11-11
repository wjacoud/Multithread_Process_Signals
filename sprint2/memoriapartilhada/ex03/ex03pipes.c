#include <stdio.h>
#include <stdlib.h> /*exit*/
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h> /*tipo de dados usados pela API*/
#include <time.h>

#define LEITURA 0
#define ESCRITA 1
#define LENGHT 100000

//Tipo de dados que o array irá ter
typedef struct {
    int x; //inteiro
    char y[20]; //string
} tipo_de_dados;

int main(int argc, char *argv[]) {
    int fd[2];
    int i;
    tipo_de_dados array[LENGHT];

    if (pipe(fd) == -1) {
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }

    for (i = 0; i < LENGHT; i++) {
        array[i].x = i;
        strcpy(array[i].y, "ISEP – SCOMP 2020");
    }

    pid_t p = fork();
    if (p == -1) {
        printf("An error ocurred with fork\n");
        return 2;
    }

    clock_t inicio = clock();

    if (p == 0) {
        close(fd[LEITURA]);
        if (write(fd[ESCRITA], array, sizeof(array)) == -1) {
            printf("An error ocurred with writing to the pipe\n");
            return 3;
        }
        close(fd[ESCRITA]);
        exit(EXIT_SUCCESS);
    }

    close(fd[ESCRITA]);

    if (read(fd[LEITURA], array, sizeof(array)) == -1) {
        printf("An error ocurred with reading from the pipe\n");
        return 3;
    }

    close(fd[LEITURA]);
    clock_t fim = clock();

    long double total_tempo = ((long double)(fim - inicio)) / CLOCKS_PER_SEC;

    printf("Começou a leitura de dados em %Lf segundos\n", (long double) inicio);
    printf("Terminou a leitura de dados em %Lf segundos\n", (long double) fim);
    printf("A transferência de dados em %Lf segundos\n", total_tempo);
    return 0;
}