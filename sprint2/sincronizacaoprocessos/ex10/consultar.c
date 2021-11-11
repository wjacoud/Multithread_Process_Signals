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

    printf("Insira o número:\n");
    scanf("%d", &numero);

    sem_wait(semaforo);
    int i, flag = 0;
    for(i = 0; i < TOTAL_REGISTOS; i++) {
        if(dataPointer->registos[i].numero == numero) {
            printf("Número: %d\n", dataPointer->registos[i].numero);
            printf("Nome: %s\n", dataPointer->registos[i].nome);
            printf("Endereço: %s\n\n", dataPointer->registos[i].endereco);
            flag = 1;
            break;
        }
    }
    if(flag == 0)
        printf("Registo número (%d) não encontrado\n", numero);

    sem_post(semaforo);

    if (sem_close(semaforo) == -1) { // Fecha Semaforo
        perror("Erro no sem_close()\n");
        exit(1);
    }

    return 0;
}