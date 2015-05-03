#!/usr/bin/perl -l

my $field = shift or die "$0: <fieldnumer>\n";
$field =~ s/(-?\d+)/$1 > 0 ? $1-1 : $1/ge;
$|++;

while (<>) {
    chomp;
    my @f = split;
    print join " ", @f[eval $field];
}

__END__

http://lists.netisland.net/archives/phlpm/phlpm-2003/threads.html#00402

From: mjd-perl-pm@plover.com
To: phl@lists.pm.org
Subject: "How did I live without this?"
Date: Thu, 14 Aug 2003 13:14:59 -0400

This is totally trivial, but it's making my life a lot easier.  It's a
command that I've named 'f' for 'field':

        #!/usr/bin/perl

        my $field = shift or usage();
        $field -= 1 if $field > 0;

        while (<>) {
                chomp;
                my @f = split;
                print $f[$field], "\n";
        }

        sub usage {
                print "$0 fieldnumber\n";
                exit 1;
        }


Now when I want to extract field #6 (the target URL) from my web log
file, I can use

        f 6 /usr/local/apache/logs/perl-access-log

Sometimes I want to run some command on all the files in the current
directory that were modified today:

        command `ls -l | grep 'Aug 14' | f -1`

The 'f -1' extracts the last field (the filename) from each line.

I do know how I got along without this.  I used to use 'awk':

        awk '{print $6}' /usr/local/apache/logs/perl-access-log

'f' is a lot less to type.

Now that I'm composing this, it occurs to me that I could save a bit
of code by rewriting this as

        #!/usr/bin/perl -aln

        sub usage {
                print STDERR "$0 fieldnumber";
                exit 1;
        }

        BEGIN {
          $field = shift or usage();
          $field -= 1 if $field > 0;
        }

        print $F[$field];

and let '-a' take care of the 'split', '-l' take care of the 'chomp',
and '-n' take care of the 'while'.  I'm not sure I like it better, though.

-
**Majordomo list services provided by PANIX <URL:http://www.panix.com>**
**To Unsubscribe, send "unsubscribe phl" to majordomo@lists.pm.org**
