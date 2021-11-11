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

typedef struct
{
    int x;
    char y[20];

} tipo_de_dados;

typedef struct
{
    tipo_de_dados abc[100000];
    int NewData;
    clock_t inicio_escrita;
    clock_t fim_escrita;

} shared_data;

int main(int argc, char *argv[])
{

    int fd, r;
    int k = 0;
    clock_t inicio_leitura;
    clock_t fim_leitura;
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
    fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
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

    tipo_de_dados array[100000];

    while (!dadosDados->NewData)
        ;

    inicio_leitura = clock();


    for (k = 0; k < 100000; k++)
    {
        array[k].x = dadosDados->abc[k].x;
        strcpy(array[k].y, dadosDados->abc[k].y);
    }

    fim_leitura = clock();

    //cálculo do valor total do tempo da transferencia de dados, ou seja, tempo a escrever + tempo a ler a partir da shm
    long double total_tempo = ((long double)((dadosDados->fim_escrita - dadosDados->inicio_escrita) + (fim_leitura - inicio_leitura))) / CLOCKS_PER_SEC;


    printf("Terminou a tranferencia de dados em %Lf segundos\n", total_tempo);

    r = munmap(dadosDados, dataSize); //fecha mapeamento
    if (r < 0)
        return 1; //verifica erro

    close(fd); //fecha a shm
    if (r < 0)
        return 1; //verifica erro

    r = shm_unlink(SHM_NAME);
    if (r < 0)
        return 1; //verifica erro

    printf("<leitor termina>\n");

    return 0;
}
