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

#include <sys/types.h> /*tipo de dados usados pela API*/
#include <fcntl.h>     /*opções de controlo de ficheiros*/
#include <sys/stat.h>  /*contantes utilizadas para modo de abertura*/
#include <sys/mman.h>  /*declarações relacionadas com a gestão de memória partilhada: shm_* e mmap()*/


#define SEMAFORO_FILE_NAME "Ex03"
#define SEMAFORO_INITIAL_VALUE 1
#define SHM_NAME "/shmex03sp" // Nome da pasta mem partilhada

typedef struct {
    char arrayString [50][80];
    int myLine;
} shared_data;

int cria_filhos(int n) {
    pid_t p;
    int i = 0;
    for (i = 0; i < n; ++i) {
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
    int i, fd, r;
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;
    pid_t p;

    time_t t;
    srand((unsigned) time(&t));

    if ((semaforo = sem_open(SEMAFORO_FILE_NAME, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) == SEM_FAILED) { // Abre o semaforo
        perror("Erro ao abrir o semaforo\n");
        exit(1);
    }

    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); /*abrir canal, criar*/
    if (fd < 0) {
        perror("In shm_open()");
        exit(1);
    }

    ftruncate(fd, dataSize); /*tamanho da shm*/

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */
    dataPointer->myLine=0;

    p = cria_filhos(50);

    if(p<0){
        printf("Erro ao criar filhos");
        exit(2);
    }

    //processos filhos
    if(p>0){

        //decrementar semaforo
        sem_wait(semaforo);
        printf("%d - comecando a escrever na SHM \n", p);
        int line = dataPointer->myLine;
        sprintf(dataPointer->arrayString[line], "[%d]Sou o filho n. %d e com o processo n.%d",line, p, getpid());
        dataPointer->myLine++;
        //int n = rand() % 5;
        //sleep(n);
        sleep(0.1);
        //incrementar semaforo
        sem_post(semaforo);
        exit(1);

    }

    //processo pai

    p = fork();
    //filho
    if(p==0){
    //VAI DECREMENTAR A MYLINE
    //decrementar semaforo
        sem_wait(semaforo);
        printf("REMOVEDOR - comecando a apagar na SHM \n");
        dataPointer->myLine--;
        //incrementar semaforo
        sem_post(semaforo);
        exit(1);

    }

    
    for (i = 0; i < 51; i++) { // espera os filhos para iniciar
        wait(NULL);
    }

    for (i = 0; i < 50; i++) { 
        printf("%s \n",dataPointer->arrayString[i]);
    }


    if (sem_close(semaforo) == -1) { // Fecha Semaforo
            perror("Erro no sem_close()\n");
            exit(1);
    }

    if (sem_unlink(SEMAFORO_FILE_NAME) == -1) {  // Remove Semaforo
            perror("Erro no sem_unlink()\n");
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
