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

#define SEMAFORO_FILE_NAME "Eeeeeeex01"
#define SEMAFORO_INITIAL_VALUE 1
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
    sem_t *semaforo;
    FILE *fr, *fw;
    int tmp;
    char chTmp;

    if ((semaforo = sem_open(SEMAFORO_FILE_NAME, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) ==
        SEM_FAILED) { // Abre o semaforo
        if(semaforo = sem_open(SEMAFORO_FILE_NAME, 0) == SEM_FAILED){
            perror("Erro ao abrir o semaforo\n");
            exit(1);
        }
    }

    int pid = cria_filhos(CHILD); // Cria filhos
    if (pid == -1) {
        perror("Erro ao criar Filhos");
        exit(1);
    }

    if (pid > 0) {  // Processo Filho
        sem_wait(semaforo);
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

        fclose(fr);
        fclose(fw);

        sem_post(semaforo);

        exit(0);
    }
    int status = 0;

    for (int i = 0; i < CHILD; ++i) { // espera os filhos para iniciar
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

    if (sem_close(semaforo) == -1) { // Fecha Semaforo
        perror("Error at sem_close()!\n");
        exit(1);
    }

    if (sem_unlink(SEMAFORO_FILE_NAME) == -1) {  // Remove Semaforo
        perror("Error at sem_unlink()!\n");
        exit(1);
    }

    return 0;
}
