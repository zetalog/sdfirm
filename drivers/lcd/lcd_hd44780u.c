#include <target/config.h>
#include <target/generic.h>
#include <target/lcd.h>
#include <target/delay.h>

/* LCD write command without wait busy flag */
static void __lcd_hw_write_cmd(uint8_t cmd)
{
	LCD_DB = (cmd);
	LCD_RS = MSK_RS_CMD;
	LCD_RW = MSK_RW_WRITE;
	LCD_EN = HIGH;
	LCD_EN = LOW;
	mdelay(1);
}

static void __lcd_hw_set_screen(uint8_t line, uint8_t offset)
{
	if (line >= MAX_LCD_ROWS)
		return;
	__lcd_hw_write_cmd(LCD_IC_SD_BASE | (line << LCD_LINE_OFFSET) | offset);
}

void lcd_hw_set_pos(uint8_t pos)
{
	__lcd_hw_set_screen((pos) / MAX_LCD_COLS, pos % MAX_LCD_COLS);
}

void lcd_hw_write_data(unsigned char ch)
{
	LCD_DB = ch;
	LCD_RS = MSK_RS_DATA;
	LCD_RW = MSK_RW_WRITE;
	LCD_EN = HIGH;
	LCD_EN = LOW;
	mdelay(1);
}

static void __lcd_hw_clear_display(void)
{
	__lcd_hw_write_cmd(LCD_CLEAR_DISPLAY);
	mdelay(1);
}

static void __lcd_hw_return_home(void)
{
	__lcd_hw_write_cmd(LCD_RETURN_HOME);
	mdelay(1);
}

/* hardware operation
 * function names with hw is the hardware control of lcd
 */
void lcd_hw_ctrl_init(void)
{
	__lcd_hw_write_cmd(LCD_INIT_CMD);
	mdelay(5);
	__lcd_hw_write_cmd(LCD_INIT_CMD);
	mdelay(1);
	/* FUNCTION SET: 8-bit interface, 2 lines display */
	__lcd_hw_write_cmd(LCD_IC_FS_BASE | MSK_LCD_8_BITS | MSK_LCD_2_LINES);
	__lcd_hw_write_cmd(LCD_IC_DOO_BASE & ~MSK_LCD_DISPLAY_ON);
	__lcd_hw_clear_display();
	__lcd_hw_write_cmd(LCD_IC_EM_BASE | MSK_LCD_INC);
	__lcd_hw_write_cmd(LCD_IC_DOO_BASE | MSK_LCD_DISPLAY_ON);
	__lcd_hw_write_cmd(LCD_IC_SHIFT_BASE | MSK_LCD_SHIFT_RIGHT);
	__lcd_hw_return_home();
}
