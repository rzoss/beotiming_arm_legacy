/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <unistd.h>


/* Forward prototypes.  */
int     _system     _PARAMS ((const char *));
int     _rename     _PARAMS ((const char *, const char *));
int     isatty		_PARAMS ((int));
clock_t _times		_PARAMS ((struct tms *));
int     _gettimeofday	_PARAMS ((struct timeval *, struct timezone *));
void    _raise 		_PARAMS ((void));
int     _unlink		_PARAMS ((void));
int     _link 		_PARAMS ((void));
int     _stat 		_PARAMS ((const char *, struct stat *));
int     _fstat 		_PARAMS ((int, struct stat *));
caddr_t _sbrk		_PARAMS ((int));
int     _getpid		_PARAMS ((int));
int     _kill		_PARAMS ((int, int));
void    _exit		_PARAMS ((int));
int     _close		_PARAMS ((int));
int     _swiclose	_PARAMS ((int));
int     _open		_PARAMS ((const char *, int, ...));
int     _swiopen	_PARAMS ((const char *, int));
int     _write 		_PARAMS ((int, char *, int));
int     _swiwrite	_PARAMS ((int, char *, int));
int     _lseek		_PARAMS ((int, int, int));
int     _swilseek	_PARAMS ((int, int, int));
int     _read		_PARAMS ((int, char *, int));
int     _swiread	_PARAMS ((int, char *, int));



int _read (int file, char * ptr, int len)
{
	return 0;
}

int _swilseek (int file, int ptr, int dir)
{
  /* This is expected to return the position in the file.  */
  return  -1;
}

int _lseek (int file, int ptr, int dir)
{
  return 0;
}


int _write (int    file, char * ptr, int    len)
{
	return 0;
}


int _open (const char* path, int flags, ...)
{
	return -1;
}


int _close (int file)
{
  return -1;
}

void _exit (int n)
{
  /* FIXME: return code is thrown away.  */
  n = n;
  while(1);
}

int _kill (int n, int m)
{
  n = n; m = m;
  errno = EINVAL;
  return -1;
}

int _getpid (int n)
{
  return 1;
  n = n;
}

/**
 *****************************************************************************
 * \fn    	: caddr_t _sbrk(int incr)
 *****************************************************************************
 *\brief 
 * Newlib calls this stub whenever malloc() runs out of heap space
 * and wants more. 
 *         									
 *****************************************************************************
 * \return      	none
 *****************************************************************************
 *  \author     M. Muehlemann                                                   
 *                                                                  
 *  \date       30.03.2007                                   
  *
 *  \bug        No bugs known
 *****************************************************************************
 */

/**** Locally used variables. ****/
extern char end[];				// end is set in the linkercsript 
								// and is the end of statically
								// allocated data (thus start of heap)

extern char end_of_ram[];

static char *heap_ptr;			// Points to current end of the heap.

caddr_t _sbrk(int incr)
{
	char  *base;		// errno should be set to  ENOMEM on error

	if (!heap_ptr) {	//  Initialize if first time through.
		heap_ptr = end;
	}

	if((heap_ptr + incr) < (char*)&end_of_ram){
		base = heap_ptr;	//  Point to end of heap.
		heap_ptr += incr;	//  Increase heap.
		return base;		//  Return pointer to start of new heap area.

	}else{
		// not correct!
		return 0;
	}

}


int _fstat (int file, struct stat * st)
{
	st->st_mode = S_IFCHR;
	return 0;

 
}

int _stat (const char *fname, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _link (void)
{
	errno = EMLINK;
	return -1;
}

int _unlink (void)
{
  return -1;
}

void _raise (void)
{
  return;
}

int _gettimeofday (struct timeval * tp, struct timezone * tzp)
{


  return 0;
}

/* Return a clock that ticks at 100Hz.  */
clock_t _times (struct tms * tp)
{
  clock_t timeval = 0;

  
  return timeval;
};


int isatty (int fd)
{
  return 1;
  fd = fd;
}

int _system (const char *s)
{
  if (s == NULL)
    return 0;
  errno = ENOSYS;
  return -1;
}

int _rename (const char * oldpath, const char * newpath)
{
  errno = ENOSYS;
  return -1;
}

