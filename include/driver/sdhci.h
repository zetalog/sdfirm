#ifndef __SDHCI_H_INCLUDE__
#define __SDHCI_H_INCLUDE__

#include <target/arch.h>

#ifndef SDHC_REG
#define SDHC_REG(n, offset)	(SDHC##n##_BASE + (offset))
#endif

/* Controller registers */
#define SDHCI_DMA_ADDRESS	0x00

#define SDHCI_BLOCK_SIZE	0x04
#define  SDHCI_MAKE_BLKSZ(dma, blksz) (((dma & 0x7) << 12) | (blksz & 0xFFF))

#define SDHCI_BLOCK_COUNT	0x06

#define SDHCI_ARGUMENT		0x08

#define SDHCI_TRANSFER_MODE	0x0C
#define  SDHCI_TRNS_DMA		_BV(0)
#define  SDHCI_TRNS_BLK_CNT_EN	_BV(1)
#define  SDHCI_TRNS_ACMD12	_BV(2)
#define  SDHCI_TRNS_READ	_BV(4)
#define  SDHCI_TRNS_MULTI	_BV(5)

#define SDHCI_COMMAND		0x0E
#define  SDHCI_CMD_RESP_MASK	0x03
#define  SDHCI_CMD_CRC		0x08
#define  SDHCI_CMD_INDEX	0x10
#define  SDHCI_CMD_DATA		0x20
#define  SDHCI_CMD_ABORTCMD	0xC0

#define  SDHCI_CMD_RESP_NONE	0x00
#define  SDHCI_CMD_RESP_LONG	0x01
#define  SDHCI_CMD_RESP_SHORT	0x02
#define  SDHCI_CMD_RESP_SHORT_BUSY 0x03

#define SDHCI_MAKE_CMD(c, f) (((c & 0xff) << 8) | (f & 0xff))
#define SDHCI_GET_CMD(c) ((c>>8) & 0x3f)

#define SDHCI_RESPONSE		0x10

#define SDHCI_BUFFER		0x20

#define SDHC_PRESENT_STATE(n)			SDHC_REG(n, 0x24) /* 32-bits */
#define SDHC_CLOCK_CONTROL(n)			SDHC_REG(n, 0x2C) /* 16-bits */
#define SDHC_TIMEOUT_CONTROL(n)			SDHC_REG(n, 0x2E) /* 16-bits */
#define SDHC_NORMAL_INTERRUPT_STATUS(n)		SDHC_REG(n, 0x30) /* 16-bits */
#define SDHC_ERROR_INTERRUPT_STATUS(n)		SDHC_REG(n, 0x32) /* 16-bits */
#define SDHC_NORMAL_INTERRUPT_STATUS_ENABLE(n)	SDHC_REG(n, 0x34) /* 16-bits */
#define SDHC_ERROR_INTERRUPT_STATUS_ENABLE(n)	SDHC_REG(n, 0x36) /* 16-bits */
#define SDHC_NORMAL_INTERRUPT_SIGNAL_ENABLE(n)	SDHC_REG(n, 0x38) /* 16-bits */
#define SDHC_ERROR_INTERRUPT_SIGNAL_ENABLE(n)	SDHC_REG(n, 0x3A) /* 16-bits */

/* Registers for aligned architecture */
#define SDHC_CLOCK_TIMEOUT_CONTROL(n)		SDHC_REG(n, 0x2C) /* 32-bits */
#define SDHC_INTERRUPT_STATUS(n)		SDHC_REG(n, 0x30) /* 32-bits */
#define SDHC_INTERRUPT_ENABLE(n)		SDHC_REG(n, 0x34) /* 32-bits */
#define SDHC_INTERRUPT_SIGNAL(n)		SDHC_REG(n, 0x38) /* 32-bits */

