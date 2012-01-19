#ifndef __LCD_HD44780U_H_INCLUDE__
#define __LCD_HD44780U_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>

#ifndef ARCH_HAVE_LCD
#define ARCH_HAVE_LCD			1
#else
#error "Multiple LCD controller defined"
#endif

#ifdef CONFIG_ARCH_AT8XC5122
#include <asm/mach-at8xc5122/gpio.h>
/* PIN MAP */
#define LCD_RS				P2_0	/* Pin 4 */
#define LCD_RW				P2_1	/* Pin 5 */
#define LCD_EN				P2_2	/* Pin 6 */
#define LCD_DB				P0	/* Pin7 - Pin14 DB0~DB7 */
#define LCD_LED_K			P3_7	/* Pin 16, backlight led's cathode */
#define LCD_BF				P0_7
#endif

#define LCD_BUSY			0x01

/* COMMON INSTRUCTION SETS PARA GROUP */
#define MSK_LCD_BUSY			0x80
/* clear display */
#define LCD_IC_CS_BASE			0x01
/* return home */
#define LCD_IC_RH_BASE			0x02
/* entry mode: cursor moving direction, enable entire shift display */
#define LCD_IC_EM_BASE			0x04
#define MSK_LCD_INC			0x02
#define MSK_LCD_ENTIRE_SHIFT		0x01
/* display on or off: display, cursor, cursor blinking */
#define LCD_IC_DOO_BASE			0x08
#define MSK_LCD_DISPLAY_ON		0x04
#define MSK_LCD_CURSOR_ON		0x02
#define MSK_LCD_CURSOR_BLINKING		0x01
/* cursor or display shift */
#define LCD_IC_SHIFT_BASE		0x10
#define MSK_LCD_SHIFT_DISPLAY		0x08
#define MSK_LCD_SHIFT_RIGHT		0x04
/* function set: BITS, LINES, FONTS */
#define LCD_IC_FS_BASE			0x20
#define MSK_LCD_8_BITS			0x10
#define MSK_LCD_2_LINES			0x08
#define MSK_LCD_BIG_FONT		0x04
/* set cgram address [6bits] */
#define LCD_IC_SC_BASE			0x40
/* set ddram address [7bits] */
#define LCD_IC_SD_BASE			0x80
/* 0x40 means the second line */
#define LCD_LINE_OFFSET			6

#define LCD_INIT_CMD			0x38
#define LCD_CLEAR_DISPLAY		0x01
#define LCD_RETURN_HOME			0x02

#define MSK_RS_CMD			0x00
#define MSK_RS_DATA			0x01
#define MSK_RW_WRITE			0x00
#define MSK_RW_READ			0x01

#ifdef CONFIG_LCD_HD44780U_MAX_ROWS
#define NR_LCD_HW_ROWS		(uint8_t)CONFIG_LCD_HD44780U_MAX_ROWS
#else
#define NR_LCD_HW_ROWS		(uint8_t)2
#endif
#ifdef CONFIG_LCD_HD44780U_MAX_COLS
#define NR_LCD_HW_COLS		(uint8_t)CONFIG_LCD_HD44780U_MAX_COLS
#else
#define NR_LCD_HW_COLS		(uint8_t)16
#endif

void lcd_hw_set_pos(uint8_t pos);
void lcd_hw_write_data(unsigned char ch);
void lcd_hw_ctrl_init(void);

#endif /* __LCD_HD44780U_H_INCLUDE__ */
