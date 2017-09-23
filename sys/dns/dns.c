/*	Rouby Pierre-Antoine
 *	
 *	Licence :
 *		GPLv3 or later
 *
 *	Researsh source : 	
 *		http://www.binarytides.com/dns-query-code-in-c-with-linux-sockets/
 * 		https://tools.ietf.org/html/rfc883#page-26
 *		http://www.tcpipguide.com/free/t_DNSMessageHeaderandQuestionSectionFormat.htm
 *		https://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml
 *
 *	For later :
 *		TODO IPv6 compatibility
 *		TODO test
 *		TODO define all type and class
 *		TODO Manage all responce type (class)
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
/*	socket()	*/
#include <sys/types.h>
#include <sys/socket.h>
/*	sockaddr_in	*/
#include <netinet/in.h>
#include <netinet/udp.h>
/*	inet_addr()	*/
#include <arpa/inet.h>
/*	int32_t	*/
/*#include <stdint.h>
*/

#ifdef DEBUG
/*	Macro to convert byte to binary	*/
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x07 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 
#endif

/*	Default DNS information	*/
#define DNS_IP4 "208.67.222.222"
#define DNS_PORT 53
#define MAX_PACKET_SIZE 65536

/*	Default DNS header value	*/
#define HEADER_QR_QUERY 0
#define HEADER_QR_RESPONSE 1

#define	HEADER_OPCODE_QUERY 0
#define	HEADER_OPCODE_IQUERY 1
#define	HEADER_OPCODE_STATUS 2
#define	HEADER_OPCODE_RESERVED 3
#define	HEADER_OPCODE_NOTIFY 4
#define	HEADER_OPCODE_UPDATE 5

#define	HEADER_RCODE_NO_ERROR 0
#define	HEADER_RCODE_FORMAT_ERROR 1
#define	HEADER_RCODE_SERVER_FAILURE 2
#define	HEADER_RCODE_NAME_ERROR 3
#define	HEADER_RCODE_NOT_IMPLEMENTED 4
#define	HEADER_RCODE_REFUSED 5
#define	HEADER_RCODE_YX_DOMAIN 6
#define	HEADER_RCODE_YX_RR_SET 7
#define	HEADER_RCODE_NX_RR_SET 8
#define	HEADER_RCODE_NOT_AUTH 9
#define	HEADER_RCODE_NOT_ZONE 10

/*	type	*/
#define	T_A 1 					/*	a host addr	*/
#ifdef ALL_TYPES
	#define T_NS 2				/*	an authoritative name server	*/
	#define T_MD 3				/*	mail destination	*/
	#define T_MF 4				/*	mail forwarder	*/
	#define	T_CNAME 5			/*	canonical name for alias	*/
	#define T_SOA 6				/*	marks the start of a zone of authority	*/
	#define T_MB 7				/*	mailbox domain name	*/
	#define T_MG 8				/*	mail group menbre	*/
	#define T_MR 9				/*	mail rename domain name	*/
	#define T_NULL 10			/*	NULL RR	*/
	#define T_WKS 11			/*	well known service description	*/
	#define T_PTR 12			/*	a domain name pointer	*/
	#define T_HINFO 13		/*	host information	*/
	#define T_MINFO 14		/*	mailbox or mail list information	*/
	#define T_MX 15				/*	mail exchange	*/
	#define T_TXT 16			/*	text strings	*/
	#define T_RP 17				/*	responsible person	*/
	#define T_AFSDB 18		/*	AFS data base	*/
	#define T_X25 19			/*	X25 PSDN addr	*/
	#define T_ISDN 20			/*	ISDN addr	*/
	#define T_RT 21				/*	route through	*/
	#define T_NSAP 22			/*	NSAP addr	*/
	#define T_NSAP_PTR 23	/*	NSAP addr pointer	*/
	#define T_SIG 24			/*	for security signature	*/
	#define T_KEY 25			/*	for security key	*/
	#define T_PX 26				/*	X400 mail mapping	*/
	#define T_GPOS 27			/*	geographical position	*/
	#define T_AAAA 28			/*	IPv6 addr	*/
#endif

/*	class	*/
#define C_R 0
#define C_IN 1
#ifdef ALL_TYPES
	#define C_C 3	/*	Chaos	*/
	#define C_H 4	/*	Hesiod	*/
	#define C_RP_MIN 65280	/*	reserved for private min value	*/
	#define C_RP_MAX 65534	/*	reserved for private max 	*/
	#define C_R2 65535
#endif

