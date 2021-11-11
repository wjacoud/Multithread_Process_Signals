#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define PIPES 6
#define LEITURA 0
#define ESCRITA 1

/* Estrutura Produto*/
struct produto
{
    char nome[20];
    float preco;
    int cod_barras;
};

int main()
{

    struct produto produtos[5];
    /* Colocando produtos na base de dados */
    strcpy(produtos[0].nome, "Coca-Cola");
    produtos[0].preco = 1.5;
    produtos[0].cod_barras = 1;
    strcpy(produtos[1].nome, "Detergente da loica");
    produtos[1].preco = 1.99;
    produtos[1].cod_barras = 2;
    strcpy(produtos[2].nome, "Queijo parmesao");
    produtos[2].preco = 2.20;
    produtos[2].cod_barras = 3;
    strcpy(produtos[3].nome, "Fiambre");
    produtos[3].preco = 1.19;
    produtos[3].cod_barras = 4;
    strcpy(produtos[4].nome, "Vinho do Alentejo");
    produtos[4].preco = 3;
    produtos[4].cod_barras = 5;

    //PIPE FD[0][X] vai ser o pipe utilizado para os filhos requisitarem informacao ao pai
    //ou seja, os filhos apenas escrevem nesse pipe, não lem, e o pai apenas lê e não escreve

    //PIPE FD[i+1][X] vai ser o pipe que o pai utiliza para enviar informacao do produto aos filhos
    //ou seja, o pai apenas escreve nesse pipe, o pai nao o lê e os filhos apenas leem desse pipe, os filhos nao escrevem
    int fd[PIPES][2], i, j, indice_, existe;
    pid_t pid;
    int cod_barras;

    //CRIAR OS PIPES
    for (i = 0; i < PIPES; i++)
    {
        if (pipe(fd[i]) == -1)
        {
            printf("An error ocurred with opening the pipe\n");
            return EXIT_FAILURE;
        }
    }
    //CRIAR OS FILHOS
    for (i = 0; i < 5; i++)
    {
        pid = fork();
        if (pid == -1)
        {
            printf("An error ocurred with fork\n");
            return EXIT_FAILURE;
        }
        if (pid == 0)
        {
            //FILHO

            //Filhos nao precisam de ler neste pipe, apenas escrever para pedir a informacao ao pai
            close(fd[0][LEITURA]);
            //Filhos nao precisam de escrever no pipe que lhes traz a informacao do produto
            close(fd[i + 1][ESCRITA]);

            printf("Filho %d - Insira o codigo de barras (1,2,3,4 ou 5):\n", i + 1);
            scanf("%d", &cod_barras);

            //Solicita a informacao ao pai, escrevendo no pipe que envia esse pedido ao pai. Envia tanto o cod barras como o indice do processo (filho 1 = indice 0)
            if (write(fd[0][ESCRITA], &cod_barras, sizeof(int)) == -1)
            {
                printf("An error ocurred with writing to the pipe\n");
                return EXIT_FAILURE;
            }
            if (write(fd[0][ESCRITA], &i, sizeof(int)) == -1)
            {
                printf("An error ocurred with writing to the pipe\n");
                return EXIT_FAILURE;
            }

            //Vai receber os dados solicitados no pipe correspondente ao filho em questão, como pipe fd[0][x] é de escrever para o pai, pipe fd[1][x] é filho 1 (cujo indice de filho 1 é 0)
            struct produto produto;
            if (read(fd[i + 1][LEITURA], &produto, sizeof(struct produto)) == -1)
            {
                printf("An error ocurred with reading to the pipe\n");
                return EXIT_FAILURE;
            }

            printf("Leitor %d - Nome: %s\nPreco: %f\n", i + 1, produto.nome, produto.preco);

            //Já não precisa de requisitar informacao ao pai, logo ja nao tem que escrever nesse pipe
            close(fd[0][ESCRITA]);
            //Já não precisa de receber a informacao dos produtos pelo seu pipe unico com pai
            close(fd[i + 1][LEITURA]);
            exit(0);
        }
        if (pid > 0)
        {

            //O pai vai buscar a informacao ao pipe de qual o produto e qual o filho que lhe pediu
            if (read(fd[0][LEITURA], &cod_barras, sizeof(int)) == -1)
            {
                printf("An error ocurred with reading to the pipe\n");
                return EXIT_FAILURE;
            }
            if (read(fd[0][LEITURA], &indice_, sizeof(int)) == -1)
            {
                printf("An error ocurred with reading to the pipe\n");
                return EXIT_FAILURE;
            }

            //Valida se existe tal produto

            existe = 0;

            for (j = 0; j < 5; j++)
            {

                if (cod_barras == produtos[j].cod_barras)
                {
                    existe = 1;
                    //Se produto existir, escreve a informacao do produto no pipe dedicado ao filho em questao , pipe indice + 1
                    if (write(fd[indice_ + 1][1], &produtos[j], sizeof(struct produto)) == -1)
                    {
                        printf("An error ocurred with writing to the pipe\n");
                        return EXIT_FAILURE;
                    }
                }
            }

            if (existe == 0)
            {
                //Se não foi encontrado nenhum produto
                struct produto produto;
                //é criado um produto com o nome not found, custo 0 , para que o filho possa imprimir essa informacao
                strcpy(produto.nome, "Not Found!\n");
                produto.preco = 0;
                //envia esse produto nao existe ao filho
                if (write(fd[indice_ + 1][ESCRITA], &produto, sizeof(struct produto)) == -1)
                {
                    printf("An error ocurred with writing to the pipe\n");
                    return EXIT_FAILURE;
                }
            }
        }
    }

    return 0;
}