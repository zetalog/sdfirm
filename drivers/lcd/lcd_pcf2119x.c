#include <target/arch.h>
#include <target/config.h>
#include <target/generic.h>
#include <target/lcd.h>
#include <target/delay.h>
#include <target/i2c.h>

#define LCD_MAX_LENGTH	0x20
#define LCD_LINE_LENGTH	(LCD_MAX_LENGTH / 2)

uint8_t __lcd_pcf_pos = 0;		/* curr pos */
uint8_t __lcd_pcf_txlen = 0;
uint8_t __lcd_pcf_rxlen = 0;
uint8_t __lcd_pcf_cmd[LCD_MAX_LENGTH];	/* cmd buf */

static void __lcd_pcf_write_cmd(uint8_t ch)
{
	if (__lcd_pcf_txlen < LCD_MAX_LENGTH)
		__lcd_pcf_cmd[__lcd_pcf_txlen++] = ch;
}

#if 0
static void __lcd_i2c_read_cmpl(void)
{
	__lcd_pcf_rxlen = 0;
}
#endif

static void __lcd_i2c_write_cmpl(void)
{
	__lcd_pcf_txlen = 0;
}

#if 0
static uint8_t __lcd_i2c_read(uint8_t rxlen)
{
	BUG_ON(__lcd_pcf_rxlen != 0);

	if (__lcd_pcf_txlen) {
		__lcd_pcf_rxlen = rxlen;
		return i2c_master_read(PCF2119X_SLAVE_ADDR,
				       rxlen);
	}
	return I2C_STATUS_STOP;
}
#endif

static uint8_t __lcd_i2c_write(void)
{
	if (__lcd_pcf_txlen) {
		return i2c_master_write(PCF2119X_SLAVE_ADDR, __lcd_pcf_txlen);
	}
	return I2C_STATUS_STOP;
}

static void __lcd_hw_pcf_cmd_clear(void)
{
	/* Control byte for Instruction */
	__lcd_pcf_write_cmd(PCF2119X_DEF_CTRL_BYTE);
	__lcd_pcf_write_cmd(PCF2119X_INC_CLEAR_DISP);
	while (__lcd_i2c_write() != I2C_STATUS_STOP);
	__lcd_i2c_write_cmpl();
	i2c_master_release();
	/* TODO: check Busy Flag (BF) instead */
	mdelay(1);
}

static void __lcd_hw_pcf_cmd_drawchar(uint8_t ch)
{
	/* Control byte for Instruction (data) */
	__lcd_pcf_write_cmd(PCF2119X_DEF_CTRL_BYTE | PCF2119X_RS_DATA);
	__lcd_pcf_write_cmd(PCF2119X_INC_WRITE_DATA | ch);
	while (__lcd_i2c_write() != I2C_STATUS_STOP);
	__lcd_i2c_write_cmpl();
	i2c_master_release();
}

#if 0
static void __lcd_hw_pcf_cmd_drawline(uint8_t *line, uint8_t length)
{
	/* Control byte for Instruction (data) */
	__lcd_pcf_write_cmd(PCF2119X_DEF_CTRL_BYTE | PCF2119X_RS_DATA);
	while (length--) {
		__lcd_pcf_write_cmd(PCF2119X_INC_WRITE_DATA | *(line));
		line++;
	}
	while (__lcd_i2c_write() != I2C_STATUS_STOP);
	__lcd_i2c_write_cmpl();
	i2c_master_release();
}

static void __lcd_hw_pcf_cmd_setline(uint8_t line)
{
	BUG_ON(line != 1 && line != 2);

	/* Control byte for Instruction */
	__lcd_pcf_write_cmd(PCF2119X_DEF_CTRL_BYTE);
	
	if (line == 1)
		__lcd_pcf_write_cmd(PCF2119X_INC_SET_DDRAM |
				     PCF2119X_INC_SET_DDRAM_LINE1);
	else
 		__lcd_pcf_write_cmd(PCF2119X_INC_SET_DDRAM |
				     PCF2119X_INC_SET_DDRAM_LINE2);
	while (__lcd_i2c_write() != I2C_STATUS_STOP);
	__lcd_i2c_write_cmpl();
	i2c_master_release();
}

static void __lcd_hw_pcf_cleanup(void)
{
	uint8_t lines = 2;
	while (lines) {
		__lcd_hw_pcf_cmd_setline(lines);
		__lcd_hw_pcf_cmd_drawline((uint8_t*)("                "), 0x10);
		/* Control byte for Instruction */
		__lcd_pcf_write_cmd(PCF2119X_DEF_CTRL_BYTE | PCF2119X_CO_CONT);
		__lcd_pcf_write_cmd(PCF2119X_INC_RETURN);
		while (__lcd_i2c_write() != I2C_STATUS_STOP);
		__lcd_i2c_write_cmpl();
		i2c_master_release();
		
		lines--;
	}
}
#endif

