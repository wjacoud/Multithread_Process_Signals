#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_NUMERO_SIZE 12
#define QTD_FILHOS 5

volatile sig_atomic_t filhos_em_execucao = QTD_FILHOS; //alínea B)

int make_children(int n)
{
    pid_t pid = 0;

    int i = 0;
    for (i = 0; i < n; ++i)
    {
        pid = fork();
        if (pid == 0)
        {
            return i + 1; //se pid = 0 estamos dentro do filho, então a função retorna o nº de ordem de criação do filho, ou seja, 1º filho a ser criado, retorna 1, por aí fora.
        }
    }
    return 0; //se chega aqui, estamos no pai, e vai retornar 0.
}

void handler(int signo, siginfo_t *sinfo, void *context)
{
    --filhos_em_execucao;
    write(STDOUT_FILENO, "Handler: Filhos em execucao=", 28);

    /* Atendendo a que não é seguro usar o printf, converter o numero para texto */
    char numero[MAX_NUMERO_SIZE];
    int i = 0;
    int n = filhos_em_execucao;
    while (n > 0 || i == 0)
    {
        numero[i] = '0' + n % 10; /* numero[i] fica com o digito das unidades */
        n = n / 10;               /* Descarta digito das unidades, "empurra" digitos para a direita */
        ++i;
    }
    numero[i] = '\n';
    numero[i + 1] = '\0';

    write(STDOUT_FILENO, numero, strlen(numero));
}

int main(void)
{
    const int NUMS_POR_FILHOS = 10; /* Quantos números se pretende que um filho imprima */

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));

    sigfillset(&act.sa_mask); /* All signals blocked */

    /* POSIX */
    act.sa_sigaction = handler; /* Função responsável por tratar o sinal (handler) */

    /*
		 * SA_NOCLDWAIT - Deixa de transformar os filhos em zombie quando morrem
		 * SA_NOCLDSTOP - Deixa de receber notificações quando o processo pára ou continua
         * SA_NOCLDSTOP - Garante que o sinal SIGCHL é enviado apenas quando o processo filho morre
		 * SIGCHLD - Este é o sinal que o filho envia ao pai aquando do exit
		 */
    act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP | SA_NOCLDWAIT;
    sigaction(SIGCHLD, &act, NULL); /* Tratamento do sinal SIGCHLD conforme act */

    //---------------------------------------------Parte 1 ----------------------------------------------------
    int id = make_children(QTD_FILHOS); //retorna >0 para filhos e 0 para o pai

    if (id > 0)
    {                                                  /* Código apenas para os processos filho */
        const int INICIO = (id - 1) * NUMS_POR_FILHOS; /* Extremo inferior do intervalo */
        const int FIM = id * NUMS_POR_FILHOS;          /* Extremo direito do intervalo */

        int i = 0;
        for (i = INICIO; i < FIM; ++i)
        {
            printf("Filho %d: %d\n", id, i);
        }
        printf("Sou o filho %d e terminei a minha impressao \n", id);
        sleep(id); //utilizado apenas para impedir que haja envio de sinais simultaneos ao pai
        exit(id);  // Filho termina com o número de ordem de criação (envia SIGCHLD ao pai)
    }
    //---------------------------------------------- PARTE 2---------------------------------------------------

    //Enquanto os filhos ainda estão a ser executados (este código apenas é executado pelo pai)

    while (filhos_em_execucao > 0) //o loop termina quando não há mais filhos em execução
    {
        pause(); /* Espera por um qualquer sinal */
        printf("Sou o pai e tenho ainda %d filhos em execucao!\n", filhos_em_execucao);
    }

    int estado = 0;
    int i;
    for (i = 0; i < QTD_FILHOS; ++i)
    {
        if (wait(&estado) == -1)
        { /* Não existindo filhos (vivos ou zombies) a função falha */
            perror("wait failed!!!");
            exit(EXIT_FAILURE);
        }
        else if (WIFEXITED(estado))
        {
            printf("Pai: Filho terminou com valor de saida: %d\n", WEXITSTATUS(estado));
        }
    }

    printf("Pai: ESPEREI POR TODOS %d\n");
    return 0;
}

//Quando os processos filhos terminam, ficam no modo zombie até que o pai termine ou até que o pai invoce o wait, para capturar o valor de saída do processo filho.
//Uma vez que ao terminar o filho, é enviado um sigchld ao pai, e o pai por sua vez possui um handler para esse sinal, o valor de saida do filho fica na "stack".
//Só após a invocação do wait por parte do pai é que os processos filhos deixam de ficar zombies.

//Com a flag SA_NOCHLDWAIT , os processos filhos não ficam à espera que o pai capture o seu valor de saída, eles ao fazer exit, deixam de existir.
//Desta forma, não existem valores de saída dos processos filho na "stack".
