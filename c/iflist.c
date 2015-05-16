/* vim:set ts=4 sw=4 ai nobackup nocindent: */

/*
 * mct, Sun Jul  8 00:02:11 EDT 2001
 */

/*

Works on:

SunOS access.ne 4.1.4 1 sun4m
SunOS falkland. 4.1.3_U1 3 sun4c

Linux quint 2.4.3 #1 Tue Apr 24 23:30:21 EDT 2001 sparc64 unknown
Linux cyprus 2.2.17 #1 Mon Nov 6 20:17:16 EST 2000 i486 unknown

SunOS postal 5.8 Generic_108528-06 sun4u sparc SUNW,Ultra-1
SunOS nocmon.netaxs.com 5.7 Generic_106542-10 i86pc i386 i86pc
SunOS ns1.earthstation.net 5.6 Generic_105181-23 sun4u sparc SUNW,Ultra-1



Not tested yet:
SunOS esimail 5.6 Generic_105181-12 sun4m sparc SUNW,SPARCstation-5

Doesn't work:
SunOS ultra 5.6 Generic_105181-15 sun4u sparc SUNW,Ultra-1
(at run time, invalid argument from first ioct())

*/

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#ifndef SIOCGIFCONF
#include <sys/sockio.h> /* solaris */
#endif

#ifndef AF_LINK
#define AF_LINK AF_INET	/* BSD puts AF_LINK on AF_INET network interfaces for some reason */
#endif

#define TEXTSIZE    1024
#define IPTOSBUFFERS    12

char *iptos(u_long in)
{
    static char output[IPTOSBUFFERS][3*4+3+1];
    static short which;
    u_char *p;

    p = (u_char *)&in;
    which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
    sprintf(output[which], "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    return output[which];
}

void fatal(char *fmt, ...)
{
	va_list ap;
	fflush(stdout);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

void pfatal(char *err)
{
	fflush(stdout);
	perror(err);
	exit(1);
}

void *xrealloc(void *oldp, size_t sz)
{
	void *p;

	if (oldp == NULL)
	{
		/* Kludge for SunOS, which doesn't allow remalloc on a NULL pointer */

		oldp = malloc(1);
		if (!oldp)
			fatal("Out of memory!\n");
	}

	p = realloc(oldp, sz);
	if (!p)
		fatal("Out of memory!\n");
	return p;
}

char *safe_strncpy(char *dst, const char *src, size_t size)
{
	dst[size-1] = '\0';
	return strncpy(dst, src, size-1);
}

char *printable(char *s)
{
	static char buf[TEXTSIZE];
	int i;

	for (i = 0; s[i]; i++)
	{
		if (i == TEXTSIZE)
			break;
		else
			buf[i] = isprint(s[i]) ? s[i] : '?';
	}

	if (i == 0)
		safe_strncpy(buf, "(nothing)", TEXTSIZE);
	else
		buf[i] = '\0';

	return buf;
}

u_long findsrc(u_long dest)
{
	struct sockaddr_in sinsrc, sindest;
	int s;
	unsigned int size;

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		pfatal("socket error");
	
	memset(&sinsrc, 0, sizeof(struct sockaddr_in));
	memset(&sindest, 0, sizeof(struct sockaddr_in));

	sindest.sin_family = AF_INET;
	sindest.sin_addr.s_addr = dest;
	sindest.sin_port = htons(53); /* can be anything, really */

	if (connect(s, (struct sockaddr *)&sindest, sizeof(sindest)) < 0)
		pfatal("connect");

	size = sizeof(sinsrc);
	if (getsockname(s, (struct sockaddr *)&sinsrc, &size) < 0)
		pfatal("getsockname");
	
	close(s);
	return sinsrc.sin_addr.s_addr;
}

char *finddev(u_long src)
{
	struct ifconf ifc;
	struct ifreq *ifrp, ifr;
	int numreqs, n, i, s;
	char *ret;

	ret = NULL;
	numreqs = 4096; /* Needs to be large -- the incrementing technique doesn't work on BSD? */
	ifc.ifc_buf = NULL;

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		fatal("socket error");
	
	printf("ifreq buffer set to %d\n", numreqs);

	for (;;)
	{
		ifc.ifc_len = sizeof(struct ifreq) * numreqs;
		ifc.ifc_buf = xrealloc(ifc.ifc_buf, ifc.ifc_len);

		if (ioctl(s, SIOCGIFCONF, &ifc) < 0)
			pfatal("ioctl");

        if (ifc.ifc_len == sizeof(struct ifreq) * numreqs)
		{
			/* assume it overflowed and try again */
			numreqs *= 2;
			printf("ifreq buffer grown to %d\n", numreqs);
			continue;
		}

		break;
	}

	printf("sucessfully retrieved interface list\n");

	for (n = 0, ifrp = ifc.ifc_req;
		n < ifc.ifc_len; n += sizeof(struct ifreq), ifrp++)
	{
		u_long addr;

		memset(&ifr, 0, sizeof(struct ifreq));
		strcpy(ifr.ifr_name, ifrp->ifr_name);

		if (ifrp->ifr_addr.sa_family != AF_INET &&
			ifrp->ifr_addr.sa_family != AF_LINK)
		{
			printf("ignoring non-AF_INET interface %s\n", printable(ifr.ifr_name));
			continue;
		}

		if (ioctl(s, SIOCGIFFLAGS, &ifr) < 0)
			pfatal("ioctl(SIOCGIFFLAGS)");

		if ((ifr.ifr_flags & IFF_UP) == 0)
		{
			printf("Ignoring down interface %s\n", printable(ifr.ifr_name));
			continue;
		}

		if (ioctl(s, SIOCGIFADDR, &ifr) < 0)
			pfatal("ioctl(SIOCGIFADDR)");
		else
			addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;

		printf("Discovered interface %s, address %s\n",
			printable(ifr.ifr_name), iptos(addr));

		if (addr == src)
		{
			printf("Interface matches source address %s\n", iptos(src));
			ret = xrealloc(NULL, sizeof(ifr.ifr_name+1));
			strcpy(ret, ifr.ifr_name);

			/* Deal with virtual hosts */
			for (i = 0; ret[i]; i++)
				if (ret[i] == ':')
					ret[i] = '\0';
		}
	}

	free(ifc.ifc_buf);
	return ret;
}

int main(int argc, char *argv[])
{
	struct hostent *hostdest;
	u_long src, dest;
	char *dev;

	if (argc != 2)
		fatal("Usage: %s <host>\n", argv[0]);

	hostdest = gethostbyname(argv[1]);

	if (hostdest == NULL)
		fatal("%s: bad hostname\n", argv[1]);
	else if (hostdest->h_length != 4)
		fatal("Something odd is happening\n");
	else
		memcpy ((void *)&dest, *(hostdest->h_addr_list), hostdest->h_length);

	src = findsrc(dest);
	dev = finddev(src);

	printf("To reach %s, use device %s with source of %s\n",
		iptos(dest), dev, iptos(src));

	return 0;
}
