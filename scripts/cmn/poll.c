#define FD_SETSIZE 64
#include <winsock2.h>
#include <errno.h>
#include <sys/poll.h>
#include <limits.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <winsock.h>
#include <errno.h>
#include <io.h>
#include <string.h>

#define CONFIG_POLL_ALL	1

#define FILE_MAX_BUFFER		8
#define FILE_MAX_HANDLE		16

#define FOPEN           0x01    /* file handle open */
#define FEOFLAG         0x02    /* end of file has been encountered */
#define FCRLF           0x04    /* CR-LF across read buffer (in text mode) */
#define FPIPE           0x08    /* file handle refers to a pipe */
#define FNOINHERIT      0x10    /* file handle opened _O_NOINHERIT */
#define FAPPEND         0x20    /* file handle opened O_APPEND */
#define FDEV            0x40    /* file handle refers to device */
#define FTEXT           0x80    /* file handle is in text mode */

struct __win32fd {
	int fd;
	char fileflags;
	BYTE rbuf[FILE_MAX_BUFFER];
	BYTE wbuf[FILE_MAX_BUFFER];
	int rlen;
	int wlen;
	int roff;
	int woff;
	HANDLE revent;
	HANDLE wevent;
	OVERLAPPED woverlap;
	OVERLAPPED roverlap;
	BOOL wpending;
	BOOL rpending;
};

struct __pollfd {
	struct pollfd;
	/* The Win32 HANDLE to wait on instead of the fd.
	 * This can be NULL, in which case we use the fd.
	 */
	void *rev;
	void *wev;
};

struct __win32fd win32fds[FILE_MAX_HANDLE];
int nr_win32fds = 0;

static long socket_mask(short ev)
{
	long r = 0;

	if (ev & POLLIN)
		r |= FD_READ | FD_ACCEPT;
	if (ev & POLLOUT)
		r |= FD_WRITE | FD_CLOSE;

	return r;
}

static int is_socket(int fd)
{
	int type, len = sizeof(type);
	return !getsockopt((SOCKET)fd, SOL_SOCKET, SO_TYPE,
			   (char *)&type, &len);
}

HANDLE *poll_socket(struct __pollfd *fd, HANDLE *hp)
{
	if (!fd->rev)
		fd->rev = WSACreateEvent();
	WSAEventSelect(fd->fd, fd->rev, socket_mask(fd->events));
	*hp = fd->rev;
	hp++;
	return hp;
}

BOOL check_socket(struct __pollfd *__fd)
{
	DWORD res;
	WSANETWORKEVENTS event;

	res = WaitForSingleObject(__fd->rev, 0);
	if (res == WAIT_FAILED || res == WAIT_TIMEOUT)
		return FALSE;
	WSAEnumNetworkEvents(__fd->fd, __fd->rev, &event);   
	if (event.lNetworkEvents & (FD_ACCEPT | FD_READ)) {
		__fd->revents |= POLLIN;
	}
	if (event.lNetworkEvents & (FD_CLOSE | FD_WRITE)) {
		__fd->revents |= POLLOUT;
	}
	return TRUE;
}

int file_find(int __fd)
{
	int i;

	for (i = 0; i < nr_win32fds; i++) {
		if (__fd == win32fds[i].fd)
			return i;
	}
	return -1;
}

static HANDLE *poll_file(struct __pollfd *fd, HANDLE *hp)
{
	int fh;
	struct __win32fd *wfd;
	
	fh = file_find(fd->fd);
	if (fh < 0)
		return hp;
	wfd = &win32fds[fh];
	
	if (fd->events & POLLIN) {
		if (wfd->rlen > 0) {
			fd->revents |= POLLIN;
		} else {
			fd->rev = CreateEvent(NULL, 0,0, NULL);
			*hp = wfd->roverlap.hEvent = fd->rev;
			hp++;
		}
	}
	if (fd->events & POLLOUT) {
		if (wfd->wlen < FILE_MAX_BUFFER) {
			fd->revents |= POLLIN;
		} else {
			fd->wev = CreateEvent(NULL, 0,0, NULL);
			*hp = wfd->woverlap.hEvent = fd->wev;
			hp++;
		}
	}
	return hp;
}

