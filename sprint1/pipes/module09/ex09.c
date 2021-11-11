#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define LEITURA 0
#define ESCRITA 1

#define NUM_CHILD 10
#define SIZE 50000

struct venda
{
    int customer_code;
    int product_code;
    int quantity;
};

int main(int argc, char *argv[]) {
    int i, j, n = 0, codigo, status;
    int produtos[SIZE];
    struct venda vendas[SIZE];
    int fd[2];
    pid_t p;

    for (i = 0; i < SIZE; i++) { // Preenche array de vendas
        vendas[i].customer_code = rand() % 5000;
        vendas[i].product_code = rand() % 5000;
        vendas[i].quantity = rand() % 22;
    }

    if (pipe(fd) == -1) { // Cria pipe
        printf("An error ocurred with opening the pipe\n");
        return 1;
    }

    for (i = 0; i < NUM_CHILD; i++) {
        p = fork();
        if (p == 0) {
            close(fd[LEITURA]);
            int limInf = 5000 * i;
            int limSup = 5000 * i + 5000;
            for (j = limInf; j < limSup; j++) { // Cada filho busca do limite inferior ao limite superior
                if (vendas[j].quantity > 20) {
                    write(fd[ESCRITA], &vendas[j].product_code, sizeof(int)); // Filho escreve produtos com mais de 20 vendas no pipe
                }
            }
            close(fd[ESCRITA]);
            exit(1);
        } else {
            close(fd[ESCRITA]);
            while (read(fd[LEITURA], &codigo, sizeof(int)) > 0) { //Pai le o pipe e grava no array produtos
                produtos[n] = codigo;
                n++;
            }
            close(fd[LEITURA]);
        }
    }

    wait(&status);
    if (WIFEXITED(status)) { // imprime o resultado gravado no array produtos quando os filhos terminam
        printf("Lista de produtos vendidos mais de 20 vezes:\n");
        for (i = 0; i < n; i++) {
            printf("%04d\t", produtos[i]);
        }
        printf("\n");
        return 0;
    }
}