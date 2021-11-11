#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h> /*tipo de dados usados pela API*/
#include <fcntl.h>     /*opções de controlo de ficheiros*/
#include <sys/stat.h>  /*contantes utilizadas para modo de abertura*/
#include <sys/mman.h>  /*declarações relacionadas com a gestão de memória partilhada: shm_* e mmap()*/

#define SHM_NAME "/shmex13"
#define STR_SIZE 100
#define WORD_SIZE 20
#define CHILD 10

typedef struct {
    int occurences[10];
	char word[10][255];
	char path[10][100];
    int NewData;
} shared_data;



int main(void) {


    int i, fd, r;
    int dataSize = sizeof(shared_data);
    shared_data *dataPointer;

    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); /*abrir canal, criar*/
    if (fd < 0) {
        perror("In shm_open()");
        exit(1);
    }

    ftruncate(fd, dataSize); /*tamanho da shm*/

    dataPointer = (shared_data *) mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); /* cria o mapeamento */

    FILE *f;
	char line[1024];
    pid_t pid;
	
    
	//Definição das palavras a serem procuradas
	strcpy(dataPointer -> word[0], "batata");
	strcpy(dataPointer -> word[1], "arroz");
	strcpy(dataPointer -> word[2], "picanha");
	strcpy(dataPointer -> word[3], "feijao");
	strcpy(dataPointer -> word[4], "caipirinha");
	strcpy(dataPointer -> word[5], "farofa");
	strcpy(dataPointer -> word[6], "batata");
	strcpy(dataPointer -> word[7], "arroz");
	strcpy(dataPointer -> word[8], "feijao");
	strcpy(dataPointer -> word[9], "farofa");
	
		
	for(i = 0; i < CHILD; i++){
		sprintf(dataPointer -> path[i], "files/file%d.txt", i+1);
		

		pid = fork();
		if(pid == -1){
			perror("Erro no fork!\n");
			exit(EXIT_FAILURE);
		}
        
        dataPointer->NewData=1;

		if(pid == 0){ //filhos
            while (!dataPointer->NewData) // Processo filho espera o final da insercao pelo processo pai
            ;
            dataPointer->NewData=0;
			f = fopen(dataPointer -> path[i], "r");
			while(fgets(line, sizeof(line), f) != NULL){
				if(strstr(line, dataPointer -> word[i]) != NULL){
                   
					dataPointer -> occurences[i]++;
				}
			}
            
			fclose(f);
            dataPointer->NewData=1;
			exit(0);
		}
        
	
    }
	
    int status = 0;
	
	for(i = 0; i < CHILD; i++){       
        wait(&status); // Espera o termino do processo filho

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        perror("Erro no processo filho");
        exit(1);
        }

		printf("O filho n.%d encontrou '%s' %d vezes no seu ficheiro - %s.\n", i+1, dataPointer -> word[i], dataPointer -> occurences[i], dataPointer -> path[i]);
	}



    r = munmap(dataPointer, dataSize); //fecha mapeamento
    if (r < 0) exit(1); //verifica erro

    close(fd); //fecha a shm
    if (r < 0) exit(1); //verifica erro

    r = shm_unlink(SHM_NAME);
    if (r < 0) exit(1); //verifica erro

    return 0;
}