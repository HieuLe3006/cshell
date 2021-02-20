all: cshell.c
	gcc -o cshell cshell.c 

clean: 
	rm -rf *.o cshell