#ident "$Id: automount.c,v 1.6 2000/08/10 21:51:50 hpa Exp $"
/* ----------------------------------------------------------------------- *
 *
 *  automount.c - Linux automounter daemon
 *   
 *   Copyright 1997-2000 Transmeta Corporation - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version.
 *   
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 * ----------------------------------------------------------------------- */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <paths.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <linux/auto_fs.h>

#include "automount.h"

const char *program;		        /* Initialized with argv[0] */
const char *version = VERSION_STRING;	/* Program version */

static pid_t my_pgrp;		        /* The "magic" process group */
static pid_t my_pid;		        /* The pid of this process */
static char *pid_file = NULL;	        /* File in which to keep pid */

int kproto_version;		        /* Kernel protocol version used */

static int mounted = 0;
static int submount = 0;

struct pending_mount {
  pid_t pid;		/* Which process is mounting for us */
  unsigned long wait_queue_token;	/* Associated kernel wait token */
  volatile struct pending_mount *next;
};

static struct autofs_point {
  char *path;		/* Mount point name */
  int pipefd;		/* File descriptor for pipe */
  int ioctlfd;		/* File descriptor for ioctls */
  dev_t dev;		/* "Device" number assigned by kernel */
  unsigned exp_timeout;	/* Timeout for expiring mounts */
  unsigned exp_runfreq;	/* Frequency for polling for timeouts */
  volatile pid_t exp_process; /* Process that is currently expiring */
  volatile struct pending_mount *mounts; /* Pending mount queue */
  struct lookup_mod *lookup; /* Lookup module */
} ap;

volatile struct pending_mount *junk_mounts = NULL;

#define DEFAULT_TIMEOUT (5*60)	/* 5 minutes */
#define CHECK_RATIO     4	/* exp_runfreq = exp_timeout/CHECK_RATIO */

static void cleanup_exit(int exit_code);


static int umount_ent(char *root, char *name)
{
  char path_buf[PATH_MAX];
  struct stat st;
  int rv = 0;
  
  sprintf(path_buf, "%s/%s", root, name);
  if ( !lstat(path_buf,&st) ) {
    if ( S_ISDIR(st.st_mode) ) {
      if ( st.st_dev != ap.dev ) {
	rv = spawnl(LOG_DEBUG, PATH_UMOUNT,
		    PATH_UMOUNT, path_buf, NULL);
	if ( !rv )
	  rmdir(path_buf);
      } else
	rmdir(path_buf);
    } else 
      unlink(path_buf);
  }
  return rv;
}	

static int umount_all(int force)
{
  int left;
  struct dirent *de;
  DIR *dp;
  
  chdir("/");
  
  if ( ap.exp_process ) {
    kill(ap.exp_process, SIGTERM);
    usleep(100000);
  }
  
  dp = opendir(ap.path);
  if ( !dp ) {
    syslog(LOG_ERR, "umount_all: opendir: %m");
    return -1;
  }
  
  left = 0;
  
  while ( (de = readdir(dp)) != NULL ) {
    if ( strcmp(de->d_name,".") && strcmp(de->d_name,"..") ) {
      if ( umount_ent(ap.path,de->d_name) ) {
	if ( force )
	  syslog(LOG_WARNING,
		 "could not unmount %s/%s",
		 ap.path,de->d_name);
	left++;
      }
    }
  }
  closedir(dp);
  
  return left;
}	

static int do_umount_autofs(void)
{
  int rv;
  
  if (ap.ioctlfd >= 0) {
    ioctl(ap.ioctlfd, AUTOFS_IOC_CATATONIC, 0);
    close(ap.ioctlfd);
  }
  if (ap.pipefd >= 0)
    close(ap.pipefd);
  rv = spawnl(LOG_ERR, PATH_UMOUNT, PATH_UMOUNT, ap.path, NULL);
  if (rv == 0 && submount)
    rmdir(ap.path);

  free(ap.path);
  mounted = 0;
  return rv;
}

static int umount_autofs(int force)
{
  if ( !mounted )
    return -1;
  if ( umount_all(force) && !force )
    return -1;
  return do_umount_autofs();
}

