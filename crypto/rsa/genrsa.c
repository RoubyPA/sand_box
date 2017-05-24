#include <stdio.h>
#include <stdlib.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#define KEYS_BITS 2048

typedef struct s_gen_keys s_gen_keys;
struct s_gen_keys
{
	RSA 		*rsa;
	BIGNUM 	*bn;
	BIO			*bio_pub;
	BIO			*bio_pri;
};

void 
free_sgk(s_gen_keys *sgk)
{
	BIO_free_all(sgk->bio_pub);
	BIO_free_all(sgk->bio_pri);
	RSA_free(sgk->rsa);
	BN_free(sgk->bn);
}

int 
gen_keys()
{	
	int ret 	= 0;
	unsigned long e = RSA_F4;
	s_gen_keys sgk;

/* 1 Gen RSA keys */
	sgk.bn = BN_new();
	ret = BN_set_word(sgk.bn,e);
	if (ret != 1)
	{
		fprintf(stderr, "[ERROR] %s (%d) : BN_set_word()\n", 
						__FILE__, __LINE__);
		free_sgk(&sgk);
		return 1;
	}
	
	sgk.rsa = RSA_new();
	ret = RSA_generate_key_ex(sgk.rsa, KEYS_BITS, sgk.bn, NULL);
	if (ret != 1)
	{
		fprintf(stderr, "[ERROR] %s (%d) : RSA_generate_key_ex()\n", 
						__FILE__, __LINE__);
		free_sgk(&sgk);
		return 1;
	}

/* 2 Save RSA in PEM file */
	sgk.bio_pub = BIO_new_file("public.pem", "w+");
	ret = PEM_write_bio_RSAPublicKey(sgk.bio_pub, sgk.rsa);
	if (ret != 1)
	{
		fprintf(stderr, "[ERROR] %s (%d) : PEM_write_bio_RSAPublicKey()\n", 
						__FILE__, __LINE__);
		free_sgk(&sgk);
		return 1;
	}

/* 3 */
	sgk.bio_pri = BIO_new_file("private.pem", "w+");
	ret = PEM_write_bio_RSAPrivateKey(sgk.bio_pri, sgk.rsa, 
																		NULL, NULL, 0, NULL, NULL);
	if (ret != 1)
	{
		fprintf(stderr, "[ERROR] %s (%d) : PEM_write_bio_RSAPrivateKey()\n", 
						__FILE__, __LINE__);
		free_sgk(&sgk);
		return 1;
	}
	
	free_sgk(&sgk);
	return 0;
}

int
main (int argc, char **argv)
{
	int ret = 0;

	ret = gen_keys();
	if (ret != 0)
	{
		fprintf(stderr, "RSA keys generation fail\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
