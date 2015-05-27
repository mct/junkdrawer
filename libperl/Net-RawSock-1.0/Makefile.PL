 use ExtUtils::MakeMaker;
WriteMakefile(
    NAME         => 'Net::RawSock',
    DISTNAME     => 'Net-RawSock',
    VERSION_FROM => 'RawSock.pm', # finds $VERSION
    LIBS         => [''],   # e.g., '-lm'
    DEFINE       => '',     # e.g., '-DHAVE_SOMETHING'
    INC          => '',     # e.g., '-I/usr/include/other'
    dist => {
      'COMPRESS' => "gzip -9f",
      'SUFFIX'   => "gz"
    }
);