/* 2.2.9 Present State Register (Cat.C Offset 024h) */
#ifdef CONFIG_SDHC_UHSII
#define SDHC_UHSII_IF_DETECTION			_BV(31)
#define SDHC_LANE_SYNCHRONIZATION		_BV(30)
#define SDHC_IN_DORMANT_STATUS			_BV(29)
#endif
#define SDHC_SUB_COMMAND_STATUS			_BV(28)
#define SDHC_COMMAND_NOT_ISSUED_BY_ERROR	_BV(27)
#ifdef CONFIG_SDHC_SPEC_4_10
#define SDHC_HOST_REGULATOR_VOLTAGE_STABLE	_BV(26)
#endif
#ifdef CONFIG_SDHC_SD
#define SDHC_CMD_LINE_SIGNAL_LEVEL		_BV(25)
#define SDHC_DAT30_LINE_SIGNAL_LEVEL_OFFSET	20
#define SDHC_DAT30_LINE_SIGNAL_LEVEL_MASK	REG_4BIT_MASK
#define SDHC_DAT30_LINE_SIGNAL_LEVEL(value)	\
	_GET_FV(SDHC_DAT30_LINE_SIGNAL_LEVEL, value)
#define SDHC_DAT3_LINE_SIGNAL_LEVEL		_BV(23)
#define SDHC_DAT2_LINE_SIGNAL_LEVEL		_BV(22)
#define SDHC_DAT1_LINE_SIGNAL_LEVEL		_BV(21)
#define SDHC_DAT0_LINE_SIGNAL_LEVEL		_BV(20)
#endif
#define SDHC_WRITE_PROTECT_SWITCH_PIN_LEVEL	_BV(19)
#define SDHC_CARD_DETECT_PIN_LEVEL		_BV(18)
#define SDHC_CARD_STATE_STABLE			_BV(17)
#define SDHC_CARD_INSERTED			_BV(16)
#define SDHC_BUFFER_READ_ENABLE			_BV(11)
#define SDHC_BUFFER_WRITE_ENABLE		_BV(10)
#ifdef CONFIG_SDHC_SD
#define SDHC_READ_TRANSFER_ACTIVE		_BV(9)
#define SDHC_WRITE_TRANSFER_ACTIVE		_BV(8)
#endif
#ifdef CONFIG_SDHC_EMMC
#define SDHC_DAT74_LINE_SIGNAL_LEVEL_OFFSET	4
#define SDHC_DAT74_LINE_SIGNAL_LEVEL_MASK	REG_4BIT_MASK
#define SDHC_DAT74_LINE_SIGNAL_LEVEL(value)	\
	_GET_FV(SDHC_DAT74_LINE_SIGNAL_LEVEL, value)
#define SDHC_DAT7_LINE_SIGNAL_LEVEL		_BV(7)
#define SDHC_DAT6_LINE_SIGNAL_LEVEL		_BV(6)
#define SDHC_DAT5_LINE_SIGNAL_LEVEL		_BV(5)
#define SDHC_DAT4_LINE_SIGNAL_LEVEL		_BV(4)
#define SDHC_DAT_LINE_SIGNAL_LEVEL(value)	\
	(SDHC_DAT30_LINE_SIGNAL_LEVEL(value) |	\
	 (value) & SDHC_DAT74_LINE_SIGNAL_LEVEL_MASK)
#endif
#ifdef CONFIG_SDHC_UHSI
#define SDHC_RETUNING_REQUEST			_BV(3)
#endif
#ifdef CONFIG_SDHC_SD
#define SDHC_DAT_LINE_ACTIVE			_BV(2)
#define SDHC_COMMAND_INHIBIT_DAT		_BV(1)
#else
#define SDHC_COMMAND_INHIBIT_DAT		0
#endif
#define SDHC_COMMAND_INHIBIT_CMD		_BV(0)
#define SDHC_COMMAND_INHIBIT			\
	(SDHC_COMMAND_INHIBIT_DAT | SDHC_COMMAND_INHIBIT_CMD)

#define SDHCI_HOST_CONTROL	0x28
#define  SDHCI_CTRL_LED		_BV(0)
#define  SDHCI_CTRL_4BITBUS	_BV(1)
#define  SDHCI_CTRL_HISPD	_BV(2)
#define  SDHCI_CTRL_DMA_MASK	0x18
#define   SDHCI_CTRL_SDMA	0x00
#define   SDHCI_CTRL_ADMA1	0x08
#define   SDHCI_CTRL_ADMA32	0x10
#define   SDHCI_CTRL_ADMA64	0x18
#define  SDHCI_CTRL_8BITBUS	_BV(5)
#define  SDHCI_CTRL_CD_TEST_INS	_BV(6)
#define  SDHCI_CTRL_CD_TEST	_BV(7)

