md5: main.o
	gcc main.o -o md5 -lssl -lcrypto
	
main.o: main.c
	gcc -Wall -pedantic -ansi -c main.c -o main.o