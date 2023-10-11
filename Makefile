CC=gcc

SimpleShell: simple-shell.o simple-execute.o
		$(CC) -g -o SimpleShell simple-shell.o simple-execute.o

simple-shell.o: simple-shell.c
		$(CC) -g -c -o simple-shell.o simple-shell.c

simple-execute.o: simple-execute.c
		$(CC) -g -c -o simple-execute.o simple-execute.c

clean:
	rm *.o
	rm SimpleShell