#define SDHCI_POWER_CONTROL	0x29
#define  SDHCI_POWER_ON		0x01
#define  SDHCI_POWER_180	0x0A
#define  SDHCI_POWER_300	0x0C
#define  SDHCI_POWER_330	0x0E

#define SDHCI_BLOCK_GAP_CONTROL	0x2A

#define SDHCI_WAKE_UP_CONTROL	0x2B
#define  SDHCI_WAKE_ON_INT	_BV(0)
#define  SDHCI_WAKE_ON_INSERT	_BV(1)
#define  SDHCI_WAKE_ON_REMOVE	_BV(2)

/* 2.2.15 Clock Control Register (Cat.C Offset 02Ch) */
#define SDHC_8BIT_DIVIDED_CLOCK_OFFSET		8
#define SDHC_8BIT_DIVIDED_CLOCK_MASK		REG_8BIT_MASK
#define SDHC_8BIT_DIVIDED_CLOCK(value)		\
	_SET_FV(SDHC_8BIT_DIVIDED_CLOCK, value)
#define SDHC_10BIT_DIVIDED_CLOCK_OFFSET		6
#define SDHC_10BIT_DIVIDED_CLOCK_MASK		REG_10BIT_MASK
#define SDHC_10BIT_DIVIDED_CLOCK(value)		\
	_SET_FV(SDHC_10BIT_DIVIDED_CLOCK, value)
#define SDHC_CLOCK_GENERATOR_SELECT		_BV(5)
#define SDHC_PLL_ENABLE				_BV(3)
#define SDHC_CLOCK_ENABLE			_BV(2)
#define SDHC_INTERNAL_CLOCK_STABLE		_BV(1)
#define SDHC_INTERNAL_CLOCK_ENABLE		_BV(0)

#define SDHCI_SOFTWARE_RESET	0x2F
#define  SDHCI_RESET_ALL	0x01
#define  SDHCI_RESET_CMD	0x02
#define  SDHCI_RESET_DATA	0x04

#ifdef CONFIG_SDHC_SPEC_4_00
#define SDHC_RESPONSE_ERROR		_BV(27)
#endif
#ifdef CONFIG_SDHC_UHSI
#define SDHC_TUNING_ERROR		_BV(26)
#endif
#define SDHC_ADMA_ERROR			_BV(25)
#ifdef CONFIG_SDHC_SD
#define SDHC_AUTO_CMD_ERROR		_BV(24)
#endif
#define SDHC_CURRENT_LIMIT_ERROR	_BV(23)
#ifdef CONFIG_SDHC_SD
#define SDHC_DATA_END_BIT_ERROR		_BV(22)
#define SDHC_DATA_CRC_ERROR		_BV(21)
#define SDHC_DATA_TIMEOUT_ERROR		_BV(20)
#define SDHC_COMMAND_INDEX_ERROR	_BV(19)
#define SDHC_COMMAND_END_BIT_ERROR	_BV(18)
#define SDHC_COMMAND_CRC_ERROR		_BV(17)
#define SDHC_COMMAND_TIMEOUT_ERROR	_BV(16)
#endif
#define SDHC_ERROR_INTERRUPT		_BV(15)
#ifdef CONFIG_SDHC_SPEC_4_10
#define SDHC_FX_EVENT			_BV(13)
#endif
#ifdef CONFIG_SDHC_UHSI
#define SDHC_RETUNING_EVENT		_BV(12)
#endif
#define SDHC_INT_C			_BV(11)
#define SDHC_INT_B			_BV(10)
#define SDHC_INT_A			_BV(9)
#define SDHC_CARD_INTERRUPT		_BV(8)
#define SDHC_CARD_REMOVAL		_BV(7)
#define SDHC_CARD_INSERTION		_BV(6)
#define SDHC_BUFFER_READ_READY		_BV(5)
#define SDHC_BUFFER_WRITE_READY		_BV(4)
#define SDHC_DMA_INTERRUPT		_BV(3)
#define SDHC_BLOCK_GAP_EVENT		_BV(2)
#define SDHC_TRANSFER_COMPLETE		_BV(1)
#define SDHC_COMMAND_COMPLETE		_BV(0)
#define SDHC_NORMAL_INTERRUPT_MASK	0x00007FFF
#define SDHC_ERROR_INTERRUPT_MASK	0xFFFF8000
#define SDHC_ALL_INTERRUPT_MASK		\
	(SDHC_NORMAL_INTERRUPT_MASK | SDHC_ERROR_INTERRUPT_MASK)
