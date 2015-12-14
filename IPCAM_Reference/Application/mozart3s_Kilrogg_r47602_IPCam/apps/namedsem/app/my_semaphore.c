#include "my_semaphore.h"

//implements Posix semaphores using mmap
#ifdef __USE_MMAP__ 
mysem_t	*mysem_open(const char *pathname, int oflag, ... )
{
	int		fd, i, created, save_errno;
	mode_t	mode;
	va_list	ap;
	mysem_t	*sem, seminit;
	struct stat	statbuff;
	unsigned int	value;
	pthread_mutexattr_t	mattr;
	pthread_condattr_t	cattr;

	created = 0;
	sem = MAP_FAILED;				/* [sic] */
again:
	if (oflag & O_CREAT) {
		va_start(ap, oflag);		/* init ap to final named argument */
		mode = va_arg(ap, va_mode_t) & ~S_IXUSR;
		value = va_arg(ap, unsigned int);
		va_end(ap);

			/* 4open and specify O_EXCL and user-execute */
		fd = open(pathname, oflag | O_EXCL | O_RDWR, mode | S_IXUSR);
		if (fd < 0) {
			if (errno == EEXIST && (oflag & O_EXCL) == 0)
				goto exists;		/* already exists, OK */
			else
				return(SEM_FAILED);
		}
		created = 1;
			/* 4first one to create the file initializes it */
			/* 4set the file size */
		bzero(&seminit, sizeof(seminit));
		if (write(fd, &seminit, sizeof(seminit)) != sizeof(seminit))
			goto err;

			/* 4memory map the file */
		sem = mmap(NULL, sizeof(mysem_t), PROT_READ | PROT_WRITE,
				   MAP_SHARED, fd, 0);
		if (sem == MAP_FAILED)
			goto err;

			/* 4initialize mutex, condition variable, and value */
		if ( (i = pthread_mutexattr_init(&mattr)) != 0)
			goto pthreaderr;
		pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
		i = pthread_mutex_init(&sem->sem_mutex, &mattr);
		pthread_mutexattr_destroy(&mattr);	/* be sure to destroy */
		if (i != 0)
			goto pthreaderr;

		if ( (i = pthread_condattr_init(&cattr)) != 0)
			goto pthreaderr;
		pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
		i = pthread_cond_init(&sem->sem_cond, &cattr);
		pthread_condattr_destroy(&cattr);	/* be sure to destroy */
		if (i != 0)
			goto pthreaderr;

		if ( (sem->sem_count = value) > sysconf(_SC_SEM_VALUE_MAX)) {
			errno = EINVAL;
			goto err;
		}
			/* 4initialization complete, turn off user-execute bit */
		if (fchmod(fd, mode) == -1)
			goto err;
		close(fd);
		sem->sem_magic = SEM_MAGIC;
		return(sem);
	}
/* end sem_open1 */
/* include sem_open2 */
exists:
	if ( (fd = open(pathname, O_RDWR)) < 0) {
		if (errno == ENOENT && (oflag & O_CREAT))
			goto again;
		goto err;
	}

	sem = mmap(NULL, sizeof(mysem_t), PROT_READ | PROT_WRITE,
			   MAP_SHARED, fd, 0);
	if (sem == MAP_FAILED)
		goto err;

		/* 4make certain initialization is complete */
	for (i = 0; i < MAX_TRIES; i++) {
		if (stat(pathname, &statbuff) == -1) {
			if (errno == ENOENT && (oflag & O_CREAT)) {
				close(fd);
				goto again;
			}
			goto err;
		}
		if ((statbuff.st_mode & S_IXUSR) == 0) {
			close(fd);
			sem->sem_magic = SEM_MAGIC;
			return(sem);
		}
		sleep(1);
	}
	errno = ETIMEDOUT;
	goto err;

pthreaderr:
	errno = i;
err:
		/* 4don't let munmap() or close() change errno */
	save_errno = errno;
	if (created)
		unlink(pathname);
	if (sem != MAP_FAILED)
		munmap(sem, sizeof(mysem_t));
	close(fd);
	errno = save_errno;
	return(SEM_FAILED);
}

int mysem_close(mysem_t *sem)
{
	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}


	if (munmap(sem, sizeof(mysem_t)) == -1)
		return(-1);

	return(0);
}

int mysem_getvalue(mysem_t *sem, int *pvalue)
{
	int		n;

	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	if ( (n = pthread_mutex_lock(&sem->sem_mutex)) != 0) {
		errno = n;
		return(-1);
	}
	*pvalue = sem->sem_count;
	pthread_mutex_unlock(&sem->sem_mutex);
	return(0);
}

int mysem_post(mysem_t *sem)
{
	int		n;

	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	if ( (n = pthread_mutex_lock(&sem->sem_mutex)) != 0) {
		errno = n;
		return(-1);
	}
	if (sem->sem_count == 0){
	    printf("enter pthread_cond_signal...sem=%p\n",sem);
	    pthread_cond_signal(&sem->sem_cond);
	    printf("leave pthread_cond_signal...\n");
	}
	sem->sem_count++;
	pthread_mutex_unlock(&sem->sem_mutex);
	return(0);
}

