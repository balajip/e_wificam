#ifndef	__error_print_h
#define	__error_print_h
#include	<stdarg.h>		/* ANSI C header file */
#include	<syslog.h>		/* for syslog() */
#include 	<stdio.h>
#include	<string.h>
#include	<errno.h>
#include 	<stdlib.h>
#define	MAXLINE		4096	/* max text line length */

void	 err_dump(const char *, ...);
void	 err_msg(const char *, ...);
void	 err_quit(const char *, ...);
void	 err_ret(const char *, ...);
void	 err_sys(const char *, ...);
#endif
