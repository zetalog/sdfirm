#ifndef __HEAP_H_INCLUDE__
#define __HEAP_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/page.h>	/* page size */

/* This allocator is written by Doug Lea and released to the public.
 * VERSION 2.6.6  Sun Mar  5 19:10:03 2000  Doug Lea  (dl at gee)
 * This is not the fastest, most space-conserving, most portable, or most
 * tunable malloc ever written. However it is among the fastest while also
 * being among the most space-conserving, portable and tunable. Consistent
 * balance across these factors results in a good general-purpose
 * allocator.
 *
 * NOTE: You may send questions/comments/complaints/performance data to
 *       dl@cs.oswego.edu
 * NOTE: You may find an updated version at
 *       ftp://g.oswego.edu/pub/misc/malloc.c
 * NOTE: You may find a high-level description at
 *       http://g.oswego.edu/dl/html/malloc.html
 *
 * Vital statistics:
 *
 * Alignment:                            8-byte
 *      8 byte alignment is currently hardwired into the design.  This
 *      seems to suffice for all current machines and C compilers.
 * Assumed pointer representation:       4 or 8 bytes
 *      Code for 8-byte pointers is untested by me but has worked
 *      reliably by Wolfram Gloger, who contributed most of the
 *      changes supporting this.
 * Assumed size_t  representation:       4 or 8 bytes
 *      Note that size_t is allowed to be 4 bytes even if pointers are 8.
 * Minimum overhead per allocated chunk: 4 or 8 bytes
 *      Each malloced chunk has a hidden overhead of 4 bytes holding size
 *      and status information.
 * Minimum allocated size: 4-byte ptrs:  16 bytes    (including 4 overhead)
 *       		  8-byte ptrs:  24/32 bytes (including, 4/8 overhead)
 *      When a chunk is freed, 12 (for 4byte ptrs) or 20 (for 8 byte ptrs but
 *      4 byte size) or 24 (for 8/8) additional bytes are needed; 4 (8) for a
 *      trailing size field and 8 (16) bytes for free list pointers. Thus, the
 *      minimum allocatable size is 16/24/32 bytes.
 *      Even a request for zero bytes (i.e., malloc(0)) returns a
 *      pointer to something of the minimum allocatable size.
 * Maximum allocated size: 4-byte size_t: 2^31 -  8 bytes
 *       		   8-byte size_t: 2^63 - 16 bytes
 *    It is assumed that (possibly signed) size_t bit values suffice to
 *    represent chunk sizes. `Possibly signed' is due to the fact that 'size_t'
 *    may be defined on a system as either a signed or an unsigned type. To be
 *    conservative, values that would appear as negative numbers are avoided.
 *    Requests for sizes with a negative sign bit when the request size is
 *    treaded as a long will return null.
 * Maximum overhead wastage per allocated chunk: normally 15 bytes
 *    Alignnment demands, plus the minimum allocatable size restriction
 *    make the normal worst-case wastage 15 bytes (i.e., up to 15
 *    more bytes will be allocated than were requested in malloc), with
 *    two exceptions:
 *      1. Because requests for zero bytes allocate non-zero space,
 *         the worst case wastage for a request of zero bytes is 24 bytes.
 *
 * Limitations
 *
 * Here are some features that are NOT currently supported
 *   No user-definable hooks for callbacks and the like.
 *   No automated mechanism for fully checking that all accesses
 *   to malloced memory stay within their bounds.
 *   No support for compaction.
 */

/* heap_size_t is the word-size used for internal bookkeeping of chunk
 * sizes. On a 64-bit machine, you can reduce heap_alloc overhead by
 * defining heap_size_t to be a 32 bit `unsigned int' at the expense of
 * not being able to handle requests greater than 2^31. This limitation
 * is hardly ever a concern; you are encouraged to set this. However, the
 * default version is the same as size_t.
 */

#if CONFIG_HEAP_SIZE > 65535
typedef uint32_t heap_size_t;
#elif CONFIG_HEAP_SIZE > 255
typedef uint16_t heap_size_t;
#else
typedef uint8_t heap_size_t;
#endif

#if CONFIG_HEAP_SIZE > 32767
typedef int32_t heap_offset_t;
#elif CONFIG_HEAP_SIZE > 127
typedef int16_t heap_offset_t;
#else
typedef int8_t heap_offset_t;
#endif
#if defined(CONFIG_HEAP) && (CONFIG_HEAP_SIZE < 256)
#error "Unsupported small heap size"
#endif

#define HEAP_SIZE_SIZE		(sizeof (heap_size_t))
#define HEAP_ADDR_SIZE		(sizeof (caddr_t))

#define INVALID_HEAP_ADDR	((caddr_t)-1)

#ifdef CONFIG_HEAP_ALIGN_1
#define HEAP_ALIGN	1
#endif
#ifdef CONFIG_HEAP_ALIGN_2
#define HEAP_ALIGN	2
#endif
#ifdef CONFIG_HEAP_ALIGN_4
#define HEAP_ALIGN	4
#endif
#ifdef CONFIG_HEAP_ALIGN_8
#define HEAP_ALIGN	8
#endif

caddr_t heap_sbrk(heap_offset_t bytes);
#ifdef CONFIG_HEAP
void heap_range_init(caddr_t start_addr);
#else
#define heap_range_init(start_addr)
#endif

caddr_t heap_alloc(heap_size_t bytes);
void heap_free(caddr_t ptr);
caddr_t heap_realloc(caddr_t ptr, heap_size_t bytes);
caddr_t heap_calloc(heap_size_t bytes);

#endif /* __HEAP_H_INCLUDE__ */
