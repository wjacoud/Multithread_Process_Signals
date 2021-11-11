#include "librariaUtil.h"

int main(int argc, char *agrv[]){

    sem_t *semaforo;
    int fd;
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;

    if((semaforo = sem_open(SEMAFORO_FILE_NAME, 0)) == SEM_FAILED){
        perror("Erro ao abrir o semaforo\n");
        exit(1);
    }

    fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR); /*abrir canal*/
    if (fd < 0) {
        perror("In shm_open()");
        exit(1);
    }


    ftruncate(fd, dataSize); /*tamanho da shm*/

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */

    int numero;
    char nome[100];
    char endereco[150];

    sem_wait(semaforo);

    printf("Insira o número:\n");
    scanf("%d", &numero);
    printf("Insira o nome:\n");
    scanf("%s", nome);
    printf("Insira o endereco:\n");
    scanf("%s", endereco);
    printf("\n");


    int i, flag = 0;
    for(i = 0; i < TOTAL_REGISTOS; i++) {
        if(dataPointer->registos[i].numero == numero) {
            printf("Número já existe na base de dados\n");
            flag = 1;
            break;
        }
    }
    if(flag == 0){
        for (i = 0; i < TOTAL_REGISTOS; i++) {
            if (dataPointer->registos[i].numero == 0) {
                dataPointer->registos[i].numero = numero;
                strcpy(dataPointer->registos[i].nome, nome);
                strcpy(dataPointer->registos[i].endereco, endereco);
                break;
            }
        }
        printf("Registo inserido com sucesso!\n");
    }

    sem_post(semaforo);

    if (sem_close(semaforo) == -1) { // Fecha Semaforo
        perror("Erro no sem_close()\n");
        exit(1);
    }

    return 0;
}