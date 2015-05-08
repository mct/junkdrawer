#!/usr/bin/perl
# vim:set ts=4 sw=4 ai nowrap:

use Getopt::Std;

use strict;
use warnings;

our ($opt_n, $opt_v, $opt_V, $opt_w);
getopts "nvVwW" or exit 1;

$opt_n and print "-n\n";
$opt_v and print "-v\n";
$opt_V and print "-V\n";
$opt_w and print "-w\n";

print "Args: ", join(" ", @ARGV), "\n";