BOOL check_file(struct __pollfd *__fd)
{
	int fh;
	struct __win32fd *wfd;
	DWORD res = FALSE;
	
	fh = file_find(__fd->fd);
	if (fh < 0)
		return FALSE;
	wfd = &win32fds[fh];

	if (__fd->events & POLLIN) {
		res = WaitForSingleObject(__fd->rev, 0);
		if (res != WAIT_FAILED && res != WAIT_TIMEOUT) {
			__fd->revents |= POLLIN;
			res = TRUE;
		}
	}
	if (__fd->events & POLLOUT) {
		res = WaitForSingleObject(__fd->wev, 0);
		if (res != WAIT_FAILED && res != WAIT_TIMEOUT) {
			__fd->revents |= POLLOUT;
			res = TRUE;
		}
	}

	return res;
}

#include <fcntl.h>
#include <sys/stat.h>

struct errentry {
        unsigned long oscode;           /* OS return value */
        int errnocode;  /* System V error code */
};

static struct errentry errtable[] = {
        {  ERROR_INVALID_FUNCTION,       EINVAL    },  /* 1 */
        {  ERROR_FILE_NOT_FOUND,         ENOENT    },  /* 2 */
        {  ERROR_PATH_NOT_FOUND,         ENOENT    },  /* 3 */
        {  ERROR_TOO_MANY_OPEN_FILES,    EMFILE    },  /* 4 */
        {  ERROR_ACCESS_DENIED,          EACCES    },  /* 5 */
        {  ERROR_INVALID_HANDLE,         EBADF     },  /* 6 */
        {  ERROR_ARENA_TRASHED,          ENOMEM    },  /* 7 */
        {  ERROR_NOT_ENOUGH_MEMORY,      ENOMEM    },  /* 8 */
        {  ERROR_INVALID_BLOCK,          ENOMEM    },  /* 9 */
        {  ERROR_BAD_ENVIRONMENT,        E2BIG     },  /* 10 */
        {  ERROR_BAD_FORMAT,             ENOEXEC   },  /* 11 */
        {  ERROR_INVALID_ACCESS,         EINVAL    },  /* 12 */
        {  ERROR_INVALID_DATA,           EINVAL    },  /* 13 */
        {  ERROR_INVALID_DRIVE,          ENOENT    },  /* 15 */
        {  ERROR_CURRENT_DIRECTORY,      EACCES    },  /* 16 */
        {  ERROR_NOT_SAME_DEVICE,        EXDEV     },  /* 17 */
        {  ERROR_NO_MORE_FILES,          ENOENT    },  /* 18 */
        {  ERROR_LOCK_VIOLATION,         EACCES    },  /* 33 */
        {  ERROR_BAD_NETPATH,            ENOENT    },  /* 53 */
        {  ERROR_NETWORK_ACCESS_DENIED,  EACCES    },  /* 65 */
        {  ERROR_BAD_NET_NAME,           ENOENT    },  /* 67 */
        {  ERROR_FILE_EXISTS,            EEXIST    },  /* 80 */
        {  ERROR_CANNOT_MAKE,            EACCES    },  /* 82 */
        {  ERROR_FAIL_I24,               EACCES    },  /* 83 */
        {  ERROR_INVALID_PARAMETER,      EINVAL    },  /* 87 */
        {  ERROR_NO_PROC_SLOTS,          EAGAIN    },  /* 89 */
        {  ERROR_DRIVE_LOCKED,           EACCES    },  /* 108 */
        {  ERROR_BROKEN_PIPE,            EPIPE     },  /* 109 */
        {  ERROR_DISK_FULL,              ENOSPC    },  /* 112 */
        {  ERROR_INVALID_TARGET_HANDLE,  EBADF     },  /* 114 */
        {  ERROR_INVALID_HANDLE,         EINVAL    },  /* 124 */
        {  ERROR_WAIT_NO_CHILDREN,       ECHILD    },  /* 128 */
        {  ERROR_CHILD_NOT_COMPLETE,     ECHILD    },  /* 129 */
        {  ERROR_DIRECT_ACCESS_HANDLE,   EBADF     },  /* 130 */
        {  ERROR_NEGATIVE_SEEK,          EINVAL    },  /* 131 */
        {  ERROR_SEEK_ON_DEVICE,         EACCES    },  /* 132 */
        {  ERROR_DIR_NOT_EMPTY,          ENOTEMPTY },  /* 145 */
        {  ERROR_NOT_LOCKED,             EACCES    },  /* 158 */
        {  ERROR_BAD_PATHNAME,           ENOENT    },  /* 161 */
        {  ERROR_MAX_THRDS_REACHED,      EAGAIN    },  /* 164 */
        {  ERROR_LOCK_FAILED,            EACCES    },  /* 167 */
        {  ERROR_ALREADY_EXISTS,         EEXIST    },  /* 183 */
        {  ERROR_FILENAME_EXCED_RANGE,   ENOENT    },  /* 206 */
        {  ERROR_NESTING_NOT_ALLOWED,    EAGAIN    },  /* 215 */
        {  ERROR_NOT_ENOUGH_QUOTA,       ENOMEM    }    /* 1816 */
};

