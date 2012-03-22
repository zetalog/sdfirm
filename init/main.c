#include <target/config.h>
#include <target/generic.h>
#include <target/arch.h>
#include <target/irq.h>
#include <target/state.h>

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
extern void state_init(void);
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

#ifndef CONFIG_PORTING
void system_init(void)
{
	main_debug(MAIN_DEBUG_INIT, 0);
	board_init();
	irq_init();
	state_init();
	tick_init();
	delay_init();
	bulk_init();

	modules_init();
	appl_init();
	irq_local_enable();
	main_debug(MAIN_DEBUG_INIT, 1);

	while (1) {
		state_run_all();
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
#include <target/state.h>
#include <target/task.h>

sid_t porting_sid = INVALID_SID;
tid_t porting_tid = INVALID_TID;

#ifdef CONFIG_UART
extern void uart_init(void);
#else
#define uart_init()
#endif

#ifdef CONFIG_LED
extern void led_init(void);
#else
#define led_init()
#endif

#ifdef CONFIG_PORTING_LED
led_no_t porting_led_light;
uint8_t porting_led_count;

#ifdef CONFIG_PORTING_LED_TIMER
static void flash_restart_timer(void)
{
	timer_schedule_shot(porting_tid, 125);
}

static void flash_start_timer(void)
{
	timer_init();
	porting_tid = timer_register(porting_sid, TIMER_DELAYABLE);
	timer_schedule_shot(porting_tid, 0);
}
#else
static void flash_restart_timer(void)
{
	mdelay(250);
	state_wakeup(porting_sid);
}

void flash_start_timer(void)
{
	delay_init();
}
#endif

void porting_led_run(void)
{
	porting_led_count++;
	if (porting_led_count & 0x01)
		led_light_on(porting_led_light, 0);
	else
		led_light_off(porting_led_light);
	flash_restart_timer();
}

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
	led_init();
	porting_sid = state_register(porting_handler);
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

	uart_putchar(HIBYTE(porting_heap_space));
	uart_putchar(LOBYTE(porting_heap_space));

	mem = (uint32_t)heap_alloc(porting_heap_space);
	mem2 = (uint32_t)heap_alloc(porting_heap_space);

	uart_putchar(HIBYTE(HIWORD(mem)));
	uart_putchar(LOBYTE(HIWORD(mem)));
	uart_putchar(HIBYTE(LOWORD(mem)));
	uart_putchar(LOBYTE(LOWORD(mem)));

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
	state_wakeup(porting_sid);
}

void porting_init(void)
{
	heap_init();
	BUG_ON(CONFIG_HEAP_SIZE <= PORTING_HEAP_UNIT);
	BUG_ON(CONFIG_HEAP_SIZE & (PORTING_HEAP_UNIT-1));
	porting_sid = state_register(porting_handler);
	state_wakeup(porting_sid);
}
#endif

#ifdef CONFIG_PORTING_TIMER
uint8_t porting_byte = 0;
uint8_t porting_ticks = 0;

void porting_handler(uint8_t event)
{
#ifdef CONFIG_TIMER_16BIT
	uart_putchar(porting_byte++);
	timer_schedule_shot(porting_tid, 1000);
#else
	if (porting_ticks == 0) {
		uart_putchar(porting_byte++);
	}
	porting_ticks++;
	porting_ticks &= (8-1);
	timer_schedule_shot(porting_tid, 125);
#endif
}

void porting_init(void)
{
	timer_init();
	porting_sid = state_register(porting_handler);
	porting_tid = timer_register(porting_sid, TIMER_DELAYABLE);
	timer_schedule_shot(porting_tid, 0);
}
#endif

#ifdef CONFIG_PORTING_DELAY
void porting_handler(uint8_t event)
{
	mdelay(250);
	mdelay(250);
	mdelay(250);
	mdelay(250);
#ifdef CONFIG_LPS_32BITS
	uart_putchar(HIBYTE(HIWORD(loops_per_ms)));
	uart_putchar(LOBYTE(HIWORD(loops_per_ms)));
#endif
	uart_putchar(HIBYTE(loops_per_ms));
	uart_putchar(LOBYTE(loops_per_ms));
	state_wakeup(porting_sid);
}

void porting_init(void)
{
	delay_init();
	porting_sid = state_register(porting_handler);
	state_wakeup(porting_sid);
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
		uart_putchar(spi_read_byte());
	}
	spi_deselect_device();
	state_wakeup(porting_sid);
}