#define SDHC_TRANSFER_FAILURE					\
	(SDHC_COMMAND_CRC_ERROR | SDHC_COMMAND_END_BIT_ERROR |	\
         SDHC_DATA_CRC_ERROR | SDHC_DATA_END_BIT_ERROR)
#define SDHC_TRANSFER_TIMEOUT					\
	(SDHC_COMMAND_TIMEOUT_ERROR | SDHC_DATA_TIMEOUT_ERROR)
#define SDHC_TRANSFER_SUCCESS					\
	(SDHC_TRANSFER_COMPLETE | SDHC_COMMAND_COMPLETE)

/* 2.2.18 Normal Interrupt Status Register (Cat.C Offset 030h)
 * 2.2.20 Normal Interrupt Status Enable Register (Cat.C Offset 036h)
 * 2.2.22 Normal Interrupt Signal Enable Register (Cat.C Offset 038h)
 */
#define SDHC_NOR_ERROR_INTERRUPT	_BV(15)
#ifdef CONFIG_SDHC_SPEC_4_10
#define SDHC_NOR_FX_EVENT		_BV(13)
#endif
#ifdef CONFIG_SDHC_UHSI
#define SDHC_NOR_RETUNING_EVENT		_BV(12)
#endif
#define SDHC_NOR_INT_C			_BV(11)
#define SDHC_NOR_INT_B			_BV(10)
#define SDHC_NOR_INT_A			_BV(9)
#define SDHC_NOR_CARD_INTERRUPT		_BV(8)
#define SDHC_NOR_CARD_REMOVAL		_BV(7)
#define SDHC_NOR_CARD_INSERTION		_BV(6)
#define SDHC_NOR_BUFFER_READ_READY	_BV(5)
#define SDHC_NOR_BUFFER_WRITE_READY	_BV(4)
#define SDHC_NOR_DMA_INTERRUPT		_BV(3)
#define SDHC_NOR_BLOCK_GAP_EVENT	_BV(2)
#define SDHC_NOR_TRANSFER_COMPLETE	_BV(1)
#define SDHC_NOR_COMMAND_COMPLETE	_BV(0)
/* 2.2.19 Error Interrupt Status Register (Cat.C Offset 032h)
 * 2.2.21 Error Interrupt Status Enable Register (Cat.C Offset 036h)
 * 2.2.23 Error Interrupt Signal Enable Register (Cat.C Offset 03Ah)
 */
#ifdef CONFIG_SDHC_SPEC_4_00
#define SDHC_ERR_RESPONSE_ERROR		_BV(11)
#endif
#ifdef CONFIG_SDHC_UHSI
#define SDHC_ERR_TUNING_ERROR		_BV(10)
#endif
#define SDHC_ERR_ADMA_ERROR		_BV(9)
#ifdef CONFIG_SDHC_SD
#define SDHC_ERR_AUTO_CMD_ERROR		_BV(8)
#endif
#define SDHC_ERR_CURRENT_LIMIT_ERROR	_BV(7)
#ifdef CONFIG_SDHC_SD
#define SDHC_ERR_DATA_END_BIT_ERROR	_BV(6)
#define SDHC_ERR_DATA_CRC_ERROR		_BV(5)
#define SDHC_ERR_DATA_TIMEOUT_ERROR	_BV(4)
#define SDHC_ERR_COMMAND_INDEX_ERROR	_BV(3)
#define SDHC_ERR_COMMAND_END_BIT_ERROR	_BV(2)
#define SDHC_ERR_COMMAND_CRC_ERROR	_BV(1)
#define SDHC_ERR_COMMAND_TIMEOUT_ERROR	_BV(0)
#endif

