/* vim:set ts=4 sw=4 nocindent: */
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

u_long findsrc(u_long dest)
{
	struct protoent *proto;
	struct sockaddr_in sinsrc, sindest;
	int s, size;

	/* Find the protocol number for UDP in /etc/protocols */
	if ((proto = getprotobyname("udp")) == NULL)
	{
		fprintf(stderr, "getprotobyname(\"udp\") failed?\n");
		exit(-1);
	}

	/* Allocate a UDP socket */
	if ((s = socket(PF_INET, SOCK_DGRAM, proto->p_proto)) < 0)
	{
		perror("socket error");
		exit(-1);
	}

	/* Zero out the source and destination strutures */
	memset(&sinsrc, 0, sizeof(struct sockaddr_in));
	memset(&sindest, 0, sizeof(struct sockaddr_in));

	/* Fill in the requested destination */
	sindest.sin_family = AF_INET;
	sindest.sin_addr.s_addr = dest;
	sindest.sin_port = htons(53); /* can be anything */

	/*
	 * "connect", however in the case of UDP there is no connection
	 * handshake, so no packets are generated.  As a side effect, the kernel
	 * will determine what source address it will use in the future to reach
	 * the destination, and associate this information with the socket
	 */
	if (connect(s, (struct sockaddr *)&sindest, sizeof(sindest)) < 0)
	{
		perror("connect");
		exit(-1);
	}

	/* Retreive the socket's source address with getsockname() */
	size = sizeof(sinsrc);
	if (getsockname(s, (struct sockaddr *)&sinsrc, &size) < 0)
	{
		perror("getsockname");
		exit(-1);
	}

	/* Cleanup the socket... */
	close(s);

	/* ...and return the source address */
	return sinsrc.sin_addr.s_addr;
}


int main(int argc, char *argv[])
{
	char *host;
	u_long foo;

	/* Default to 207.106.1.2 (ns1.netaxs.com) if no host is specified */
	host = (argc >= 2) ? argv[1] : "207.106.1.2";
	foo = inet_addr(host);
	foo = htonl(foo);
	printf("%lu\n", foo);

	return 0;
}