static void maybe_umount_autofs_and_exit(void)
{
  char path_buf[PATH_MAX];
  struct stat st;
  struct dirent *de;
  DIR *dp;
  int left;

  /* If we are expiring, wait til next round to exit.  */
  if ( ap.exp_process )
    return;

  chdir("/");
  
  dp = opendir(ap.path);
  if ( !dp ) {
    syslog(LOG_ERR, "maybe_umount_autofs: opendir: %m");
    return;
  }
  
  left = 0;
  while ( (de = readdir(dp)) != NULL ) {
    if ( strcmp(de->d_name,".") && strcmp(de->d_name,"..") ) {
      sprintf(path_buf, "%s/%s", ap.path, de->d_name);
      if ( !lstat(path_buf,&st) ) {
        if ( S_ISDIR(st.st_mode) ) {
          if ( st.st_dev != ap.dev )
	    left++;
	}
      }
    }
  }
  closedir(dp);

  if (!left) {
    if (do_umount_autofs() == 0)
      cleanup_exit(0);
  }
}

static int mount_autofs(char *path)
{
  int pipefd[2];
  char options[128];
  char our_name[128];
  struct stat st;
  
  if ( mounted ) {
    /* This can't happen */
    syslog(LOG_ERR, "mount_autofs: already mounted");
    return -1;
  }
  
  if ( path[0] != '/' ) {
    errno = EINVAL;	/* Must be an absolute pathname */
    return -1;
  }
  
  ap.path = strdup(path);
  if ( !ap.path ) {
    errno = ENOMEM;
    return -1;
  }
  ap.pipefd = ap.ioctlfd = -1;
  
  /* In case the directory doesn't exist, try to mkdir it */
  if ( mkdir(path, 0555) < 0 && errno != EEXIST )
    return -1;
  
  if ( pipe(pipefd) < 0 )
    return -1;
  
  sprintf(options, "fd=%d,pgrp=%u,minproto=2,maxproto=%d", pipefd[1],
	  (unsigned)my_pgrp, AUTOFS_PROTO_VERSION);
  sprintf(our_name, "automount(pid%u)", (unsigned)my_pid);
  
  if ( spawnl(LOG_CRIT, PATH_MOUNT, PATH_MOUNT, "-t", "autofs", "-o",
	      options, our_name, path, NULL) ) {
    close(pipefd[0]);
    close(pipefd[1]);
    return -1;
  }
  
  close(pipefd[1]);	/* Close kernel pipe end */
  ap.pipefd = pipefd[0];
  
  chdir(path);
  ap.ioctlfd = open(".", O_RDONLY); /* Root directory for ioctl()'s */
  chdir("/");
  if ( ap.ioctlfd < 0 ) {
    umount_autofs(1);
    return -1;
  }
  
  stat(path,&st);
  ap.dev = st.st_dev;	/* Device number for mount point checks */
  
  ap.mounts = NULL;	/* No pending mounts */
  mounted = 1;
  
  return 0;
}

static void run_expire(void)
{
  struct autofs_packet_expire pkt;
  sigset_t s, olds;
  pid_t f;
  
  if ( ap.exp_process )	/* Another process is already expiring */
    return;		/* Try again next interval */
  
  if ( ioctl(ap.ioctlfd, AUTOFS_IOC_EXPIRE, &pkt) ) {
    /* Don't fork if there is nothing do expire, but if we are a submount,
       see if maybe we shouldn't exit. */
    if (submount)
      maybe_umount_autofs_and_exit();
    return;	
  }
  
  /* Temporarily block SIGCHLD and SIGALRM between forking and setting
     ap.exp_process */
  
  sigemptyset(&s);
  sigaddset(&s, SIGCHLD);
  sigaddset(&s, SIGALRM);
  sigprocmask(SIG_BLOCK, &s, &olds);
  
  f = fork();
  if ( f < 0 )
    return;
  if ( f > 0 ) {
    ap.exp_process = f;
    sigprocmask(SIG_SETMASK, &olds, NULL);
    return;
  }
  
  /* This is the actual expire run, run as a subprocess */
  
  reset_signals();
  close(ap.pipefd);
  
  syslog(LOG_DEBUG, "running expiration on path %s", ap.path);

  do {
    if ( pkt.hdr.type == autofs_ptype_expire ) {
      if ( !umount_ent(ap.path,pkt.name) )
	syslog(LOG_NOTICE, "expired %s/%s", ap.path,
	       pkt.name);
    }
  } while ( ioctl(ap.ioctlfd, AUTOFS_IOC_EXPIRE, &pkt) == 0 );
  _exit(0);
}

