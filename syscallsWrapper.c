
/* These are wrappers to system calls
	typically avaiable to avoid common portability problems
	between different UNIX implementations 
	I hold no ownership on these wrappers, actually
	most of them are taken from a book called
	CSAPP-2e */
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include "syscallsWrapper.h"

/* begin unixerror */
void unixError(const char *msg) /* unix-style error */
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
}
/* end unixerror */


/* Begin signal handling */
handler_t *Signal(int signum, handler_t *handler) 
{
	struct sigaction action, old_action;

	action.sa_handler = handler;  
	sigemptyset(&action.sa_mask); /* block sigs of type being handled */
	action.sa_flags = SA_RESTART; /* restart syscalls if possible */

	if (sigaction(signum, &action, &old_action) < 0)
		return SIG_ERR;
	return (old_action.sa_handler);
}
/* End signal handling */

/* Portable fork */
/* After calling fork, it depends on the UNIX system whether 
the child or the parent starts first, so to make the code easier for
debuggin, we'll make a portable version that randomly calls one before
the other.
Both the idea and the code is taken from : CSAPP-2e Chapter 8 */

/* Sleep for a random period between [0, MAX_SLEEP] us. */
#define MAX_SLEEP 100000

/* Macro that maps val into the range [0, RAND_MAX] */
#define CONVERT(val) (((double)val)/(double)RAND_MAX)

pid_t Fork(void)
{
	static struct timeval time;
	unsigned bool, secs;
	pid_t pid;
	
	/* Generate a different seed each time the function is called */
	gettimeofday(&time, NULL);
	srand(time.tv_usec);

	/* Determine whether to sleep in parent of child and for how long */
	bool = (unsigned)(CONVERT(rand()) + 0.5);
	secs = (unsigned)(CONVERT(rand()) * MAX_SLEEP);

	/* Call the real fork function */
	if ((pid = fork()) < 0)
		return pid;

	/* Randomly decide to sleep in the parent or the child */
	if (pid == 0) { /* Child */
		if(bool) {
			usleep(secs);
		}
	}
	else { /* Parent */
		if (!bool) {
			usleep(secs);
		}
	}
	/* Return the PID like a normal fork call */
	return pid;
}
/* End portable fork */
