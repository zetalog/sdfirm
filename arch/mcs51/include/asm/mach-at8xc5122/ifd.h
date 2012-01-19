#ifndef __IFD_AT8XC5122_H_INCLUDE__
#define __IFD_AT8XC5122_H_INCLUDE__

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

Sbit(CRST1,	0xB0, 5);	/* Alternate Card Reset */
Sbit(CIO1,	0xB0, 4);	/* Alternate Card I/O */
Sbit(CCLK1,	0x90, 7);	/* Alternate Card Clock */
Sbit(CPRES,	0x90, 2);	/* Smart Card Presence */

Sfr(SCIBUF,	0xAA);		/* Smart Card Transmit/Receive Buffer */
Sfr(SCSR,	0xAB);		/* Smart Card Selection Register */
Sfr(SCCON,	0xAC);		/* Smart Card Interface Contacts */
Sfr(SCETU0,	0xAC);		/* Smart Card ETU Register 0 */
Sfr(SCISR,	0xAD);		/* Smart Card UART Interface Status */
Sfr(SCETU1,	0xAD);		/* Smart Card ETU Register 1 */
Sfr(SCIIR,	0xAE);		/* Smart Card UART Interrupt Identification */
Sfr(SCIER,	0xAE);		/* Smart Card UART Interrupt enable */
Sfr(SCWT0,	0xB4);		/* Smart Card Character-Block Waiting Time Register 0 */
Sfr(SCGT0,	0xB4);		/* Smart Card Transmit Guard Time Register 0 */
Sfr(SCWT1,	0xB5);		/* Smart Card Character-Block Waiting Time Register 1 */
Sfr(SCGT1,	0xB5);		/* Smart Card Transmit Guard Time Register 1 */
Sfr(SCWT2,	0xB6);		/* Smart Card Character-Block Waiting Time Register 2 */
Sfr(SCICR,	0xB6);		/* Smart Card Interface Control */
Sfr(DCCKPS,	0xBF);		/* DC/DC Converter Reload Register */
Sfr(SCWT3,	0xC1);		/* Smart Card Character-Block Waiting Time Register 3 */
Sfr(SCICLK,	0xC1);		/* Smart Card Clock Prescaler */

#define MSK_PMOD_CPRESRES	0x08

#define MSK_SCICR_RESET		0x80	/* SCICR */
#define MSK_SCICR_CARDDET	0x40
#define MSK_SCICR_VCARD1	0x20
#define MSK_SCICR_VCARD0	0x10
#define MSK_SCICR_VCARD		0x30
#define MSK_SCICR_UART		0x08
#define MSK_SCICR_WTEN		0x04
#define OST_SCICR_WTEN		0x02
#define MSK_SCICR_CREP		0x02
#define MSK_SCICR_CONV		0x01
#define MSK_SCCON_CLK		0x80	/* SCCON */
#define MSK_SCCON_CARDC8	0x20
#define MSK_SCCON_CARDC4	0x10
#define MSK_SCCON_CARDIO	0x08
#define MSK_SCCON_CARDCLK	0x04
#define MSK_SCCON_CARDRST	0x02
#define MSK_SCCON_CARDVCC	0x01
#define MSK_SCETU_COMP		0x80	/* SCETU1 */
#define MSK_SCETU1_COMP		0x80

#define MSK_SCSR_BGTEN		0x40	/* SCSR */
#define MSK_SCSR_CREPSEL	0x10
#define MSK_SCSR_ALTKPS1	0x08
#define MSK_SCSR_ALTKPS0	0x04
#define MSK_SCSR_SCCLK1		0x02
#define MSK_SCSR_SCRS		0x01

#define MSK_SCICLK_XTSCS	0x80	/* SCICLK */

/* DC/DC */
#define DCCKPS_MODE_PUMP	0x00	/* DCCKPS */
#define DCCKPS_MODE_REGULATOR	0x80	/* DCCKPS */
#define DCCKPS_BOOST_OFFSET	4
#define DCCKPS_BOOST_MASK	(0x03<<DCCKPS_BOOST_OFFSET)
#define DCCKPS_BOOST_OVFADJ	0x00
#define DCCKPS_BOOST_MAX	0x03

