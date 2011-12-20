#include <target/generic.h>

#ifdef CONFIG_ATOMIC_ADD_RETURN
atomic_count_t atomic_add_return(atomic_count_t i, atomic_t *v)
{
	atomic_count_t __temp = 0;
	v->counter += i;
	__temp = v->counter;
	return __temp;
}
#endif

#ifdef CONFIG_ATOMIC_SUB_RETURN
atomic_count_t atomic_sub_return(atomic_count_t i, atomic_t *v)
{
	atomic_count_t __temp = 0;
	v->counter -= i;
	__temp = v->counter;
	return __temp;
}
#endif
