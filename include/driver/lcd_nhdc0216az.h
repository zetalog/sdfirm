#ifndef __LCD_NHDC0216AZ_H_INCLUDE__
#define __LCD_NHDC0216AZ_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>

#ifndef ARCH_HAVE_LCD
#define ARCH_HAVE_LCD			1
#else
#error "Multiple LCD controller defined"
#endif

struct lcd_nhdc0216az_pins {
	uint8_t lcd_db;
	uint16_t lcd_rs;
	uint16_t lcd_rw;
	uint16_t lcd_en;
	lcd_init_cb lcd_init;
};
__TEXT_TYPE__(const struct lcd_nhdc0216az_pins, lcd_nhdc0216az_pins_t);

/* RS/RW settings */
#define LCD_RS_INST			0
#define LCD_RS_DATA			1
#define LCD_RW_WRITE			0
#define LCD_RW_READ			1

#define LCD_INST_DISPLAY_CLEAR		0x01
#define LCD_INST_DISPLAY_CURSOR_HOME	0x02
#define LCD_INST_ENTRY_MODE_SET		0x04
#define LCD_INST_DISPLAY_ON_OFF		0x08
#define LCD_INST_DISPLAY_CURSOR_SHIFT	0x10
#define LCD_INST_FUNCTION_SET		0x20
#define LCD_INST_RAM_ADDRESS_SET	0x40
#define LCD_INST_DD_RAM_ADDRESS_SET	0x80

/* entry mode set */
#define LCD_INCREMENT			0x02
#define LCD_DECREMENT			0x00
#define LCD_SHIFT_ON			0x01
#define LCD_SHIFT_OFF			0x00
#define LCD_DISPLAY_ON			0x04
#define LCD_DISPLAY_OFF			0x00
#define LCD_CURSOR_ON			0x02
#define LCD_CURSOR_OFF			0x00
#define LCD_BLINKING_ON			0x01
#define LCD_BLINKING_OFF		0x00
#define LCD_SHIFT_DISPLAY		0x08
#define LCD_SHIFT_CURSOR		0x00
#define LCD_SHIFT_RIGHT			0x04
#define LCD_SHIFT_LEFT			0x00
#define LCD_DATA_8BIT			0x10
#define LCD_DATA_4BIT			0x00
#define LCD_LINE_DUAL			0x08
#define LCD_LINE_SIGNAL			0x00
#define LCD_DOTS_5X10			0x04
#define LCD_DOTS_5X8			0x00
#define LCD_MASK_BUSY_FLAG		0x80
#define LCD_MASK_LCD_ADDRESS_COUNTER	0x7F
/* 0x40 means the second line */
#define LCD_LINE_OFFSET			6

#ifdef CONFIG_LCD_NHD_C0216AZ_MAX_ROWS
#define NR_LCD_HW_ROWS		(uint8_t)CONFIG_LCD_NHD_C0216AZ_MAX_ROWS
#else
#define NR_LCD_HW_ROWS		(uint8_t)2
#endif
#ifdef CONFIG_LCD_NHD_C0216AZ_MAX_COLS
#define NR_LCD_HW_COLS		(uint8_t)CONFIG_LCD_NHD_C0216AZ_MAX_COLS
#else
#define NR_LCD_HW_COLS		(uint8_t)16
#endif

void __lcd_hw_write_ms(uint8_t rs, uint8_t db, uint8_t ms);
void __lcd_hw_write_us(uint8_t rs, uint8_t db, uint8_t us);
uint8_t __lcd_hw_read_us(uint8_t rs, uint8_t us);
#define __lcd_hw_write_inst_ms(inst, ms)	__lcd_hw_write_ms(LCD_RS_INST, inst, ms)
#define __lcd_hw_write_inst_us(inst, us)	__lcd_hw_write_us(LCD_RS_INST, inst, us)
#define __lcd_hw_read_inst()			__lcd_hw_read_us(LCD_RS_INST, 1)

#define __lcd_hw_display_clear()		\
	__lcd_hw_write_inst_ms(LCD_INST_DISPLAY_CLEAR, 2)
#define __lcd_hw_display_cursor_home()		\
	__lcd_hw_write_inst_ms(LCD_INST_DISPLAY_CURSOR_HOME, 2)
#define __lcd_hw_entry_mode_set(id, s)		\
	__lcd_hw_write_inst_us(LCD_INST_ENTRY_MODE_SET | id | s, 40)
#define __lcd_hw_display_onoff(d, c, b)		\
	__lcd_hw_write_inst_us(LCD_INST_DISPLAY_ON_OFF | d | c | b, 40)
#define __lcd_hw_display_cursor_shift(sc, rl)	\
	__lcd_hw_write_inst_us(LCD_INST_DISPLAY_CURSOR_SHIFT | sc | rl, 40)
#define __lcd_hw_display_shift(rl)		\
	__lcd_hw_display_cursor_shift(LCD_SHIFT_DISPLAY, rl)
#define __lcd_hw_cursor_shift(rl)		\
	__lcd_hw_display_cursor_shift(LCD_SHIFT_CURSOR, rl)
#define __lcd_hw_function_set(dl, n, f)		\
	__lcd_hw_write_inst_us(LCD_INST_FUNCTION_SET | dl | n | f, 40)
#define __lcd_hw_busy_flag()			\
	__lcd_hw_read_inst() & LCD_MASK_BUSY_FLAG
#define __lcd_hw_address_counter()		\
	__lcd_hw_read_inst() & LCD_MASK_LCD_ADDRESS_COUNTER
#define __lcd_hw_dd_ram_address_set(add)	\
	__lcd_hw_write_inst_us(LCD_INST_DD_RAM_ADDRESS_SET | add, 40)

void lcd_hw_set_pos(uint8_t pos);
#define lcd_hw_write_data(byte)		__lcd_hw_write_us(LCD_RS_DATA, byte, 40)
#define lcd_hw_read_data()		__lcd_hw_read_us(LCD_RS_DATA, 40)
void lcd_hw_ctrl_init(void);

void lcd_nhdc0216az_register(lcd_nhdc0216az_pins_t *pins);

#endif /* __LCD_NHD_C0216AZ_H_INCLUDE__ */
