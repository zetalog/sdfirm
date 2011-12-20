#ifndef __ATOMIC_H_INCLUDE__
#define __ATOMIC_H_INCLUDE__

#include <target/types.h>

/* atomic_t should be 8 bit signed type */
typedef int32_t atomic_count_t;

typedef struct {atomic_count_t counter;}	atomic_t;

#define ATOMIC_INIT(i)			{(i)}

#define atomic_read(l)			((l)->counter)
#define atomic_set(l, i)		(((l)->counter) = (i))
#define atomic_inc(l)			(((l)->counter)++)
#define atomic_dec(l)			(((l)->counter)--)

atomic_count_t atomic_add_return(atomic_count_t i, atomic_t *v);
atomic_count_t atomic_sub_return(atomic_count_t i, atomic_t *v);
#define atomic_dec_return(v)		atomic_sub_return(1, (v))
#define atomic_inc_return(v)		atomic_add_return(1, (v))
#define atomic_add_and_test(i,v)	(atomic_add_return((i), (v)) == 0)
#define atomic_inc_and_test(v)		(atomic_inc_return(v) == 0)
#define atomic_sub_and_test(i,v)	(atomic_sub_return((i), (v)) == 0)
#define atomic_dec_and_test(v)		(atomic_sub_return(1, (v)) == 0)

#endif /* __ATOMIC_H_INCLUDE__ */
