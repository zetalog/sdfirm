#ifndef __LCD_PCF2119X_H_INCLUDE__
#define __LCD_PCF2119X_H_INCLUDE__

#include <target/config.h>
#include <target/i2c.h>

#ifndef ARCH_HAVE_LCD
#define ARCH_HAVE_LCD			1
#else
#error "Multiple LCD controller defined"
#endif

#ifdef CONFIG_LCD_PCF2119X_DISP_LINE
#define NR_LCD_HW_ROWS		(uint8_t)CONFIG_LCD_PCF2119X_DISP_LINE
#else
#define NR_LCD_HW_ROWS		(uint16_t)2
#endif
#define NR_LCD_HW_COLS		(uint8_t)16

#ifdef CONFIG_LCD_PCF2119X_MUL_MODE32
#define PCF2119X_INC_FUNC_SL	0	/* 2*16 / 1*32 */
#else
#define PCF2119X_INC_FUNC_SL	1	/* 1*16 */
#endif
#ifdef CONFIG_LCD_PCF2119X_DISP_LINE
#define PCF2119X_INC_FUNC_M		(uint8_t)(CONFIG_LCD_PCF2119X_DISP_LINE-1)
#endif
#ifdef CONFIG_LCD_PCF2119X_DATA_LENGTH_8BITS
#define PCF2119X_INC_FUNC_DL		CONFIG_LCD_PCF2119X_DATA_LENGTH_8BITS
#endif

#define PCF2119X_CO_OFFSET	7	
#define PCF2119X_CO_CONT	(1 << PCF2119X_CO_OFFSET)	/* not last control byte */
#define PCF2119X_CO_LAST	(0 << PCF2119X_CO_OFFSET)	/* last control byte */

#define PCF2119X_RS_OFFSET	6
#define PCF2119X_RS_INC		(0 << PCF2119X_RS_OFFSET)
#define PCF2119X_RS_DATA	(1 << PCF2119X_RS_OFFSET)
#define PCF2119X_DEF_CTRL_BYTE	(0x00)

/* PCF2119X Instruction Definition */
#define PCF2119X_INC_FUNC_FIX		0x20	/* fixed value */
#define PCF2119X_INC_FUNC_EXT_INC_MSK	0x01
#define PCF2119X_INC_FUNC_VAL	( PCF2119X_INC_FUNC_FIX		 | \
				 (PCF2119X_INC_FUNC_SL << 1)	 | \
				 (PCF2119X_INC_FUNC_M << 2)	 | \
				 (PCF2119X_INC_FUNC_DL << 4))

#define PCF2119X_INC_BF_AC_BF_OFFSET	7
/* FIXME: Why we need these? bug? */
#define PCF2119X_INC_WRITE_DATA		0x80	/* fixed value */
//#define PCF2119X_INC_READ_DATA		0xC0	/* fixed value */

/* standard inc */
#define PCF2119X_INC_CLEAR_DISP		0x01	/* fixed value */
#define PCF2119X_INC_RETURN		0x02	/* fixed value */
#define PCF2119X_INC_ENTRY_MODE		0x04	/* fixed value */
#define PCF2119X_INC_ENTRY_MODE_LD_MSK	0x02
#define PCF2119X_INC_ENTRY_MODE_S_MSK	0x01
#define PCF2119X_INC_DISP_CTRL		0x08	/* fixed value */
#define PCF2119X_INC_DISP_CTRL_D_MSK	0x04
#define PCF2119X_INC_DISP_CTRL_C_MSK	0x02
#define PCF2119X_INC_DISP_CTRL_B_MSK	0x01
#define PCF2119X_INC_CURS_DISP		0x10	/* fixed value */
#define PCF2119X_INC_CURS_DISP_SC_MSK	0x08
#define PCF2119X_INC_CURS_DISP_RL_MSK	0x04
#define PCF2119X_INC_SET_CGRAM		0x40	/* fixed value */
#define PCF2119X_INC_SET_DDRAM		0x80	/* fixed value */
#define PCF2119X_INC_SET_DDRAM_LINE1	0x00
#define PCF2119X_INC_SET_DDRAM_LINE2	0x40
/* 0x40 means the second line */
#define LCD_LINE_OFFSET			6

/* extend inc */
#define PCF2119X_INC_SCR_CONF		0x02	/* fixed value */
#define PCF2119X_INC_SCR_CONF_L_MSK	0x01
#define PCF2119X_INC_DISP_CONF		0x04	/* fixed value */
#define PCF2119X_INC_DISP_CONF_P_MSK	0x02
#define PCF2119X_INC_DISP_CONF_Q_MSK	0x01
#define PCF2119X_INC_ICON_CTL		0x08	/* fixed value */
#define PCF2119X_INC_ICON_CTL_IM_MSK	0x04
#define PCF2119X_INC_ICON_CTL_IB_MSK	0x02
#define PCF2119X_INC_ICON_CTL_DM_MSK	0x01
#define PCF2119X_INC_HV_GEN		0x40	/* fixed value */
#define PCF2119X_INC_HV_GEN_S_MSK	0x03
#define PCF2119X_INC_HV_GEN_S_1		0
#define PCF2119X_INC_HV_GEN_S_2		1
#define PCF2119X_INC_HV_GEN_S_3		2
#define PCF2119X_INC_HV_GEN_S_NONE	3
#define PCF2119X_INC_TEMP_CTL		0x10	/* fixed value */
#define PCF2119X_INC_TEMP_CTL_TC_MSK	0x03
#define PCF2119X_INC_VLCD_SET		0x80	/* fixed value */
#define PCF2119X_INC_VLCD_SET_V_MSK	0x40
#define PCF2119X_INC_VLCD_SET_VAB_MSK	0x3F

/* XXX: Refer to PCF2119X 11.2.1 */
#define PCF2119X_SLAVE_ADDR	0x3B

void lcd_hw_set_pos(uint8_t pos);
void lcd_hw_write_data(unsigned char ch);
void lcd_hw_ctrl_init(void);

#endif /* __LCD_PCF2119X_H_INCLUDE__ */
