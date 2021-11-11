#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define NUM_FILHOS 4
#define PECAS 1000
#define ESCRITA 1
#define LEITURA 0

struct armazem
{
    int total;
};

void criarPipes(int fd[NUM_FILHOS][2])
{
    int i;
    for (i = 0; i < NUM_FILHOS; i++)
    {
        if (pipe(fd[i]) == -1)
        {
            printf("An error ocurred with opening the pipe\n");
            exit(EXIT_FAILURE);
        }
    }
}

int main()
{
    int pids[NUM_FILHOS], status;
    int fd[NUM_FILHOS][2];

    struct armazem global;
    global.total = 0;

    int i, a, f = 5, c = 0, h = 100, t = 10;

    criarPipes(fd);

    for (i = 0; i < NUM_FILHOS; i++)
    {
        //guarda o valor do pid dos filhos criados no vetor
        pids[i] = fork();

        if (pids[i] == 0) //significa que estamos no processo filho
        {
            if (i == 0) //M1
            {
                close(fd[i][LEITURA]);
                while (c != PECAS)
                {
                    c += 5;
                    write(fd[i][ESCRITA], &f, sizeof(f));
                    printf("M1 CUT 5 PIECES, SENDING TO M2...\n");
                }
                close(fd[i][ESCRITA]);
                exit(0);
            }
            if (i == 1) //M2
            {
                close((fd[i - 1][ESCRITA]));
                close((fd[i][0]));
                while (c != PECAS)
                {
                    read(fd[i - 1][LEITURA], &f, sizeof(int));
                    c += f;
                    write(fd[i][ESCRITA], &f, sizeof(f));
                    printf("M2 folded 5 PIECES, SENDING TO M3...\n");
                }
                close(fd[i][ESCRITA]);
                close(fd[i - 1][LEITURA]);
                exit(0);
            }
            if (i == 2) //M3
            {
                close((fd[i - 1][ESCRITA]));
                close((fd[i][LEITURA]));
                while (c != PECAS)
                {
                    read(fd[i - 1][LEITURA], &f, sizeof(int));
                    c += f;
                    if (c % 10 == 0) //10 em 10
                    {
                        write(fd[i][ESCRITA], &t, sizeof(t)); //envia 10
                        printf("M3 WELDED 10 PIECES, SENDING TO M4...\n");
                    }
                }
                close(fd[i][ESCRITA]);
                close(fd[i - 1][LEITURA]);
                exit(0);
            }
            if (i == 3) //M4
            {
                close((fd[i - 1][ESCRITA]));
                close((fd[i][LEITURA]));
                while (c != PECAS)
                {
                    read(fd[i - 1][LEITURA], &f, sizeof(f));
                    c += f;
                    if (c % 100 == 0) //100 em 100
                    {
                        write(fd[i][ESCRITA], &h, sizeof(h)); //envia 100
                        printf("M4 PACKED 100 PIECES, SENDING TO A1...\n");
                    }
                }
                close(fd[i][ESCRITA]);
                close(fd[i - 1][LEITURA]);
                exit(0);
            }
        }
    }

    close(fd[NUM_FILHOS - 1][ESCRITA]);      //fd[3][1] M4
    while (global.total != PECAS) //A1
    {
        read(fd[NUM_FILHOS - 1][LEITURA], &a, sizeof(int));
        printf("M4 SENT 100 PIECES, ADDING TO INVENTORY...\n");
        global.total += a;
        printf("INVENTORY UPDATED: %d\n", global.total);
    }
    close(fd[NUM_FILHOS - 1][LEITURA]); //fd[3][0] M4

    for (i = 0; i < NUM_FILHOS; i++)
    {
        waitpid(pids[i], &status, 0);
    }

    return 0;
}
