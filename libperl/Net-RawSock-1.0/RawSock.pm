# RawSock.pm
#
# Copyright (c) 2001 Stephane Aubert. All rights reserved. This program is 
# free software; you can redistribute it and/or modify it under the same 
# terms as Perl itself.

package Net::RawSock;

use strict;
use warnings;

require Exporter;
require DynaLoader;

use vars qw($VERSION @ISA);
our @ISA = qw(Exporter DynaLoader);
our @EXPORT = qw();
our $VERSION = '1.0';

bootstrap Net::RawSock $VERSION;

1;
__END__
# Below is the stub of documentation for RawSock.pm

C<Net::RawSock> - module to send network raw IP datagrams.

=head1 SYNOPSIS

=head1 DESCRIPTION

C<Net::RawSock> provides a base function to send raw IP 
datagrams from Perl. The raw IP datagam is packed into a buffer
and written on the network layer with the write_ip() function.

=head1 COPYRIGHT

Copyright (c) 2001 Stephane Aubert. All rights reserved. This program is 
free software; you can redistribute it and/or modify it under the same 
terms as Perl itself.

=head1 AUTHOR

Stephane Aubert E<lt>Stephane.Aubert@hsc-labs.frE<gt>

=cut

