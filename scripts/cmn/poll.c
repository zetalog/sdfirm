#include <limits.h>
#define FD_SETSIZE 64
#include <sys/types.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <winsock.h>
#include <errno.h>
#include <io.h>
#include <string.h>

#define CONFIG_POLL_OBJECT	1

#ifdef CONFIG_POLL_SOCKET
int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
	struct timeval tv;
	struct timeval *tvp;
	fd_set readFDs, writeFDs, exceptFDs;
	fd_set *readp, *writep, *exceptp;
	struct pollfd *pollEnd, *p;
	int selected;
	int result;
	int maxFD;
	
	if (!fds) {
		pollEnd = NULL;
		readp = NULL;
		writep = NULL;
		exceptp = NULL;
		maxFD = 0;
	} else {
		pollEnd = fds + nfds;
		readp = &readFDs;
		writep = &writeFDs;
		exceptp = &exceptFDs;
		
		FD_ZERO(readp);
		FD_ZERO(writep);
		FD_ZERO(exceptp);
		
		maxFD = 0;
		for (p = fds; p < pollEnd; p++) {
			if (p->fd > maxFD) maxFD = p->fd;
		}
		
		if (maxFD >= FD_SETSIZE) {
			// At least one fd is too big
			errno = EINVAL;
			return -1;
		}
		
		for (p = fds; p < pollEnd; p++) {
			if (p->fd < 0) {
				;
			} else {
				if (p->events & POLLIN)  FD_SET(p->fd, readp);
				if (p->events & POLLOUT) FD_SET(p->fd, writep);
				if (p->events != 0)      FD_SET(p->fd, exceptp);
			}
		}
	}
        
	if (timeout >= 0) {
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		tvp = &tv;
	} else {
		tvp = NULL;
	}
	
	selected = select(maxFD+1, readp, writep, exceptp, tvp);
	if (selected < 0) {
		result = -1;
	}  else if (selected > 0) {
		int polled = 0;
		for (p = fds; p < pollEnd; p++) {
			p->revents = 0;
			if (p->fd < 0) {
				;
			} else {
				if ((p->events & POLLIN)   &&  FD_ISSET(p->fd, readp)) {
					p->revents |= POLLIN;
				}
				if ((p->events & POLLOUT)  &&  FD_ISSET(p->fd, writep)) {
					p->revents |= POLLOUT;
				}
				if ((p->events != 0)       &&  FD_ISSET(p->fd, exceptp)) {
					p->revents |= POLLERR;
				}
				
				if (p->revents) polled++;
			}
		}
		result = polled;
	} else {
		for (p = fds; p < pollEnd; p++) {
			p->revents = 0;
		}
		result = 0;
	}
	
	return result;
}
#endif

#ifdef CONFIG_POLL_OBJECT
int poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
	DWORD msecs;
	int i;
	HANDLE handle,*handles;
	int result;
	int count = 0;
	
	/* first, convert timeout to milliseconds */
	if (timeout == -1) {
		msecs = INFINITE;
	} else {
		msecs = timeout;
	}

	if (nfds < 1)
		return EINVAL;
	
	handles = malloc(sizeof(HANDLE) * nfds);
	if (!handles) {
		errno = EAGAIN;
		return -1;
	}

	for (i = 0; i < nfds; i++) {
		handles[i] = (HANDLE)fds[i].fd;
	}
	
	result = WaitForMultipleObjects(nfds, handles, FALSE, msecs);
	if (result == WAIT_FAILED) {
		errno = EINVAL;
		count = -1;
		goto out;
	}

	if (result == WAIT_TIMEOUT) {
		goto out;
	}

	for (i = 0; i < nfds; i++) {
		handle = handles[i];
		result = WaitForSingleObject(handle, 0);
		if (result == WAIT_FAILED || result == WAIT_TIMEOUT)
			continue;
		if (fds[i].events == POLLOUT)
			fds[i].revents = POLLRDNORM;
		if (fds[i].events == POLLIN)
			fds[i].revents = POLLWRNORM;
		count++;
	}
out:
	free(handles);
	return count;
}
#endif