static sig_atomic_t shutdown = 0;
static sig_atomic_t prune = 0;
static sig_atomic_t reinit = 0;
static sig_atomic_t expire_now = 0;

static void sig_shutdown(int sig)
{
  shutdown = sig;
}

static void sig_prune(int sig)
{
  prune = 1;
}

static void sig_expire(int sig)
{
  expire_now = 1;
}

static void sig_reinit(int sig)
{
  prune = 1;
  reinit = 1;
}

static int get_pkt(int fd, struct autofs_packet_missing *pkt)
{
  char *buf = (char *) pkt;
  int bytes = sizeof(*pkt);

  do {
    int i = read(fd, buf, bytes);
    if (!i)
      break;
    if (i < 0) {
      if (errno == EINTR) {
	if ( prune ) {
	  umount_all(0);
	  prune = 0;
	}
	if ( expire_now ) {
	  run_expire();
	  expire_now = 0;
	  alarm(ap.exp_runfreq);
	}
	if ( !shutdown && !reinit )
	  continue;
      }
      break;
    }
    buf += i;
    bytes -= i;
  } while (bytes);
  return bytes;
}

static int send_ready(unsigned int wait_queue_token)
{
  if ( ioctl(ap.ioctlfd, AUTOFS_IOC_READY, wait_queue_token) < 0 ) {
    syslog(LOG_ERR, "AUTOFS_IOC_READY: %m");
    return 1;
  } else
    return 0;
}

static int send_fail(unsigned int wait_queue_token)
{
  if ( ioctl(ap.ioctlfd, AUTOFS_IOC_FAIL, wait_queue_token) < 0 ) {
    syslog(LOG_ERR, "AUTOFS_IOC_READY: %m");
    return 1;
  } else
    return 0;
}

static int handle_packet(void)
{
  struct autofs_packet_missing pkt;
  struct stat st;
  sigset_t allsignals, oldsig, chldsig;
  pid_t f;

  if (get_pkt(ap.pipefd, &pkt))
    return -1;
  
  sigemptyset(&chldsig);
  sigaddset(&chldsig, SIGCHLD);
  sigfillset(&allsignals);

  chdir(ap.path);
  if ( lstat(pkt.name,&st) || (S_ISDIR(st.st_mode) && st.st_dev == ap.dev) ) {
    /* Need to mount or symlink */
    struct pending_mount *mt;
    
    /* Block SIGCHLD while mucking with linked lists */
    sigprocmask(SIG_BLOCK, &chldsig, NULL);
    if ( (mt = (struct pending_mount *) junk_mounts) ) {
      junk_mounts = junk_mounts->next;
    } else if ( !(mt = malloc(sizeof(struct pending_mount))) ) {
      syslog(LOG_ERR, "handle_packet: malloc: %m");
      send_fail(pkt.wait_queue_token);
      return 1;
    }
    sigprocmask(SIG_UNBLOCK, &chldsig, NULL);

    syslog(LOG_INFO, "attempting to mount entry %s/%s",
	   ap.path, pkt.name);

    sigprocmask(SIG_BLOCK, &allsignals, &oldsig);

    f = fork();
    if ( f == -1 ) {
      sigprocmask(SIG_SETMASK, &oldsig, NULL);
      syslog(LOG_ERR, "handle_packet: fork: %m");
      send_fail(pkt.wait_queue_token);
      return 1;
    } else if ( !f ) {
      int i;
      
      reset_signals();		/* Set up a standard signal environment */
      close(ap.pipefd);
      close(ap.ioctlfd);
      
      i = ap.lookup->lookup_mount(ap.path,pkt.name,pkt.len,ap.lookup->context);
      _exit(i ? 1 : 0);
    } else {
      /* Important: set up data structures while signals still blocked */
      mt->pid = f;
      mt->wait_queue_token = pkt.wait_queue_token;
      mt->next = ap.mounts;
      ap.mounts = mt;

      sigprocmask(SIG_SETMASK, &oldsig, NULL);
    }
  } else {
    /* Already there (can happen if a process connects to a
       directory while we're still working on it) */
    chdir("/");
    send_ready(pkt.wait_queue_token);
  }
  return 0;
}