void porting_init(void)
{
	spi_init();
	porting_spi = spi_register_device(&porting_spi_device);
	porting_sid = state_register(porting_handler);
	state_wakeup(porting_sid);
}
#endif

#ifdef CONFIG_PORTING_GPT
uint8_t porting_byte = 0;
uint8_t porting_ticks = 0;

void tick_handler(void)
{
	if (!porting_ticks) {
		uart_putchar(porting_byte++);
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
	porting_sid = state_register(porting_handler);
}
#endif

#ifdef CONFIG_PORTING_TSC
void porting_handler(uint8_t event)
{
	tsc_count_t counter = tsc_read_counter();

#if TSC_MAX > 65535
	uart_putchar(HIBYTE(HIWORD(counter)));
	uart_putchar(LOBYTE(HIWORD(counter)));
#endif
#if TSC_MAX > 255
	uart_putchar(HIBYTE(counter));
#endif
	uart_putchar(LOBYTE(counter));
	state_wakeup(porting_sid);
}

void porting_init(void)
{
	tsc_hw_ctrl_init();
	porting_sid = state_register(porting_handler);
	state_wakeup(porting_sid);
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
		uart_putchar(0x00);
		task_schedule();
		uart_putchar(0x01);
		task_schedule();
		uart_putchar(0x02);
		task_schedule();
		uart_putchar(0x03);
		task_schedule();
		uart_putchar(0x04);
		task_schedule();
		uart_putchar(0x05);
		task_schedule();
		uart_putchar(0x06);
		task_schedule();
		uart_putchar(0x07);
		task_schedule();
	}
}

static void task1(void *priv)
{

	while (!pfgs[1]) {
		uart_putchar(0x10);
		task_schedule();
		uart_putchar(0x11);
		task_schedule();
		uart_putchar(0x12);
		task_schedule();
		uart_putchar(0x13);
		task_schedule();
		uart_putchar(0x14);
		task_schedule();
		uart_putchar(0x15);
		task_schedule();
		uart_putchar(0x16);
		task_schedule();
		uart_putchar(0x17);
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
#define PORTING_GPIO_PORT	CONFIG_PORTING_GPIO_PORT
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

#ifdef CONFIG_PORTING_GPIO_OUT
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
		uart_putchar(i);
	}
}

void porting_init(void)
{
	porting_sid = state_register(porting_handler);
	delay_init();
	state_wakeup(porting_sid);
}
#endif

#ifdef CONFIG_PORTING_UART
/* XXX: UART Metering
 * uart_putchar will be forced an output of 0x55 that can be easily
 * observed by the oscilloscope to measure the baudrate.  Frequency
 * displayed by the oscilloscope will be a half of the baudrate/10.
 * The frequency captured by the oscilloscope "osc_freq" and the uart
 * baudrate "uart_baud" could have following relationship:
 * osc_freq = uart_baud/20
 */
#define UART_METER_BAUD_IS_20FREQ	0x55
#define UART_METER_BAUD_IS_40FREQ	0x99
#define UART_METER			UART_METER_BAUD_IS_20FREQ
void porting_handler(uint8_t event)
{
	while (1)
		uart_putchar(UART_METER);
}

void porting_init(void)
{
	porting_sid = state_register(porting_handler);
	state_wakeup(porting_sid);
}
#endif

#ifdef CONFIG_PORTING_LOAD
uint8_t porting_byte = 0;
uint8_t porting_length = 0;
char porting_string[] = ".data sections is initialized correctly.";

void porting_handler(uint8_t event)
{
	uart_putchar((uint8_t)(porting_string[porting_byte]));
	porting_byte++;
	if (porting_byte > porting_length) porting_byte = 0;
	state_wakeup(porting_sid);
}

void porting_init(void)
{
	porting_length = text_strlen(porting_string);
	porting_sid = state_register(porting_handler);
	state_wakeup(porting_sid);
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

#ifdef CONFIG_GPIO
extern void gpio_init(void);
#else
#define gpio_init()
#endif

void system_init(void)
{
	board_init();
	irq_init();
	state_init();
	tick_init();
	gpio_init();
	/* omit delay_init() here for porting */
	uart_init();
	/* omit heap_init() here for porting */
	/* omit timer_init() here for porting */
	/* omit task_init() here for porting */
	porting_init();
	irq_local_enable();

	while (1) {
		state_run_all();
	}
}
#endif
