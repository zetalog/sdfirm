#include <target/config.h>
#include <target/generic.h>
#include <target/arch.h>
#include <target/irq.h>
#include <stdio.h>

__near__ uint32_t system_device_id = 0;
text_char_t system_vendor_name[] = CONFIG_VENDOR_NAME;
text_char_t system_device_name[] = CONFIG_PRODUCT_NAME;

uint16_t system_product_id(void)
{
	return MAKEWORD(DEV_PRODUCT_ID, HIBYTE(LOWORD(system_device_id)));
}

extern void board_init(void);
extern void appl_init(void);
extern void modules_init(void);
extern void bh_init(void);
#ifdef CONFIG_TICK
extern void tick_init(void);
#else
#define tick_init()
#endif
#ifdef CONFIG_TIMER
extern void timer_init(void);
#else
#define timer_init()
#endif
#ifdef CONFIG_TASK
extern void task_init(void);
#else
#define task_init()
#endif
extern void delay_init(void);
#ifdef CONFIG_HEAP
extern void heap_init(void);
#else
#define heap_init()
#endif
#ifdef CONFIG_BULK
extern void bulk_init(void);
#else
#define bulk_init()
#endif
#ifdef CONFIG_GPIO
void gpio_init(void);
#else
#define gpio_init()
#endif
#ifdef CONFIG_DEBUG_PRINT
void debug_init(void);
#else
#define debug_init()
#endif
#ifdef CONFIG_TIMER
void timer_init(void);
#else
#define timer_init()
#endif

#ifndef CONFIG_PORTING
void system_init(void)
{
	main_debug(MAIN_DEBUG_INIT, 0);
	board_init();
	gpio_init();
	debug_init();
	printf("Welcome to sdfirm\r\n");
	irq_init();
	bh_init();
	tick_init();
	delay_init();
	timer_init();
	heap_init();
	bulk_init();

	modules_init();
	appl_init();
	irq_local_enable();
	main_debug(MAIN_DEBUG_INIT, 1);

	while (1) {
#ifdef CONFIG_IDLE
		while (!bh_resumed_any()) {
			dbg_dump(0xAA);
			wait_irq();
		}
		dbg_dump(0xAB);
#endif
		bh_run_all();
	}
}

void system_suspend(void)
{
}

void system_resume(void)
{
}
#else

/* Porting steps guide:
 * 1. CONFIG_PORTING_UART to make debug prompts can work.
 * 2. CONFIG_PORTING_LOAD to make sure .data/.bss sections are correct.
 * 3. CONFIG_PORTING_TSC to make sure tsc_read_counter can work.
 * 4. CONFIG_PORTING_DELAY to make sure calibrate_delay can work.
 * 5. CONFIG_PORTING_GPT to make sure gpt_oneshot_timeout can work.
 * 6. CONFIG_PORTING_TIMER to make sure timer_register can work.
 * 7. Now you can jump to system_init for other modules.
 */
#include <target/config.h>
#include <target/generic.h>
#include <target/arch.h>
#include <target/uart.h>
#include <target/led.h>
#include <target/gpt.h>
#include <target/tsc.h>
#include <target/heap.h>
#include <target/jiffies.h>
#include <target/delay.h>
#include <target/timer.h>
#include <target/bh.h>
#include <target/task.h>

bh_t porting_bh = INVALID_BH;
tid_t porting_tid = INVALID_TID;

#ifdef CONFIG_LED
extern void led_init(void);
#else
#define led_init()
#endif

#ifdef CONFIG_PORTING_LED
led_no_t porting_led_light;
uint8_t porting_led_count;

void __porting_led_run(void)
{
	porting_led_count++;
	if (porting_led_count & 0x01)
		led_light_on(porting_led_light, 0);
	else
		led_light_off(porting_led_light);
}

void __porting_led_init(void)
{
	led_init();
	porting_led_light = led_claim_light();
}

