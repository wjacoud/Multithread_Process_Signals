#include <stdio.h>
#include <stdlib.h> /*exit*/
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h> /*tipo de dados usados pela API*/
#include <fcntl.h>     /*opções de controlo de ficheiros*/
#include <sys/stat.h>  /*contantes utilizadas para modo de abertura*/
#include <sys/mman.h>  /*declarações relacionadas com a gestão de memória partilhada: shm_* e mmap()*/

#define LEITURA 0
#define ESCRITA 1
#define LENGHT 1000000

#define SHM_NAME "/shmpipes" // Nome da pasta mem partilhada

typedef struct {
    int x[LENGHT];
    int NewData;
} shared_data;

int main(int argc, char *argv[]) {
    int i, fd, r, fdp[2], array[LENGHT];
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;

    //   PIPE
    if (pipe(fdp) == -1) {
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }

    for (i = 0; i < LENGHT; i++) { // preenche o Array
        array[i] = i;
    }

    pid_t p = fork(); // Cria processo
    if (p == -1) {
        printf("An error ocurred with fork\n");
        return 2;
    }

    clock_t inicio = clock(); // Pulso onde começa a escrita

    if (p == 0) {
        close(fdp[LEITURA]);

//        A copiar o array inteiro em uma só operação levaria menos tempo, porém não seria o mesmo procedimento adotado na memória partilhada.

//        if (write(fdp[ESCRITA], array, sizeof(array)) == -1) {
//            printf("An error ocurred with writing to the pipe\n");
//            return 3;
//        }

        for (i = 0; i < LENGHT; i++) {
            if (write(fdp[ESCRITA], &array[i], sizeof(int)) == -1) {
                printf("An error ocurred with writing to the pipe\n");
                return 3;
            }
        }

        close(fdp[ESCRITA]);
        exit(EXIT_SUCCESS);
    }

    close(fdp[ESCRITA]);

//    if (read(fdp[LEITURA], array, sizeof(array)) == -1) {
//        printf("An error ocurred with reading from the pipe\n");
//        return 3;
//    }

    for (i = 0; i < LENGHT; i++) {
        if (read(fdp[LEITURA], &array[i], sizeof(int)) == -1) {
            printf("An error ocurred with reading from the pipe\n");
            return 3;
        }
    }

    close(fdp[LEITURA]);
    clock_t fim = clock(); // Pulso onde termina a leitura

    long double total_tempo = ((long double) (fim - inicio)) / CLOCKS_PER_SEC;

    printf("\tPIPES\n");
    printf("Começou a escrita de dados em %.Lf pulsos\n", (long double) inicio);
    printf("Terminou a leitura de dados em %.Lf pulsos\n", (long double) fim);
    printf("A transferência de dados em %.6Lf segundos\n\n", total_tempo);

    // SH MEM

    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); /*abrir canal, criar*/
    if (fd < 0) {
        perror("In shm_open()");
        exit(1);
    }

    ftruncate(fd, dataSize); /*tamanho da shm*/

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                       0); /* cria o mapeamento */
    dataPointer->NewData = 0;

    p = fork();
    if (p == -1) {
        printf("An error ocurred with fork\n");
        return 2;
    }

    inicio = clock();

    if (p > 0) { // Processo Pai
        for (i = 0; i < LENGHT; i++) { // executa escrita na SH MEM
            dataPointer->x[i] = array[i];
        }
        dataPointer->NewData = 1;
    } else {
        while (!dataPointer->NewData) //para não aceder a shm ao mesmo tempo
            ;
        for (i = 0; i < LENGHT; i++) { // executa leitura da SH MEM
            array[i] = dataPointer->x[i];
        }
        exit(EXIT_SUCCESS);
    }

    fim = clock();
    total_tempo = ((long double) (fim - inicio)) / CLOCKS_PER_SEC;

    printf("\tSHARED MEMORY\n");
    printf("Começou a escrita de dados em %.Lf pulsos\n", (long double) inicio);
    printf("Terminou a leitura de dados em %.Lf pulsos\n", (long double) fim);
    printf("A transferência de dados em %.6Lf segundos\n\n", total_tempo);

    r = munmap(dataPointer, dataSize); //fecha mapeamento
    if (r < 0)
        return 1; //verifica erro

    close(fd); //fecha a shm
    if (r < 0)
        return 1; //verifica erro

    r = shm_unlink(SHM_NAME);
    if (r < 0)
        return 1; //verifica erro
    return 0;
}