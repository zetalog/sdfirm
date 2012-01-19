#include <target/arch.h>
#include <target/kbd.h>
#include <target/delay.h>
#include <target/irq.h>

static void __kbd_hw_reset_port(void);

uint16_t __kbd_hw_last_status = 0x00;

/* XXX: Keyboard Layout
 * You may edit following codes to make it suitable for your application.
 */
#if __KBD_HW_MAX_SCANS == 16
static text_byte_t __kbd_hw_ascii_keys[__KBD_HW_MAX_SCANS] = {
	0x1B,	0x30,	0x0D,	'+',
	0x31,	0x32,	0x33,	'-',
	0x34,	0x35,	0x36,	'*',
	0x37,	0x38,	0x39,	'%',
};
#endif

#if __KBD_HW_MAX_SCANS == 12
static text_byte_t __kbd_hw_ascii_keys[__KBD_HW_MAX_SCANS] = {
	/* 0 1 2 3 4 ESC
	 * 5 6 7 8 9 ENT
	 */
	0x30,	0x31,	0x32,	0x33,	0x34,	0x1B,
	0x35,	0x36,	0x37,	0x38,	0x39,	0x0D,
};
#endif

unsigned char kbd_hw_translate_key(uint8_t scancode)
{
	return __kbd_hw_ascii_keys[scancode];
}

/* translate the keycode into scancode(key index) */
static uint8_t __kbd_hw_translate_scan(uint8_t row, uint8_t col)
{
	return row * __KBD_HW_COL_MAX + col;
}

static uint16_t __kbd_hw_get_status(void)
{
	uint8_t level, row, col, scancode;
	uint16_t status = 0x00;

	for (col = 0; col < NR_KEY_COLS; col++) {
		level = 0xFF;
		level &= ~(1 << col);
		__kbd_hw_write_row_ports(__KBD_HW_ROW_MASK);
		__kbd_hw_write_col_ports(level);
		nop();
		level = __kbd_hw_read_rows();

		for (row = 0; row < NR_KEY_ROWS; row++) {
			scancode = __kbd_hw_translate_scan(row, col);
			if (level & (1 << row))
				status &= ~(1 << scancode);
			else
				status |= (1 << scancode);
		}
	}

	__kbd_hw_reset_port();
	return status;
}

static void __kbd_hw_handle_status(uint16_t status)
{
	uint8_t scancode;
	uint16_t flag;

	for (scancode = 0; scancode < __KBD_HW_MAX_SCANS; scancode++) {
		flag = 1<<scancode;
		if (bits_raised_any(status, flag)) {
			if (!bits_raised_any(__kbd_hw_last_status, flag)) {
				kbd_event_down(scancode);
				raise_bits(__kbd_hw_last_status, flag);
			}
		} else {
			if (bits_raised_any(__kbd_hw_last_status, flag)) {
				kbd_event_up(scancode);
				unraise_bits(__kbd_hw_last_status, flag);
			}
		}
	}
}

static void __kbd_hw_unraise_flags(void)
{
	KBF = 0;
}

static void __kbd_hw_config_detect(void)
{
	uint8_t row, kbls = ~__KBD_HW_ROW_MASK;
	
	for (row = 0; row < __KBD_HW_ROW_MAX; row++) {
		if (__kbd_hw_last_status &
		    (((1<<__KBD_HW_COL_MAX)-1) << (row*__KBD_HW_COL_MAX)))
			raise_bits(kbls, 1 << row);
	}

	__kbd_hw_unraise_flags();
	KBLS = kbls;
	KBE = __KBD_HW_ROW_MASK;
}

static void __kbd_hw_scan_async(void)
{
	uint16_t status;

	status = __kbd_hw_get_status();
	/* bouncing filter according to data sheet */
	mdelay(10);
	status &= __kbd_hw_get_status();
	__kbd_hw_handle_status(status);
	__kbd_hw_config_detect();
}

void kbd_hw_scan_sync(uint8_t sec)
{
	uint8_t i, j;

	for (i = 0; i < sec; i++) {
		for (j = 0; j < 100; j++) {
			__kbd_hw_scan_async();
		}
	}
}

static void __kbd_hw_reset_port(void)
{
	__kbd_hw_write_row_ports(__KBD_HW_ROW_MASK); /* pullup */
	__kbd_hw_write_col_ports(~__KBD_HW_COL_MASK); /* output low */
}

static void __kbd_hw_config_port(void)
{
	KBD_DDR_R = ~__KBD_HW_ROW_MASK;	/* row as input */
	KBD_DDR_C = __KBD_HW_COL_MASK;	/* col as output */
	__kbd_hw_reset_port();
}

DEFINE_ISR(IRQ_KBD)
{
	__kbd_hw_scan_async();
}

void kbd_hw_ctrl_init(void)
{
	__kbd_hw_config_port();
	__kbd_hw_config_detect();
	irq_register_vector(IRQ_KBD, IRQ_KBD_isr);
}
