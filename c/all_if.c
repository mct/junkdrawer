/*
 * Obtain all all of the interfaces for a host.  Based on
 * code from Lincoln Steins' IO::Interface module, and
 * tcptraceroute by Michael C. Toren.
 *   http://michael.toren.net/code/tcptraceroute/
 *
 * Thu Dec 20 17:51:10 EST 2001 Kyle R. Burton mortis@voicenet.com
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#if !defined( __OpenBSD__ )
#include <net/if.h>
#endif
#include <arpa/inet.h>


#if defined __OpenBSD__ || defined __FreeBSD__
#define HASSALEN
#endif


#if defined( OSIOCGIFCONF )
#define PLATFORM_SIOCGIFCONF OSIOCGIFCONF
#else
#define PLATFORM_SIOCGIFCONF SIOCGIFCONF
#endif

#if !defined( AF_LINK )
#define AF_LINK AF_INET /* BSD defines some AF_INET network interfaces as AF_LINK */
#endif

#define FALSE 0
#define TRUE (!FALSE)

int enumerate_interfaces( void );
int if_is_inet( struct ifreq* ifrp );
int if_is_up( int sck, struct ifreq* ifrp );
int create_socket( void );
char* iptos( unsigned long in );

int main( int argc, char** argv )
{
  enumerate_interfaces();
  return 0;
}

int enumerate_interfaces()
{
  struct ifconf ifc;
  struct ifreq *ifrp, ifr;
  unsigned long addr;
  struct sockaddr_in *sinp;
  int reqsBlockSize = 1024;
  int numreqs = reqsBlockSize;
  int maxReqsBlockSize = 200000;
  int rv = TRUE;
  int sck, i;
  char *p;
  int salen = sizeof( struct ifreq );
  u_char family = 0;

  memset( &ifc, 0, sizeof(ifc) );
  memset( &ifr, 0, sizeof(ifr) );

  sck = create_socket();

  while(1) {
    ifc.ifc_len = sizeof( struct ifreq ) * numreqs;
    ifc.ifc_buf = malloc( ifc.ifc_len );

    if( ioctl( sck, SIOCGIFCONF, &ifc ) < 0 ) {
      printf("%s(%d) Error during ioctl call: %s\n",__FILE__,__LINE__,
        strerror(errno));
      exit( errno );
    }

    if( ifc.ifc_len >= ( sizeof( struct ifreq ) * numreqs ) ) {
      /* Assume it overflowed and try again */
      numreqs += reqsBlockSize;
      free( ifc.ifc_buf );
      if( numreqs > maxReqsBlockSize ) {
        rv = FALSE;
        break;
      }
      continue;
    }
    break; /* success. */
  }

  #if defined HASSALEN
    printf("%s(%d) HASSALEN\n",__FILE__,__LINE__);
  #endif
  printf("%s(%d) using salen of: %d\n",__FILE__,__LINE__, salen);


  printf("%s(%d) ifc.ifc_len: %d\n",__FILE__,__LINE__, ifc.ifc_len );
  for( p = ifc.ifc_buf; p < ( ifc.ifc_buf + ifc.ifc_len ); p += salen ) {
    ifrp = (struct ifreq*)p;
    memset( &ifr, 0, sizeof( struct ifreq ) );
    strcpy( ifr.ifr_name, ifrp->ifr_name );

    #if defined HASSALEN

      salen = sizeof( ifrp->ifr_name ) + ifrp->ifr_addr.sa_len;
      if( salen < sizeof( struct ifreq ) ) {
        printf("%s(%d) tried, but reverting from: %d\n",__FILE__,__LINE__, salen);
        salen = sizeof( struct ifreq );
      }

      addr = (( struct sockaddr_in *) &ifrp->ifr_addr)->sin_addr.s_addr;
      /* family = (( struct sockaddr_in *) &ifrp->ifr_addr)->sin_family; */
      if( ioctl( sck, SIOCGIFFLAGS, &ifr ) ) {
        printf("%s(%d) ioctl error: %s\n",__FILE__,__LINE__,strerror(errno));
        exit(0);
      }
      family = ( ( struct sockaddr_in *) &ifr.ifr_addr )->sin_family;

    #else

      salen = sizeof(*ifrp);
      if( ioctl( sck, SIOCGIFADDR, &ifr ) ) {
        printf("%s(%d) ioctl error: %s\n",__FILE__,__LINE__,strerror(errno));
        exit(0);
      }
      addr = (( struct sockaddr_in *) &ifr.ifr_addr)->sin_addr.s_addr;
      family = AF_INET;

    #endif

    for( i = 0; ifr.ifr_name[i]; ++i ) {
      if( ':' == ifr.ifr_name[i] ) {
        printf("%s(%d) vhost issue: %s\n",__FILE__,__LINE__,ifr.ifr_name);
        ifr.ifr_name[i] = '\0';
      }
    }

    if( !if_is_inet( ifrp ) ) {
      printf("%s(%d) skipping, addr is not inet or link: %s\n",
        __FILE__,__LINE__,ifr.ifr_name);
      continue;
    }

    if( !if_is_up( sck, ifrp ) ) {
      printf("%s(%d) skipping, interface is down: %s\n",
        __FILE__,__LINE__,ifr.ifr_name);
      continue;
    }


    sinp = ( struct sockaddr_in *) &ifr.ifr_addr;

    printf("%s(%d) AF_INET, AF_INET6, family: %d %d %d\n",__FILE__,__LINE__,AF_INET, AF_INET6, family );

    if( AF_INET6 == (AF_INET6 & family) ) {
      printf("%s(%d) if is AF_INET6: %s\n",__FILE__,__LINE__,ifr.ifr_name );
      continue;
    }

    printf("%s(%d) if: %s %s\n",__FILE__,__LINE__,ifr.ifr_name, iptos( addr ) );
  }

  free( ifc.ifc_buf );

  return rv;
}

int if_is_inet( struct ifreq* ifrp )
{
  return ifrp->ifr_addr.sa_family == AF_INET
      || ifrp->ifr_addr.sa_family == AF_LINK;
}

int if_is_up( int sck, struct ifreq* ifrp )
{
  if( ioctl( sck, SIOCGIFFLAGS, ifrp ) < 0 ) {
    printf("%s(%d) ioctl error: %s\n",__FILE__,__LINE__, strerror( errno ) );
    exit(0);
  }
  return 0 != (ifrp->ifr_flags & IFF_UP);
}

int create_socket( void )
{
  int sck = socket( AF_INET, SOCK_DGRAM, 0 );
  if( sck < 0 ) {
    printf("%s(%d) Error creating udp socket: %s\n",__FILE__,__LINE__,
      strerror(errno));
    exit( errno );
  }
  return sck;
}

#define IPTOSBUFFERS 12
char* iptos( unsigned long in )
{
  static char output[IPTOSBUFFERS][3*4+3+1];
  static short which = 0;
  unsigned char *p;
  p = (unsigned char*)&in;
  which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
  sprintf( output[which], "%d.%d.%d.%d", p[0], p[1], p[2], p[3] );
  return output[which];
}

