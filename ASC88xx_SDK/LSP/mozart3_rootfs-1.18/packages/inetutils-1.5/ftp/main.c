/*
 * Copyright (c) 1985, 1989, 1993, 1994, 2002
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if 0
static char sccsid[] = "@(#)main.c	8.6 (Berkeley) 10/9/94";
#endif

/*
 * FTP User Program -- Command Interface.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*#include <sys/ioctl.h>*/
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/ftp.h>

#include <ctype.h>
#include <error.h>
#include <netdb.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

/* Define macro to nothing so declarations in ftp_var.h become definitions. */
#define FTP_EXTERN
#include "ftp_var.h"

#if HAVE_READLINE_READLINE_H
# include <readline/readline.h>
#endif


char *program_name;

#define DEFAULT_PROMPT "ftp> "
static char *prompt = 0;

static void
usage (int err)
{
  if (err != 0)
    {
      fprintf (stderr, "Usage: %s [OPTION...] [HOST [PORT]]\n", program_name);
      fprintf (stderr, "Try `%s --help' for more information.\n", program_name);
    }
  else
    {
      fprintf (stdout, "Usage: %s [OPTION...] [HOST [PORT]]\n", program_name);
      puts ("Remote file transfer.\n\n\
  -d, --debug                Turn on debugging mode\n\
  -g, --no-glob              Turn off file name globbing\n\
  -i, --no-prompt            Don't prompt during multiple-file transfers\n\
  -n, --no-login             Don't automatically login to the remove system\n\
  -t, --trace                Enable packet tracing\n\
  -p, --prompt[=PROMPT]      Print a command-line prompt (optionally PROMPT),\n\
                             even if not on a tty\n\
  -v, --verbose              Be verbose\n\
      --help                 Give this help list\n\
  -V, --version              Print program version");

      fprintf (stdout, "\nSubmit bug reports to %s.\n", PACKAGE_BUGREPORT);
    }
  exit (err);
}

static const char *short_options = "dginp::tv";
static struct option long_options[] =
{
  { "trace", no_argument, 0, 't' },
  { "verbose", no_argument, 0, 'v' },
  { "no-login", no_argument, 0, 'n' },
  { "no-prompt", no_argument, 0, 'i' },
  { "debug", no_argument, 0, 'd' },
  { "no-glob", no_argument, 0, 'g' },
  { "help", no_argument, 0, '&' },
  { "prompt", optional_argument, 0, 'p' },
  { "version", no_argument, 0, 'V' },
  { 0 }
};

int
main (int argc, char *argv[])
{
  int ch, top;
  struct passwd *pw = NULL;
  char *cp;

  program_name = argv[0];

  sp = getservbyname ("ftp", "tcp");
  if (sp == 0)
    error (1, 0, "ftp/tcp: unknown service");
  doglob = 1;
  interactive = 1;
  autologin = 1;

  while ((ch = getopt_long (argc, argv, short_options, long_options, 0))
	 != EOF)
    {
      switch (ch) {
      case 'd': /* Enable ebug mode.  */
	options |= SO_DEBUG;
	debug++;
	break;

      case 'g': /* No glob.  */
	doglob = 0;
	break;

      case 'i': /* No prompt.  */
	interactive = 0;
	break;

      case 'n': /* No automatic login.  */
	autologin = 0;
	break;

      case 't': /* Enable packet tracing.  */
	trace++;
	break;

      case 'v': /* Verbose.  */
	verbose++;
	break;

      case 'p': /* Print command line prompt.  */
	prompt = optarg ? optarg : DEFAULT_PROMPT;
	break;

      case '&': /* Usage.  */
	usage (0);
	/* Not reached.  */

      case 'V': /* Version.  */
	printf ("ftp (%s) %s\n", PACKAGE_NAME, PACKAGE_VERSION);
	exit (0);

      case '?':
      default:
	usage (1);
	/* Not reached.  */
      }
    }

  argc -= optind;
  argv += optind;

  fromatty = isatty (fileno (stdin));
  if (fromatty)
    {
      verbose++;
      if (! prompt)
	prompt = DEFAULT_PROMPT;
    }

  cpend = 0;	/* no pending replies */
  proxy = 0;	/* proxy not active */
  passivemode = 0; /* passive mode not active */
  crflag = 1;	/* strip c.r. on ascii gets */
  sendport = -1;	/* not using ports */
  /*
   * Set up the home directory in case we're globbing.
   */
  cp = getlogin ();
  if (cp != NULL)
    pw = getpwnam (cp);
  if (pw == NULL)
    pw = getpwuid(getuid());
  if (pw != NULL)
    {
      char *buf = malloc (strlen (pw->pw_dir) + 1);
      if (buf)
	{
	  strcpy (buf, pw->pw_dir);
	  home = buf;
	}
    }
  if (argc > 0)
    {
      char *xargv[5];

      if (setjmp (toplevel))
	exit (0);
      signal(SIGINT, intr);
      signal(SIGPIPE, lostpeer);
      xargv[0] = program_name;
      xargv[1] = argv[0];
      xargv[2] = argv[1];
      xargv[3] = argv[2];
      xargv[4] = NULL;
      setpeer (argc+1, xargv);
    }
  top = setjmp (toplevel) == 0;
  if (top)
    {
      signal (SIGINT, intr);
      signal (SIGPIPE, lostpeer);
    }
  for (;;)
    {
      cmdscanner(top);
      top = 1;
    }
}