/* DNS header structure	*/
typedef struct
{
	unsigned short id;	/*	Identifier : 2 bytes	*/

	unsigned short rd :1;			/*	Recusion Desired : 1 bit	*/
	unsigned short tc :1;			/*	Truncation Flag : 1 bit	*/
	unsigned short aa :1;			/*	Authoritative	Answer Flag : 1 bit*/
	unsigned short opcode :4;	/*	Operation Code : type of query : 4 bits	*/
	unsigned short qr :1;			/*	Query/Response flags : 1 bit	*/

	unsigned short rcode :4;	/*	Response Code	(to 0 in query) : 4 bits	*/
	unsigned short z :3;			/*	Zero three reserved bits (to 0) : 3 bits	*/
	unsigned short ra :1;		/*	Recursion Available : 1 bit	*/

	unsigned short qdcount;	/*	Query Count : 2 bytes	*/
	unsigned short ancount;	/*	Answer Record Count : 2 bytes	*/
	unsigned short nscount;	/*	Aurhority Record Count : 2 bytes	*/
	unsigned short arcount;	/*	Additional Record Count	: 2 bytes	*/
} dns_header;

/*	DNS query struct	*/
typedef struct 
{
	unsigned short qtype; 	/*	type (A/CNAME/...)	*/
	unsigned short qclass;	/*	class (IN/...)	*/
} dns_query;


/*	DNS response data	*/
typedef struct 
{
	unsigned short name;
	/*	use dns_query for type and class ?	*/
	unsigned short type;	/*	type (A/...)	*/
	unsigned short class;	/*	class (IN/...)	*/
	/*int32_t ttl*/					/*	HACK in use int, struct size = 16 ?	*/
	unsigned short ttl[2]; 	/*	HACK ingnor ttl[0] pb if ttl > 65536	*/
	unsigned short rdlenght;
} res_data;

/*	HACK dirty solution but YOLO !	*/
/*	ip struct	*/
typedef struct
{
	/*	a.b.c.d	*/
	unsigned short a;
	unsigned short b;
	unsigned short c;
	unsigned short d;
} s_ip;


void
help (int argc, char **argv)
{
	printf("Usage: %s [options] domain\n", argv[0]);
	printf("Options:\n");
	printf(" -h\t\tDisplay this help\n");
	printf(" -s ip\t\tChange default server address\n");
	printf(" -p port\tChange default port number\n");
	printf(" -v\t\tVerbose mode\n");
}

/*	Convert domain name to domain name server format
 *	www.gnu.org => 3www3gnu3org
 *	Source : www.binarytides.com/dns-query-code-in-c-with-linux-sockets/
 */
void 
toDnsName(unsigned char *dns, unsigned char *host)
{
	int lock = 0;
	int	i;

	strcat((char*)host,"."); 

	for(i = 0 ; i < strlen((char*)host) ; i++)
		{
			if(host[i]=='.')
				{
					*dns++ = i-lock;
					for(;lock<i;lock++)
						{
							*dns++=host[lock];
						}
					lock++; 
				}
		}
	*dns++='\0';
}

#ifdef DEBUG
/*	Display data	*/
void
display_data_bin_hex(unsigned char *data, int size)
{
	int i;
	printf("\n\tData (%p)\n", data);

	for(i = 0 ; i < size ; i+=2)
		{
			printf("\t" BYTE_TO_BINARY_PATTERN 
							" " BYTE_TO_BINARY_PATTERN 
							"\t%.2x %.2x\t%c %c\n", 
							BYTE_TO_BINARY(data[i]), BYTE_TO_BINARY(data[i+1]),
							data[i], data[i+1], 
							data[i] ? data[i] : '.', 
							data[i+1] ? data[i+1] : '.');
		}
}

/*	Display header value	*/
void
displayHeader (dns_header * h)
{
	printf("head (%p) => size : %ld bytes\n", 
				(void *)h, sizeof(*h));
	printf("\tid\t: %d\n", h->id);
	printf("\tqr\t: %d\n", h->qr);
	printf("\topcode\t: %d\n", h->opcode);
	printf("\taa\t: %d\n", h->aa);
	printf("\ttc\t: %d\n", h->tc);
	printf("\trd\t: %d\n", h->rd);
	printf("\tra\t: %d\n", h->ra);
	printf("\tz\t: %d\n", h->z);
	printf("\trcode\t: %d\n", h->rcode);
	printf("\tqdcount\t: %d\n", h->qdcount);
	printf("\tancount\t: %d\n", h->ancount);
	printf("\tnscount\t: %d\n", h->nscount);
	printf("\tarcount\t: %d\n\n", h->arcount);
}

