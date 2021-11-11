#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define THREADS 8
#define GRAU 8

int m1[GRAU][GRAU];
int m2[GRAU][GRAU];
int mResult[GRAU][GRAU];

void* threadFillFunction1(void *arg) {
    for (int i = 0; i < GRAU; i++) {
        for (int k = 0; k < GRAU; k++) {
            m1[i][k] = i + k + 1;
        }
    }
    pthread_exit((void *) NULL); //termina a thread a NULL (as que nao encontram)

}

void* threadFillFunction2(void *arg) {
    for (int i = 0; i < GRAU; i++) {
        for (int k = 0; k < GRAU; k++) {
            m2[i][k] = i + k + 3;
        }
    }
    pthread_exit((void *) NULL); //termina a thread a NULL (as que nao encontram)

}

void *threadLineMulti(void *arg) {
    int *linha = (int *)arg;
    int i = *linha;
    int j, k;

    for (j = 0; j < GRAU; j++) {
        for (k = 0; k < GRAU; k++) {
            mResult[i][j] += m1[i][k] * m2[k][j];
        }
    }

    pthread_exit((void *) NULL); //termina a thread a NULL (as que nao encontram)
}

int main(int argc, char *agrv[]) {
    int i;
    pthread_t threads[THREADS];

    if (pthread_create(&threads[0], NULL, threadFillFunction1, (void *)NULL)) {
        printf("Error while creating threads \n");
        exit(1);
    }
    if (pthread_create(&threads[1], NULL, threadFillFunction2, (void *)NULL)) {
        printf("Error while creating threads \n");
        exit(1);
    }

    for (i = 0; i< 2; i++) {
        if (pthread_join(threads[i], NULL)) {
            printf("Error while waiting for thread %d \n", i);
            exit(1);
        }
    }

    int args[THREADS];
    for (i = 0; i < THREADS; i++) {
        args[i] = i;
        if (pthread_create(&threads[i], NULL, threadLineMulti, (void *)&args[i])) {
            printf("Error while creating threads \n");
            exit(1);
        }
    }

    for (i = 0; i < THREADS; i++) {
        if (pthread_join(threads[i], NULL)) {
            printf("Error while waiting for thread %d \n", i);
            exit(1);
        }
    }
    printf("All threads terminated\n");
    printf("Matriz 1\n");
    for (int i = 0; i < GRAU; i++) {
        for (int k = 0; k < GRAU; k++) {
            printf("%4d ", m1[i][k]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Matriz 2\n");
    for (int i = 0; i < GRAU; i++) {
        for (int k = 0; k < GRAU; k++) {
            printf("%4d ", m2[i][k]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Matriz produto\n");
    for (int i = 0; i < GRAU; i++) {
        for (int k = 0; k < GRAU; k++) {
            printf("%4d ", mResult[i][k]);
        }
        printf("\n");
    }
    return 0;
}