#define SDHC_COMMAND_MASK					\
	(SDHC_COMMAND_COMPLETE | SDHC_COMMAND_TIMEOUT_ERROR |	\
	 SDHC_COMMAND_CRC_ERROR | SDHC_COMMAND_END_BIT_ERROR |	\
	 SDHC_COMMAND_INDEX_ERROR)
#define SDHC_TRANSFER_MASK					\
	(SDHC_TRANSFER_COMPLETE | SDHC_DMA_INTERRUPT |		\
	 SDHC_BUFFER_READ_READY | SDHC_BUFFER_WRITE_READY |	\
	 SDHC_DATA_TIMEOUT_ERROR | SDHC_DATA_CRC_ERROR |	\
	 SDHC_DATA_END_BIT_ERROR | SDHC_ADMA_ERROR)
#define SDHC_CARD_DETECTION_MASK				\
	(SDHC_CARD_INSERTION | SDHC_CARD_REMOVAL)

/* 3E-3F reserved */

#define SDHCI_CAPABILITIES	0x40
#define  SDHCI_TIMEOUT_CLK_MASK	0x0000003F
#define  SDHCI_TIMEOUT_CLK_SHIFT 0
#define  SDHCI_TIMEOUT_CLK_UNIT	0x00000080
#define  SDHCI_CLOCK_BASE_MASK	0x00003F00
#define  SDHCI_CLOCK_V3_BASE_MASK	0x0000FF00
#define  SDHCI_CLOCK_BASE_SHIFT	8
#define  SDHCI_MAX_BLOCK_MASK	0x00030000
#define  SDHCI_MAX_BLOCK_SHIFT  16
#define  SDHCI_CAN_DO_8BIT	_BV(18)
#define  SDHCI_CAN_DO_ADMA2	_BV(19)
#define  SDHCI_CAN_DO_ADMA1	_BV(20)
#define  SDHCI_CAN_DO_HISPD	_BV(21)
#define  SDHCI_CAN_DO_SDMA	_BV(22)
#define  SDHCI_CAN_VDD_330	_BV(24)
#define  SDHCI_CAN_VDD_300	_BV(25)
#define  SDHCI_CAN_VDD_180	_BV(26)
#define  SDHCI_CAN_64BIT	_BV(28)

#define SDHCI_CAPABILITIES_1	0x44
#define  SDHCI_CLOCK_MUL_MASK	0x00FF0000
#define  SDHCI_CLOCK_MUL_SHIFT	16

#define SDHCI_MAX_CURRENT	0x48

/* 4C-4F reserved for more max current */

#define SDHCI_SET_ACMD12_ERROR	0x50
#define SDHCI_SET_INT_ERROR	0x52

#define SDHCI_ADMA_ERROR	0x54

/* 55-57 reserved */

#define SDHCI_ADMA_ADDRESS	0x58

/* 60-FB reserved */

#define SDHCI_SLOT_INT_STATUS	0xFC

#define SDHCI_HOST_VERSION	0xFE
#define  SDHCI_VENDOR_VER_MASK	0xFF00
#define  SDHCI_VENDOR_VER_SHIFT	8
#define  SDHCI_SPEC_VER_MASK	0x00FF
#define  SDHCI_SPEC_VER_SHIFT	0
#define   SDHCI_SPEC_100	0
#define   SDHCI_SPEC_200	1
#define   SDHCI_SPEC_300	2
#define   SDHCI_SPEC_400	3
#define   SDHCI_SPEC_410	4
#define   SDHCI_SPEC_420	5

#define SDHCI_GET_VERSION(x)	((x)->version & SDHCI_SPEC_VER_MASK)

/* Other definitions */
#define SDHCI_MAX_DIV_SPEC_200	256
#define SDHCI_MAX_DIV_SPEC_300	2046

#define SDHC_INTERNAL_CLOCK_STABLE_TOUT_MS	150

/* Host SDMA buffer boundary.
 * Valid values from 4K to 512K in powers of 2.
 */
#define SDHCI_DEFAULT_BOUNDARY_SIZE	(512 * 1024)
#define SDHCI_DEFAULT_BOUNDARY_ARG	(7)

