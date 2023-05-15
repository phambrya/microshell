myshell:	myshell.o argparse.o builtin.o
			gcc -o myshell myshell.o argparse.o builtin.o

myshell.o:	myshell.c argparse.h builtin.h
			gcc -g -c myshell.c
		
argparse.o:	argparse.c argparse.h
			gcc -g -c argparse.c

clean:	
		rm -f myshell myshell.exe
		rm -f *.o *.bak *~*