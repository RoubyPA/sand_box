#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

/* Fallback Function */
void
signal_fallback(int signum)
{
	fprintf(stdout, "PID : %d \n", getpid()) ;
	fprintf(stdout, "\tSignal (%d) %s\n", signum, sys_siglist[signum] ) ;
}

/* Main function */
int
main(void)
{
	int	signum ;

	fprintf(stdout, "PID = %d\n", getpid() ) ;

  /* Set signal listener */
	for(signum = 1 ; signum <= NSIG ; signum++)
    {
		  if(signal(signum, signal_fallback) == SIG_ERR)
		    {
			    fprintf(stderr, "[ERROR] Signal N°%d non interseptable avec la fonction signal()\n", signum) ;
		    }
	  }

  /* Main loop */
  while(1)
    pause();

	return EXIT_SUCCESS ;
}
