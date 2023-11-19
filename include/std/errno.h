#ifndef	__STD_ERRNO_H_INCLUDE__
#define __STD_ERRNO_H_INCLUDE__

#define EPERM            1
#define ENOENT           2
#define EINTR            4
#define EIO              5
#define ECHILD          10
#define EAGAIN          11
#define ENOMEM          12
#define EACCES          13
#define EFAULT          14
#define EBUSY           16
#define EEXIST          17
#define ENODEV          19
#define EINVAL          22
#define	ENOSPC		28
#define EPIPE           32
#define ERANGE          34
#define EDEADLK         35
#define ENOLCK          37
#define ENOSYS          38
#define ELOOP           40
#define ENOMSG          42
#define ENODATA         61
#define ETIME           62
#define EPROTO          71
#define EOVERFLOW	75
#define EOPNOTSUPP      95
#define ENOTSUP         EOPNOTSUPP
#define EINPROGRESS     115

#ifndef __ASSEMBLY__
extern int errno;
#endif /* __ASSEMBLY__ */

#endif /* __STD_ERRNO_H_INCLUDE__ */
