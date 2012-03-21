#include <target/arch.h>
#include <target/config.h>
#include <target/generic.h>
#include <target/lcd.h>
#include <target/delay.h>
#include <target/gpio.h>

lcd_nhdc0216az_pins_t *__lcd_hw_pins;

void lcd_nhdc0216az_register(lcd_nhdc0216az_pins_t *pins)
{
	__lcd_hw_pins = pins;
}

static void __lcd_hw_init(void)
{
	lcd_init_cb cb = __lcd_hw_pins->lcd_init;
	BUG_ON(!cb);
	cb();
}

static void __lcd_hw_write_rs(uint8_t rs)
{
	uint16_t pin;
	pin = __lcd_hw_pins->lcd_rs;
	gpio_write_pin(HIBYTE(pin), LOBYTE(pin), rs);
}

static void __lcd_hw_write_rw(uint8_t rw)
{
	uint16_t pin;
	pin = __lcd_hw_pins->lcd_rw;
	gpio_write_pin(HIBYTE(pin), LOBYTE(pin), rw);
}

static void __lcd_hw_write_en(uint8_t en)
{
	uint16_t pin;
	pin = __lcd_hw_pins->lcd_en;
	gpio_write_pin(HIBYTE(pin), LOBYTE(pin), en);
}

static void __lcd_hw_write(uint8_t rs, uint8_t db)
{
	gpio_write_port(__lcd_hw_pins->lcd_db, 0xFF, db);
	__lcd_hw_write_rs(rs);
	__lcd_hw_write_rw(LCD_RW_WRITE);
	/* ensure following timing
	 * RS, R/W Setup Time:
	 *   Tas >= 60ns (NT7605)
	 *   Tsu1 >= 100ns (NHDC0216AZ)
	 */
	mdelay(1);
	__lcd_hw_write_en(1);
	db = gpio_read_port(__lcd_hw_pins->lcd_db, 0xFF);
	/* ensure following timing
	 * Enable "H" Level Pulse Width
	 *   Twhe >= 300ns (NT7605)
	 *   Tw >= 300ns (NHDC0216AZ)
	 * Read Data Output Delay will be
	 *   Tds >= 150ns
	 *   Tsu2 >= 60ns (NHDC0216AZ)
	 */
	mdelay(1);
	__lcd_hw_write_en(0);
	/* ensure following timing
	 * Enable Cycle Time
	 *   Tcycle >= 500ns (NT7605)
	 *   Tc >= 500ns (NHDC0216AZ)
	 * RS, R/W Address Hold Time will be
	 *   Tah >= 10ns (NT7605)
	 *   Th1 >= 10ns (NHDC0216AZ)
	 * Data Hold Time will be
	 *   Tdhw >= 10ns (NT7605)
	 *   Th2 >= 10ns (NHDC0216AZ)
	 */
	mdelay(1);
}

void __lcd_hw_write_ms(uint8_t rs, uint8_t db, uint8_t ms)
{
	__lcd_hw_write(rs, db);
	mdelay(ms);
	while (__lcd_hw_busy_flag());
}

void __lcd_hw_write_us(uint8_t rs, uint8_t db, uint8_t us)
{
	__lcd_hw_write(rs, db);
	udelay(us);
	while (__lcd_hw_busy_flag());
}

uint8_t __lcd_hw_read_us(uint8_t rs, uint8_t us)
{
	uint8_t db;

	__lcd_hw_write_rs(rs);
	__lcd_hw_write_rw(LCD_RW_READ);
	/* ensure following timing
	 * RS, R/W Setup Time:
	 *   Tas >= 60ns (NT7605)
	 *   Tsu1 >= 100ns (NHDC0216AZ)
	 */
	udelay(1);
	__lcd_hw_write_en(1);
	db = gpio_read_port(__lcd_hw_pins->lcd_db, 0xFF);
	/* ensure following timing
	 * Enable "H" Level Pulse Width
	 *   Twhe >= 300ns (NT7605)
	 *   Tw >= 300ns (NHDC0216AZ)
	 * Read Data Output Delay will be
	 *   Trd <= 190ns
	 *   Td >= 60ns (NHDC0216AZ)
	 */
	udelay(1);
	__lcd_hw_write_en(0);
	/* ensure following timing
	 * Enable Cycle Time
	 *   Tcycle >= 500ns (NT7605)
	 *   Tc >= 500ns (NHDC0216AZ)
	 * RS, R/W Address Hold Time will be
	 *   Tah >= 10ns (NT7605)
	 *   Th >= 10ns (NHDC0216AZ)
	 * Data Hold Time will be
	 *   Tdhr >= 10ns (NT7605)
	 *   Tdh >= 20ns (NHDC0216AZ)
	 */
	udelay(1);
	udelay(us);
	return db;
}

static void __lcd_hw_set_screen(uint8_t line, uint8_t offset)
{
	if (line >= MAX_LCD_ROWS)
		return;
	__lcd_hw_dd_ram_address_set((line << LCD_LINE_OFFSET) | offset);
}

void lcd_hw_set_pos(uint8_t pos)
{
	__lcd_hw_set_screen(pos / MAX_LCD_COLS,
			    pos % MAX_LCD_COLS);	
}

void lcd_hw_ctrl_init(void)
{
	__lcd_hw_init();
	mdelay(30);
	__lcd_hw_function_set(LCD_DATA_8BIT,
			      LCD_LINE_DUAL,
			      LCD_DOTS_5X8);
	__lcd_hw_display_onoff(LCD_DISPLAY_ON,
			       LCD_CURSOR_OFF,
			       LCD_BLINKING_OFF);
	__lcd_hw_display_clear();
	__lcd_hw_entry_mode_set(LCD_INCREMENT,
				LCD_SHIFT_OFF);
	__lcd_hw_cursor_shift(LCD_SHIFT_RIGHT);
	__lcd_hw_display_cursor_home();
}