struct sdhci_host {
	void *ioaddr;
	unsigned int version;
	unsigned int max_clk;   /* Maximum Base Clock frequency */
	unsigned int clk_mul;   /* Clock Multiplier value */
#if 0
	struct gpio_desc pwr_gpio;	/* Power GPIO */
	struct gpio_desc cd_gpio;	/* Card Detect GPIO */
#endif
};

#define sdhci_writel(host, val, reg)	\
	__raw_writel(val, (uintptr_t)(host->ioaddr) + reg)
#define sdhci_writew(host, val, reg)	\
	__raw_writew(val, (uintptr_t)(host->ioaddr) + reg)
#define sdhci_writeb(host, val, reg)	\
	__raw_writeb(val, (uintptr_t)(host->ioaddr) + reg)
#define sdhci_readl(host, reg)		\
	__raw_readl((uintptr_t)(host->ioaddr) + reg)
#define sdhci_readw(host, reg)		\
	__raw_readw((uintptr_t)(host->ioaddr) + reg)
#define sdhci_readb(host, reg)		\
	__raw_readb((uintptr_t)(host->ioaddr) + reg)

#define __sdhc_enable_irq(mmc, irqs)	\
	__raw_setl(irqs, SDHC_INTERRUPT_ENABLE(mmc))
#define __sdhc_disable_irq(mmc, irqs)	\
	__raw_clearl(irqs, SDHC_INTERRUPT_ENABLE(mmc))
#ifdef SYS_REALTIME
#define sdhc_enable_irq(mmc, irqs)		\
	do {					\
		__sdhc_enable_irq(mmc, irqs);	\
		sdhc_mask_irq(mmc, irqs);	\
	} while (0)
#define sdhc_disable_irq(mmc, irqs)		\
	__sdhc_disable_irq(mmc, irqs)
#else
#define sdhc_enable_irq(mmc, irqs)		\
	do {					\
		__sdhc_enable_irq(mmc, irqs);	\
		sdhc_unmask_irq(mmc, irqs);	\
	} while (0)
#define sdhc_disable_irq(mmc, irqs)		\
	do {					\
		__sdhc_disable_irq(mmc, irqs);	\
		sdhc_mask_irq(mmc, irqs);	\
	} while (0)
#endif
#define sdhc_mask_irq(mmc, irqs)		\
	__raw_clearl(irqs, SDHC_INTERRUPT_SIGNAL(mmc))
#define sdhc_unmask_irq(mmc, irqs)		\
	__raw_setl(irqs, SDHC_INTERRUPT_SIGNAL(mmc))
#define sdhc_clear_irq(mmc, irqs)		\
	__raw_setl(irqs, SDHC_INTERRUPT_STATUS(mmc))
#define sdhc_irq_status(mmc)			\
	__raw_readl(SDHC_INTERRUPT_STATUS(mmc))
#define sdhc_mask_all_irqs(mmc)			\
	__raw_writel(0, SDHC_INTERRUPT_SIGNAL(mmc))
#define sdhc_clear_all_irqs(mmc)		\
	__raw_writel(SDHC_ALL_INTERRUPT_MASK, SDHC_INTERRUPT_STATUS(mmc))

#define sdhc_state_present(mmc, state)		\
	(__raw_readl(SDHC_PRESENT_STATE(mmc)) & (state))

#define sdhc_set_clock(mmc, step)		\
	__raw_setw(step, SDHC_CLOCK_CONTROL(mmc))
#define sdhc_clear_clock(mmc, step)		\
	__raw_clearw(step, SDHC_CLOCK_CONTROL(mmc))

void sdhci_send_command(uint8_t cmd, uint32_t arg);
void sdhci_recv_response(uint8_t *resp, uint8_t size);
void sdhci_detect_card(void);
bool sdhci_set_clock(uint32_t clock);
void sdhci_set_width(uint8_t width);
void sdhci_init(uint32_t f_min, uint32_t f_max);
void sdhci_irq_init(void);
void sdhci_irq_poll(void);
void sdhci_start_transfer(void);
void sdhci_stop_transfer(void);

#endif /* __SDHCI_H_INCLUDE__ */