#ifdef CONFIG_PORTING_LED_TIMER
void porting_timer_handler(void)
{
	__porting_led_run();
	timer_schedule_shot(porting_tid, 125);
}

timer_desc_t porting_timer = {
	TIMER_BH,
	porting_timer_handler,
};

static void porting_led_init(void)
{
	timer_init();
	__porting_led_init();
	porting_tid = timer_register(&porting_timer);
	timer_schedule_shot(porting_tid, 0);
}
#else
void porting_bh_handler(void)
{
	__porting_led_run();
	mdelay(250);
	bh_resume(porting_bh);
}

static void porting_led_init(void)
{
	delay_init();
	__porting_led_init();
	porting_bh = bh_register_handler(porting_handler);
}
#endif

void porting_led_init(void)
{
	porting_led_light = led_claim_light();
	flash_start_timer();
}

void porting_handler(uint8_t event)
{
	porting_led_run();
}

void porting_init(void)
{
	porting_led_init();
}
#endif

#ifdef CONFIG_PORTING_HEAP
#define PORTING_HEAP_LOOP		64
#define PORTING_HEAP_UNIT		(CONFIG_HEAP_SIZE / PORTING_HEAP_LOOP)
heap_size_t porting_heap_space = PORTING_HEAP_UNIT;

void porting_heap_test(void)
{
	uint32_t mem, mem2;

	dbg_dump(HIBYTE(porting_heap_space));
	dbg_dump(LOBYTE(porting_heap_space));

	mem = (uint32_t)heap_alloc(porting_heap_space);
	mem2 = (uint32_t)heap_alloc(porting_heap_space);

	dbg_dump(HIBYTE(HIWORD(mem)));
	dbg_dump(LOBYTE(HIWORD(mem)));
	dbg_dump(HIBYTE(LOWORD(mem)));
	dbg_dump(LOBYTE(LOWORD(mem)));

	if (mem) heap_free((caddr_t)mem);
	if (mem2) heap_free((caddr_t)mem2);

	porting_heap_space += PORTING_HEAP_UNIT;
	if (porting_heap_space >= CONFIG_HEAP_SIZE || porting_heap_space == 0)
		porting_heap_space = PORTING_HEAP_UNIT;
}

void porting_handler(uint8_t event)
{
	while (1)
	porting_heap_test();
	bh_resume(porting_bh);
}

void porting_init(void)
{
	heap_init();
	BUG_ON(CONFIG_HEAP_SIZE <= PORTING_HEAP_UNIT);
	BUG_ON(CONFIG_HEAP_SIZE & (PORTING_HEAP_UNIT-1));
	porting_bh = bh_register_handler(porting_handler);
	bh_resume(porting_bh);
}
#endif

#ifdef CONFIG_PORTING_TIMER
uint8_t porting_byte = 0;
uint8_t porting_ticks = 0;

void porting_timer_handler(void)
{
#ifdef CONFIG_TIMER_16BIT
	dbg_dump(porting_byte++);
	timer_schedule_shot(porting_tid, 1000);
#else
	if (porting_ticks == 0) {
		dbg_dump(porting_byte++);
	}
	porting_ticks++;
	porting_ticks &= (8-1);
	timer_schedule_shot(porting_tid, 125);
#endif
}

timer_desc_t porting_timer = {
	TIMER_BH,
	porting_timer_handler,
};

void porting_init(void)
{
	timer_init();
	porting_tid = timer_register(&porting_timer);
	timer_schedule_shot(porting_tid, 0);
}
#endif

#ifdef CONFIG_PORTING_DELAY
void porting_bh_handler(uint8_t event)
{
#ifdef CONFIG_LPS_32BITS
	dbg_dump(HIBYTE(HIWORD(loops_per_ms)));
	dbg_dump(LOBYTE(HIWORD(loops_per_ms)));
#endif
	dbg_dump(HIBYTE(loops_per_ms));
	dbg_dump(LOBYTE(loops_per_ms));
	mdelay(250);
	mdelay(250);
	mdelay(250);
	mdelay(250);
	bh_resume(porting_bh);
}

