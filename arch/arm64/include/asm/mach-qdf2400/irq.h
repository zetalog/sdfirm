#ifndef __IRQ_QDF2400_H_INCLUDE__
#define __IRQ_QDF2400_H_INCLUDE__

/* Generic timer */
#define IRQ_TIMER	30

/* IRQs routed to IMC */
#ifdef CONFIG_QDF2400_IMC
/* BAM UART */
#define IRQ_UART0	104
#define IRQ_UART1	105
#define IRQ_UART2	106
#define IRQ_UART3	107
#define IRQ_UART8	130
#define IRQ_UART9	131
#define IRQ_UART10	132
#define IRQ_UART11	133

/* IMEM error capture */
#define IRQ_SYS_IMEM	167
#define IRQ_MSG_IMEM	283
#define IRQ_RAM_0_IMEM	320
#define IRQ_RAM_1_IMEM	322
#define IRQ_RAM_2_IMEM	324
#define IRQ_RAM_3_IMEM	439
#define IRQ_RAM_4_IMEM	441
#define IRQ_RAM_5_IMEM	446

#define NR_IRQS		512
#endif

/* IRQs routed to APC */
#ifdef CONFIG_QDF2400_APC
#define IRQ_UART0	487
#define IRQ_UART1	488
#define IRQ_UART2	489
#define IRQ_UART3	490
#define IRQ_UART8	513
#define IRQ_UART9	514
#define IRQ_UART10	515
#define IRQ_UART11	516

#define NR_IRQS		832
#endif

#endif  /* __IRQ_QDF2400_H_INCLUDE__ */