#ifdef CONFIG_POLL_PIPE_SOCKET
struct pollfd {
	int fd;
	short events, revents;
	/*
	 * The Win32 HANDLE to wait on instead of the fd.
	 * This can be NULL, in which case we use the fd.
	 */
	void *ev;
};
#define POLLIN		1
#define POLLOUT		2

int poll(struct pollfd *fds, unsigned int n, int timeout);

/*======================================================================*/
/*======================================================================*/

#include <poll.h>
#include <errno.h>
#include <winsock2.h>

#define PIPE_DELAY 30		/* Update interval for pipe thread */

struct th_fd_info {
	HANDLE fd, event;
};

static long mask(short ev)
{
	long r = FD_ACCEPT | FD_CLOSE;

	if (ev & POLLIN)
		r |= FD_READ;
	if (ev & POLLOUT)
		r |= FD_WRITE;

	return r;
}

static int is_socket(int fd)
{
	int type, len = sizeof(type);
	return !getsockopt((SOCKET)fd, SOL_SOCKET, SO_TYPE,
			   (char *)&type, &len);
}

/* XXX: is_pipe/is_socket Ordering
 *
 * This will return true for sockets, too, not just pipes.
 * So we should check is_socket() before checking this one.
 */
static int is_pipe(int fd)
{
	return GetFileType((HANDLE)fd) == FILE_TYPE_PIPE;
}

static DWORD WINAPI thread_go( LPVOID ptr )
{
	struct th_fd_info *inf = ptr;
	HANDLE fd = inf->fd;
	HANDLE ev = inf->event;
	DWORD len = 0;

	free(inf);
	while (PeekNamedPipe(fd, NULL,0,NULL, &len, NULL)) {
		if (len)
			SetEvent(ev);
		else
			ResetEvent(ev);
		Sleep(PIPE_DELAY);
		len = 0;
	}
	SetEvent(ev);

	return 0;
}

static void start_thread(struct pollfd *fd)
{
	DWORD id;
	struct th_fd_info *info;
	
	info = malloc(sizeof(struct th_fd_info));
	info->fd = (HANDLE)fd->fd;
	info->event = fd->ev = CreateEvent(NULL, 0,0, NULL);
	CreateThread(NULL,0, thread_go,info, 0, &id); 
}

static void check_wsa_events(struct pollfd *fd, unsigned int n)
{
	for (; n--; ++fd) {
		if (is_socket(fd->fd)) {
			if (!fd->ev)
				fd->ev = WSACreateEvent();
			WSAEventSelect(fd->fd, fd->ev, mask(fd->events));
		} else if (is_pipe(fd->fd) && !fd->ev) {
			start_thread(fd);
		}
	}
}


int poll(struct pollfd *pollfd, unsigned int n, int timeout)
{
	HANDLE *handles, *hp;
	struct pollfd *pp = pollfd;
	int num = 0;
	
	handles = malloc(sizeof (HANDLE) * n);
	if (!handles)
		return -ENOMEM;

	hp = handles;
	hp += n;
	pp += n;

	check_wsa_events(pollfd, n);

	while (hp-- > handles && pp-- > pollfd) {
		*hp = (pp->ev ? (HANDLE)pp->ev : (HANDLE)pp->fd);
	}

	do {
		DWORD r;

		if (!n)
			return -1;
		r = MsgWaitForMultipleObjects(n,
					      handles,
					      FALSE,
					      timeout,
					      QS_ALLINPUT);
		if (r >= WAIT_OBJECT_0 && r < WAIT_OBJECT_0 + n) {
			int i = r - WAIT_OBJECT_0;

			if (!pollfd[i].events)
				pollfd[i].events = POLLIN;
			pollfd[i].revents = pollfd[i].events;
			if (is_socket(pollfd[i].fd))
				WSAResetEvent(pollfd[i].ev);
			++num;
			timeout = 0;
			++i;
			handles += i;
			pollfd += i;
			n -= i;
			if (!n)
				return num;
		} else if (r == WAIT_TIMEOUT)
			return num;
		else
			return -1;
	} while (1);
}
#endif
