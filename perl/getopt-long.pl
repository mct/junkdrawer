#!/usr/bin/perl
# vi:set ai sw=4 ts=4 et smarttab:

use Getopt::Long qw(:config  gnu_getopt);

use strict;
use warnings;

my ($device, $verbose, $dry_run, $help);

my $result = GetOptions (
        "device|line|l=s"   => \$device,
        "verbose|v+"        => \$verbose,
        "dry-run|n"         => \$dry_run,
        "help|h"            => \$help,
    ) or exit 255;

print "Device:  $device\n"   if $device;
print "Verbose: $verbose\n"  if $verbose;
print "Dry Run: $dry_run\n"  if $dry_run;
print "Help:    $help\n"     if $help;
