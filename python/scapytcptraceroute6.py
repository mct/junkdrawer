#!/usr/bin/env python3

'''
scapytcptraceroute6.py -- A TCP traceroute sandbox for IPv6, using scapy
Copyright (c) 2024, Michael Toren <mct@toren.net>
Released under the terms of the 2-clause BSD license
'''

import argparse
import random
import socket
from scapy.all import *

open_sockets = []
def new_sport():
    global open_sockets
    s = socket.socket()
    s.bind(('', 0))
    open_sockets.append(s)
    return s.getsockname()[1]

def lookup_aaaa(hostname):
    ipv6_addresses = socket.getaddrinfo(hostname, None, socket.AF_INET6)
    #for address in ipv6_addresses:
    #    print(type(address), repr(address))
    return ipv6_addresses[0][4][0]

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--sport', type=int, default=None)
    parser.add_argument('--track-port', action='store_true')
    parser.add_argument('--from-ttl', '-f', type=int, default=1)
    parser.add_argument('--max-ttl', '-m', type=int, default=30)
    parser.add_argument('--debug', '-d', action='store_true')
    parser.add_argument('--retry', '-r', type=int, default=1)
    parser.add_argument('--timeout', '-t', type=float, default=1)
    parser.add_argument('--interval', '-i', type=float, default=0.1)
    parser.add_argument('destination')
    parser.add_argument('dport', default="80", nargs='?')
    args = parser.parse_args()

    if args.track_port:
        assert args.sport is None, "Cannot combine --sport with --track-port"
    elif args.sport is None:
        args.sport = new_sport()

    try:
        args.dport = int(args.dport)
    except ValueError:
        try:
            args.dport = socket.getservbyname(args.dport)
        except OSError:
            print("Unknown service:", args.dport)
            exit(1)

    dst = lookup_aaaa(args.destination)
    print(f"Tracing the path to {dst} on port {args.dport}")

    probes = PacketList()
    for ttl in range(args.from_ttl, args.max_ttl+1):
        p = IPv6(dst=dst, hlim=ttl)

        if args.track_port:
            p /= TCP(sport=new_sport(), dport=args.dport)
        else:
            p /= IPv6ExtHdrFragment(offset=0, m=0, id=random.randint(0, 2**32)) / TCP(sport=args.sport, dport=args.dport)

        probes.append(p)

    if args.debug:
        probes.show()
        for p in probes:
            p.show()

    ans, unans = sr(probes, timeout=args.timeout, inter=args.interval, retry=args.retry)
    ans.make_table(lambda s,r: (s.dst, s.hlim, r.src))

    if args.debug:
        print()
        for i in ans:
            print(i)
