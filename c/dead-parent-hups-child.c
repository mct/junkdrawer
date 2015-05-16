/*
 * $Id: dead-parent-hups-child.c,v 1.1 2005/07/07 17:33:59 mct Exp $
 * mct, Thu Jul  7 13:12:55 EDT 2005
 * vim:set ts=4 sw=4 ai nocindent:
 */

/*
 * Sets prctl(PR_SET_PDEATHSIG, SIGHUP), and exec's a program and arguments
 * specified on the command line.  For use when running ssh from tunnel-warden,
 * so that we don't end up with orphaned ssh processes.
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <program> [arguments]\n", argv[0]);
		return 1;
	}

	if (prctl(PR_SET_PDEATHSIG, SIGHUP) != 0)
	{
		perror("prctl(PR_SET_PDEATHSIG)");
		return 2;
	}

	execvp(argv[1], &argv[1]);

	fprintf(stderr, "exec: %s: ", argv[1]);
	perror("");
	return 255;
}
