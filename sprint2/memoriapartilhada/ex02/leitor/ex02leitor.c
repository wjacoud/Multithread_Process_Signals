#include <stdio.h>
#include <stdlib.h> /*exit*/
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/types.h> /*tipo de dados usados pela API*/
#include <fcntl.h>     /*opções de controlo de ficheiros*/
#include <sys/stat.h>  /*contantes utilizadas para modo de abertura*/
#include <sys/mman.h>  /*declarações relacionadas com a gestão de memória partilhada: shm_* e mmap()*/

#define SHM_NAME "/shmproduto"

typedef struct
{
    int cod;
    float preco;
    char descricao[100];
    int NewData;

} produto;

int main(int argc, char *argv[])
{

    int fd, r;
    int dataSize = sizeof(produto);
    produto *dadosProduto;

    /*
     * O_CREAT - cria se ainda não existir
     * O_EXCL - (com o O_CREAT) cria apenas, devolve erro se já existir
     * O_RDWR - abre para leitura e para escrita
     * ........
     * mode define at <sys/stat.h>
     * S_IRUSR - read permission, owner
     * S_IWUSR - write permission, owner
     */

    /*abrir canal já existente, não criar*/
    fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("In shm_open()");
        exit(1);
    }

    ftruncate(fd, dataSize); /*indica o tamanho da shm*/

    dadosProduto = (produto *)mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */

    //isto serve para não aceder a shm ao mesmo tempo?
    while (!dadosProduto->NewData)
        ;

    printf("O codigo do produto: %d\n", dadosProduto->cod);
    printf("A descricao do produto: %s\n", dadosProduto->descricao);
    printf("O preco do produto: %.2f €\n", dadosProduto->preco);


    r = munmap(dadosProduto, dataSize); //fecha mapeamento
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
