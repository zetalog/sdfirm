#ifndef __PM_LM3S9B92_H_INCLUDE__
#define __PM_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/mem.h>
#include <asm/mach/clk.h>

#define RCGC0		SYSTEM(0x100)
#define RCGC1		SYSTEM(0x104)
#define RCGC2		SYSTEM(0x108)
#define SCGC0		SYSTEM(0x110)
#define SCGC1		SYSTEM(0x114)
#define SCGC2		SYSTEM(0x118)
#define DCGC0		SYSTEM(0x120)
#define DCGC1		SYSTEM(0x124)
#define DCGC2		SYSTEM(0x128)

#define SRCR0		SYSTEM(0x040)
#define SRCR1		SYSTEM(0x044)
#define SRCR2		SYSTEM(0x048)

/* peripherals */
#define DEV_WDT0	3
#define DEV_ADC0	16
#define DEV_ADC1	17
#define DEV_PWM		20
#define DEV_CAN0	24
#define DEV_CAN1	25
#define DEV_WDT1	28
#define DEV_UART0	32
#define DEV_UART1	33
#define DEV_UART2	34
#define DEV_SSI0	36
#define DEV_SSI1	37
#define DEV_QEI0	40
#define DEV_QEI1	41
#define DEV_I2C0	44
#define DEV_I2C1	46
#define DEV_TIMER0	48
#define DEV_TIMER1	49
#define DEV_TIMER2	50
#define DEV_TIMER3	51
#define DEV_COMP0	56
#define DEV_COMP1	57
#define DEV_COMP2	58
#define DEV_I2S0	60
#define DEV_EPI0	62
#define DEV_GPIOA	64
#define DEV_GPIOB	65
#define DEV_GPIOC	66
#define DEV_GPIOD	67
#define DEV_GPIOE	68
#define DEV_GPIOF	69
#define DEV_GPIOG	70
#define DEV_GPIOH	71
#define DEV_GPIOJ	72
#define DEV_UDMA	77
#define DEV_USB0	80
#define DEV_EMAC0	92
#define DEV_EPHY0	94

#define DEV_MODE_ON	0x00 /* run mode */
#define DEV_MODE_SLP	0x01 /* sleep mode */
#define DEV_MODE_DSLP	0x02 /* deep sleep mode */
#define DEV_MODE_OFF	0x03

#define PM_GATE_MASK			0x1F
#define PM_GATE_A(__a, __vi)		((__a)+(((__vi) & ~PM_GATE_MASK) >> 3))
#define PM_GATE_V(__vi)			(1<<((__vi) & PM_GATE_MASK))

void pm_hw_resume_device(uint8_t dev, uint8_t mode);
void pm_hw_suspend_device(uint8_t dev, uint8_t mode);
void pm_hw_reset_device(uint8_t dev);
uint8_t pm_hw_device_mode(uint8_t dev);

#endif /* __PM_LM3S9B92_H_INCLUDE__ */
