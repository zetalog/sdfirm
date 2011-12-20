#ifndef __CIRCBF_H_INCLUDE__
#define __CIRCBF_H_INCLUDE__

#include <host/types.h>

/* Buffer 'size' must always be a power of tow. */

#define CIRCBF_INIT_DECLARE()					\
	{ 0, 0, {0} }
#define CIRCBF_INIT_ASSIGN(buf)					\
	{ 0, 0, buf }

#define DECLARE_CIRCBF8_MEMBER(name, size)			\
	struct circbf_a_##name {				\
		uint8_t head;					\
		uint8_t tail;					\
		uint8_t buffer[size];				\
	} name
#define DECLARE_CIRCBF8(name, size)				\
	DECLARE_CIRCBF8_MEMBER(name, size) = CIRCBF_INIT_DECLARE()
#define ASSIGN_CIRCBF8_MEMBER(name)				\
	struct circbf_b_##name {				\
		uint8_t head;					\
		uint8_t tail;					\
		uint8_t *buffer;				\
	} name
#define ASSIGN_CIRCBF8(name, buf)				\
	ASSIGN_CIRCBF8_MEMBER(name) = CIRCBF_INIT_ASSIGN(buf)

#define DECLARE_CIRCBF16_MEMBER(name, size)			\
	struct circbf_a_##name {				\
		uint16_t head;					\
		uint16_t tail;					\
		uint8_t buffer[size];				\
	} name
#define DECLARE_CIRCBF16(name, size)				\
	DECLARE_CIRCBF16_MEMBER(name, size) = CIRCBF_INIT_DECLARE()
#define ASSIGN_CIRCBF16_MEMBER(name)				\
	struct circbf_b_##name {				\
		uint16_t head;					\
		uint16_t tail;					\
		uint8_t *buffer;				\
	} name
#define ASSIGN_CIRCBF16(name, buf)				\
	ASSIGN_CIRCBF16_MEMBER(name) = CIRCBF_INIT_ASSIGN(buf)
#define ASSIGN_CIRCBF16_REF(name, ref, val)			\
	struct circbf_b_##name *ref = val			\

#define DECLARE_CIRCBF32_MEMBER(name, size)			\
	struct circbf_a_##name {				\
		uint32_t head;					\
		uint32_t tail;					\
		uint8_t buffer[size];				\
	} name
#define DECLARE_CIRCBF32(name, size)				\
	DECLARE_CIRCBF32_MEMBER(name, size) = CIRCBF_INIT_DECLARE()
#define ASSIGN_CIRCBF32_MEMBER(name)				\
	struct circbf_b_##name {				\
		uint32_t head;					\
		uint32_t tail;					\
		uint8_t *buffer;				\
	} name
#define ASSIGN_CIRCBF32(name, buf)				\
	ASSIGN_CIRCBF32_MEMBER(name) = CIRCBF_INIT_ASSIGN(buf)
#define ASSIGN_CIRCBF32_REF(name, ref, val)			\
	struct circbf_b_##name *ref = val			\

/* Space between the cursors. */
#define __CIRCBF_CALC(__stop__, __start__, __size__)		\
	(((__stop__) - (__start__)) & ((__size__)-1))
/* Advance the cursor. */
#define ____CIRCBF_GROW(cursor, size, n)			\
	((cursor) = ((cursor)+n) & ((size)-1))
#define __CIRCBF_GROW(cursor, size)				\
	((cursor) = ((cursor)+1) & ((size)-1))

/* Return count in buffer.  */
#define circbf_count(__circ__, __size__)			\
	__CIRCBF_CALC((__circ__)->head, (__circ__)->tail, (__size__))
/* Return space available, 0..size-1.
 * We always leave one free char as a completely full buffer has
 * head == tail, which is the same as empty.
 */
#define circbf_space(circ, size)				\
	__CIRCBF_CALC(((circ)->tail), (((circ)->head)+1), (size))

/* Return count up to the end of the buffer.
 * Carefully avoid accessing head and tail more than once, so they can
 * change underneath us without returning inconsistent results.
 */
#define circbf_count_end(circ, size)				\
	min((size) - ((circ)->tail),				\
	    (((circ)->head) + (size) - ((circ)->tail)) & ((size)-1))
/* Return space available up to the end of the buffer.  */
#define circbf_space_end(circ, size)				\
	min((size) - ((circ)->head),				\
	    ((size) - 1 - ((circ)->head) + ((circ)->tail)) & ((size)-1))

#define circbf_woff(__circ__)	((__circ__)->head)
#define circbf_roff(__circ__)	((__circ__)->tail)
#define circbf_wpos(__circ__)	(&(__circ__)->buffer[(__circ__)->head])
#define circbf_rpos(__circ__)	(&(__circ__)->buffer[(__circ__)->tail])
#define circbf_write(circ, size, n)				\
	____CIRCBF_GROW((circ)->head, size, n)
#define circbf_read(circ, size, n)				\
	____CIRCBF_GROW((circ)->tail, size, n)

#define circbf_in(circ, size, byte)				\
	do {							\
		(circ)->buffer[(circ)->head] = (byte);		\
		__CIRCBF_GROW((circ)->head, size);		\
	} while (0)
#define circbf_out(circ, size, byte)				\
	do {							\
		(byte) = (circ)->buffer[(circ)->tail];		\
		__CIRCBF_GROW((circ)->tail, size);		\
	} while (0)
/* Put even when the buffer is full. */
#define circbf_push(circ, size, byte)				\
	do {							\
		assert((circ)->head < size);			\
		(circ)->buffer[(circ)->head] = (byte);		\
		if (circbf_space((circ), (size)) < 1)		\
			__CIRCBF_GROW((circ)->tail, size);	\
		__CIRCBF_GROW((circ)->head, size);		\
	} while (0)
/* Get even when the buffer is empty. */
#define circbf_pull(circ, size, byte)				\
	do {							\
		if (circbf_count((circ), (size)) > 0) {		\
			(byte) = (circ)->buffer[(circ)->tail];	\
			__CIRCBF_GROW((circ)->tail, size);	\
		}						\
	} while (0)
/* Make the buffer empty. */
#define INIT_CIRCBF_DECLARE(circ)				\
	do {							\
		(circ)->head = 0;				\
		(circ)->tail = 0;				\
	} while (0)
#define INIT_CIRCBF_ASSIGN(circ, buf)				\
	do {							\
		(circ)->head = 0;				\
		(circ)->tail = 0;				\
		(circ)->buffer = (buf);				\
	} while (0)

#endif /* __CIRCBF_H_INCLUDE__ */