int mysem_trywait(mysem_t *sem)
{
	int		n, rc;

	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	if ( (n = pthread_mutex_lock(&sem->sem_mutex)) != 0) {
		errno = n;
		return(-1);
	}
	if (sem->sem_count > 0) {
		sem->sem_count--;
		rc = 0;
	} else {
		rc = -1;
		errno = EAGAIN;
	}
	pthread_mutex_unlock(&sem->sem_mutex);
	return(rc);
}

int mysem_wait(mysem_t *sem)
{
	int		n;

	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	if ( (n = pthread_mutex_lock(&sem->sem_mutex)) != 0) {
		errno = n;
		return(-1);
	}
	printf("enter waitting while...sem=%p\n",sem);
	while (sem->sem_count == 0){
	    printf("sem->sem_count=%d\n",sem->sem_count);
	    pthread_cond_wait(&sem->sem_cond, &sem->sem_mutex);
	    printf("sem->sem_count=%d\n",sem->sem_count);
	}
	printf("break waitting while...\n");
	sem->sem_count--;
	pthread_mutex_unlock(&sem->sem_mutex);
	return(0);
}

int mysem_unlink(const char *pathname)
{
	if (unlink(pathname) == -1)
		return(-1);
	return(0);
}
#elif __USE_FIFOS__ 
//implements Posix semaphores using FIFOs
int mysem_wait(mysem_t *sem)
{
	char	c;

	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	if (read(sem->sem_fd[0], &c, 1) == 1)
		return(0);
	return(-1);
}
int mysem_post(mysem_t *sem)
{
	char	c;

	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	if (write(sem->sem_fd[1], &c, 1) == 1)
		return(0);
	return(-1);
}
int mysem_unlink(const char *pathname)
{
	return(unlink(pathname));
}
mysem_t	*mysem_open(const char *pathname, int oflag, ... )
{
	int		i, flags, save_errno;
	char	c;
	mode_t	mode;
	va_list	ap;
	mysem_t	*sem;
	unsigned int	value;

	if (oflag & O_CREAT) {
		va_start(ap, oflag);		/* init ap to final named argument */
		mode = va_arg(ap, va_mode_t);
		value = va_arg(ap, unsigned int);
		va_end(ap);

		if (mkfifo(pathname, mode) < 0) {
			if (errno == EEXIST && (oflag & O_EXCL) == 0)
				oflag &= ~O_CREAT;	/* already exists, OK */
			else
				return(SEM_FAILED);
		}
	}

/* *INDENT-OFF* */
	if ( (sem = malloc(sizeof(mysem_t))) == NULL)
		return(SEM_FAILED);
	sem->sem_fd[0] = sem->sem_fd[1] = -1;

	if ( (sem->sem_fd[0] = open(pathname, O_RDONLY | O_NONBLOCK)) < 0)
		goto error;
	if ( (sem->sem_fd[1] = open(pathname, O_WRONLY | O_NONBLOCK)) < 0)
		goto error;
/* *INDENT-ON* */

		/* 4turn off nonblocking for sem_fd[0] */
	if ( (flags = fcntl(sem->sem_fd[0], F_GETFL, 0)) < 0)
		goto error;
	flags &= ~O_NONBLOCK;
	if (fcntl(sem->sem_fd[0], F_SETFL, flags) < 0)
		goto error;

	if (oflag & O_CREAT) {		/* initialize semaphore */
		for (i = 0; i < value; i++)
			if (write(sem->sem_fd[1], &c, 1) != 1)
				goto error;
	}

	sem->sem_magic = SEM_MAGIC;
	return(sem);

error:
	save_errno = errno;
	if (oflag & O_CREAT)
		unlink(pathname);		/* if we created FIFO */
	close(sem->sem_fd[0]);		/* ignore error */
	close(sem->sem_fd[1]);		/* ignore error */
	free(sem);
	errno = save_errno;
	return(SEM_FAILED);
}
int mysem_close(mysem_t *sem)
{
	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	sem->sem_magic = 0;		/* in case caller tries to use it later */
	if (close(sem->sem_fd[0]) == -1 || close(sem->sem_fd[1]) == -1) {
		free(sem);
		return(-1);
	}
	free(sem);
	return(0);
}
int mysem_trywait(mysem_t *sem)
{
  return 0;
}
int mysem_getvalue(mysem_t *sem, int *pvalue)
{
  return 0;
}
#elif __USE_SVSEM__
//implements Posix semaphores using System V semaphores
int mysem_wait(mysem_t *sem)
{
	struct sembuf	op;

	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	op.sem_num = 0;// Operate on the first sem
	op.sem_op = -1;// The sem_op is -1 (negative). This means that the process will wait the semval is equal to 1 and then semval will be decremented by 1.   
	op.sem_flg = 0;//This means that the process could sleep because of waiting.
	if (semop(sem->sem_semid, &op, 1) < 0)
		return(-1);
	return(0);
}
int mysem_unlink(const char *pathname)
{
	int		semid;
	key_t	key;

	if ( (key = ftok(pathname, 0)) == (key_t) -1)
		return(-1);
	if (unlink(pathname) == -1)
		return(-1);
	if ( (semid = semget(key, 1, SVSEM_MODE)) == -1)
		return(-1);
	if (semctl(semid, 0, IPC_RMID) == -1)
		return(-1);
	return(0);
}
int mysem_trywait(mysem_t *sem)
{
	struct sembuf	op;

	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	op.sem_num = 0;
	op.sem_op = -1;
	op.sem_flg = IPC_NOWAIT;
	if (semop(sem->sem_semid, &op, 1) < 0)
		return(-1);
	return(0);
}
int mysem_post(mysem_t *sem)
{
	struct sembuf	op;

	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	op.sem_num = 0;// Operate on the first sem
	op.sem_op = 1;// The sem_op is 1 (positive). This means that semval will be incremented by 1. Release the resource controlled by the sem.    
	op.sem_flg = 0;//This means that the process could sleep because of waiting.
	if (semop(sem->sem_semid, &op, 1) < 0)
		return(-1);
	return(0);
}
int mysem_getvalue(mysem_t *sem, int *pvalue)
{
	int		val;

	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}

	if ( (val = semctl(sem->sem_semid, 0, GETVAL)) < 0)
		return(-1);
	*pvalue = val;
	return(0);
}
int mysem_close(mysem_t *sem)
{
	if (sem->sem_magic != SEM_MAGIC) {
		errno = EINVAL;
		return(-1);
	}
	sem->sem_magic = 0;		/* just in case */

	free(sem);
	return(0);
}
mysem_t	*mysem_open(const char *pathname, int oflag, ... )
{
	int		i, fd, semflag, semid, save_errno;
	key_t	key;
	mode_t	mode;
	va_list	ap;
	mysem_t	*sem;
	union semun	arg;
	unsigned int	value;
	struct semid_ds	seminfo;
	struct sembuf	initop;

		/* 4no mode for sem_open() w/out O_CREAT; guess */
	semflag = SVSEM_MODE;
	semid = -1;

	if (oflag & O_CREAT) {
		va_start(ap, oflag);		/* init ap to final named argument */
		mode = va_arg(ap, va_mode_t);
		value = va_arg(ap, unsigned int);
		va_end(ap);

			/* 4convert to key that will identify System V semaphore */
		if ( (fd = open(pathname, oflag, mode)) == -1)
			return(SEM_FAILED);
		close(fd);
		if ( (key = ftok(pathname, 0)) == (key_t) -1)
			return(SEM_FAILED);

		semflag = IPC_CREAT | (mode & 0777);
		if (oflag & O_EXCL)
			semflag |= IPC_EXCL;

			/* 4create the System V semaphore with IPC_EXCL */
		if ( (semid = semget(key, 1, semflag | IPC_EXCL)) >= 0) {
				/* 4success, we're the first so initialize to 0 */
			arg.val = 0;
			if (semctl(semid, 0, SETVAL, arg) == -1)
				goto err;
				/* 4then increment by value to set sem_otime nonzero */
			if (value > SEMVMX) {
				errno = EINVAL;
				goto err;
			}
        	initop.sem_num = 0;
        	initop.sem_op  = value;
        	initop.sem_flg = 0;
			if (semop(semid, &initop, 1) == -1)
				goto err;
			goto finish;

		} else if (errno != EEXIST || (semflag & IPC_EXCL) != 0)
			goto err;
		/* else fall through */
	}
/* end sem_open1 */
/* include sem_open2 */
	/*
	 * (O_CREAT not secified) or
	 * (O_CREAT without O_EXCL and semaphore already exists).
	 * Must open semaphore and make certain it has been initialized.
	 */
	if ( (key = ftok(pathname, 0)) == (key_t) -1)
		goto err;
	if ( (semid = semget(key, 0, semflag)) == -1)
		goto err;

	arg.buf = &seminfo;
	for (i = 0; i < MAX_TRIES; i++) {
		if (semctl(semid, 0, IPC_STAT, arg) == -1)
			goto err;
		if (arg.buf->sem_otime != 0)
			goto finish;
		sleep(1);
	}
	errno = ETIMEDOUT;
err:
	save_errno = errno;		/* don't let semctl() change errno */
	if (semid != -1)
		semctl(semid, 0, IPC_RMID);
	errno = save_errno;
	return(SEM_FAILED);

finish:
/* *INDENT-OFF* */
	if ( (sem = malloc(sizeof(mysem_t))) == NULL)
		goto err;
/* *INDENT-ON* */
	sem->sem_semid = semid;
	sem->sem_magic = SEM_MAGIC;
	return(sem);
}
#endif