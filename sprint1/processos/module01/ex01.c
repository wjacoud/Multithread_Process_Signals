#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>


int main(void) {
    int x = 1;
    pid_t p = fork(); /*pid_t: sys/types.h; fork(): unistd.h*/
    if (p == 0) {
        x = x + 1;
        printf("1. x = %d\n", x);
    } else {
        x = x - 1;
        printf("2. x = %d\n", x);
    }
    printf("3. x = %d\n", x);
    exit(EXIT_SUCCESS);
}

//  a)  2. x = 0
//      3. x = 0
//      1. x = 2
//      3. x = 2
//      O PID do processo filho será retornado pela função Fork e armazenado na variável p no processo pai.
//      Já no processo filho, a variável p será igual a 0 no retorno da função Fork.
//      Sendo assim, no processo pai, a variável x será decrementada e será impresso o valor 0.
//      No processo filho, a variável será incrementada e será impresso o valor 2.

//  b)  Não há garântia, pois o tempo de execução de cada processo é variável.
