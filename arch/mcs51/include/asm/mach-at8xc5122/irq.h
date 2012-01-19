#ifndef __IRQ_AT8XC5122_H_INCLUDE__
#define __IRQ_AT8XC5122_H_INCLUDE__

#include <asm/reg.h>

/*--------------------------- IRQ REGISTERS ----------------------------*/
Sfr(ISEL,	0xA1);		/* IRQ selection */
Sfr(IEN0,	0xA8);		/* IRQ enabling 0 */
Sfr(IEN1,	0xB1);		/* IRQ enabling 1 */
Sfr(IPH0,	0xB7);		/* IRQ priority high 0 */
Sfr(IPL0,	0xB8);		/* IRQ priority low  0 */
Sfr(IPH1,	0xB3);		/* IRQ priority high 1 */
Sfr(IPL1,	0xB2);		/* IRQ priority low  1 */

Sbit(T1,	0xB0, 5);	/* External Event 1 */
Sbit(T0,	0xB0, 4);	/* External Event 0 */
Sbit(INT1,	0xB0, 3);	/* External Interrupt 1 */
Sbit(INT0,	0xB0, 2);	/* External Interrupt 0 */

/* IEN0 & IP0 masks */
#define MSK_IRQ_ALL		0x80
#define MSK_IRQ_X1		0x04
#define MSK_IRQ_X0		0x01

/* ISEL */
#define MSK_ISEL_CPLEV		0x80
#define MSK_ISEL_PRESIT		0x20
#define MSK_ISEL_RXIT		0x10
#define MSK_ISEL_OELEV		0x08
#define MSK_ISEL_OEEN		0x04
#define MSK_ISEL_PRESEN		0x02
#define MSK_ISEL_RXEN		0x01

/*---------------------------- IRQ VECTORS -----------------------------*/
#define IRQ_INT0		0
#define IRQ_GPT			1
#define IRQ_INT1		2
#define IRQ_TSC			3
#define IRQ_UART		4
#define IRQ_KBD			7
#define IRQ_SPI			9
#define IRQ_SCIB		10
#define IRQ_USB			13

/*--------------------------- IRQ PRIORITIES ---------------------------*/
#define IRQ_PRIO_0		0x00
#define IRQ_PRIO_1		0x01
#define IRQ_PRIO_2		0x02
#define IRQ_PRIO_3		0x03

#ifdef CONFIG_TICK
#define irq_hw_ctrl_enable()		(EA = 1)
#define irq_hw_ctrl_disable()		(EA = 0)
#endif
void irq_hw_enable_vector(uint8_t irq, boolean maskable);
void irq_hw_set_priority(uint8_t irq, uint8_t prio);

#endif /* __IRQ_AT8XC5122_H_INCLUDE__ */
