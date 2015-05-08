#!/usr/bin/perl -w
# (c) Copyright 2002, Michel C. Toren <mct@toren.net>
# mct, Tue Dec 24 13:06:10 EST 2002
#
# tested on:
#	Linux quint 2.4.19 #1 Sun Aug 4 01:14:19 EDT 2002 sparc64 unknown
#	Linux antarctica 2.4.19 #1 Tue Oct 29 21:33:52 EST 2002 i686 unknown
#	SunOS access.ne 4.1.4 1 sun4m
#	SunOS postal 5.8 Generic_108528-06 sun4u sparc SUNW,Ultra-1
#	FreeBSD x-wing 4.6.2-RELEASE-p3 FreeBSD 4.6.2-RELEASE-p3 #1
#	NetBSD uriel 1.5.3_ALPHA NetBSD 1.5.3_ALPHA (URIEL) #2
#	NetBSD grappa 1.6F NetBSD 1.6F (GRAPPA) #8

# http://cvsweb.netbsd.org/bsdweb.cgi/src/usr.sbin/traceroute/traceroute.c.diff?r1=1.25&r2=1.26&f=h

#Use a little bit of magic suggested by Ken Hornstein to find an
#appropriate source address for the UDP packets. Fixes PR bin/4427
#by Luke Mewburn.


use Socket;
my $host = shift || "4.2.2.2";
my $udp = getprotobyname("udp") or die "getprotobyname: $!\n";
socket(S, PF_INET, SOCK_DGRAM, $udp) or die "scoket: $!\n";
my $s = sockaddr_in 1, (inet_aton $host or die "invalid destination: $host\n");
connect S, $s or die "connect: $!\n";
print inet_ntoa((sockaddr_in getsockname S)[1]), "\n";