/*	Display rdata struct value	*/
void
display_rdata (res_data *r)
{
	printf("rdata (%p) => size : %ld bytes\n",
					(void *)r, sizeof(*r));
	printf("\tname\t: %d\t\t0x%.4x\n", ntohs(r->name), ntohs(r->name));
	printf("\ttype\t: %d\t\t0x%.4x\n", ntohs(r->type), ntohs(r->type));
	printf("\tclass\t: %d\t\t0x%.4x\n", ntohs(r->class), ntohs(r->class));
	printf("\tttl\t: %d\t\t0x%.4x\n", ntohs(r->ttl[1]), ntohs(r->ttl[1]));	
	printf("\trdlenght\t: %d\t0x%.4x\n", ntohs(r->rdlenght), ntohs(r->rdlenght));
}
#endif

/*	Display rcode return err	*/
void
display_rcode_error(unsigned char err)
{
	fprintf(stderr, "[ERROR]\trcode => (%d) ", err);
	switch (err)
		{
			case HEADER_RCODE_FORMAT_ERROR:
				fprintf(stderr, "Format error\n");
				break;
			case HEADER_RCODE_SERVER_FAILURE:
				fprintf(stderr, "Server failure\n");
				break;
			case HEADER_RCODE_NAME_ERROR:
				fprintf(stderr, "Name error\n");
				break;
			case HEADER_RCODE_NOT_IMPLEMENTED:
				fprintf(stderr, "Not implemented\n");
				break;
			case HEADER_RCODE_REFUSED:
				fprintf(stderr, "Rcode refused\n");
				break;
#ifdef ALL_TYPES
			case HEADER_RCODE_YX_DOMAIN:
				fprintf(stderr, "YX domain\n");
				break;
			case HEADER_RCODE_YX_RR_SET:
				fprintf(stderr, "YX RR set\n");
				break;
			case HEADER_RCODE_NX_RR_SET:
				fprintf(stderr, "NX RR set\n");
				break;
			case HEADER_RCODE_NOT_AUTH:
				fprintf(stderr, "Not auth\n");
				break;
			case HEADER_RCODE_NOT_ZONE:
				fprintf(stderr, "Not zone\n");
				break;
#endif
			default:
				fprintf(stderr, "Unkown\n");
				break;
		}
}


