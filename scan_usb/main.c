#include <stdio.h>
#include <stdlib.h>
/* directory */
#include <dirent.h>
/* mount */
#include <sys/mount.h>
/* error */
#include <errno.h>
#include <string.h>
/* sleep */
#include <unistd.h>
/* singal */
#include <signal.h>

#define PATH_LEN 256
#define DEV_PATH "/dev/"
#define MOUNT_PATH "./usb/"

/* TMP */
#define _DEBUG

void 
callback_signal(int signum)
{
	int return_value = 0;

	printf("[EXIT] User interruption... (%d)\n", signum);

	return_value = umount(MOUNT_PATH);
	if (return_value < 0)
		{
			fprintf (	stderr, "[ERROR] %s(%d)\t%s (%d)\n", 
								__FILE__, __LINE__,
								strerror(errno), errno);
			exit(EXIT_FAILURE);
		}
	
	printf(	"[INFO] Umount %s\n", MOUNT_PATH);

	exit(signum); 
}

int
main (int argc, char **argv)
{
	DIR *dir = NULL;	
	struct dirent *obj_lst = NULL;
	int return_value = 0;
	unsigned char mounted = 0;
	char buffer[PATH_LEN];

	printf("[INFO] Start deamon %s (%d)\n", argv[0], getpid());

	return_value = signal(SIGINT, callback_signal);
	if (return_value == SIG_ERR)
		{
			fprintf (	stderr, "[ERROR] %s(%d)\t%s (%d)\n", 
								__FILE__, __LINE__,
								strerror(errno), errno);
			exit(EXIT_FAILURE);
		}

	dir = opendir(DEV_PATH);
	if (dir == NULL)
		{	
			/* TODO time */
			fprintf (	stderr, "[ERROR] %s(%d)\t%s (%d)\n", 
								__FILE__, __LINE__,
								strerror(errno), errno);
			exit(EXIT_FAILURE);
		}

	while(1)
		{
			while ((obj_lst = readdir(dir)) != NULL)
				{
					if (strstr(obj_lst->d_name, "sdb1"))
						{
#ifdef _DEBUG
							fprintf(stderr, "> %s\n", obj_lst->d_name);
#endif

							printf("[INFO] USB (%s) detected\n", 
											obj_lst->d_name);

							if (mounted == 0)
								{
									strcpy(buffer, DEV_PATH);
									strcat(buffer, obj_lst->d_name);

									return_value = mount(	(const char *)buffer, 
																				(const char *)MOUNT_PATH,
																				"vfat", MS_NOATIME, NULL);
									if (return_value < 0)
										{						
											fprintf (	stderr, "[ERROR] %s(%d)\t%s (%d)\n", 
																__FILE__, __LINE__,
																strerror(errno), errno);
											exit(EXIT_FAILURE);
										}
									mounted = 1; /* TMP */
									printf(	"[INFO] Mount %s --> %s\n", 
													obj_lst->d_name, MOUNT_PATH);
								}
							printf("[INFO] Mounted\n");
						}
				}
				sleep(5);

				/* return start dir list */
				seekdir(dir, 0);
		}
	
	return EXIT_SUCCESS;	
}