/* size of the table */
#define ERRTABLESIZE (sizeof(errtable)/sizeof(errtable[0]))
/* The following two constants must be the minimum and maximum
   values in the (contiguous) range of Exec Failure errors. */
#define MIN_EXEC_ERROR ERROR_INVALID_STARTING_CODESEG
#define MAX_EXEC_ERROR ERROR_INFLOOP_IN_RELOC_CHAIN
/* These are the low and high value in the range of errors that are
   access violations */
#define MIN_EACCES_RANGE ERROR_WRITE_PROTECT
#define MAX_EACCES_RANGE ERROR_SHARING_BUFFER_EXCEEDED

void maperr(unsigned long oserrno)
{
	int i;

	/* check the table for the OS error code */
	for (i = 0; i < ERRTABLESIZE; ++i) {
                if (oserrno == errtable[i].oscode) {
                        errno = errtable[i].errnocode;
                        return;
                }
        }

        /* The error code wasn't in the table.  We check for a range of */
        /* EACCES errors or exec failure errors (ENOEXEC).  Otherwise   */
        /* EINVAL is returned.                                          */

        if (oserrno >= MIN_EACCES_RANGE && oserrno <= MAX_EACCES_RANGE)
                errno = EACCES;
        else if (oserrno >= MIN_EXEC_ERROR && oserrno <= MAX_EXEC_ERROR)
                errno = ENOEXEC;
        else
                errno = EINVAL;
}

