#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h> /*tipo de dados usados pela API*/
#include <fcntl.h>     /*opções de controlo de ficheiros*/
#include <sys/stat.h>  /*contantes utilizadas para modo de abertura*/
#include <sys/mman.h>  /*declarações relacionadas com a gestão de memória partilhada: shm_* e mmap()*/

#define TOTAL_LENGHT 1000
#define LENGHT 1
#define CHILD LENGHT
#define SHM_NAME "/shmEEEEx10"

typedef struct {
    int array[LENGHT];
} shared_data;

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
    int i, fd, r;
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;

    srand((unsigned) time(&t));
    int arrayMaster[TOTAL_LENGHT];
    for (i = 0; i < TOTAL_LENGHT; i++) {
        arrayMaster[i] = rand() % 1000; // Preenche Array
    }

    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); /*abrir canal, criar*/
    if (fd < 0) {
        perror("In shm_open()");
        exit(1);
    }

    ftruncate(fd, dataSize); /*tamanho da shm*/

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */

    clock_t inicio = clock(); // Pulso onde começa a criar filhos

    int id = cria_filhos(CHILD); // Cria filhos

    if (id == -1) {
        perror("Fork Error");
        exit(1);
    }

    if (id > 0) {  // Processo Filho
        int i;
        int max = arrayMaster[(id - 1) * (1000 / CHILD)]; // primeiro elemento do array

        for (i = (id - 1) * (1000 / CHILD); i < id * (1000 / CHILD); i++) {
            if (max < arrayMaster[i]) {
                max = arrayMaster[i];
            }
        }

        dataPointer->array[id - 1] = max; // armazena o maximo valor

        r = munmap(dataPointer, dataSize); //fecha mapeamento
        if (r < 0) exit(1); //verifica erro

        close(fd); //fecha a shm
        if (r < 0) exit(1); //verifica erro

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

        int max = dataPointer->array[0];

        for (i = 0; i < LENGHT; i++) {
            if (max < dataPointer->array[i]) {
                max = dataPointer->array[i];
            }
        }

        clock_t fim = clock(); // Pulso onde Encontra o Max

        printf("Max number : %d\n", max);

        long double total_tempo = ((long double)(fim - inicio)) / CLOCKS_PER_SEC;

        printf("\tRelatorio Performance\n");
        printf("Começou a escrita de dados em %.Lf pulsos\n", (long double) inicio);
        printf("Terminou a leitura de dados em %.Lf pulsos\n", (long double) fim);
        printf("A transferência de dados em %.6Lf segundos\n\n", total_tempo);

        r = munmap(dataPointer, dataSize); //fecha mapeamento
        if (r < 0) exit(1); //verifica erro

        close(fd); //fecha a shm
        if (r < 0) exit(1); //verifica erro

        r = shm_unlink(SHM_NAME);
        if (r < 0) exit(1); //verifica erro
    }
    return 0;
}

// A melhor performance do sistema se dá com a criação de apenas um processo filho. Conforme pode ser atestado a executar o programa.
// Isso acontece pois a arquitetura multiprocessador têm o seguinte problema que afeta a sua performance:
// dois ou mais processadores não podem aceder à mesma zona de memória a que outro processador está a aceder para escrita.
