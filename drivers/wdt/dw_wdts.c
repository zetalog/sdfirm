#include <target/wdt.h>

void dw_wdt_feed(int id)
{
	dw_wdt_write(WDT_CRR(id), WDT_RESTART_KEY);
}

int dw_wdt_has_timed_out(int id)
{
	return dw_wdt_read(WDT_STAT(id)) & 0x1;
}

void dw_wdt_clear_timeout(int id)
{
	dw_wdt_read(WDT_EOI(id));
}

void dw_wdt_disable(int id)
{
	uint32_t value = dw_wdt_read(WDT_CR(id));
	dw_wdt_write(WDT_CR(id), value & ~WDT_ENABLE);
}

void dw_wdt_ctrl_init(int id, uint32_t timeout)
{
	uint32_t version;
	
	version = dw_wdt_read(WDT_COMP_VERSION(wdt_id));
	printf("WDT Version: 0x%X\n", version);
	dw_wdt_write(WDT_TORR(id), timeout & 0xF);
	dw_wdt_write(WDT_CR(id), WDT_ENABLE | WDT_RESET_ENABLE);
}