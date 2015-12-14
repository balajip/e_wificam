#ident "$Id: spawn.c,v 1.2 1997/10/06 21:44:07 hpa Exp $"
/* ----------------------------------------------------------------------- *
 * 
 *  spawn.c - run programs synchronously with output redirected to syslog
 *   
 *   Copyright 1997 Transmeta Corporation - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/wait.h>

#include "automount.h"

/* Used by subprocesses to avoid carrying over the main daemon's rather
   weird signalling environment */
void reset_signals(void)
{
  struct sigaction sa;
  sigset_t allsignals;
  int i;
  
  sigfillset(&allsignals);
  sigprocmask(SIG_BLOCK, &allsignals, NULL);
  
  sa.sa_handler = SIG_DFL;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  for ( i = 1 ; i < NSIG ; i++ )
    sigaction(i, &sa, NULL);
  
  sigprocmask(SIG_UNBLOCK, &allsignals, NULL);
}

#define ERRBUFSIZ 2047		/* Max length of error string excl \0 */

int spawnv(int logpri, const char *prog, const char * const *argv) {
  pid_t f;
  int status, pipefd[2];
  char errbuf[ERRBUFSIZ+1], *p, *sp;
  int errp, errn;
  sigset_t allsignals, tmpsig, oldsig;
  
  sigfillset(&allsignals);
  sigprocmask(SIG_BLOCK, &allsignals, &oldsig);
  
  if ( pipe(pipefd) )
    return -1;

  f = fork();
  if ( f < 0 ) {
    sigprocmask(SIG_SETMASK, &oldsig, NULL);
    return -1;
  } else if ( f == 0 ) {
    reset_signals();
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    dup2(pipefd[1], STDERR_FILENO);
    close(pipefd[1]);
    execv(prog, (char * const *)argv);
    _exit(255);	/* execv() failed */
  } else {
    /* Careful here -- if we enable SIGCHLD yet we may not receive the
       waitpid() at the end */
    
    tmpsig = oldsig;

    sigaddset(&tmpsig, SIGCHLD);
    sigprocmask(SIG_SETMASK, &tmpsig, NULL);

    close(pipefd[1]);

    if ( f < 0 ) {
      close(pipefd[0]);
      return -1;
    }

    errp = 0;
    do {
      while ( (errn = read(pipefd[0], errbuf+errp, ERRBUFSIZ-errp)) == -1
	      && errno == EINTR );
      
      if ( errn > 0 ) {
	errp += errn;

	sp = errbuf;
	while ( errp && (p = memchr(sp, '\n', errp)) ) {
	  *p++ = '\0';
	  if ( sp[0] )	/* Don't output empty lines */
	    syslog(logpri, ">> %s", sp);
	  errp -= (p-sp);
	  sp = p;
	}

	if ( errp && sp != errbuf )
	  memmove(errbuf, sp, errp);

	if ( errp >= ERRBUFSIZ ) {
	  /* Line too long, split */
	  errbuf[errp] = '\0';
	  syslog(logpri, ">> %s", errbuf);
	  errp = 0;
	}
      }
    } while ( errn > 0 );
    close(pipefd[0]);

    if ( errp > 0 ) {
      /* End of file without \n */
      errbuf[errp] = '\0';
      syslog(logpri, ">> %s", errbuf);
    }

    if ( waitpid(f, &status, 0) != f )
      status = -1;		/* waitpid() failed */

    sigprocmask(SIG_SETMASK, &oldsig, NULL);

    return status;
  }
}

int spawnl(int logpri, const char *prog, ...)
{
  va_list arg;
  int argc;
  char **argv, **p;
  
  va_start(arg,prog);
  for ( argc = 1 ; va_arg(arg,char *) ; argc++ ); 
  va_end(arg);
  
  if ( !(argv = alloca(sizeof(char *) * argc)) )
    return -1;
  
  va_start(arg,prog);
  p = argv;
  while ((*p++ = va_arg(arg,char *)));
  va_end(arg);
  
  return spawnv(logpri, prog, (const char **)argv);
}

