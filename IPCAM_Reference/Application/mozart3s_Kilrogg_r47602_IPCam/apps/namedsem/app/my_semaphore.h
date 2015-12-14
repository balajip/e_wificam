/*
 *******************************************************************************
 * $Header: $
 *
 *  Copyright (c) 2007-2010 VN Inc. All rights reserved.
 *
 *  +-----------------------------------------------------------------+
 *  | THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY ONLY BE USED |
 *  | AND COPIED IN ACCORDANCE WITH THE TERMS AND CONDITIONS OF SUCH  |
 *  | A LICENSE AND WITH THE INCLUSION OF THE THIS COPY RIGHT NOTICE. |
 *  | THIS SOFTWARE OR ANY OTHER COPIES OF THIS SOFTWARE MAY NOT BE   |
 *  | PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY OTHER PERSON. THE   |
 *  | OWNERSHIP AND TITLE OF THIS SOFTWARE IS NOT TRANSFERRED.        |
 *  |                                                                 |
 *  | THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT   |
 *  | ANY PRIOR NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY |
 *  | VN INC.                                                     |
 *  +-----------------------------------------------------------------+
 *
 * $History: $
 *
 *******************************************************************************
 */

#ifndef _MY_SEMAPHORE_H_
#define _MY_SEMAPHORE_H_

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>	/* for S_xxx file mode constants */
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <fcntl.h>		/* for nonblocking */
#include <unistd.h>
#include <dirent.h>
#include <pthread.h> 
#include <stdarg.h>		/* for variable arg lists */
#define	MAX_TRIES	10	/* for waiting for initialization */
#define va_mode_t mode_t 
#ifdef __USE_MMAP__ 
typedef struct {
  pthread_mutex_t	sem_mutex;	/* lock to test and set semaphore value */
  pthread_cond_t	sem_cond;	/* for transition from 0 to nonzero */
  unsigned int		sem_count;	/* the actual semaphore value */
  int				sem_magic;	/* magic number if open */
} mysem_t;
#define	SEM_MAGIC	0x67458923
#elif __USE_FIFOS__
typedef struct {
  int	sem_fd[2];	/* two fds: [0] for reading, [1] for writing */
  int	sem_magic;	/* magic number if open */
} mysem_t;
#define	SEM_MAGIC	0x89674523
#elif __USE_SVSEM__
typedef struct {
  int	sem_semid;		/* the System V semaphore ID */
  int	sem_magic;		/* magic number if open */
} mysem_t;

union semun {				/* define union for semctl() */
  int              val;
  struct semid_ds *buf;
  unsigned short  *array;
};
#define IPC_R 000400 /* read permission */
#define IPC_W 000200 /* write/alter permission */
#define SEM_A IPC_W /* alter permission */
#define SEM_R IPC_R /* read permission */
#define SVSEM_MODE (SEM_R | SEM_A | SEM_R>>3 | SEM_R>>6)
#ifndef	SEMVMX
#define	SEMVMX	32767		/* historical System V max value for sem */
#endif
#define	SEM_MAGIC	0x45678923

#endif


#define	SEM_FAILED	((mysem_t *)(-1))	/* avoid compiler warnings */

int		 mysem_close(mysem_t *);
int		 mysem_getvalue(mysem_t *, int *);
mysem_t		*mysem_open(const char *, int, ... );
int		 mysem_post(mysem_t *);
int		 mysem_trywait(mysem_t *);
int		 mysem_unlink(const char *);
int		 mysem_wait(mysem_t *);

#endif