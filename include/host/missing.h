#ifndef __MISSING_H_INCLUDE__
#define __MISSING_H_INCLUDE__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef WIN32
 #include <sys/select.h>
 #include <sys/time.h>
 #include <sys/types.h>
 #include <unistd.h>
 #include <errno.h>
 #include <stdlib.h>
#else
 #include <time.h>
 #include <windows.h>
 #define __FUNCTION__	"fun()"
 #define sleep(x)	Sleep(x*1000)
 #define ftruncate(f,s)	chsize(f,s)
 #define msleep(x)	Sleep(x);
 #define sleep(x)	Sleep(x*1000)
 #define __attribute__(x)
 #define __attribute_used__
 #define __LITTLE_ENDIAN_BITFIELD	1
 #define fsync		_commit
 #define vsnprintf	_vsnprintf
 #define snprintf	_snprintf
 #define strcasecmp	_stricmp
 #define strncasecmp	_strnicmp
#endif
#include <stdint.h>

#endif /* __MISSING_H_INCLUDE__ */
