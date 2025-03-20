#include <target/wdt.h>

void wdt_init(void)
{
	wdt_hw_con_init();
	//timer_register(wdt_timer_handler);
}