RETSIGTYPE
intr (int sig ARG_UNUSED)
{
  longjmp (toplevel, 1);
}

RETSIGTYPE
lostpeer (int sig ARG_UNUSED)
{
  if (connected)
    {
      if (cout != NULL)
	{
	  shutdown (fileno (cout), 1+1);
	  fclose (cout);
	  cout = NULL;
	}
      if (data >= 0)
	{
	  shutdown (data, 1+1);
	  close (data);
	  data = -1;
	}
      connected = 0;
    }
  pswitch (1);
  if (connected)
    {
      if (cout != NULL)
	{
	  shutdown (fileno(cout), 1+1);
	  fclose (cout);
	  cout = NULL;
	}
      connected = 0;
    }
  proxflag = 0;
  pswitch (0);
}

/*
char *
tail(filename)
	char *filename;
{
	char *s;

	while (*filename) {
		s = strrchr(filename, '/');
		if (s == NULL)
			break;
		if (s[1])
			return (s + 1);
		*s = '\0';
	}
	return (filename);
}
*/

/*
 * Command parser.
 */
void
cmdscanner (int top)
{
  struct cmd *c;
  int l;

  if (!top)
    putchar ('\n');
  for (;;)
    {

#if HAVE_LIBREADLINE
      if (line)
	{
	  free (line);
	  line = 0;
	}
      line = readline (prompt);
      if (!line)
	quit (0, 0);
      l = strlen (line);
      if (l >= MAXLINE)
        {
          printf("Line too long.\n");
          break;
        }
      if (line && *line)
	add_history (line);
      if (l == 0)
	break;
#else
      if (prompt)
	{
	  printf ("%s",prompt);
	  fflush(stdout);
	}

      if (fgets (line, sizeof line, stdin) == NULL)
	quit(0, 0);
      l = strlen (line);
      if (l == 0)
	break;
      if (line[--l] == '\n')
	{
	  if (l == 0)
	    break;
	  line[l] = '\0';
	}
      else if (l == sizeof (line) - 2)
	{
	  printf ("sorry, input line too long\n");
	  while ((l = getchar ()) != '\n' && l != EOF)
				/* void */;
	  break;
	} /* else it was a line without a newline */
#endif
      makeargv ();
      if (margc == 0)
	continue;

      c = getcmd (margv[0]);
      if (c == (struct cmd *) -1)
	{
	  printf ("?Ambiguous command\n");
	  continue;
	}
      if (c == 0)
	{
	  printf ("?Invalid command\n");
	  continue;
	}
      if (c->c_conn && !connected)
	{
	  printf ("Not connected.\n");
	  continue;
	}
      (*c->c_handler) (margc, margv);
      if (bell && c->c_bell)
	putchar('\007');
      if (c->c_handler != help)
	break;
    }
  signal (SIGINT, intr);
  signal (SIGPIPE, lostpeer);
}

/*
 * Slice a string up into argc/argv.
 */

int slrflag;

void
makeargv ()
{
  char **argp;

  margc = 0;
  argp = margv;
  stringbase = line;		/* scan from first of buffer */
  argbase = argbuf;		/* store from first of buffer */
  slrflag = 0;
  while ((*argp++ = slurpstring ()))
    margc++;
}

/*
 * Parse string into argbuf;
 * implemented with FSM to
 * handle quoting and strings
 */
char *
slurpstring ()
{
  int got_one = 0;
  char *sb = stringbase;
  char *ap = argbase;
  char *tmp = argbase;		/* will return this if token found */

  if (*sb == '!' || *sb == '$') {	/* recognize ! as a token for shell */
    switch (slrflag)	/* and $ as token for macro invoke */
      {
      case 0:
	slrflag++;
	stringbase++;
	return ((*sb == '!') ? "!" : "$");
	/* NOTREACHED */

      case 1:
	slrflag++;
	altarg = stringbase;
	break;
      
      default:
	break;
      }
  }

 S0:
  switch (*sb)
    {
    case '\0':
      goto OUT;

    case ' ':
    case '\t':
      sb++; goto S0;

    default:
      switch (slrflag)
	{
	case 0:
	  slrflag++;
	  break;

	case 1:
	  slrflag++;
	  altarg = sb;
	  break;

	default:
	  break;
	}
      goto S1;
    }

 S1:
  switch (*sb)
    {
    case ' ':
    case '\t':
    case '\0':
      goto OUT;	/* end of token */

    case '\\':
      sb++; goto S2;	/* slurp next character */

    case '"':
      sb++; goto S3;	/* slurp quoted string */

    default:
      *ap++ = *sb++;	/* add character to token */
      got_one = 1;
      goto S1;
    }

 S2:
  switch (*sb)
    {
    case '\0':
      goto OUT;
      
    default:
      *ap++ = *sb++;
      got_one = 1;
      goto S1;
    }

 S3:
  switch (*sb)
    {
    case '\0':
      goto OUT;
      
    case '"':
      sb++; goto S1;
      
    default:
      *ap++ = *sb++;
      got_one = 1;
      goto S3;
    }

 OUT:
  if (got_one)
    *ap++ = '\0';
  argbase = ap;			/* update storage pointer */
  stringbase = sb;		/* update scan pointer */
  if (got_one)
    return (tmp);
  switch (slrflag)
    {
    case 0:
      slrflag++;
      break;

    case 1:
      slrflag++;
      altarg = (char *) 0;
      break;

    default:
      break;
    }
  return ((char *) 0);
}