/*	Main function	*/
int
main(int argc, char **argv)
{
	int optchar;
	int i;
	int sfd;
	s_ip ip;
	ssize_t qsize;
	ssize_t msize;
	int sdomain = AF_INET;
	short verbose = 0;
	
	int port = DNS_PORT;
	char *cdns = NULL; /*	custom dns ptr	*/

	unsigned char data[MAX_PACKET_SIZE] = "";
	
	res_data *rdata = NULL;
	dns_header *header = NULL;
	unsigned char *qname = NULL;
	dns_query *qinfo = NULL;

	struct sockaddr_in dns;
	
	/*	test arg number	*/
	if (argc < 2)
		{	
			printf("Usage : %s [options] domain\n", argv[0]);
			exit(EXIT_FAILURE);
		}

	/*	get argument	*/
	while ((optchar = getopt(argc, argv, "vhs:p:")) != EOF)
		{
			switch (optchar)
				{
					case 'h':	/*	display help and exit	*/
						help(argc, argv);
						exit(EXIT_SUCCESS);
						break;
					case 's': /*	set new server ip	*/
						cdns = optarg;
						break;
					case 'p':	/*	set new server port	*/
						port = atoi(optarg);
						break;
					case 'v':	/*	verbose mode	*/
						verbose = 1;
						break;
					default:
						break;
				}
		}

	/*	set socket	*/
	sfd = socket(sdomain, SOCK_DGRAM, IPPROTO_UDP);
	if (sfd == -1)
		{
			fprintf(stderr, "(%d) %s\n", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}

	/*	set dns sockaddr_in	*/
	dns.sin_family 			= sdomain;
	dns.sin_port 				= htons(port);
	dns.sin_addr.s_addr = inet_addr(cdns != NULL ? cdns : DNS_IP4);

	/*	set DNS header	*/
	header = (dns_header *)&data; /*	header is ptr on data string buffer	*/

	header->id = (unsigned short) htons(getpid());	/*	need sesion id 
																									 *	so take pid	*/
	header->qr = (unsigned short) HEADER_QR_QUERY;
	header->opcode = HEADER_OPCODE_QUERY;
	header->aa = 0;
	header->tc = 0;
	header->rd = (unsigned short)1;					/*	Recursion mode	*/
	header->ra = 0;
	header->z = 0;
	header->rcode = HEADER_RCODE_NO_ERROR;	/*	set without error	*/
	header->qdcount = htons(1); 						/*	One question	*/
	header->ancount = 0;
	header->nscount = 0;
	header->arcount = 0;

#ifdef DEBUG
	displayHeader (header);
#endif

	qsize = sizeof(dns_header);	/* question size = sizeof structure dns_header */
	qname = (unsigned char*) &data[qsize];	/*	qname comme after header	*/

	toDnsName(qname, (unsigned char *)argv[argc-1]);	/*	qname in dns format	*/

	qsize += (strlen((const char *)qname) + 1);	/*	add qname at qsize	*/
	qinfo = (dns_query *) &data[qsize];					/*	qinfo after qname	*/

	qinfo->qtype = (unsigned short)htons(T_A);	/*	set type to A	*/
	qinfo->qclass = (unsigned short)htons(1); 	/*	set class to IN	*/

	qsize += sizeof (dns_query);	/*	update qsize	*/

#ifdef DEBUG
	display_data_bin_hex (data, qsize);
#endif

	if (verbose)
		{
			printf("Server : %s:%d\n\n", cdns != NULL ? cdns : DNS_IP4, port); 
			printf("Sending packet ... ");
		}

	/*	send request	*/
	msize = sendto(sfd, (char *)data, qsize, 
										0, (struct sockaddr *)&dns, sizeof(dns));
	if (msize == -1)
		{
			fprintf(stderr, "(%d) %s\n", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}

	if (verbose)
		{
			printf("Done\n");
			printf("Receiving answer ... ");
			fflush(stdout);
		}

	i = sizeof dns; 
	/*	resceve dns responce	*/
/*	msize = recv(sfd, (void *)data, MAX_PACKET_SIZE, 0);	*/
	msize = recvfrom(sfd, (char *)data, MAX_PACKET_SIZE, 0,
										(struct sockaddr *)&dns, (socklen_t *)&i);	
	if (msize == -1)
		{
			fprintf(stderr, "(%d) %s\n", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}

	if (verbose)
			printf("Done\n");

	/*	read all reponce	*/
	do
		{
			/*	set header on the top off data	*/
			header = (dns_header *)&data;
	
#ifdef DEBUG
			displayHeader(header);
			display_data_bin_hex(data, msize);
#endif

			/*	rcode error	*/
			if (header->rcode != HEADER_RCODE_NO_ERROR)
				{
					display_rcode_error(header->rcode);
					exit(EXIT_FAILURE);
				}

			/*	the dns response contaigne client question
			 *	so qsize == start of response
			 */

			/*	get rdata in data after qsize addr	*/
			rdata = (res_data *)&data[qsize];

#ifdef DEBUG
			display_data_bin_hex((unsigned char *)rdata, sizeof(res_data));
			display_rdata(rdata);
#endif

			qsize += sizeof(res_data);	/*	update qsize	*/

			/*	display domain name	*/
			printf("%s\t", argv[argc-1]);

			/*	display ttl	*/
			printf("%d\t", ntohs(rdata->ttl[1]));	/*	HACK	*/

			/*	get class in response	*/
			switch (ntohs(rdata->class))
				{
					case C_R:
						printf("(reserved)\t");
						break;
					case C_IN:
						printf("IN\t");
						break;
#ifdef ALL_TYPES
					case C_C:
						printf("(chaos)\t");
						break;
					case C_H:
						printf("(hesiod)\t");
						break;
					case C_R2:
						printf("(reserved)\t");
						break;
					default:
						if (ntohs(rdata->class) > C_RP_MIN && 
								ntohs(rdata->class) < C_RP_MAX)
							printf("(reserved for private use)\t");
						else
							printf("(Unassigned)\t");
						break;
#endif
#ifndef ALL_TYPES
					default:
						printf("(Unassigned\t)");
						break;
#endif
				}

			/*	get type in response	*/
			switch (ntohs(rdata->type))
				{
					case T_A:
						printf("A\t");
						break;
#ifdef ALL_TYPES
					case T_NS:
						printf("NS\t");
						break;
					case T_MD:
						printf("MD\t");
						break;
					case T_MF:
						printf("MF\t");
						break;
					case T_CNAME:
						printf("CNAME\t");
						break;
					case T_SOA:
						printf("SOA\t");
						break;
					case T_PTR:
						printf("PTR\t");
						break;
					case T_AAAA:
						printf("AAAA\t");
						break;
#endif
					default:
						printf("(unknown type)\t");
						break;
				}

			/*	parse ip addr
			 *	HACK Not best solution need change */
			if (ntohs(rdata->type) == T_A)
				{
					ip.a = data[qsize];
					ip.b = data[++qsize];
					ip.c = data[++qsize];
					ip.d = data[++qsize];
					printf("%.1d.%.1d.%.1d.%.1d\n", ip.a, ip.b, ip.c, ip.d);
 				}
			else
					printf("Data parsing not implemented...\n");

			qsize++; /*	increment qsize because data finish with '\0'	*/
		}
	while (qsize < msize); /*	if data not finish to read, continue	*/

	/*	close socket (better if close before data parsing ?)	*/
	close(sfd);

	exit(EXIT_SUCCESS);
}

