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

    int fd;
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
    dadosProduto = (produto *)mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */

    //isto serve para não aceder a shm ao mesmo tempo?
    dadosProduto->NewData = 0;
    printf("Insira o código do produto: \n");
    scanf("%d", &dadosProduto->cod);

    printf("Insira a descricao do produto: \n");
    scanf("%s", dadosProduto->descricao);

    printf("Insira o preço do produto: \n");
    scanf("%f", &dadosProduto->preco);

    dadosProduto->NewData = 1;

    printf("<escritor end>\n");

    return 0;
}