#define SCIB_VOLTAGE_0V			0x00
#define SCIB_VOLTAGE_1_8V		0x01
#define SCIB_VOLTAGE_3V			0x02
#define SCIB_VOLTAGE_5V			0x03

#define SCIB_BOOST_NORMAL		0x00
#define SCIB_BOOST_30_PERCENT		0x01
#define SCIB_BOOST_50_PERCENT		0x02
#define SCIB_BOOST_80_PERCENT		0x03

/* IRQs */
#define SCPE			0 /* Character Reception Parity Error */
#define SCRC			1 /* UART Received Character */
#define SCTC			2 /* UART Transmitted Character */
#define SCWTO			3 /* Waiting Time Counter Timeout */
#define VCARDOK			4 /* SCISR, Card Voltage Correct */
#define VCARDERR		4 /* SCIIR/SCIER, Card Voltage Error */
#define ICARDERR		5 /* Card Current Overflow */
#define CARDIN			6 /* SCISR, Card Presence */
#define SCTBE			7 /* UART Transmit Buffer Empty */
#define SCIB_IRQ(irq)		(1<<(irq))

#define __ifd_hw_flag_raised(flag)	(SCISR & SCIB_IRQ(flag))
#define __ifd_hw_xchg_raised(irq)	(SCISR & (SCIB_IRQ(irq)))
//#define __ifd_hw_xchg_raised(irq)	(SCISR & (SCIB_IRQ(irq) | SCIB_IRQ(SCWTO) | SCIB_IRQ(SCPE)))

/* Card Presence Detection
 *   Registers involves:
 * 	SCICR.CARDDET	select the card in sense
 * 	ISEL.CPLEV	detect level selection bit
 * 	PMOD.CPRESRES	connect to the internal pullup if there is no external pullup
 * 	ISEL.PRESEN	detect interrupt enable bit
 * 	ISEL.PRESIT	detect interrupt flag
 */
#ifdef CONFIG_IFD_AT8XC5122_PRES_HIGH
#define __ifd_hw_detect_config()	\
	(SCSR |= MSK_SCSR_SCRS, SCICR &= ~MSK_SCICR_CARDDET)
#define __ifd_hw_detect_pres()		(ISEL &= ~MSK_ISEL_CPLEV)
#define __ifd_hw_detect_abse()		(ISEL |= MSK_ISEL_CPLEV)
#endif
#ifdef CONFIG_IFD_AT8XC5122_PRES_LOW
#define __ifd_hw_detect_config()	\
	(SCSR |= MSK_SCSR_SCRS, SCICR |= MSK_SCICR_CARDDET)
#define __ifd_hw_detect_pres()		(ISEL |= MSK_ISEL_CPLEV)
#define __ifd_hw_detect_abse()		(ISEL &= ~MSK_ISEL_CPLEV)
#endif

#ifdef CONFIG_IFD_AT8XC5122_PRES_EXTERNAL
#define __ifd_hw_detect_pullup()	(PMOD0 |= MSK_PMOD_CPRESRES)
#else
#define __ifd_hw_detect_pullup()	(PMOD0 &= ~MSK_PMOD_CPRESRES)
#endif

#define __ifd_hw_enable_detect()	(ISEL |= MSK_ISEL_PRESEN)
#define __ifd_hw_disable_detect()	(ISEL &= ~MSK_ISEL_PRESEN)

#define __ifd_hw_unraise_detect()	(ISEL &= ~MSK_ISEL_PRESIT)
boolean __ifd_hw_detect_present(void);

#ifdef CONFIG_IFD_AT8XC5122_MAX_FREQ
#define IFD_HW_FREQ_MAX			CONFIG_IFD_AT8XC5122_MAX_FREQ
#endif

#ifdef CONFIG_IFD_AT8XC5122_MAX_DATA
#define IFD_HW_DATA_MAX			(uint32_t)CONFIG_IFD_AT8XC5122_MAX_DATA
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

void ifd_hw_start_wtc(void);
void ifd_hw_stop_wtc(void);

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

#endif /* __IFD_AT8XC5122_H_INCLUDE__ */
