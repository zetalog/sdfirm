#ifndef __LCD_DRIVER_H_INCLUDE__
#define __LCD_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_LCD_HD44780U
#include <driver/lcd_hd44780u.h>
#endif
#ifdef CONFIG_LCD_PCF2119X
#include <driver/lcd_pcf2119x.h>
#endif
#ifdef CONFIG_LCD_NHDC0216AZ
#include <driver/lcd_nhdc0216az.h>
#endif
#ifdef CONFIG_LCD_NHDC0216CIZ
#include <driver/lcd_nhdc0216ciz.h>
#endif

#ifndef ARCH_HAVE_LCD
#define NR_LCD_HW_ROWS		0
#define NR_LCD_HW_COLS		0
#define lcd_hw_set_pos(pos)
#define lcd_hw_write_data(ch)
#define lcd_hw_ctrl_init()
#endif

#endif /* __LCD_DRIVER_H_INCLUDE__ */