int file_open(const char *path, int oflag, ...)
{
	va_list ap;
	int pmode;
	int fh;
	char fileflags;
	struct __win32fd *wfd;
	HANDLE osfh;
	DWORD fileaccess;               /* OS file access (requested) */
	DWORD fileshare;                /* OS file sharing mode */
	DWORD filecreate;               /* OS method of opening/creating */
	DWORD fileattrib;               /* OS file attribute flags */
	DWORD isdev;                    /* device indicator in low byte */
	SECURITY_ATTRIBUTES SecurityAttributes;

	va_start(ap, oflag);
	pmode = va_arg(ap, int);
	va_end(ap);
	
	/* default sharing mode is DENY NONE */
	SecurityAttributes.nLength = sizeof(SecurityAttributes);
	SecurityAttributes.lpSecurityDescriptor = NULL;

	if (oflag & _O_NOINHERIT) {
		SecurityAttributes.bInheritHandle = FALSE;
		fileflags = FNOINHERIT;
	} else {
		SecurityAttributes.bInheritHandle = TRUE;
		fileflags = 0;
	}

	/* figure out binary/text mode */
	if ((oflag & _O_BINARY) == 0) {
		if (oflag & _O_TEXT)
			fileflags |= FTEXT;
		else if (_fmode != _O_BINARY)   /* check default mode */
			fileflags |= FTEXT;
	}

	/* decode the access flags */
	switch (oflag & (_O_RDONLY | _O_WRONLY | _O_RDWR)) {
	case _O_RDONLY:
		/* read access */
		fileaccess = GENERIC_READ;
		break;
	case _O_WRONLY:
		/* write access */
		fileaccess = GENERIC_WRITE;
		break;
	case _O_RDWR:
		/* read and write access */
		fileaccess = GENERIC_READ | GENERIC_WRITE;
		break;
	default:
                /* error, bad oflag */
		errno = EINVAL;
		return -1;
        }

	fileshare = FILE_SHARE_READ | FILE_SHARE_WRITE;

	/* decode open/create method flags */
	switch (oflag & (_O_CREAT | _O_EXCL | _O_TRUNC)) {
	case 0:
	case _O_EXCL:
		/* ignore EXCL w/o CREAT */
		filecreate = OPEN_EXISTING;
		break;
	case _O_CREAT:
		filecreate = OPEN_ALWAYS;
		break;
	case _O_CREAT | _O_EXCL:
	case _O_CREAT | _O_TRUNC | _O_EXCL:
		filecreate = CREATE_NEW;
		break;
	case _O_TRUNC:
	case _O_TRUNC | _O_EXCL:
		/* ignore EXCL w/o CREAT */
		filecreate = TRUNCATE_EXISTING;
		break;
	case _O_CREAT | _O_TRUNC:
		filecreate = CREATE_ALWAYS;
		break;
	default:
		/* this can't happen ... all cases are covered */
		errno = EINVAL;
		return -1;
	}

	/* decode file attribute flags if _O_CREAT was specified */
	fileattrib = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED;
        if (oflag & _O_CREAT) {
		if (!(pmode & _S_IWRITE))
			fileattrib = FILE_ATTRIBUTE_READONLY;
	}

	/* Set temporary file (delete-on-close) attribute if requested. */
	if (oflag & _O_TEMPORARY) {
		fileattrib |= FILE_FLAG_DELETE_ON_CLOSE;
		fileaccess |= DELETE;
		fileshare  |= FILE_SHARE_DELETE;
	}

	/* Set temporary file (delay-flush-to-disk) attribute if requested. */
        if (oflag & _O_SHORT_LIVED)
		fileattrib |= FILE_ATTRIBUTE_TEMPORARY;

        /* Set sequential or random access attribute if requested. */
	if (oflag & _O_SEQUENTIAL)
		fileattrib |= FILE_FLAG_SEQUENTIAL_SCAN;
	else if (oflag & _O_RANDOM)
		fileattrib |= FILE_FLAG_RANDOM_ACCESS;

	if (nr_win32fds > FILE_MAX_HANDLE) {
		/* too many open files */
		errno = EMFILE;
		return -1;
	}
	fh = nr_win32fds;

	/* try to open/create the file */
	osfh = CreateFile((LPTSTR)path, fileaccess, fileshare,
			  &SecurityAttributes, filecreate,
			  fileattrib, NULL);
	if (osfh == (HANDLE)(-1)) {
		/* OS call to open/create file failed! */
		maperr(GetLastError());
		return -1;
        }

        /* find out what type of file (file/device/pipe) */
	if ((isdev = GetFileType(osfh)) == FILE_TYPE_UNKNOWN) {
		CloseHandle(osfh);
		maperr(GetLastError());
		return -1;
        }

	/* is isdev value to set flags */
	if (isdev == FILE_TYPE_CHAR)
		fileflags |= FDEV;
	else if (isdev == FILE_TYPE_PIPE)
		fileflags |= FPIPE;

        /* the file is open. now, set the info in fd array */
	wfd = &win32fds[fh];
	nr_win32fds++;
	wfd->fd = (int)osfh;
	wfd->rpending = wfd->wpending = 0;
	wfd->revent = wfd->wevent = NULL;
	wfd->rlen = wfd->wlen = 0;
	wfd->roff = wfd->woff = 0;

        fileflags |= FOPEN;

	/* Set FAPPEND flag if appropriate. Don't do this for devices or
	 * pipes.
	 */
        if (!(fileflags & (FDEV|FPIPE)) && (oflag & _O_APPEND))
		fileflags |= FAPPEND;
	win32fds[fh].fileflags = fileflags;

        return (int)osfh;
}