static void sig_child(int sig)
{
  pid_t pid;
  int status, old_errno;
  volatile struct pending_mount *mt, * volatile *mtp;
  
  /* We have to preserve errno, since we could have gotten
     SIGCHLD between a system call and reading errno. */
  old_errno = errno;

  while ( (pid = waitpid(-1, &status, WNOHANG)) > 0 ) {
    if ( pid == ap.exp_process ) {
      ap.exp_process = 0;
      if (submount)
	maybe_umount_autofs_and_exit();
    } else {
      for ( mtp = &ap.mounts ; (mt = *mtp) ; mtp = &mt->next ) {
	if ( mt->pid == pid ) {
	  if ( !WIFEXITED(status) && !WIFSIGNALED(status) )
	    break;
	  else if ( WIFSIGNALED(status) ||
		    WEXITSTATUS(status) != 0 )
	    send_fail(mt->wait_queue_token);
	  else
	    send_ready(mt->wait_queue_token);
	  *mtp = mt->next; /* Remove from linked list */
	  /* We can't use free() from a signal handler, so put this on
	     a "junk list" which we can reclaim later */
	  mt->next = junk_mounts;
	  junk_mounts = mt;
	  break;
	}
      }
    }
  }

  errno = old_errno;
}

static void become_daemon(void)
{
  FILE *pidfp;
  pid_t pid;
  int nullfd;
  
  /* Don't BUSY any directories unneccessarily */
  chdir("/");
  
  /* Detach from foreground process */
  if ( !submount ) {
    pid = fork();
    if ( pid > 0 )
      exit(0);
    else if ( pid < 0 ) {
      fprintf(stderr, "%s: Could not detach process\n", program);
      exit(1);
    }
  }
  
  /* Open syslog */ 
  openlog("automount", LOG_PID, LOG_DAEMON);

  /* Initialize global data */
  my_pid = getpid();

  /* Make our own process group for "magic" reason: processes that share
     our pgrp see the raw filesystem behine the magic.  So if we are a 
     submount, don't change -- otherwise we won't be able to actually
     perform the mount.  */
  if ( !submount && setpgrp() ) {
    syslog(LOG_CRIT, "setpgrp: %m");
    exit(1);
  }
  my_pgrp = getpgrp();

  /* Redirect all our file descriptors to /dev/null */
  if ( (nullfd = open("/dev/null", O_RDWR)) < 0 ) {
    syslog(LOG_CRIT, "cannot open /dev/null: %m");
    exit(1);
  }
  
  if ( dup2(nullfd, STDIN_FILENO) < 0 ||
       dup2(nullfd, STDOUT_FILENO) < 0 ||
       dup2(nullfd, STDERR_FILENO) < 0 ) {
    syslog(LOG_CRIT, "redirecting file descriptors failed: %m");
    exit(1);
  }
  close(nullfd);

  /* Write pid file if requested */
  if ( pid_file ) {
    if ( (pidfp = fopen(pid_file, "wt")) ) {
      fprintf(pidfp, "%lu\n", (unsigned long) my_pid);
      fclose(pidfp);
    } else {
      syslog(LOG_WARNING, "failed to write pid file %s: %m", pid_file);
      pid_file = NULL;
    }
  }
}

