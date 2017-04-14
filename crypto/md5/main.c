#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <openssl/md5.h>

#define BYTE_LEN 1
#define MD5_LEN 33
#define PATH_LEN 256
#define DATA_BUFFER 1024

void
md5_to_string (const unsigned char *data, char *str)
{
	int i = 0;
	char buf[2];

	/* Set empty string */
	strcpy (str, "");

	while (i < MD5_DIGEST_LENGTH)
		{
			sprintf (buf, "%02x", data[i]);
			strcat (str, buf);
			i++;
		}
}

int
main (int argc, char **argv)
{
	unsigned char md5_data[MD5_DIGEST_LENGTH];
	char file_str[PATH_LEN];
	FILE *file = NULL;
	MD5_CTX context;
	int bytes;
	int cur_byte = 0;
	int end_byte;
	unsigned char data[DATA_BUFFER];
	char md5_str[MD5_LEN];

	if (argc < 2)
		{
			fprintf (stderr, "Usage: md5 [file_path]");
			exit (EXIT_FAILURE);
		}

	strcpy (file_str, argv[1]);

	file = fopen (file_str, "r");
	if (file == NULL)
		{
			fprintf (stderr, "[ERROR] %s(%d) : %s (%d)", __FILE__, __LINE__,
			         strerror (errno), errno);
			exit (EXIT_FAILURE);
		}

	/* file length */
	fseek (file, 0L, SEEK_END);
	end_byte = ftell (file);

	rewind(file);

	MD5_Init (&context);

	while ((bytes = fread (data, BYTE_LEN, DATA_BUFFER, file)) != 0)
		{
			MD5_Update (&context, data, bytes);
			cur_byte += bytes;

		}

	MD5_Final (md5_data, &context);

	md5_to_string (md5_data, md5_str);

	printf ("%s  %s\n", md5_str, file_str);

	return EXIT_SUCCESS;
}
