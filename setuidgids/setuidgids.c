// vim:set ts=2 sw=2 ai et:

/*
 * setuidgids -- similar to setuidgid (singular) from djb's daemontools, but
 * also calls setgroups(2) to set the specified user's supplementary groups.
 *
 * Copyright 2014 Michael Toren <mct@toren.net>
 * Licensed under the terms of the Do What The Fuck You Want To Public License
 */

#ifndef __linux__
#warning "This program has only been tested on Linux"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <err.h>

int main(int argc, char *argv[]) {
  struct passwd *pw;

  if (argc < 3)
    errx(1, "Usage: %s <username> <program> [args ..]", argv[0]);

  pw = getpwnam(argv[1]);
  if (!pw)
    errx(1, "Unknown user");
  
  if (initgroups(pw->pw_name, pw->pw_gid) != 0)
    err(1, "initgroups");

  if (setgid(pw->pw_gid) != 0)
    err(1, "setgid");

  if (setuid(pw->pw_uid) != 0)
    err(1, "setuid");

  execvp(argv[2], &argv[2]);
  err(1, argv[2]);
}
