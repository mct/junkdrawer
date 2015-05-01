#!/bin/bash
#
# (c) Michael C. Toren <mct@toren.net>
#
# I don't remember when I wrote this; it's a kludge, but it works.  Filters
# text from STDIN through ispell(1), and prints the results to STDOUT.
# Intended to be called from vi, something along the lines of: 
#
#	:%!sp
# or:
#
#	:'x,'y !sp
# 

set -e

tmp="/tmp/ispell-$$-$USER"

if [ -e $tmp ]
then
	echo "***  Error:  temp file \"$tmp\" exists"
	exit 1
else
	if [ -n "$*" ]
	then
		echo "$*" > $tmp
	else
		cat > $tmp
	fi
fi

ispell -M -S -x $tmp < /dev/tty > /dev/tty

r=$?

cat $tmp
rm -f $tmp

exit $r
