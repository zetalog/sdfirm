#include <target/lcd.h>

#define INVALID_POS		NR_LCD_CHARS
#define HOME_POS		0x00

uint8_t lcd_cursor = HOME_POS;

static uint8_t lcd_get_row(uint8_t pos);
static uint8_t lcd_get_col(uint8_t pos);

static uint8_t lcd_get_row(uint8_t pos)
{
	return pos / MAX_LCD_COLS;
}

static uint8_t lcd_get_col(uint8_t pos)
{
	return pos % MAX_LCD_COLS;
}

static void lcd_reset_cursor(uint8_t pos)
{
	if (pos < INVALID_POS)
		lcd_cursor = (pos);
	if (lcd_cursor >= INVALID_POS)
		lcd_cursor = HOME_POS;
	lcd_hw_set_pos(lcd_cursor);
}

static void lcd_advance_cursor(void)
{
	lcd_reset_cursor(++lcd_cursor);
	lcd_hw_set_pos(lcd_cursor);
}

void lcd_draw_point(unsigned char ch, unsigned int x, unsigned int y)
{
	uint8_t pos;
	
	if (y >= MAX_LCD_ROWS)
		y = lcd_get_row(lcd_cursor);
	if (x >= MAX_LCD_COLS)
		x = lcd_get_col(lcd_cursor);

	pos = (uint8_t)y * MAX_LCD_COLS + (uint8_t)x;

	lcd_reset_cursor(pos);
	lcd_hw_write_data(ch);
	lcd_advance_cursor();
}

void lcd_draw_row(text_char_t *str, unsigned int line)
{
	uint8_t pos;

	if (line >= MAX_LCD_ROWS)
		line = 0;

	pos = (uint8_t)line*MAX_LCD_COLS;
	lcd_reset_cursor(pos);
	for (; pos < (uint8_t)(line+1)*MAX_LCD_COLS; pos++) {
		if (*str == 0)
			lcd_putchar(' ');
		else
			lcd_putchar(*str++);
	}
}

void lcd_putchar(unsigned char ch)
{
	lcd_draw_point(ch, MAX_LCD_COLS, MAX_LCD_ROWS);
}

/* lcd initialize */
void lcd_init(void)
{
	DEVICE_FUNC(DEVICE_FUNC_LCD);
	lcd_hw_ctrl_init();
}
