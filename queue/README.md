# queue

A simple, disk-backed, directory-based queue.  Each queue item is a
file in the queue directory.

* **queue-add** atomically adds a file to the queue

* **queue-worker** iterates through the queue, handles appropriate
		   locking, and feeds the file to the standard input
                   of the program specific on the command line.

## License

Either Public Domain, or any BSD license you choose.

## Author

[Michael Toren](http://github.com/mct)
