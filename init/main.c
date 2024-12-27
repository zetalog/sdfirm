#include <target/generic.h>
#include <target/arch.h>
#include <target/irq.h>
#include <target/bh.h>
#include <target/delay.h>
#include <target/timer.h>
#include <target/jiffies.h>
#include <target/task.h>
#include <target/smp.h>
#include <target/clk.h>
#include <target/cmdline.h>
#include <target/mem.h>
#include <target/heap.h>
#include <target/paging.h>
#include <target/console.h>
#include <target/percpu.h>
#include <target/panic.h>
#include <target/bench.h>
#include <target/bulk.h>
#include <target/gpio.h>
#include <target/perf.h>

__near__ uint32_t system_device_id = 0;
text_char_t system_vendor_name[] = CONFIG_VENDOR_NAME;
text_char_t system_device_name[] = CONFIG_PRODUCT_NAME;

uint16_t system_product_id(void)
{
	return MAKEWORD(DEV_PRODUCT_ID, HIBYTE(LOWORD(system_device_id)));
}

#ifdef CONFIG_RIS
void ris_entry(void);
#else
#define ris_entry()	do { } while (0)
#endif
#ifdef CONFIG_GEM5
extern void simpoint_entry(void);
#endif

void system_init(void)
{
	idmap_early_con_init();
	mem_init();
	early_fixmap_init();
	main_debug(MAIN_DEBUG_INIT, 0);
	board_early_init();
	clk_init();
	gpio_init();
	debug_init();
	irq_init();
	tick_init();
	delay_init();
#ifdef CONFIG_PORTING_DELAY
	while (1) {
		printf("%llx\r\n", tsc_read_counter());
		printf("%llx\r\n", tsc_read_counter());
		printf("=====\r\n");
		delay(5);
	}
#endif
	fixmap_late_con_init();
	paging_init();
	page_init();
	heap_init();
	bulk_init();
	percpu_init();

#ifdef CONFIG_GEM5
	con_log("Simpoint: Start simpoint_entry\n");
	simpoint_entry();
#endif
	ris_entry();
	bh_init();
	timer_init();

	modules_init();
	board_late_init();
	perf_init();
	task_init();
	appl_init();
	smp_init();
}

void system_suspend(void)
{
}

void system_resume(void)
{
}
