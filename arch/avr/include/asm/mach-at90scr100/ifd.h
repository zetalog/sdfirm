#ifndef __IFD_AT90SCR100_H_INCLUDE__
#define __IFD_AT90SCR100_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/irq.h>
#include <asm/mach/clk.h>
#include <asm/mach/gpio.h>

#ifndef ARCH_HAVE_IFD
#define ARCH_HAVE_IFD			1
#else
#error "Multiple interface devices defined"
#endif

/* DC/DC converter register */
#define DCCR		_SFR_MEM8(0xEF)
#define DCON		7	/* R/W */
#define DCRDY		6	/* R */
#define DCBUSY		5	/* R */

/* smart card interface control register */
#define SCICR		_SFR_MEM8(0xFF)
#define SCIRESET	7	/* R/W */
#define CARDDET		6	/* R/W */
#define VCARD1		5	/* R/W */
#define VCARD0		4	/* R/W */
#define UART		3	/* R/W */
#define WTEN		2	/* R/W */
#define CREP		1	/* R/W */
#define CONV		0	/* R/W */

/* smart card contacts register */
#define SCCON		_SFR_MEM8(0xFE)
#define CLK		7	/* R/W */
#define CARDC8		5	/* R/W */
#define CARDC4		4	/* R/W */
#define CARDIO		3	/* R/W */
#define CARDCLK		2	/* R/W */
#define CARDRST		1	/* R/W */
#define CARDVCC		0	/* R/W */

/* smart card UART interface status register */
#define SCISR		_SFR_MEM8(0xFD)
#define CARDIN		6	/* R */
#define VCARDOK		4	/* R */

#define SCTBE		7	/* transmit buffer empty */
#define SCWTO		3	/* waiting time timeout */
#define SCTC		2	/* transmitted character */
#define SCRC		1	/* received character */
#define SCPE		0	/* character reception parity error */

/* smart card UART interrupt identification register */
#define SCIIR		_SFR_MEM8(0xFC)
#define VCARDERR	4

/* smart card UART interrupt enable register */
#define SCIER		_SFR_MEM8(0xFB)
#define CARDINE		6
#define EVCARDER	4

/* smart card selection register */
#define SCSR		_SFR_MEM8(0xFA)
#define BGTEN		6	/* R/W */
#define CREPSEL		4	/* R/W */
#define CPRESRES	3	/* R/W */

/* smart card transmit/receive buffer */
#define SCIBUF		_SFR_MEM8(0xF9)

/* smart card ETU register */
#define SCETUH		_SFR_MEM8(0xF8)
#define SCETUL		_SFR_MEM8(0xF7)
#define SCETU		_SFR_MEM16(0xF7)
#define COMP		15
#define COMPH		7

/* smart card transmit guard time register */
#define SCGTH		_SFR_MEM8(0xF6)
#define SCGTL		_SFR_MEM8(0xF5)
#define SCGT		_SFR_MEM16(0xF5)

/* smart card character/block waiting time register */
#define SCWT		_SFR_MEM32(0xF1)
#define SCWT3		_SFR_MEM8(0xF4)
#define SCWT2		_SFR_MEM8(0xF3)
#define SCWT1		_SFR_MEM8(0xF2)
#define SCWT0		_SFR_MEM8(0xF1)

/* smart card clock register */
#define SCICLK		_SFR_MEM8(0xF0)

#define SCICR_VCARD_OFFSET	4
#define SCICR_VCARD_MASK	(0x03<<SCICR_VCARD_OFFSET)

/* voltage class */
#define SCIB_CLS(cls)		(cls ? 4-cls : cls)

/* ICC presence detection */
#define SCIB_IRQ(irq)		_BV(irq)

#define __ifd_hw_flag_raised(flag)	(SCISR &   SCIB_IRQ(flag))
#define __ifd_hw_xchg_raised(irq)	(SCISR &   SCIB_IRQ(irq))
#define __ifd_hw_unraise_xchg(irq)	(SCISR &= ~SCIB_IRQ(irq))

#ifdef CONFIG_IFD_AT90SCR100_PRES_EXTERNAL
#define __ifd_hw_detect_pullup()	(SCSR |=  _BV(CPRESRES))
#else
#define __ifd_hw_detect_pullup()	(SCSR &= ~_BV(CPRESRES))
#endif

#define __ifd_hw_irq_enable(irq)	(SCIER |=  _BV(irq))
#define __ifd_hw_irq_disable(irq)	(SCIER &= ~_BV(irq))

/* ICC contacts */
#define __ifd_hw_enable_clk()		(SCCON |=  _BV(CLK))
#define __ifd_hw_disable_clk()		(SCCON &= ~_BV(CLK))
#define __ifd_hw_enable_io()		(SCICR |=  _BV(UART))
#define __ifd_hw_disable_io()		(SCICR &= ~_BV(UART))
#define __ifd_hw_pullup_contact(c)	(SCCON |=  _BV(c))
#define __ifd_hw_pulldown_contact(c)	(SCCON &= ~_BV(c))

boolean __ifd_hw_enable_vcc(void);
void __ifd_hw_disable_vcc(void);

/* DC/DC converter */
#define __ifd_hw_dcdc_busy()		(DCCR & _BV(DCBUSY))
#define __ifd_hw_dcdc_ready()		(DCCR & _BV(DCRDY))
void __ifd_hw_dcdc_on(void);
void __ifd_hw_dcdc_off(void);

#define IFD_HW_FREQ_MAX			12000

#ifdef CONFIG_IFD_AT90SCR100_MAX_DATA
#define IFD_HW_DATA_MAX			(uint32_t)CONFIG_IFD_AT90SCR100_MAX_DATA
#else
#define IFD_HW_DATA_MAX			(uint32_t)115200
#endif

void ifd_hw_deactivate(void);
boolean ifd_hw_activate(void);
void ifd_hw_reset(void);
void ifd_hw_warm_reset(void);

#ifdef CONFIG_IFD_CLOCK_CONTROL
void ifd_hw_suspend_clock(uint8_t level);
void ifd_hw_resume_clock(void);
#else
#define ifd_hw_suspend_clock(level)
#define ifd_hw_resume_clock()
#endif

void ifd_hw_config_cls(uint8_t cls);
void ifd_hw_config_etu(uint16_t F, uint8_t D);
void ifd_hw_config_conv(uint8_t conv);
void ifd_hw_config_crep(boolean on);
void ifd_hw_delay_gt(uint32_t gt, boolean blk);
void ifd_hw_delay_wt(uint32_t wt, boolean blk);

#define ifd_hw_start_wtc()		(SCICR |= _BV(WTEN))
#define ifd_hw_stop_wtc()		(SCICR &= ~_BV(WTEN))

/* slot buffer management */
uint8_t ifd_hw_read_byte(void);
void ifd_hw_write_byte(uint8_t b);

/* slot management */
void ifd_hw_slot_reset(void);
#define ifd_hw_slot_select(sid)

uint8_t ifd_hw_cf_nr_freq(void);
uint32_t ifd_hw_cf_get_freq(uint8_t index);
void ifd_hw_set_freq(uint32_t freq);

boolean ifd_hw_get_contact(uint8_t pin);
boolean ifd_hw_icc_present(void);

void ifd_hw_ctrl_init(void);

#endif /* __IFD_AT90SCR100_H_INCLUDE__ */