/*
 * cleanup_exit() is valid to call once we have daemonized
 */

static void cleanup_exit(int exit_code)
{
  if ( ap.lookup )
    close_lookup(ap.lookup);

  if ( pid_file )
    unlink(pid_file);

  closelog();

  exit(exit_code);
}

static unsigned long getnumopt(char *str, char option)
{
  unsigned long val;
  char *end;

  val = strtoul(str, &end, 0);
  if ( ! *str || *end ) {
    fprintf(stderr, "%s: option -%c requires a numeric argument, got %s\n",
	    program, option, str);
    exit(1);
  }
  return val;
}

static void usage(void)
{
  fprintf(stderr, "Usage: %s [options] path map_type [args...]\n", program);
}

int main(int argc, char *argv[])
{
  char *path, *map, *mapfmt;
  const char **mapargv;
  struct sigaction sa;
  int mapargc, opt;
  static const struct option long_options[] = {
    {"help",     0, 0, 'h'},
    {"pid-file", 1, 0, 'p'},
    {"timeout",  1, 0, 't'},
    {"version",  0, 0, 'v'},
    {"submount", 0, &submount, 1},
    {0,0,0,0}
  };
  
  program = argv[0];
  
  memset(&ap, 0, sizeof ap);	/* Initialize ap so we can test for null */
  ap.exp_timeout = DEFAULT_TIMEOUT;
  
  opterr = 0;
  while ( (opt = getopt_long(argc, argv, "+hp:t:v", long_options,
			     NULL)) != EOF ) {
    switch( opt ) {
    case 'h':
      usage();
      exit(0);
    case 'p':
      pid_file = optarg;
      break;
    case 't':
      ap.exp_timeout = getnumopt(optarg, opt);
      break;
    case 'v':
      printf("Linux automount version %s\n", version);
      exit(0);
    case '?':
    case ':':
      printf("%s: Ambiguous or unknown options\n", program);
      exit(1);
    }
  }
  
  if ( geteuid() != 0 ) {
    fprintf(stderr, "%s: This program must be run by root.\n", program);
    exit(1);
  }
  
  /* Remove the options */
  argv += optind;
  argc -= optind;
  
  if ( argc < 2 ) {
    usage();
    exit(1);
  }
  
  become_daemon();
  
  path    = argv[0];
  map     = argv[1];
  mapargv = (const char **) &argv[2];
  mapargc = argc-2;         
  
  syslog(LOG_INFO, "starting automounter version %s, path = %s, "
	 "maptype = %s, mapname = %s", version, path, map,
	 (mapargc < 1) ? "none" : mapargv[0]);
  
  if ( mapargc ) {
    int i;
    syslog(LOG_DEBUG, "Map argc = %d", mapargc);
    for ( i = 0 ; i < mapargc ; i++ )
      syslog(LOG_DEBUG, "Map argv[%d] = %s", i, mapargv[i]);
  }
  
  if ( (mapfmt = strchr(map,',')) )
    *(mapfmt++) = '\0';
  
  if ( !(ap.lookup = open_lookup(map, "", mapfmt, mapargc, mapargv)) )
    cleanup_exit(1);
  
  if (mount_autofs(path) < 0) {
    syslog(LOG_CRIT, "%s: mount failed!", path);
    cleanup_exit(1);
  }	

  /* The following signals cause a shutdown event to occur */
  sa.sa_handler = sig_shutdown;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGCHLD);
  sigaddset(&sa.sa_mask, SIGALRM);
  sa.sa_flags = 0;
  sigaction(SIGIO, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  /* USR2 is special, but is handled by the same handler */
  sigaction(SIGUSR2, &sa, NULL);

  /* The following signals cause a shutdown event to occur, but if we get
     more than one, permit the signal to proceed so we don't loop.  This
     is basically the complete list of "this shouldn't happen" signals. */
  sa.sa_flags = SA_ONESHOT;
  sigaction(SIGXCPU, &sa, NULL);
  sigaction(SIGXFSZ, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);
#ifdef SIGPWR
  sigaction(SIGPWR, &sa, NULL);
#endif
#ifndef DEBUG
  /* When debugging, these signals should be in the default state; when
     in production, we want to at least attempt to catch them and shut down. */
  sigaction(SIGILL, &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
  sigaction(SIGABRT, &sa, NULL);
  sigaction(SIGTRAP, &sa, NULL);
  sigaction(SIGFPE, &sa, NULL);
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGBUS, &sa, NULL);
  sigaction(SIGPROF, &sa, NULL);
  sigaction(SIGPIPE, &sa, NULL);
#ifdef SIGSYS
  sigaction(SIGSYS, &sa, NULL);
#endif
#ifdef SIGSTKFLT
  sigaction(SIGSTKFLT, &sa, NULL);
#endif
#ifdef SIGLOST
  sigaction(SIGLOST, &sa, NULL);
#endif
#ifdef SIGEMT
  sigaction(SIGEMT, &sa, NULL);
#endif
#endif /* DEBUG */
  
  /* The SIGCHLD handler is the bottom handler of the mount request */
  sa.sa_handler = sig_child;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGALRM);
  sa.sa_flags = SA_NOCLDSTOP;	/* Don't need info about stopped children */
  sigaction(SIGCHLD, &sa, NULL);
  
  /* The SIGALRM handler controls expiration of entries. */
  sa.sa_handler = sig_expire;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGALRM, &sa, NULL);

  /* SIGUSR1 causes a prune event */
  sa.sa_handler = sig_prune;
  sigaction(SIGUSR1, &sa, NULL);

  /* SIGHUP causes complete reinitialization */
  sa.sa_handler = sig_reinit;
  sigaction(SIGHUP, &sa, NULL);
  
  /* The following signals shouldn't occur, and are ignored */
  sa.sa_handler = SIG_IGN;
  sigaction(SIGVTALRM, &sa, NULL);
  sigaction(SIGURG, &sa, NULL);
  sigaction(SIGWINCH, &sa, NULL);
