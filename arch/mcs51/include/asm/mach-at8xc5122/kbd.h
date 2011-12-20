#ifndef __KBD_ATC8X5122_H_INCLUDE__
#define __KBD_ATC8X5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/gpio.h>
#include <asm/mach/irq.h>

#ifndef ARCH_HAVE_KBD
#define ARCH_HAVE_KBD			1
#else
#error "Multiple KBD controller defined"
#endif

Sfr(KBF,	0x9E);		/* Keyboard Flag Register */
Sfr(KBE,	0x9D);		/* Keyboard Input enable Register */
Sfr(KBLS,	0x9C);		/* Keyboard Level Selector */
#define KBD_PORT			P5

#ifdef CONFIG_KBD_AT8XC5122_ROW4_COL4
#define __KBD_HW_ROW_MAX		4
#define __KBD_HW_COL_MAX		4
#endif
#ifdef CONFIG_KBD_AT8XC5122_ROW2_COL6
#define __KBD_HW_ROW_MAX		2
#define __KBD_HW_COL_MAX		6
#endif
#ifdef CONFIG_KBD_AT8XC5122_ROW6_COL2
#define __KBD_HW_ROW_MAX		6
#define __KBD_HW_COL_MAX		2
#endif

#ifdef CONFIG_KBD_AT8XC5122_ROW_INV
#define __kbd_hw_row_inv(row)	(__KBD_HW_ROW_MAX-row-1)
#else
#define __kbd_hw_row_inv(row)	(row)
#endif
#ifdef CONFIG_KBD_AT8XC5122_COL_INV
#define __kbd_hw_col_inv(col)	(__KBD_HW_COL_MAX-col-1)
#else
#define __kbd_hw_col_inv(col)	(col)
#endif

#ifdef CONFIG_KBD_AT8XC5122_ROW_HIGH
#define __KBD_HW_COL_MASK	((1<<__KBD_HW_COL_MAX)-1)
#define __KBD_HW_ROW_MASK	(((1<<__KBD_HW_ROW_MAX)-1)<<__KBD_HW_COL_MAX)
#define __kbd_hw_row_mask(row)	(1<<(__kbd_hw_row_inv(row)+__KBD_HW_COL_MAX))
#define __kbd_hw_col_mask(col)	(1<<__kbd_hw_col_inv(col))
#endif
#ifdef CONFIG_KBD_AT8XC5122_ROW_LOW
#define __KBD_HW_COL_MASK	(((1<<__KBD_HW_COL_MAX)-1)<<__KBD_HW_ROW_MAX)
#define __KBD_HW_ROW_MASK	((1<<__KBD_HW_ROW_MAX)-1)
#define __kbd_hw_row_mask(row)	(1<<__kbd_hw_row_inv(row))
#define __kbd_hw_col_mask(row)	(1<<(__kbd_hw_col_inv(col)+__KBD_HW_ROW_MAX))
#endif

#define __KBD_HW_MAX_SCANS		(__KBD_HW_ROW_MAX * __KBD_HW_COL_MAX)
#define __kbd_hw_write_ports(u8)	(KBD_PORT = u8)
#define __kbd_hw_read_cols()		(KBD_PORT & __KBD_HW_COL_MASK)

/* driver implementations */
#ifdef CONFIG_KBD_AT8XC5122_MAX_ROWS
#define KBD_HW_ROW_KEYS			CONFIG_KBD_AT8XC5122_MAX_ROWS
#else
#define KBD_HW_ROW_KEYS			__KBD_HW_ROW_MAX
#endif
#ifdef CONFIG_KBD_AT8XC5122_MAX_COLS
#define KBD_HW_COL_KEYS			CONFIG_KBD_AT8XC5122_MAX_COLS
#else
#define KBD_HW_COL_KEYS			__KBD_HW_COL_MAX
#endif

void kbd_hw_ctrl_init(void);
void kbd_hw_scan_sync(uint8_t sec);
unsigned char kbd_hw_translate_key(uint8_t scancode);

#endif /* __KBD_ATC8X5122_H_INCLUDE__ */
