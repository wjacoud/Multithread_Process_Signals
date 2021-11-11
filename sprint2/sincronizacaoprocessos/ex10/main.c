#include "librariaUtil.h"

void executar(char *programa){
    pid_t p = fork();

    if(p < 0){
        printf("ERRO no fork");
        exit(EXIT_FAILURE);
    }
    //utilizando o fork com o comando execpl conseguimos executar outro executavel
    //sem que seja terminado o processo atual
    if(p == 0){
        execlp(programa, programa, (char *)NULL);
        exit(EXIT_FAILURE);
    }

    while(wait(NULL)>0){
        //espera que todos os filhos terminem
    }
}

int menu() {
    int opcao;
    printf("1. Consultar um registo \n");
    printf("2. Inserir informacao \n");
    printf("3. Consultar todos os registos \n");
    printf("0. Sair \n");
    scanf("%d", &opcao);
    return opcao;
}

int main(int argc, char *agrv[]) {


    sem_t *semaforo;
    int fd, r;
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;

    if ((semaforo = sem_open(SEMAFORO_FILE_NAME, O_CREAT | O_EXCL, 0644, SEMAFORO_INITIAL_VALUE)) ==
        SEM_FAILED) { // Abre o semaforo
        if((semaforo = sem_open(SEMAFORO_FILE_NAME, 0)) == SEM_FAILED){
            perror("Erro ao abrir o semaforo\n");
            exit(1);
        }
    }

    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); /*abrir canal, criar*/
    if (fd < 0) {
        fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
        if (fd < 0){
            perror("In shm_open()");
            exit(1);
        }
    }

    ftruncate(fd, dataSize); /*tamanho da shm*/

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */


    int opcao;

    do{
        opcao = menu();

        switch(opcao){
            case 0: break;
            case 1: executar("./consultar.bin");
                    break;
            case 2: executar("./inserir.bin");
                    break;
            case 3: executar("./consultar_todos.bin");
                    break;
            default: opcao = menu();
        }

    }while(opcao!=0);


    
    r = munmap(dataPointer, dataSize); //fecha mapeamento
    if (r < 0) exit(1); //verifica erro

    close(fd); //fecha a shm
    if (r < 0) exit(1); //verifica erro

    r = shm_unlink(SHM_NAME);
    if (r < 0) exit(1); //verifica erro


    if (sem_close(semaforo) == -1) { // Fecha Semaforo
            perror("Erro no sem_close()\n");
            exit(1);
    }

    if (sem_unlink(SEMAFORO_FILE_NAME) == -1) {  // Remove Semaforo
            perror("Erro no sem_unlink()\n");
            exit(1);
    }

    return 0;
}