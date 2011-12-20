#include <target/generic.h>
#include <target/state.h>

#include "entry.c"

void main(void)
{
	system_init();
}

#pragma disable_warning 190
#ifdef SDCC
void board_init_early(void);

void system_init_early(void)
{
	board_init_early();
}

unsigned char _sdcc_external_startup()
{
	system_init_early();
	return 0;
}
#endif