void porting_init(void)
{
	delay_init();
	porting_bh = bh_register_handler(porting_bh_handler);
	bh_resume(porting_bh);
}
#endif

#ifdef CONFIG_PORTING_SPI
#include <target/spi.h>

#ifdef CONFIG_SPI
extern void spi_init(void);
#else
#define spi_init()
#endif

spi_t porting_spi;

spi_device_t porting_spi_device = {
	SPI_MODE_0,
	SPI_MAX_FREQ,
	0,
};

void porting_handler(uint8_t event)
{
	uint8_t i;
	spi_select_device(porting_spi);
	for (i = 0; i < 256; i++) {
		spi_write_byte(i);
		dbg_dump(spi_read_byte());
	}
	spi_deselect_device();
	bh_resume(porting_bh);
}

void porting_init(void)
{
	spi_init();
	porting_spi = spi_register_device(&porting_spi_device);
	porting_bh = bh_register_handler(porting_handler);
	bh_resume(porting_bh);
}
#endif

#ifdef CONFIG_PORTING_GPT
uint8_t porting_byte = 0;
uint8_t porting_ticks = 0;

void tick_handler(void)
{
	if (!porting_ticks) {
		dbg_dump(porting_byte++);
		if (porting_byte > 5)
			porting_byte = 0;
	}
	porting_ticks++;
	if (porting_ticks > 20)
		porting_ticks = 0;
	gpt_oneshot_timeout(mul16u(porting_byte, 50));
}

void porting_handler(uint8_t event)
{
}

void porting_init(void)
{
	gpt_hw_ctrl_init();
	gpt_oneshot_timeout(porting_byte);
	porting_bh = bh_register_handler(porting_handler);
}
#endif

#ifdef CONFIG_PORTING_TSC
void porting_handler(uint8_t event)
{
	tsc_count_t counter = tsc_read_counter();

#if TSC_MAX > 65535
	dbg_dump(HIBYTE(HIWORD(counter)));
	dbg_dump(LOBYTE(HIWORD(counter)));
#endif
#if TSC_MAX > 255
	dbg_dump(HIBYTE(counter));
#endif
	dbg_dump(LOBYTE(counter));
	bh_resume(porting_bh);
}

void porting_init(void)
{
	tsc_hw_ctrl_init();
	porting_bh = bh_register_handler(porting_handler);
	bh_resume(porting_bh);
}
#endif

#ifdef CONFIG_PORTING_TASK
static void task0(void *priv);
static void task1(void *priv);

pid_t pids[2];
task_call_cb pfns[2] = {
	task0,
	task1,
};
boolean pfgs[2];
uint8_t psts[2][128];

static void task0(void *priv)
{
	while (!pfgs[0]) {
		dbg_dump(0x00);
		task_schedule();
		dbg_dump(0x01);
		task_schedule();
		dbg_dump(0x02);
		task_schedule();
		dbg_dump(0x03);
		task_schedule();
		dbg_dump(0x04);
		task_schedule();
		dbg_dump(0x05);
		task_schedule();
		dbg_dump(0x06);
		task_schedule();
		dbg_dump(0x07);
		task_schedule();
	}
}

static void task1(void *priv)
{

	while (!pfgs[1]) {
		dbg_dump(0x10);
		task_schedule();
		dbg_dump(0x11);
		task_schedule();
		dbg_dump(0x12);
		task_schedule();
		dbg_dump(0x13);
		task_schedule();
		dbg_dump(0x14);
		task_schedule();
		dbg_dump(0x15);
		task_schedule();
		dbg_dump(0x16);
		task_schedule();
		dbg_dump(0x17);
		task_schedule();
	}
}

void porting_init(void)
{
	int i;

	timer_init();
	
	task_init();
	
	for (i = 0; i < 2; i++) {
		pfgs[i] = false;
		pids[i] = task_create(pfns[i], NULL,
				      (caddr_t)(psts[i]), 128);
	}
}
#endif

