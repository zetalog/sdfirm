#ifndef __IRQ_VEXPRESSA9_H_INCLUDE__
#define __IRQ_VEXPRESSA9_H_INCLUDE__

#define TRAP_RESET	1
#define TRAP_NMI	2
#define TRAP_HARD	3
#define TRAP_MPU	4
#define TRAP_BUS	5
#define TRAP_USAGE	6
#define TRAP_SVC	11
#define TRAP_DEBUG	12
#define TRAP_PENDSV	14
#define TRAP_TICK	15
#define NR_TRAPS	16

#define IRQ_WDT		0
#define IRQ_SWI		1
#define IRQ_TIMER01	2
#define IRQ_TIMER23	3
#define IRQ_RTC		4
#define IRQ_UART0	5
#define IRQ_UART1	6
#define IRQ_UART2	7
#define IRQ_UART3	8
#define IRQ_MCI_0	9
#define IRQ_MCI_1	10
#define IRQ_AACI	11
#define IRQ_KMI0	12
#define IRQ_KMI1	13
#define IRQ_CLCD	14
#define IRQ_ETH		15
#define IRQ_USB		16
#define IRQ_PCIE	17
#define NR_IRQS		48

#endif  /* __IRQ_VEXPRESSA9_H_INCLUDE__ */
