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

#define COMBOIOS 4
#define CIDADE1 1
#define CIDADE2 2
#define CIDADE3 3
#define CIDADE4 4

#define THREADS 4

typedef struct {
    int numeroDoComboio;
    int destino;
    int origem;
} Comboio;

Comboio comboio[COMBOIOS];
pthread_mutex_t mutexTrajeto_Cidade1_Cidade2;
pthread_mutex_t mutexTrajeto_Cidade2_Cidade3;
pthread_mutex_t mutexTrajeto_Cidade2_Cidade4;

pthread_mutex_t buscaTrajeto(Comboio *comboio) {
    switch (comboio->origem) {
        case CIDADE1:
            comboio->origem = CIDADE2;
            return mutexTrajeto_Cidade1_Cidade2;
            break;
        case CIDADE2:
            switch (comboio->destino) {
                case CIDADE1:
                    comboio->origem = CIDADE1;
                    return mutexTrajeto_Cidade1_Cidade2;
                    break;
                case CIDADE3:
                    comboio->origem = CIDADE3;
                    return mutexTrajeto_Cidade2_Cidade3;
                    break;
                case CIDADE4:
                    comboio->origem = CIDADE4;
                    return mutexTrajeto_Cidade2_Cidade4;
                    break;
            }
            break;
        case CIDADE3:
            comboio->origem = CIDADE2;
            return mutexTrajeto_Cidade2_Cidade3;
            break;
        case CIDADE4:
            comboio->origem = CIDADE2;
            return mutexTrajeto_Cidade2_Cidade4;
            break;
    }
    pthread_mutex_t coletor;
    return coletor;
}

char *get_time() {
    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';
    return time_str;
}

void *func_thread(void *args) {

    Comboio *comboio = (Comboio *) args;
    printf("Partida do comboio %d as: %s\n", comboio->numeroDoComboio, get_time());
    printf("Comboio%d a viajar da Cidade%d para Cidade%d\n", comboio->numeroDoComboio, comboio->origem,
           comboio->destino);
    pthread_mutex_t mutexInicio = buscaTrajeto(comboio);
    if (pthread_mutex_lock(&mutexInicio) == -1) {
        printf("Error while locking the mutex thread \n");
        exit(1);
    }

    printf("Viagem Comboio%d concluida\n", comboio->numeroDoComboio);
    sleep(2);
    if (pthread_mutex_unlock(&mutexInicio) == -1) {
        printf("Error while unlocking the mutex thread \n");
        pthread_exit((void *)1);
    }

    if (comboio->origem == comboio->destino) {
        sleep(1);
        printf("Viagem Comboio%d concluída as: %s\n", comboio->numeroDoComboio, get_time());
        pthread_exit(NULL);
    }

    printf("Comboio%d a viajar da Cidade%d para Cidade%d\n", comboio->numeroDoComboio, comboio->origem, comboio->destino);
    pthread_mutex_t second_trip_mutex = buscaTrajeto(comboio);

    if (pthread_mutex_lock(&second_trip_mutex) == -1) {
        printf("Error while unlocking the mutex thread \n");
        pthread_exit((void *)1);
    }
    sleep(2);
    if (pthread_mutex_unlock(&second_trip_mutex) == -1) {
        printf("Error while unlocking the mutex thread \n");
        pthread_exit((void *)1);
    }
    printf("Viagem Comboio%d concluída\n", comboio->numeroDoComboio);
    sleep(1);
    printf("Viagem Comboio%d concluída as: %s\n", comboio->numeroDoComboio, get_time());
    sleep(1);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[THREADS];
    srand(time(NULL));
    int i = 0;
    for (i = 0; i < COMBOIOS; i++) {
        comboio[i].numeroDoComboio = i + 1;
        comboio[i].destino = (rand() % 4) + 1;
        comboio[i].origem = (rand() % 4) + 1;
        while (comboio[i].destino == comboio[i].origem) {
            comboio[i].destino = (rand() % 4) + 1;
            comboio[i].origem = (rand() % 4) + 1;
        }
    }

    if (pthread_mutex_init(&mutexTrajeto_Cidade1_Cidade2, NULL) != 0) {
        printf("Error while initializing the mutex thread \n");
        exit(1);
    }
    if (pthread_mutex_init(&mutexTrajeto_Cidade2_Cidade3, NULL) != 0){
        printf("Error while initializing the mutex thread \n");
        exit(1);
    }
    if (pthread_mutex_init(&mutexTrajeto_Cidade2_Cidade4, NULL) != 0){
        printf("Error while initializing the mutex thread \n");
        exit(1);
    }

    for (i = 0; i < THREADS; i++) {
        if (pthread_create(&threads[i], NULL, func_thread, (void *) &comboio[i]) != 0) {
            printf("Error while creating the thread \n");
            exit(1);
        }
    }

    for (i = 0; i < THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Error while waiting for thread \n");
            exit(1);
        }
    }
    pthread_mutex_destroy(&mutexTrajeto_Cidade1_Cidade2);
    pthread_mutex_destroy(&mutexTrajeto_Cidade2_Cidade3);
    pthread_mutex_destroy(&mutexTrajeto_Cidade2_Cidade4);

    return 0;
}