#ifdef CONFIG_PORTING_GPIO
#define PORTING_GPIO_PORT	CONFIG_PORTING_MINOR
#define PORTING_GPIO_PIN	CONFIG_PORTING_GPIO_PIN
#define PORTING_GPIO_DELAY	1
#ifdef CONFIG_PORTING_GPIO_PP
#define PORTING_GPIO_PAD	GPIO_PAD_PP
#endif
#ifdef CONFIG_PORTING_GPIO_OD
#define PORTING_GPIO_PAD	GPIO_PAD_OD
#endif
#ifdef CONFIG_PORTING_GPIO_KB
#define PORTING_GPIO_PAD	GPIO_PAD_KB
#endif
#ifdef CONFIG_PORTING_GPIO_NP
#define PORTING_GPIO_RES	0
#endif
#ifdef CONFIG_PORTING_GPIO_WU
#define PORTING_GPIO_RES	GPIO_PAD_WU
#endif
#ifdef CONFIG_PORTING_GPIO_WD
#define PORTING_GPIO_RES	GPIO_PAD_WD
#endif

#ifdef CONFIG_PORTING_OUT
uint8_t __porting_gpio(uint8_t i)
{
	i++;
	gpio_write_pin(PORTING_GPIO_PORT, PORTING_GPIO_PIN,
		       i & 0x01);
	return i;
}
#else
uint8_t __porting_gpio(uint8_t i)
{
	return gpio_read_pin(PORTING_GPIO_PORT, PORTING_GPIO_PIN);
}
#endif

void porting_handler(uint8_t event)
{
	uint8_t i = 0;

	gpio_hw_porting_init();
	gpio_config_mux(PORTING_GPIO_PORT, PORTING_GPIO_PIN, GPIO_MUX_NONE);
	gpio_config_pad(PORTING_GPIO_PORT, PORTING_GPIO_PIN,
			PORTING_GPIO_PAD | PORTING_GPIO_RES, 2);
	while (1) {
		mdelay(PORTING_GPIO_DELAY);
		i = __porting_gpio(i);
		dbg_dump(i);
	}
}

void porting_init(void)
{
	porting_bh = bh_register_handler(porting_handler);
	delay_init();
	bh_resume(porting_bh);
}
#endif

#ifdef CONFIG_PORTING_UART
/* XXX: UART Metering
 * dbg_dump will be forced an output of 0x55 that can be easily
 * observed by the oscilloscope to measure the baudrate.  Frequency
 * displayed by the oscilloscope will be a half of the baudrate/10.
 * The frequency captured by the oscilloscope "osc_freq" and the uart
 * baudrate "uart_baud" could have following relationship:
 * osc_freq = uart_baud/20
 */
#define UART_METER_BAUD_IS_20FREQ	0x55
#define UART_METER_BAUD_IS_40FREQ	0x99
#define UART_METER			UART_METER_BAUD_IS_20FREQ

#ifdef CONFIG_UART
extern void uart_init(void);
#else
#define uart_init()
#endif

#ifdef CONFIG_PORTING_SYNC
void porting_handler(uint8_t event)
{
	while (1)
		dbg_dump(UART_METER);
}

void porting_init(void)
{
	porting_bh = bh_register_handler(porting_handler);
	bh_resume(porting_bh);
}
#endif

#ifdef CONFIG_PORTING_ASYNC
#define PORTING_UART_PORT	CONFIG_PORTING_MINOR
#define PORTING_UART_SIZE	CONFIG_PORTING_SIZE

uint8_t porting_uart_oob[1];

boolean porting_uart_sync(uint8_t *byte)
{
	return true;
}

static void porting_uart_none(void)
{
}

#ifdef CONFIG_PORTING_OUT
static void porting_uart_tx_poll(void)
{
	dbg_dump(0x30);
	bulk_request_submit(uart_bulk_tx(PORTING_UART_PORT),
			    PORTING_UART_SIZE);
	dbg_dump(0x31);
}

