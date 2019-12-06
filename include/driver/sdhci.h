#ifndef __SDHCI_H_INCLUDE__
#define __SDHCI_H_INCLUDE__

#include <target/arch.h>

/*
 * Controller registers
 */

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

#define SDHCI_PRESENT_STATE	0x24
#define  SDHCI_CMD_INHIBIT	_BV(0)
#define  SDHCI_DATA_INHIBIT	_BV(1)
#define  SDHCI_DOING_WRITE	_BV(8)
#define  SDHCI_DOING_READ	_BV(9)
#define  SDHCI_SPACE_AVAILABLE	_BV(10)
#define  SDHCI_DATA_AVAILABLE	_BV(11)
#define  SDHCI_CARD_PRESENT	_BV(16)
#define  SDHCI_CARD_STATE_STABLE	_BV(17)
#define  SDHCI_CARD_DETECT_PIN_LEVEL	_BV(18)
#define  SDHCI_WRITE_PROTECT	_BV(19)

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

#define SDHCI_CLOCK_CONTROL	0x2C
#define  SDHCI_DIVIDER_SHIFT	8
#define  SDHCI_DIVIDER_HI_SHIFT	6
#define  SDHCI_DIV_MASK	0xFF
#define  SDHCI_DIV_MASK_LEN	8
#define  SDHCI_DIV_HI_MASK	0x300
#define  SDHCI_PROG_CLOCK_MODE  _BV(5)
#define  SDHCI_CLOCK_CARD_EN	_BV(2)
#define  SDHCI_CLOCK_INT_STABLE	_BV(1)
#define  SDHCI_CLOCK_INT_EN	_BV(0)

#define SDHCI_TIMEOUT_CONTROL	0x2E

#define SDHCI_SOFTWARE_RESET	0x2F
#define  SDHCI_RESET_ALL	0x01
#define  SDHCI_RESET_CMD	0x02
#define  SDHCI_RESET_DATA	0x04

#define SDHCI_INT_STATUS	0x30
#define SDHCI_INT_ENABLE	0x34
#define SDHCI_SIGNAL_ENABLE	0x38
#define  SDHCI_INT_RESPONSE	_BV(0)
#define  SDHCI_INT_DATA_END	_BV(1)
#define  SDHCI_INT_DMA_END	_BV(3)
#define  SDHCI_INT_SPACE_AVAIL	_BV(4)
#define  SDHCI_INT_DATA_AVAIL	_BV(5)
#define  SDHCI_INT_CARD_INSERT	_BV(6)
#define  SDHCI_INT_CARD_REMOVE	_BV(7)
#define  SDHCI_INT_CARD_INT	_BV(8)
#define  SDHCI_INT_ERROR	_BV(15)
#define  SDHCI_INT_TIMEOUT	_BV(16)
#define  SDHCI_INT_CRC		_BV(17)
#define  SDHCI_INT_END_BIT	_BV(18)
#define  SDHCI_INT_INDEX	_BV(19)
#define  SDHCI_INT_DATA_TIMEOUT	_BV(20)
#define  SDHCI_INT_DATA_CRC	_BV(21)
#define  SDHCI_INT_DATA_END_BIT	_BV(22)
#define  SDHCI_INT_BUS_POWER	_BV(23)
#define  SDHCI_INT_ACMD12ERR	_BV(24)
#define  SDHCI_INT_ADMA_ERROR	_BV(25)

#define  SDHCI_INT_NORMAL_MASK	0x00007FFF
#define  SDHCI_INT_ERROR_MASK	0xFFFF8000

#define  SDHCI_INT_CMD_MASK	(SDHCI_INT_RESPONSE | SDHCI_INT_TIMEOUT | \
		SDHCI_INT_CRC | SDHCI_INT_END_BIT | SDHCI_INT_INDEX)
#define  SDHCI_INT_DATA_MASK	(SDHCI_INT_DATA_END | SDHCI_INT_DMA_END | \
		SDHCI_INT_DATA_AVAIL | SDHCI_INT_SPACE_AVAIL | \
		SDHCI_INT_DATA_TIMEOUT | SDHCI_INT_DATA_CRC | \
		SDHCI_INT_DATA_END_BIT | SDHCI_INT_ADMA_ERROR)
#define SDHCI_INT_ALL_MASK	((unsigned int)-1)

#define SDHCI_ACMD12_ERR	0x3C

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

#define SDHCI_GET_VERSION(x) (x->version & SDHCI_SPEC_VER_MASK)

/*
 * End of controller registers.
 */

#define SDHCI_MAX_DIV_SPEC_200	256
#define SDHCI_MAX_DIV_SPEC_300	2046

/*
 * quirks
 */
#define SDHCI_QUIRK_32BIT_DMA_ADDR	(1 << 0)
#define SDHCI_QUIRK_REG32_RW		(1 << 1)
#define SDHCI_QUIRK_BROKEN_R1B		(1 << 2)
#define SDHCI_QUIRK_NO_HISPD_BIT	(1 << 3)
#define SDHCI_QUIRK_BROKEN_VOLTAGE	(1 << 4)
#define SDHCI_QUIRK_WAIT_SEND_CMD	(1 << 6)
#define SDHCI_QUIRK_USE_WIDE8		(1 << 8)

/* to make gcc happy */
struct sdhci_host;

/*
 * Host SDMA buffer boundary. Valid values from 4K to 512K in powers of 2.
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

void sdhci_send_command(uint8_t cmd, uint32_t arg);
void sdhci_recv_response(uint8_t *resp, uint8_t size);
bool sdhci_detect_card(void);
bool sdhci_set_clock(uint32_t clock);
void sdhci_set_width(uint8_t width);
void sdhci_init(uint32_t f_max, uint32_t f_min);
void sdhci_start(void);

#endif /* __SDHCI_H_INCLUDE__ */
