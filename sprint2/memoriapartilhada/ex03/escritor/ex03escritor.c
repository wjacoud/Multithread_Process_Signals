#include <stdio.h>
#include <stdlib.h> /*exit*/
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/types.h> /*tipo de dados usados pela API*/
#include <fcntl.h>     /*opções de controlo de ficheiros*/
#include <sys/stat.h>  /*contantes utilizadas para modo de abertura*/
#include <sys/mman.h>  /*declarações relacionadas com a gestão de memória partilhada: shm_* e mmap()*/
#include <time.h>

#define SHM_NAME "/shm100000"

//Tipo de dados que o array da shared memory irá ter
typedef struct
{
    int x; //inteiro
    char y[20]; //string

} tipo_de_dados;

//Informação utilizada pela memória partilhada
typedef struct
{
    tipo_de_dados abc[100000];
    int NewData;
    clock_t inicio_escrita;
    clock_t fim_escrita;

} shared_data;

int main(int argc, char *argv[])
{

    int fd,r;
    int k = 0;
    int dataSize = sizeof(shared_data);
    shared_data *dadosDados;

    /*
     * O_CREAT - cria se ainda não existir
     * O_EXCL - (com o O_CREAT) cria apenas, devolve erro se já existir
     * O_RDWR - abre para leitura e para escrita
     * ........
     * mode define at <sys/stat.h>
     * S_IRUSR - read permission, owner
     * S_IWUSR - write permission, owner
     */

    /*abrir canal, criar*/
    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("In shm_open()");
        exit(1);
    }

    ftruncate(fd, dataSize); /*indica o tamanho da shm*/

    /*
        * PROT_READ|PROT_WRITE - permissões de leitura e de escrita
        * MAP_SHARED - disponível para outros processos
        * 
    */
    dadosDados = (shared_data *)mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */

    //inicialmente a 0 para mecanismo de espera ativa
    dadosDados->NewData = 0;

    //inicialmente preenche-se um array com 100 000 erstruturas do tipo "tipo_de_dados"
    tipo_de_dados array[100000];

    for(k=0; k<100000; k++){
        array[k].x=k;
        strcpy(array[k].y, "ISEP – SCOMP 2020");
    }

    printf("ENTER para preencher os dados...\n");
    getchar(); //aguarda alguma tecla para começar o processo de cópia através de memória partilhada

    dadosDados->inicio_escrita=clock();

    for(k=0; k<100000; k++){
        dadosDados->abc[k].x=array[k].x;
        strcpy(dadosDados->abc[k].y,array[k].y);
    }

    dadosDados->fim_escrita=clock();
    
    dadosDados->NewData = 1;

    r = munmap(dadosDados, dataSize); //fecha mapeamento
    if (r < 0)
        return 1; //verifica erro

    close(fd); //fecha a shm
    if (r < 0)
        return 1; //verifica erro


    printf("<escritor end>\n");

    return 0;
}