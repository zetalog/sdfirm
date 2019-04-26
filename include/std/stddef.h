#ifndef __GNU_STDDEF_H
#define __GNU_STDDEF_H

#ifndef __ASSEMBLY__

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ long unsigned int
typedef __SIZE_TYPE__ size_t;
#endif

#ifndef __WCHAR_TYPE__
#define __WCHAR_TYPE__ int
#ifndef __cplusplus
typedef __WCHAR_TYPE__ wchar_t;
#endif
#endif

/* A null pointer constant.  */
#ifndef __cplusplus
#ifndef NULL
#define NULL ((void *)0)
#endif
#else   /* C++ */
#define NULL 0
#endif  /* C++ */

#endif

#endif /* __GNU_STDDEF_H */
