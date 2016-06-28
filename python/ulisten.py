#!/usr/bin/python
# vim:set ts=4 sw=4 ai et smarttab:
# Fri Jun 28 12:31:23 PDT 2013

import sys
import socket
import os

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(('0.0.0.0', 6565))

while True:
    x = sock.recvfrom(1024*10)
    print repr(x)
