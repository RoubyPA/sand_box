/* Example of serial port reading on UNIX-like system
 * Date: 2017
 * 
 * Read on serial port
 */
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>

#define BUFFER_SIZE 256
#define BAUD_RATE B9600

int
main(int argc, char **argv)
{
	bool		quit 				= false;
	int 		fd_port 		= NULL;
	int 		buffer_len 	= 0;
	char 		buffer[BUFFER_SIZE];
	struct 	termios conf;

	/* Open port */
	fd_port = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd_port == NULL)
		{
			fprintf(stderr, "[ERROR] %s(%d) : %s(%d)", 
							__FILE__, __LINE__, 
							strerror(errno), errno);
			exit(EXIT_FAILURE);
		}

	/* Get port configuration */
	tcgetattr(fd_port, &conf);
	
	/* Set baud rate */
	cfsetispeed(&conf, BAUD_RATE);
	cfsetospeed(&conf, BAUD_RATE);

	/* Set local and read mode */
	conf.c_cflag |= (CLOCAL | CREAD);

	/* Set caracter */
	conf.c_cflag &= ~CSIZE;
	conf.c_cflag |= CS8;

	/* Flush port */
	tcflush(fd_port, TCIFLUSH);

	/* Set new port conf */
	tcsetattr(fd_port, TCSANOW, &conf);

	/* Main loop */
	while(!quit)
		{
			strcpy(buffer, "");
			buffer_len = read(fd_port, buffer, BUFFER_SIZE);
			if(buffer_len != 0)
				{
					fprintf(stdout, ">>> %s", buffer);
					fflush(stdout);
					if (strstr(buffer, "quit"))
						{
							fprintf(stdout, "Bye !\n");
							fflush(stdout);
							quit = !quit;
						}
				}
		}

	/* Close port */
	close(fd_port);

	return EXIT_SUCCESS;
}



