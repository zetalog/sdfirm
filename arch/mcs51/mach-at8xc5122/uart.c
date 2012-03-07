#include <target/uart.h>

/* frame error detection */
#define __uart_hw_enable_fed()		(PCON |= MSK_PCON_SMOD0)
#define __uart_hw_disable_fed()		(PCON &= ~MSK_PCON_SMOD0)
#define __uart_hw_fed_raised()		(SCON & MSK_SCON_FE)

/* auto address recognition */
#define __uart_hw_enable_aar()		(SCON |= MSK_SCON_SM2)
#define __uart_hw_disable_aar()		(SCON &= ~MSK_SCON_SM2)

/* UART working mode, where mode can be 0, 1, 2, 3 */
#define __UART_HW_MODE_OFFSET		6
#define __UART_HW_MODE_MASK		(0x03 << __UART_HW_MODE_OFFSET)
#define __UART_HW_MODE_ASYNC_8N1	1
#define __UART_HW_MODE_ASYNC_9N1	3
#define __uart_hw_config_mode(mode)						\
	do {									\
		__uart_hw_disable_aar();					\
		SCON = (((mode)<<__UART_HW_MODE_OFFSET) | MSK_SCON_REN);	\
	} while (0)

/* BRG source configuration */
#define __UART_HW_BRG_VAR_IBRG		(MSK_BDRCON_TBCK | MSK_BDRCON_RBCK | MSK_BDRCON_BRR)
#define __UART_HW_BRG_VAR_TIMER1	(0)
#define __uart_hw_brg_enable(src)	(BDRCON = (src))
#define __uart_hw_brg_disable()		(BDRCON = (MSK_BDRCON_TBCK | MSK_BDRCON_RBCK))

#define __uart_hw_ri_raised()		(SCON &   UART_SCON_RI)
#define __uart_hw_unraise_ri()		(SCON &= ~UART_SCON_RI)
#define __uart_hw_ti_raised()		(SCON &   UART_SCON_TI)
#define __uart_hw_unraise_ti()		(SCON &= ~UART_SCON_TI)

#ifdef CONFIG_UART_AT8XC5122_TIMER1
#define __UART_HW_BRG_SOURCE		__UART_HW_BRG_VAR_TIMER1

static void __uart_hw_config_brg(uint32_t baudrate)
{
	/* config baud rate for Timer1:
	 * TH1 = 256 - (((2 ^ SMOD1) * CLK_T1) / (192 * baudrate))
	 */
	BUG();
}
#else
#define __UART_HW_BRG_SOURCE		__UART_HW_BRG_VAR_IBRG

#define __uart_hw_enable_smod1()	(PCON |= MSK_PCON_SMOD1)
#define __uart_hw_enable_spd()		(BDRCON |= MSK_BDRCON_SPD)
#define __uart_hw_disable_smod1()	(PCON &= ~MSK_PCON_SMOD1)
#define __uart_hw_disable_spd()		(BDRCON &= ~MSK_BDRCON_SPD)

