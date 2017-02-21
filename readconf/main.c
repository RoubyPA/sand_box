/******************************************************************************
 *	File		: main.c																													*
 *	Date		: 2016																														*
 *	Author 	: Rouby Pierre-Antoine																						*
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define BUFFER_SIZE 256
#define CONF_MAX_PARAM 4

/******************************************************************************
 *	openconf function																													*
 ******************************************************************************/
FILE *
openconf(char *path, FILE *fd)
{
	char *buffer 	= malloc(BUFFER_SIZE) ;

	fd = fopen(path, "r") ;
	if (fd == NULL)
	{
		fprintf(stderr, 	"[ERROR] %s (%d) : %s\n",
											path, errno, strerror(errno)) ;
		exit(EXIT_FAILURE) ;
	}

	return fd ;
}


/******************************************************************************
 *	closeconf function																												*
 ******************************************************************************/
FILE *
closeconf(char *path, FILE *fd)
{
	if(fclose(fd) != 0)
	{
		fprintf(stderr, 	"[ERROR] %s (%d) : %s\n",
											path, errno, strerror(errno)) ;
		exit(EXIT_FAILURE) ;
	}

	return fd ;
}


/******************************************************************************
 *	findconf function																													*
 ******************************************************************************/
char *
findconf(const char *conf, char *value, FILE *fd)
{
	bool	find 		= false ;
	int		i 			= 0 ;
	char *buffer 	= malloc(BUFFER_SIZE) ;

	while(fgets(buffer, BUFFER_SIZE, fd) != NULL && find == false)
	{
		if (strstr(buffer, conf))
		{
			find = true ;
			strcpy(value, (index(buffer, '=')+1)) ;

			while(value[++i] != '\n' &&
						value[i] != '\0' 	&&
						i < BUFFER_SIZE) ;
			value[i] = '\0' ;
		}
	}
	rewind(fd) ;

	return value ;
}


/******************************************************************************
 *	main function																															*
 ******************************************************************************/
int
main (int argc, char **argv)
{
	int 	optchar ;
	int		port ;
	char *path 			= malloc(BUFFER_SIZE) ;
	char *ip 				= malloc(BUFFER_SIZE) ;
	char *online 		= malloc(BUFFER_SIZE) ;
	char *strport 	= malloc(BUFFER_SIZE) ;
	FILE *file  		= NULL ;

	if(argc < 3)
	{
		fprintf(stdout, "Usage : %s -c [file]\n", argv[0]) ;
		exit(EXIT_FAILURE) ;
	}

	while ((optchar = getopt(argc, argv, "c:")) != EOF)
	{
		switch (optchar)
		{
			case 'c' :
				strcpy(path, optarg) ;
				break ;
			default :
				fprintf(stdout, "Unknown option: %c\n", optchar) ;
				exit(EXIT_FAILURE) ;
				break ;
		}
	}

/*	open file	*/
	file = openconf(path, file) ;

	ip 				= findconf("ip", 			ip, 			file) ;
	online 		= findconf("online", 	online, 	file) ;
	strport 	= findconf("port", 		strport, 	file) ;

/*	close file	*/
	file = closeconf(path, file) ;

	fprintf(stdout, "Path\t: %s\n", 	path) ;
	fprintf(stdout, "IP\t: %s\n", 		ip) ;
	fprintf(stdout, "online\t: %s\n", online) ;
	fprintf(stdout, "port\t: %d\n", 	atoi(strport)) ;

	return EXIT_SUCCESS ;
}