static void porting_uart_tx_iocb(void)
{
	size_t i;
	uint8_t val = UART_METER;

	dbg_dump(0x32);
	for (i = 0; i < PORTING_UART_SIZE; i++) {
		BULK_WRITE_BEGIN(val) {
			dbg_dump(val);
		} BULK_WRITE_END
	}
	dbg_dump(0x33);
}

static void porting_uart_tx_done(void)
{
	dbg_dump(0x34);
	dbg_dump(bulk_cid);
	dbg_dump(0x35);
}
#define porting_uart_rx_poll	porting_uart_none
#define porting_uart_rx_iocb	porting_uart_none
#define porting_uart_rx_done	porting_uart_none
#else
static void porting_uart_rx_poll(void)
{
	dbg_dump(0x30);
	bulk_request_submit(uart_bulk_rx(PORTING_UART_PORT),
			    PORTING_UART_SIZE);
	dbg_dump(0x31);
}

static void porting_uart_rx_iocb(void)
{
	size_t i;
	uint8_t val = 0;

	dbg_dump(0x32);
	for (i = 0; i < PORTING_UART_SIZE; i++) {
		BULK_READ_BEGIN(val) {
			dbg_dump(val);
		} BULK_READ_END
	}
	dbg_dump(0x33);
}

static void porting_uart_rx_done(void)
{
	dbg_dump(0x34);
	dbg_dump(bulk_cid);
	dbg_dump(0x35);
}

#define porting_uart_tx_poll	porting_uart_none
#define porting_uart_tx_iocb	porting_uart_none
#define porting_uart_tx_done	porting_uart_none
#endif

bulk_user_t porting_uart_tx = {
	porting_uart_tx_poll,
	porting_uart_tx_iocb,
	porting_uart_tx_done,
};

bulk_user_t porting_uart_rx = {
	porting_uart_rx_poll,
	porting_uart_rx_iocb,
	porting_uart_rx_done,
};

uart_user_t porting_uart = {
	UART_DEF_PARAMS,
	UART_DEF_BAUDRATE,
	NULL,
	NULL,
	0,
	0,
	&porting_uart_tx,
	&porting_uart_rx,
	porting_uart_sync,
	porting_uart_oob,
	1,
};

void porting_init(void)
{
	timer_init();
	bulk_init();
	uart_init();
	uart_startup(PORTING_UART_PORT, &porting_uart);
	dbg_dump(uart_bulk_tx(PORTING_UART_PORT));
	dbg_dump(uart_bulk_rx(PORTING_UART_PORT));
}
#endif
#endif

#ifdef CONFIG_PORTING_LOAD
uint8_t porting_byte = 0;
uint8_t porting_length = 0;
char porting_string[] = ".data sections is initialized correctly.";

void porting_handler(uint8_t event)
{
	dbg_dump((uint8_t)(porting_string[porting_byte]));
	porting_byte++;
	if (porting_byte > porting_length) porting_byte = 0;
	bh_resume(porting_bh);
}

void porting_init(void)
{
	porting_length = text_strlen(porting_string);
	porting_bh = bh_register_handler(porting_handler);
	bh_resume(porting_bh);
}
#endif

#if 0
void led_off(void)
{
	led_light_off(0);
}

void led_on(void)
{
	led_light_on(0, LED_COLOR_GREEN);
}
#endif

void system_init(void)
{
	board_init();
	gpio_init();
	debug_init();
	irq_init();
	bh_init();
	tick_init();
	/* omit delay_init() here for porting */
	/* omit timer_init() here for porting */
	/* omit heap_init() here for porting */
	bulk_init();
	porting_init();
	/* omit task_init() here for porting */

	irq_local_enable();

	while (1) {
#ifdef CONFIG_IDLE
		while (!bh_resumed_any()) {
			dbg_dump(0xAA);
			wait_irq();
		}
		dbg_dump(0xAB);
#endif
		bh_run_all();
	}
}
#endif
