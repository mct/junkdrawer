/*
 * rawsock.xs
 *
 * XS wrapper for SOCK_RAW
 *
 * Copyright (c) 2001 Stephane Aubert. All rights reserved. This program is 
 * free software; you can redistribute it and/or modify it under the same 
 * terms as Perl itself.
 *
 */

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>


struct IPHDR {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    u_int8_t ihl:4;
    u_int8_t version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    u_int8_t    version:4;
    u_int8_t ihl:4;
#else
#error  "Please fix <bytesex.h>"
#endif
    u_int8_t tos;
    u_int16_t tot_len;
    u_int16_t id;
    u_int16_t frag_off;
    u_int8_t ttl;
    u_int8_t protocol;
    u_int16_t check;
    u_int32_t saddr;
    u_int32_t daddr;
    /*The options start here. */
};

MODULE = Net::RawSock  PACKAGE = Net::RawSock PREFIX = rawsock_

PROTOTYPES: DISABLE

int 
write_ip(pkt)
  char *pkt

  CODE:
    int fd, fl, val=1, len;
    struct sockaddr_in sa;
    struct IPHDR  *ip_hdr;

    ip_hdr = (struct IPHDR *)pkt;
    bzero((char *)&sa,sizeof(sa));
    sa.sin_addr.s_addr = ip_hdr->daddr;
    //sa.sin_addr.s_addr = 0x0100007f; /*for localhost*/
    sa.sin_family = AF_INET;

    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) { 
      RETVAL = 1;
    } else {
      fl = fcntl ( fd, F_GETFL, 0); fl |= O_NONBLOCK; fcntl ( fd, F_SETFL, fl);
      if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val)) < 0) {  
        RETVAL = 2;
      } else {
        len = ntohs(ip_hdr->tot_len);
        if (sendto(fd,(char *)pkt,len,0x0,(struct sockaddr *)&sa,sizeof(sa))<0) {
          RETVAL = 3;
        } else {
          RETVAL = 0;
          close(fd);
        }
      }
    }
  OUTPUT:
    RETVAL
 