void file_close(int fd)
{
	int fh;
	struct __win32fd *wfd;

	fh = file_find(fd);
	if (fh < 0) {
		CloseHandle((HANDLE)fd);
		return;
	}
	wfd = &win32fds[fh];
}

static int check_events(struct __pollfd *fd, HANDLE *hp, int nfds)
{
	HANDLE *ihp = hp;

	for (; nfds--; ++fd) {
		if (is_socket(fd->fd)) {
			hp = poll_socket(fd, hp);
		} else {
			hp = poll_file(fd, hp);
		}
	}
	return (hp-ihp) / sizeof (HANDLE);
}

struct __pollfd *alloc_pollfd(struct pollfd *fds, nfds_t n)
{
	int i;
	struct __pollfd *__fds = malloc(sizeof (struct __pollfd) * n);

	if (!fds)
		return NULL;
	for (i = 0; i < n; i++) {
		__fds[i].fd = fds[i].fd;
		__fds[i].events = fds[i].events;
		__fds[i].revents = 0;
		__fds[i].rev = __fds[i].wev = NULL;
	}
	return __fds;
}

void free_pollfd(struct __pollfd *__fds, struct pollfd *fds, nfds_t n)
{
	int i;

	if (__fds) {
		for (i = 0; i < n; i++) {
			if (__fds[i].rev)
				CloseHandle(__fds[i].rev);
			if (__fds[i].wev)
				CloseHandle(__fds[i].wev);
		}
		free(__fds);
	}
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
	HANDLE *handles, *hp;
	struct __pollfd *pp, *__fds = NULL;
	int num = -1;
	int nhdls, i;
	
	__fds = alloc_pollfd(fds, nfds);
	if (!__fds) goto end;
	handles = malloc(sizeof (HANDLE) * nfds * 2);
	if (!handles) goto end;

	pp = __fds;
	hp = handles;
	num = 0;
	nhdls = 0;

	nhdls = check_events(pp, hp, nfds);

	do {
		DWORD r;

		if (!nhdls)
			return -1;
		r = MsgWaitForMultipleObjects(nhdls,
					      handles,
					      FALSE,
					      timeout,
					      QS_ALLEVENTS);
		if (r == WAIT_FAILED) {
			num = -1;
			goto end;
		} else if (r == WAIT_TIMEOUT) {
			goto end;
		} else if (r >= WAIT_OBJECT_0) {
			MSG msg;
			int cnt = r - WAIT_OBJECT_0;

			if (cnt == nhdls) {
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					if (msg.message == WM_QUIT) {
						num = -1;
						goto end;
					}
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			for (i = 0; i < nfds; i++) {
				if (is_socket(__fds[i].fd)) {
					if (check_socket(&__fds[i]))
						num++;
				} else {
					if (check_file(&__fds[i]))
						num++;
				}

			}
		}
	} while (1);

end:
	if (__fds)
		free_pollfd(__fds, fds, nfds);
	if (handles)
		free(handles);
	return num;
}