#ifdef SIGUNUSED
  sigaction(SIGUNUSED, &sa, NULL);
#endif
  
  /* If this ioctl() doesn't work, it is kernel version 2 */
  if ( ioctl(ap.ioctlfd, AUTOFS_IOC_PROTOVER, &kproto_version) ) {
    syslog(LOG_DEBUG, "kproto: %m");
    kproto_version = 2;
  }
  
  syslog(LOG_INFO, "using kernel protocol version %d", kproto_version);
  
  if ( kproto_version < 3 ) {
    ap.exp_timeout = ap.exp_runfreq = 0;
    syslog(LOG_INFO, "kernel does not support timeouts");
  } else {
    unsigned long timeout;
    
    ap.exp_runfreq = (ap.exp_timeout+CHECK_RATIO-1) / CHECK_RATIO;
    
    timeout = ap.exp_timeout;
    ioctl(ap.ioctlfd, AUTOFS_IOC_SETTIMEOUT, &timeout);
    
    /* We often start several automounters at the same time.  Add some
       randomness so we don't all expire at the same time. */
    if ( ap.exp_timeout )
      alarm(ap.exp_timeout + my_pid % ap.exp_runfreq);
  }

  /* Initialization successful.  If we're a submount, send outself SIGSTOP to let
     our parent know that we have grown up and don't need supervision anymore. */
  if ( submount )
    kill(my_pid, SIGSTOP);
  
  while ( !shutdown ) {
    if (handle_packet() && errno != EINTR)
      break;
    if (shutdown == SIGUSR2 && umount_autofs(0))
      shutdown = 0;
    if (reinit) {
      reinit = 0;
      close_lookup(ap.lookup);
      if ( !(ap.lookup = open_lookup(map, "", mapfmt, mapargc, mapargv)) ) {
	syslog(LOG_CRIT, "reinitialization failed, shutting down");
	umount_autofs(1);
	cleanup_exit(1);
      }
    }
  }
  
  syslog(LOG_INFO, "shutting down, path = %s", path);

  umount_autofs(1);
  cleanup_exit(0);
  return 0;
}
