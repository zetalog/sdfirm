#ifndef __SYS_POLL_H_INCLUDE__
#define __SYS_POLL_H_INCLUDE__

typedef struct pollfd {
	int fd;                         /* file desc to poll */
	short events;                   /* events of interest on fd */
	short revents;                  /* events that occurred on fd */
} pollfd_t;

typedef int nfds_t;

#define POLLIN  0x0001
#define POLLOUT 0x0004
#define POLLERR 0x0008

// synonyms
#define POLLNORM POLLIN
#define POLLPRI POLLIN
#define POLLRDNORM POLLIN
#define POLLRDBAND POLLIN
#define POLLWRNORM POLLOUT
#define POLLWRBAND POLLOUT

// ignored
#define POLLHUP 0x0010
#define POLLNVAL 0x0020

int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#endif