static void __lcd_hw_pcf_init_cmds(void)
{
	uint8_t val;

	/* Control byte for Instruction */
	__lcd_pcf_write_cmd(PCF2119X_DEF_CTRL_BYTE);

	/* standard mode */
	val  = PCF2119X_INC_FUNC_VAL;
	val &= ~PCF2119X_INC_FUNC_EXT_INC_MSK;
	__lcd_pcf_write_cmd(val);

	/* D: Display on, C: curser off, B: blink off */
	val  =  PCF2119X_INC_DISP_CTRL;
	val |=  PCF2119X_INC_DISP_CTRL_D_MSK;
	val &= ~PCF2119X_INC_DISP_CTRL_C_MSK;
	val &= ~PCF2119X_INC_DISP_CTRL_B_MSK;
	__lcd_pcf_write_cmd(val);

	/* I/D: increment, S: no shift */
	val  =  PCF2119X_INC_ENTRY_MODE;
	val |=  PCF2119X_INC_ENTRY_MODE_LD_MSK;
	val &= ~PCF2119X_INC_ENTRY_MODE_S_MSK;
	__lcd_pcf_write_cmd(val);

	/* ext mode */
	val  = PCF2119X_INC_FUNC_VAL;
	val |= PCF2119X_INC_FUNC_EXT_INC_MSK;
	__lcd_pcf_write_cmd(val);
	
	/* P: left to right, Q: top to bottom */
	val =   PCF2119X_INC_DISP_CONF;
	val &= ~PCF2119X_INC_DISP_CONF_P_MSK;
	val &= ~PCF2119X_INC_DISP_CONF_Q_MSK;
	__lcd_pcf_write_cmd(val);
	
	/* TC1: 0, TC2: 0 */
	val =   PCF2119X_INC_TEMP_CTL;
	val &= ~PCF2119X_INC_TEMP_CTL_TC_MSK;
	__lcd_pcf_write_cmd(val);
	
	/* HV Stages 3 */
	val  = PCF2119X_INC_HV_GEN;
	val |= PCF2119X_INC_HV_GEN_S_3;
	__lcd_pcf_write_cmd(val);
		
	/* set Vlcd, store to VA */
	val  =  PCF2119X_INC_VLCD_SET;
	val &= ~PCF2119X_INC_VLCD_SET_V_MSK;
	val |=  0x2F;	/* from the sample */
	__lcd_pcf_write_cmd(val);

	/* standard mode */
	val  = PCF2119X_INC_FUNC_VAL;
	val &= ~PCF2119X_INC_FUNC_EXT_INC_MSK;
	__lcd_pcf_write_cmd(val);

	/* DDRAM Address set to 00hex */
	val = PCF2119X_INC_SET_DDRAM;
	__lcd_pcf_write_cmd(val);
	val = PCF2119X_INC_RETURN;
	__lcd_pcf_write_cmd(val);

	while (__lcd_i2c_write() != I2C_STATUS_STOP);
	__lcd_i2c_write_cmpl();
	i2c_master_release();
}

void lcd_hw_set_pos(uint8_t pos)
{
	if (pos < LCD_MAX_LENGTH) {
		__lcd_pcf_pos = pos;
		/* Control byte for Instruction */
		__lcd_pcf_write_cmd(PCF2119X_DEF_CTRL_BYTE);
		__lcd_pcf_write_cmd(PCF2119X_INC_SET_DDRAM |
				   (((pos) / MAX_LCD_COLS) << LCD_LINE_OFFSET) |
				   (pos % MAX_LCD_COLS));

		while (__lcd_i2c_write() != I2C_STATUS_STOP);
		__lcd_i2c_write_cmpl();
		i2c_master_release();
	}
}

void lcd_hw_write_data(unsigned char ch)
{
	__lcd_hw_pcf_cmd_drawchar(ch);
}

#if 0
/* read only for testing now */
static void __lcd_hw_pcf_read_test(void)
{
	__lcd_pcf_write_cmd(PCF2119X_DEF_CTRL_BYTE | PCF2119X_RS_DATA);
	/* read 2 bytes */
	__lcd_i2c_read(2);
}
#endif

static void __lcd_hw_pcf_init(void)
{
	__lcd_hw_pcf_cmd_clear();
	__lcd_hw_pcf_init_cmds();

#if 0
	/* now cmd_clear() works well */
	__lcd_hw_pcf_cleanup();

	devid = i2c_probe_devid(PCF2119X_SLAVE_ADDR);

	__lcd_hw_pcf_read_test();
#endif
}

static void __lcd_hw_i2c_iocb(void)
{
	uint8_t i;

	/* both of RX/TX will send some bytes */
	for (i = 0; i < __lcd_pcf_txlen; i++) {
		i2c_write_byte(__lcd_pcf_cmd[i]);
	}
	if (i2c_dir_mode() == I2C_MODE_RX) {
		for (i = 0; i < __lcd_pcf_rxlen; i++) 
			(void)i2c_read_byte();
	}
}

i2c_device_t lcd_pcf2119x = {
	__lcd_hw_i2c_iocb,
};

void lcd_hw_ctrl_init(void)
{
	DDRB = (1<<DDB2);
	PORTB = (1<<PB2);
	nop();
	PORTB = (0<<PB2);
	nop();

	i2c_register_device(&lcd_pcf2119x);

	__lcd_hw_pcf_init();
}
