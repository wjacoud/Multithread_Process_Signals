
all:
	gcc -Wall main.c -o main.bin
	gcc -Wall inserir.c -o inserir.bin
	gcc -Wall consultar.c -o consultar.bin
	gcc -Wall consultar_todos.c -o consultar_todos.bin

run:
	gcc -Wall main.c -o main.bin
	gcc -Wall inserir.c -o inserir.bin
	gcc -Wall consultar.c -o consultar.bin
	gcc -Wall consultar_todos.c -o consultar_todos.bin
	./main.bin

clean:
	rm *.bin
	rm /dev/shm/*