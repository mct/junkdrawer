#!/usr/bin/python
# vim:set ts=4 sw=4 ai et smarttab:
# Fri Jun 28 12:31:23 PDT 2013

import sys
import socket
import os

host = sys.argv[1]
port = sys.argv[2]
data = sys.argv[3]

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect((host, int(port)))
s.send(data)

while True:
    data = s.recv(4096)
    print "%r" % data
