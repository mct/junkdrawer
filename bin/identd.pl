#!/usr/bin/perl

# A really stupid identd that returns a fixed string, to get around servers
# (mostly IRC) that require ident to be running.  Launched from inetd, as in:
#
#       ident           stream  tcp     nowait  nobody  /home/mct/junkdrawer/bin/identd.pl identd.pl

use strict;
use warnings;

my $fixed = shift || 'ident-sucks';
my $line = <STDIN>;
$line =~ s/\s+$//;
print "$line : USERID : UNIX : $fixed\r\n";
