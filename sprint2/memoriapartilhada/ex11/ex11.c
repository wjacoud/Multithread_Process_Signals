#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#define TOTAL_LENGHT 1000
#define LENGHT 10
#define CHILD LENGHT
#define LEITURA 0
#define ESCRITA 1

int cria_filhos(int n) {
    pid_t p;
    for (int i = 0; i < n; ++i) {
        p = fork();

        if (p < 0)
            return -1;
        else if (p == 0)
            return i + 1;
    }
    return 0;
}

int main(void) {

    time_t t;
    int i, fdp[2], max = 0, tmp = 0;

    srand((unsigned) time(&t));
    int arrayMaster[TOTAL_LENGHT];
    for (i = 0; i < TOTAL_LENGHT; i++) {
        arrayMaster[i] = rand() % 1000; // Preenche Array
    }

    clock_t inicio = clock(); // Pulso onde começa a criar filhos

    if (pipe(fdp) == -1) {
        perror("An error ocurred with opening the pipe");
        exit(1);
    }

    int id = cria_filhos(CHILD); // Cria filhos

    if (id == -1) {
        perror("Fork Error");
        exit(1);
    }

    if (id > 0) {  // Processo Filho
        int i;
        max = arrayMaster[(id - 1) * (1000 / CHILD)]; // primeiro elemento do array

        for (i = (id - 1) * (1000 / CHILD); i < id * (1000 / CHILD); i++) {
            if (max < arrayMaster[i]) {
                max = arrayMaster[i];
            }
        }

        close(fdp[LEITURA]);
        if (write(fdp[ESCRITA], &max, sizeof(int)) == -1) {
            perror("An error ocurred with writing to the pipe");
            exit(1);
        }
        close(fdp[ESCRITA]);

        exit(0);

    } else { // Processo pai

        int status = 0;

        for (i = 0; i < CHILD; ++i) { // espera os filhos para iniciar
            wait(&status);
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                perror("Erro no processo filho");
                exit(1);
            }
        }

        close(fdp[ESCRITA]);

        if (read(fdp[LEITURA], &max, sizeof(int)) == -1) {
            perror("An error ocurred with reading from the pipe");
            exit(1);
        }

        for (i = 1; i < LENGHT; i++) {
            if (read(fdp[LEITURA], &tmp, sizeof(int)) == -1) {
                perror("An error ocurred with reading from the pipe");
                exit(1);
            }
            if (max < tmp) {
                max = tmp;
            }
        }

        close(fdp[LEITURA]);

        clock_t fim = clock(); // Pulso onde Encontra o Max

        printf("Max number : %d\n", max);

        long double total_tempo = ((long double)(fim - inicio)) / CLOCKS_PER_SEC;

        printf("\tRelatorio Performance\n");
        printf("Começou a escrita de dados em %.Lf pulsos\n", (long double) inicio);
        printf("Terminou a leitura de dados em %.Lf pulsos\n", (long double) fim);
        printf("A transferência de dados em %.6Lf segundos\n\n", total_tempo);

    }
    return 0;
}