static void __uart_hw_config_brg(uint32_t baudrate)
{
	uint8_t fac;
	uint16_t tmp;

	/* config baud rate for IBGR:
	 *
	 * BRL = 256 - ((2 ^ SMOD1 * CLK_IBRG) / (6 ^ (1 - SPD) * 32 * baudrate))
	 * given factor = 6 ^ (1 - SPD) * 32 / 2 ^ SMOD1, then
	 * BRL = 256 - ((2 * CLK_IBRG) / (32 * baudrate))
	 *     = 256 - ((CLK_IBRG / baudrate) / factor)
	 *     = 256 - (((CLK_PERIPH * 1000000) / baudrate) / factor)
	 *     = 256 - (((CLK_PERIPH * 1250) / (baudrate / 800)) / factor)
	 *
	 * since BRL > 0, then
	 * 256 > ((CLK_PERIPH * 1250) / (factor * (baudrate / 800)))
	 * ((CLK_PERIPH * 1250) / 256) < (factor * (baudrate / 800))
	 * factor > (((CLK_PERIPH * 1250) / (baudrate / 800)) / 256)
	 *
	 * given BRL = BRL + 0.5
	 * BRL =  256 - (((CLK_PERIPH * 1250) / (baudrate / 800 )) / factor)
	 *     = (513 - (((CLK_PERIPH * 1250) / (baudrate / 1600)) / factor)) / 2
	 */
	fac = HIBYTE(div16u((uint16_t)(mul16u(CLK_PERIPH, 1250)),
			    (uint16_t)(div32u(baudrate, 800))));
	if (fac >= 192) {
		__uart_hw_disable_spd();
		__uart_hw_disable_smod1();
		fac = 0xC0;
	} else if (fac >= 96) {
		__uart_hw_enable_smod1();
		__uart_hw_disable_spd();
		fac = 0x60;
	} else if (fac >= 32) {
		__uart_hw_disable_smod1();
		__uart_hw_enable_spd();
		fac = 0x20;
	} else {
		__uart_hw_enable_smod1();
		__uart_hw_enable_spd();
		fac = 0x10;
	}

	tmp = div16u((uint16_t)(mul16u(CLK_PERIPH, 1250)),
		     (uint16_t)((uint16_t)(div32u(baudrate, 1600))));
	tmp = div16u(tmp, fac);
	BRL = (uint8_t)div16u((uint16_t)(513-tmp), 2);
}
#endif

/* should config mode according to the params */
void uart_hw_set_params(uint8_t params, uint32_t baudrate)
{
	/* config parity, stop bits, data length */
	/* the only mode that we can support is 8N1 */
	BUG_ON(params != (8 | UART_PARITY_NONE | UART_STOPB_ONE));
	__uart_hw_config_mode(__UART_HW_MODE_ASYNC_8N1);

	/* XXX: BRG Source / Generator Configuration Order
	 *
	 * BRG source configuration must be done before BRG generator
	 * configuration.
	 */
	__uart_hw_brg_enable(__UART_HW_BRG_SOURCE);
	__uart_hw_config_brg(baudrate);
}

void uart_hw_write_byte(uint8_t byte)
{
	SBUF = byte;
	while (!__uart_hw_ti_raised());
	__uart_hw_unraise_ti();
}

uint8_t uart_hw_read_byte(void)
{
	uint8_t byte;
	while (!__uart_hw_ri_raised());
	byte = SBUF;
	__uart_hw_unraise_ri();
	return byte;
}

#ifdef CONFIG_UART_SYNC
void uart_hw_sync_start(void)
{
}

void uart_hw_sync_stop(void)
{
}

void uart_hw_sync_init(void)
{
	clk_hw_resume_dev(DEV_UART);
	__uart_hw_disable_fed();
}
#endif

#ifdef CONFIG_UART_ASYNC
void uart_hw_stop_rx(void)
{
	__uart_hw_unraise_ri();
}

void uart_hw_stop_tx(void)
{
	__uart_hw_unraise_ti();
}

void uart_hw_start_tx(void)
{
}

static void uart_hw_handle_rx(void)
{
}

static void uart_hw_handle_tx(void)
{
}

static void uart_hw_handle_irq(void)
{
	if (__uart_hw_ri_raised())
		uart_hw_handle_rx();
	if (__uart_hw_ti_raised())
		uart_hw_handle_tx();
}

#ifdef SYS_REALTIME
void uart_hw_irq_poll(void)
{
	uart_hw_handle_irq();
}
#else
void DEFINE_ISR(uart_isr(void), IRQ_UART)
{
	uart_hw_handle_irq();
}

void uart_hw_irq_init(void)
{
	irq_hw_set_priority(IRQ_UART, IRQ_PRIO_1);
	irq_hw_enable_vector(IRQ_UART, true);
}
#endif

static void uart_hw_async_init(void)
{
	clk_hw_resume_dev(DEV_UART);
	__uart_hw_disable_fed();
	uart_hw_irq_init();
}
#endif
