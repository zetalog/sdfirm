#ifndef __IRQ_LM3S3826_H_INCLUDE__
#define __IRQ_LM3S3826_H_INCLUDE__

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

#define IRQ_GPIOA	0
#define IRQ_GPIOB	1
#define IRQ_GPIOC	2
#define IRQ_GPIOD	3
#define IRQ_GPIOE	4
#define IRQ_UART0	5
#define IRQ_UART1	6
#define IRQ_SSI0	7
#define IRQ_I2C0	8
#define IRQ_ADC0S0	14 /* ADC0 Sequence0 */
#define IRQ_ADC0S1	15 /* ADC0 Sequence1 */
#define IRQ_ADC0S2	16 /* ADC0 Sequence2 */
#define IRQ_ADC0S3	17 /* ADC0 Sequence3 */
#define IRQ_WDT		18 /* Watchdog Timer 0/1 */
#define IRQ_GPT0A	19 /* Timer 0A */
#define IRQ_GPT0B	20 /* Timer 0B */
#define IRQ_GPT1A	21 /* Timer 1A */
#define IRQ_GPT1B	22 /* Timer 1B */
#define IRQ_GPT2A	23 /* Timer 2A */
#define IRQ_GPT2B	24 /* Timer 2B */
#define IRQ_AC0		25 /* Analog Comparator0 */
#define IRQ_AC1		26 /* Analog Comparator1 */
#define IRQ_SYS		28 /* System Control */
#define IRQ_FMC		29 /* Flash Memory Control */
#define IRQ_UART2	33
#define IRQ_SSI1	34
#define IRQ_I2C1	37
#define IRQ_HM		43 /* Hibernation Module */
#define IRQ_USB		44
#define IRQ_PWMG3	45 /* PWM Generator3 */
#define IRQ_UDMAS	46 /* uDMA Software */
#define IRQ_UDMAE	47 /* uDMA Error */
#define NR_IRQS		55

#endif  /* __IRQ_LM3S3826_H_INCLUDE__ */
