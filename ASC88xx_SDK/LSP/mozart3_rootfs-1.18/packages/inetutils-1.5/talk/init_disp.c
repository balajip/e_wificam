/*
 * Copyright (c) 1983, 1993
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

#ifndef lint
static char sccsid[] = "@(#)init_disp.c	8.2 (Berkeley) 2/16/94";
#endif /* not lint */

/*
 * Initialization code for the display package,
 * as well as the signal handling routines.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#else
#include <sys/ioctl.h>
#ifdef HAVE_SYS_IOCTL_COMPAT_H
#include <sys/ioctl_compat.h>
#endif
#endif

#include <signal.h>
#include <error.h>
#include "talk.h"

RETSIGTYPE
sig_sent(int sig ARG_UNUSED)
{

	message("Connection closing. Exiting");
	quit();
}

/*
 * Set up curses, catch the appropriate signals,
 * and build the various windows.
 */
int
init_display()
{
#ifdef HAVE_SIGACTION
	struct sigaction siga;
#else
#ifdef HAVE_SIGVEC
	struct sigvec sigv;
#endif
#endif

	if (initscr() == NULL)
		error (1, 0, 
                       "Terminal type unset or lacking necessary features.");

#ifdef HAVE_SIGACTION
	sigaction (SIGTSTP, (struct sigaction *)0, &siga);
	sigaddset(&siga.sa_mask, SIGALRM);
	sigaction (SIGTSTP, &siga, (struct sigaction *)0);
#else /* !HAVE_SIGACTION */
#ifdef HAVE_SIGVEC
	sigvec (SIGTSTP, (struct sigvec *)0, &sigv);
	sigv.sv_mask |= sigmask (SIGALRM);
	sigvec (SIGTSTP, &sigv, (struct sigvec *)0);
#endif /* HAVE_SIGVEC */
#endif /* HAVE_SIGACTION */

	curses_initialized = 1;
	clear();
	refresh();
	noecho();
	crmode();

	signal(SIGINT, sig_sent);
	signal(SIGPIPE, sig_sent);

	/* curses takes care of ^Z */
	my_win.x_nlines = LINES / 2;
	my_win.x_ncols = COLS;
	my_win.x_win = newwin(my_win.x_nlines, my_win.x_ncols, 0, 0);
	scrollok(my_win.x_win, FALSE);
	wclear(my_win.x_win);

	his_win.x_nlines = LINES / 2 - 1;
	his_win.x_ncols = COLS;
	his_win.x_win = newwin(his_win.x_nlines, his_win.x_ncols,
	    my_win.x_nlines+1, 0);
	scrollok(his_win.x_win, FALSE);
	wclear(his_win.x_win);

	line_win = newwin(1, COLS, my_win.x_nlines, 0);
	box(line_win, '-', '-');
	wrefresh(line_win);
	/* let them know we are working on it */
	current_state = "No connection yet";
}

/*
 * Trade edit characters with the other talk. By agreement
 * the first three characters each talk transmits after
 * connection are the three edit characters.
 */
int
set_edit_chars()
{
	int cc;
	char buf[3];

#ifdef HAVE_TCGETATTR
  	struct termios tty;
	cc_t disable = (cc_t)-1, erase, werase, kill;

#if !defined (_POSIX_VDISABLE) && defined (HAVE_FPATHCONF) && defined (_PC_VDISABLE)
	disable = fpathconf (0, _PC_VDISABLE);
#endif

	erase = werase = kill = disable;

	if (tcgetattr (0, &tty) >= 0) {
		erase = tty.c_cc[VERASE];
#ifdef VWERASE
		werase = tty.c_cc[VWERASE];
#endif
		kill = tty.c_cc[VKILL];
	}

	if (erase == disable)
		erase = '\177';	/* rubout */
	if (werase == disable)
		werase = '\027'; /* ^W */
	if (kill == disable)
		kill = '\025';	/* ^U */

	my_win.cerase = erase;
	my_win.werase = werase;
	my_win.kill = kill;
#else /* !HAVE_TCGETATTR */
	struct sgttyb tty;
	struct ltchars ltc;

	ioctl(0, TIOCGETP, &tty);
	ioctl(0, TIOCGLTC, (struct sgttyb *)&ltc);
	my_win.cerase = tty.sg_erase;
	my_win.kill = tty.sg_kill;
	if (ltc.t_werasc == (char) -1)
		my_win.werase = '\027';	 /* control W */
	else
		my_win.werase = ltc.t_werasc;
#endif /* HAVE_TCGETATTR */

	buf[0] = my_win.cerase;
	buf[1] = my_win.kill;
	buf[2] = my_win.werase;
	cc = write(sockt, buf, sizeof(buf));
	if (cc != sizeof(buf) )
		p_error("Lost the connection");
	cc = read(sockt, buf, sizeof(buf));
	if (cc != sizeof(buf) )
		p_error("Lost the connection");
	his_win.cerase = buf[0];
	his_win.kill = buf[1];
	his_win.werase = buf[2];
}

/*
 * All done talking...hang up the phone and reset terminal thingy's
 */
int
quit()
{

	if (curses_initialized) {
		wmove(his_win.x_win, his_win.x_nlines-1, 0);
		wclrtoeol(his_win.x_win);
		wrefresh(his_win.x_win);
		endwin();
	}
	if (invitation_waiting)
		send_delete();
	exit(0);
}
