#include <target/generic.h>
#include <target/state.h>

#ifdef CONFIG_DEBUG_PRINT
void dbg_print(uint8_t tag, uint8_t val)
{
	dbg_dump(tag);
	dbg_dump(val);
	dbg_dump(tag);
}

void dbg_dump_str(const text_char_t *str)
{
	const text_char_t *temp;
	for (temp = str; *temp != '\0'; temp++)
		dbg_dump((uint8_t)(*temp));
	dbg_dump('\0');
}
#endif

#ifdef CONFIG_DEBUG_PANIC
void dbg_panic(const text_char_t *file, int line)
{
	main_debug(MAIN_DEBUG_PANIC, 0<<4);
	dbg_dump_str(file);
	dbg_dump(LOBYTE(line));
	dbg_dump(HIBYTE(line));
	main_debug(MAIN_DEBUG_PANIC, 1<<4);

	while (1)
		state_run_idle();
}
#endif
