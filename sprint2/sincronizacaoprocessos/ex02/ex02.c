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

#define SEMAFORO_INITIAL_VALUE 0
#define NUMERO_DE_SEMAFOROS 8
#define IN_FILE_NAME "Numbers.txt"
#define OUT_FILE_NAME "Output.txt"
#define NUMBERS_PER_CHILD 200
#define CHILD 8

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

int main(int argc, char *agrv[]) {
    sem_t *semaforo[NUMERO_DE_SEMAFOROS];
    FILE *fr, *fw;
    int i, tmp;
    char chTmp;
    char semName[20];

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) { // Cria os semaforos
        sprintf(semName, "Eeeeex2sem%d", i+1);
        if ((semaforo[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) {
            perror("Erro ao abrir o semaforo");
            exit(1);
        }
    }
    sem_post(semaforo[0]); // Incrementa sem do primeiro processo filho

    int pid = cria_filhos(CHILD); // Cria filhos
    if (pid == -1) {
        perror("Erro ao criar Filhos");
        exit(1);
    }

    if (pid > 0) {  // Processo Filho
        sem_wait(semaforo[pid - 1]);
        fr = fopen(IN_FILE_NAME, "r");
        if (pid == 1)
            fw = fopen(OUT_FILE_NAME, "w");
        else
            fw = fopen(OUT_FILE_NAME, "a");
        int end = 200 * (1 + pid), start = 200 * (pid), count = start; //contador dos numeros

        fprintf(fw, "\tProcesso Filho :%d\n", pid);

        while (!feof(fr)) {
            fscanf(fr, "%d", &tmp);
            if (count >= start) {
                fprintf(fw, "%d\n", tmp);
            }
            if (count >= end) break;
            count++;
        }
        printf("Filho %d", pid);
        fclose(fr);
        fclose(fw);

        if(pid < CHILD)
            sem_post(semaforo[pid]);

        exit(0);
    }
    int status = 0;

    for (i = 0; i < CHILD; ++i) { // espera os filhos para iniciar
        wait(&status);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            perror("Erro no processo filho");
            exit(1);
        }
    }

    fw = fopen(OUT_FILE_NAME, "r");

    while ((chTmp = fgetc(fw)) != EOF) {
        printf("%c", chTmp);
    }

    fclose(fw);

    for (i = 0; i < NUMERO_DE_SEMAFOROS; i++) {
        sprintf(semName, "Eeeeex2sem%d", i + 1);
        if (sem_close(semaforo[i]) == -1) { // Fecha Semaforo
            perror("Erro no sem_close()\n");
            exit(1);
        }

        if (sem_unlink(semName) == -1) {  // Remove Semaforo
            perror("Erro no sem_unlink()\n");
            exit(1);
        }
    }

    return 0;
}
