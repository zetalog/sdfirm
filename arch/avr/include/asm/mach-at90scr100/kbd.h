#ifndef __KBD_AT90SCR100_H_INCLUDE__
#define __KBD_AT90SCR100_H_INCLUDE__

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

/* KEYBOARD */
#define KBF	_SFR_MEM8(0x8F)	/* Keyboard Flag Register */
#define KBE	_SFR_MEM8(0x8E)	/* Keyboard Input enable Register */
#define KBLS	_SFR_MEM8(0x8D)	/* Keyboard Level Selector */

/* Port A as Keyboard Input Line */
/* Port E as Keyboard Output Line */

/* PORT CONFIGURATION */
#define KBD_PIN_R			PINA
#define KBD_PIN_C			PINE
#define KBD_DDR_R			DDRA
#define KBD_DDR_C			DDRE
#define KBD_PORT_R			PORTA
#define KBD_PORT_C			PORTE
#define KBD_COLS			KBD_PIN_C
#define KBD_ROWS			KBD_PIN_R	

#ifdef CONFIG_KBD_AT90SCR100_ROW2_COL6
#define __KBD_HW_ROW_MAX		2
#define __KBD_HW_COL_MAX		6
#endif

#ifdef CONFIG_KBD_AT90SCR100_ROW4_COL4
#define __KBD_HW_ROW_MAX		4
#define __KBD_HW_COL_MAX		4
#endif

#define __KBD_HW_MAX_SCANS		(__KBD_HW_ROW_MAX * __KBD_HW_COL_MAX)

#if __KBD_HW_MAX_SCANS > 16
#error "Invalid KBD keyboard layout"
#endif

#define __kbd_hw_read_cols()		(KBD_COLS)
#define __kbd_hw_read_rows()		(KBD_ROWS)
#define __kbd_hw_write_row_ports(u8)	(KBD_PORT_R = u8)
#define __kbd_hw_write_col_ports(u8)	(KBD_PORT_C = u8)

/* driver implementations */
#ifdef CONFIG_KBD_AT90SCR100_MAX_ROWS
#define KBD_HW_ROW_KEYS			CONFIG_KBD_AT90SCR100_MAX_ROWS
#else
#define KBD_HW_ROW_KEYS			__KBD_HW_ROW_MAX
#endif
#ifdef CONFIG_KBD_AT90SCR100_MAX_COLS
#define KBD_HW_COL_KEYS			CONFIG_KBD_AT90SCR100_MAX_COLS
#else
#define KBD_HW_COL_KEYS			__KBD_HW_COL_MAX
#endif

#ifdef CONFIG_KBD_AT90SCR100_ROW_HIGH
#define __KBD_HW_ROW_OFFSET	CONFIG_KBD_AT90SCR100_ROW_OFFSET
#elif defined(CONFIG_KBD_AT90SCR100_ROW_LOW)
#define __KBD_HW_ROW_OFFSET	0
#endif

#ifdef CONFIG_KBD_AT90SCR100_COL_HIGH
#define __KBD_HW_COL_OFFSET	CONFIG_KBD_AT90SCR100_COL_OFFSET
#elif defined(CONFIG_KBD_AT90SCR100_COL_LOW)
#define __KBD_HW_COL_OFFSET	0
#endif

#define __KBD_HW_ROW_MASK	(((1<<__KBD_HW_ROW_MAX)-1)<<__KBD_HW_ROW_OFFSET)
#define __KBD_HW_COL_MASK	(((1<<__KBD_HW_COL_MAX)-1)<<__KBD_HW_COL_OFFSET)

void kbd_hw_ctrl_init(void);
void kbd_hw_scan_sync(uint8_t sec);
unsigned char kbd_hw_translate_key(uint8_t scancode);

#endif /* __KBD_AT90SCR100_H_INCLUDE__ */
