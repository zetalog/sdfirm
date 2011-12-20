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
