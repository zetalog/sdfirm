#include <target/console.h>

#ifdef __GNUC__
const char __stack_chk_fmt_fail[] = \
	"STACK FAULT: Buffer Overflow in function %llx.\n";

/* "canary" value that is inserted by the compiler into the stack frame.
 * If ASLR was enabled we could use
 * void (*__stack_chk_guard)(void) = __stack_chk_fail;
 */
void *__stack_chk_guard = (void*)0x0AFF;

/* Error path for compiler generated stack "canary" value check code. If
 * the stack canary has been overwritten this function gets called on exit
 * of the function.
 */
void __stack_chk_fail(void)
{
	con_err(__stack_chk_fmt_fail,
		(uint64_t)((uintptr_t)__builtin_return_address(0)));
}
#endif
