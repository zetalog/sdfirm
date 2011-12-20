#ifndef __TYPES_SIM_H_INCLUDE__
#define __TYPES_SIM_H_INCLUDE__

#ifdef WIN32
typedef unsigned short		wchar_t;
typedef unsigned __int64	uint64_t;
#define ARCH_HAVE_UINT64_T 1
#endif
typedef uint32_t		size_t;
typedef uint32_t		caddr_t;
#define _SIZE_T_DEFINED		1
#define ARCH_HAVE_SIZE_T 1
#define ARCH_HAVE_CADDR_T 1

#define __text__	const
#define __near__
#define __far__
#ifndef inline
#define inline		__inline
#endif

#ifndef __FUNCTION__
#define __FUNCTION__		__FILE__
#endif

#define __TEXT_TYPE__(type, name)	\
	typedef type name

#include <windows.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>

#ifdef WIN32
typedef SOCKET			sockfd_t;
#endif

#endif /* __TYPES_SIM_H_INCLUDE__*/
