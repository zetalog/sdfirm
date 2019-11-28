#ifndef __LCD_H_INCLUDE__
#define __LCD_H_INCLUDE__

#include <target/generic.h>

typedef void (*lcd_init_cb)(void);

#include <driver/lcd.h>

#define MAX_LCD_ROWS		(uint8_t)NR_LCD_HW_ROWS
#define MAX_LCD_COLS		(uint8_t)NR_LCD_HW_COLS
#define NR_LCD_CHARS		(uint8_t)(MAX_LCD_ROWS * MAX_LCD_COLS)

#ifdef CONFIG_LCD
void lcd_putchar(unsigned char ch);
void lcd_draw_point(unsigned char ch, unsigned int x, unsigned int y);
void lcd_draw_row(text_char_t *str, unsigned int line);
#else
#define lcd_putchar(ch)
#define lcd_draw_point(ch, x, y)
#define lcd_draw_row(str, line)
#endif

#endif /* __LCD_H_INCLUDE__ */
