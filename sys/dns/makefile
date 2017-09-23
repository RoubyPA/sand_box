# Makefiles

all : dns

dns : dns.o
	gcc dns.o -o dns

dns.o : dns.c
	gcc -Wall -pedantic -c dns.c -o dns.o 
#-DDEBUG

clean : 
	rm *.o
