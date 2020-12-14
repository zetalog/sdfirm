#ifndef __REG_MOTHER_VEXPRESSA9_H_INCLUDE__
#define __REG_MOTHER_VEXPRESSA9_H_INCLUDE__

/* DUI0447J 4.2.1 ARM Legacy Memory Map */

/* Motherboard Express uATX Memory Map */
#define UATX_PERIPH1(cs, ao)	((CE9X4_##cs) + ((ao) << 12))
#define UATX_PERIPH2(cs, ao)	((CE9X4_##cs) + ((ao) << 24))

/* Chip select 0/1/2 */
#define UATX_NOR1	UATX_PERIPH2(CS0, 0)
#define UATX_NOR2	UATX_PERIPH2(CS1, 0)
#define UATX_SRAM	UATX_PERIPH2(CS2, 0)

/* Chip select 7 */
#define UATX_SYSREG	UATX_PERIPH1(CS7, 0)
#define UATX_SYSCTL	UATX_PERIPH1(CS7, 1)
#define UATX_PCIE	UATX_PERIPH1(CS7, 2)

#define UATX_AACI	UATX_PERIPH1(CS7, 4)
#define UATX_MMCI	UATX_PERIPH1(CS7, 5)
#define UATX_KMI0	UATX_PERIPH1(CS7, 6)
#define UATX_KMI1	UATX_PERIPH1(CS7, 7)

#define UATX_UART0	UATX_PERIPH1(CS7, 9)
#define UATX_UART1	UATX_PERIPH1(CS7, 10)
#define UATX_UART2	UATX_PERIPH1(CS7, 11)
#define UATX_UART3	UATX_PERIPH1(CS7, 12)

#define UATX_WDT	UATX_PERIPH1(CS7, 15)

#define UATX_TIMER01	UATX_PERIPH1(CS7, 17)
#define UATX_TIMER23	UATX_PERIPH1(CS7, 18)

#define UATX_DVI	UATX_PERIPH1(CS7, 22)
#define UATX_RTC	UATX_PERIPH1(CS7, 23)

#define UATX_CF		UATX_PERIPH1(CS7, 26)

#define UATX_CLCD	UATX_PERIPH1(CS7, 31)

/* Chip select 3 */
#define UATX_VRAM	UATX_PERIPH2(CS3, 0)
#define UATX_ETH	UATX_PERIPH2(CS3, 2)
#define UATX_USB	UATX_PERIPH2(CS3, 3)

#endif /* __REG_MOTHER_VEXPRESSA9_H_INCLUDE__ */