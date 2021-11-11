
# exemplo de makefile 
# variaveis do make
NAME = ex01escritor
INCLUDES = 
SOURCES =   ${NAME}.c  
OBJFILES =  ${NAME}.o  
EXE = ${NAME}.bin
CFLAGS= -lrt
#mensagem recebida por parametro na forma:
# make action msg="mensagem"
MSG = $(msg)

# regras de sufixo
.SUFFIXES : .c .o

# como transformar um .c num .o ; $< -- nome do ficheiro
.c.o:
	gcc -g -Wall -c $< $(CFLAGS)  

# para fazer o programa principal 
${EXE}: ${OBJFILES}
	gcc -g -o ${EXE} ${OBJFILES} $(CFLAGS)  

# execução do programa principal 
run:  ${EXE}
	./${EXE}

# execução do programa principal 
vg:  ${EXE}
	valgrind ./${EXE}


# Limpeza...
clean: 
	rm ${OBJFILES} ${EXE}

# Configuração do Git
gitconfig:
	git config --global user.email "1200630@isep.ipp.pt"
	git config --global user.name "WILSON HERMES JACOUD JUNIOR"
	git config --global push.default simple
	
# Actualização do Git	
git:
	git add -A
	git commit -a -m "${MSG}"
	git pull
	git push
	
#debug
#recorrendo ao eclipse	