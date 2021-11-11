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

#define SHM_NAME "/shmEEx12"
#define STR_SIZE 50
#define NR_DISC 10

typedef struct {
    int numero;
    char nome[STR_SIZE];
    int disciplinas[NR_DISC];
    int NewData;
} shared_data;

int main(void) {

    int i, fd, r;
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;
    pid_t p;

    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); /*abrir canal, criar*/
    if (fd < 0) {
        perror("In shm_open()");
        exit(1);
    }

    ftruncate(fd, dataSize); /*tamanho da shm*/

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */

    p = fork(); // cria somente um processo filho
    if (p == -1) {
        perror("Fork Error");
        exit(1);
    }

    if (p > 0){  // Processo Pai

        dataPointer->NewData = 0;

        // Processo pai insere os dados na memória partlhada
        printf("Insira o número do aluno: \n");
        scanf("%d", &dataPointer->numero);

        printf("Insira o nome do aluno: \n");
        scanf("%s", dataPointer->nome);

        for (i = 0; i < NR_DISC; i++){ // numero de disciplinas
            printf("Insira a nota da %dª disciplina: \n", i+1);
            scanf("%d", &dataPointer->disciplinas[i]);
        }

        dataPointer->NewData = 1;
    } else {
        while (!dataPointer->NewData) // Processo filho espera o final da insercao pelo processo pai
            ;
        dataPointer->NewData = 0;

        int max = dataPointer->disciplinas[0];
        int min = dataPointer->disciplinas[0];
        int avg, sum = dataPointer->disciplinas[0];

        for (i = 1; i < NR_DISC; i++) {  // Compara menor valor, maior e soma as notas para obter-se a media
            if(max < dataPointer->disciplinas[i]) max = dataPointer->disciplinas[i];
            if(min > dataPointer->disciplinas[i]) min = dataPointer->disciplinas[i];
            sum += dataPointer->disciplinas[i];
        }
        avg = sum / NR_DISC;

        printf("Aluno: %s\tNúmero: %d\n", dataPointer->nome, dataPointer->numero);
        printf("Nota mínima: %d\tNota máxima: %d\tMédia: %d\n", min, max, avg);

        dataPointer->NewData = 1;

        exit(0);
    }
    int status = 0;

    wait(&status); // Espera o termino do processo filho

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        perror("Erro no processo filho");
        exit(1);
    }

    r = munmap(dataPointer, dataSize); //fecha mapeamento
    if (r < 0) exit(1); //verifica erro

    close(fd); //fecha a shm
    if (r < 0) exit(1); //verifica erro

    r = shm_unlink(SHM_NAME);
    if (r < 0) exit(1); //verifica erro

    return 0;
}