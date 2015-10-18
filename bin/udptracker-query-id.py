#!/usr/bin/python
# vim:set ts=4 sw=4 ai et smarttab:

import random
import sys
import time
from socket import *
from struct import pack, unpack
import signal

def handler(signum, frame):
    print "%d %s %-16s %s:%d" % (time.time(), time.strftime("%Y-%m-%d %H:%M:%S"), "alarm", hostname, port)
    sys.exit()

signal.signal(signal.SIGALRM, handler)
signal.alarm(10)

sock = socket(AF_INET,SOCK_DGRAM)
sock.setblocking(1)
sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
sock.setsockopt(SOL_SOCKET, SO_RCVBUF, 128*1024)
sock.setsockopt(SOL_SOCKET, SO_SNDBUF, 128*1024)
sock.bind(('', 0))

hostname, port = sys.argv[1], int(sys.argv[2])

try:
    hostname = gethostbyname(hostname)
except:
    print "%d %s %-16s %s:%d" % (time.time(), time.strftime("%Y-%m-%d %H:%M:%S"), "dnsfail", hostname, port)
    sys.exit()

id = random.randint(0, 0xffffffff)
msg = pack("!QLL", 0x41727101980, 0, id) # action=0, Connection Request
print "-->", msg.encode("hex")
sock.sendto(msg, (hostname, port))

msg, (remote_ip, remote_port) = sock.recvfrom(128*1024)
print "<--", msg.encode("hex")
action, returned_id, connection_id = unpack("!LLQ", msg)
assert action == 0
assert returned_id == id

print "%d %s %016x %s:%d" % (time.time(), time.strftime("%Y-%m-%d %H:%M:%S"), connection_id, hostname, port)
