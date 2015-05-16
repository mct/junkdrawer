# mct's junkdrawer of junkcode

A dumping ground for random code snippets, tiny tools, and test programs I've
written, or accumulated over the years.  They're generally either too small,
too silly, too undocumented, or too specific to warrant being released on their
own.  Inspired by [Tridge's junk code](https://www.samba.org/junkcode/).  I'm
putting them here because they may be useful to someone, or I've had occasion
to reference them in discussion with someone, or to refer to them myself as
templates or examples.

## Author, and License

Most of this code was written by [Michael Toren](http://github.com/mct)

For things I've written, licened as either Public Domain, or any BSD license
you choose.

Anything in here I haven't written is clearly indicated so.

## Index

* **bin**:  Command line utilities from $HOME/bin

* **c**, **perl**, **python**: Random code snippets

* **queue**: A simple, disk-backed, directory-based queue

* **setuidgids**: Similar to setuidgid (singular) from djb's daemontools, but
   also calls setgroups(2) to set the specified user's supplementary groups.
