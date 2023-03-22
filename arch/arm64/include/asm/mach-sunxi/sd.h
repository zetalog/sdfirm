#ifndef __SD_SUNXI_H_INCLUDE__
#define __SD_SUNXI_H_INCLUDE__

#include <target/generic.h>
#include <target/clk.h>

#define SMHC0_BASE			UL(0x01C0F000)
#define SMHC1_BASE			UL(0x01C10000)
#define SMHC2_BASE			UL(0x01C11000)

#define SMHC_BASE(n)			(SMHC0_BASE + ((n) << 12))

#ifdef CONFIG_SMHC_SUNXI
#include <asm/mach/smhc.h>
#ifndef ARCH_HAVE_SD
#define ARCH_HAVE_SD 1
#else
#error "Multiple SD controller defined"
#endif
#endif

#define SD_CLASS2	1
#define SD_CLASS5	1
#define SD_CLASS8	1
#define SD_CLASS10	1

#ifdef CONFIG_SMHC_SUNXI_BOOT_0
#define MMC_BOOT_ID		0
#define SD_PHY_VERSION		20
#define SDIO_PHY_VERSION	0
#define MMC_PHY_VERSION		0
#endif
#ifdef CONFIG_SMHC_SUNXI_BOOT_1
#define MMC_BOOT_ID		1
#define SD_PHY_VERSION		30
#define SDIO_PHY_VERSION	30
#define MMC_PH_VERSION		0
#endif
#ifdef CONFIG_SMHC_SUNXI_BOOT_2
#define MMC_BOOT_ID		2
#define SD_PHY_VERSION		30
#define SDIO_PHY_VERSION	0
#define MMC_PH_VERSION		51
#endif

#define SUNXI_MMC_CLK_POWERSAVE		(0x1 << 17)
#define SUNXI_MMC_CLK_ENABLE		(0x1 << 16)
#define SUNXI_MMC_CLK_DIVIDER_MASK	(0xff)

#define SUNXI_MMC_GCTRL_SOFT_RESET	(0x1 << 0)
#define SUNXI_MMC_GCTRL_FIFO_RESET	(0x1 << 1)
#define SUNXI_MMC_GCTRL_DMA_RESET	(0x1 << 2)
#define SUNXI_MMC_GCTRL_RESET		(SUNXI_MMC_GCTRL_SOFT_RESET|\
					 SUNXI_MMC_GCTRL_FIFO_RESET|\
					 SUNXI_MMC_GCTRL_DMA_RESET)
#define SUNXI_MMC_GCTRL_DMA_ENABLE	(0x1 << 5)
#define SUNXI_MMC_GCTRL_ACCESS_BY_AHB   (0x1 << 31)

#define SUNXI_MMC_CMD_RESP_EXPIRE	(0x1 << 6)
#define SUNXI_MMC_CMD_LONG_RESPONSE	(0x1 << 7)
#define SUNXI_MMC_CMD_CHK_RESPONSE_CRC	(0x1 << 8)
#define SUNXI_MMC_CMD_DATA_EXPIRE	(0x1 << 9)
#define SUNXI_MMC_CMD_WRITE		(0x1 << 10)
#define SUNXI_MMC_CMD_AUTO_STOP		(0x1 << 12)
#define SUNXI_MMC_CMD_WAIT_PRE_OVER	(0x1 << 13)
#define SUNXI_MMC_CMD_SEND_INIT_SEQ	(0x1 << 15)
#define SUNXI_MMC_CMD_UPCLK_ONLY	(0x1 << 21)
#define SUNXI_MMC_CMD_START		(0x1 << 31)

#define SUNXI_MMC_RINT_RESP_ERROR		(0x1 << 1)
#define SUNXI_MMC_RINT_COMMAND_DONE		(0x1 << 2)
#define SUNXI_MMC_RINT_DATA_OVER		(0x1 << 3)
#define SUNXI_MMC_RINT_TX_DATA_REQUEST		(0x1 << 4)
#define SUNXI_MMC_RINT_RX_DATA_REQUEST		(0x1 << 5)
#define SUNXI_MMC_RINT_RESP_CRC_ERROR		(0x1 << 6)
#define SUNXI_MMC_RINT_DATA_CRC_ERROR		(0x1 << 7)
#define SUNXI_MMC_RINT_RESP_TIMEOUT		(0x1 << 8)
#define SUNXI_MMC_RINT_DATA_TIMEOUT		(0x1 << 9)
#define SUNXI_MMC_RINT_VOLTAGE_CHANGE_DONE	(0x1 << 10)
#define SUNXI_MMC_RINT_FIFO_RUN_ERROR		(0x1 << 11)
#define SUNXI_MMC_RINT_HARD_WARE_LOCKED		(0x1 << 12)
#define SUNXI_MMC_RINT_START_BIT_ERROR		(0x1 << 13)
#define SUNXI_MMC_RINT_AUTO_COMMAND_DONE	(0x1 << 14)
#define SUNXI_MMC_RINT_END_BIT_ERROR		(0x1 << 15)
#define SUNXI_MMC_RINT_SDIO_INTERRUPT		(0x1 << 16)
#define SUNXI_MMC_RINT_CARD_INSERT		(0x1 << 30)
#define SUNXI_MMC_RINT_CARD_REMOVE		(0x1 << 31)
#define SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT      \
	(SUNXI_MMC_RINT_RESP_ERROR |		\
	 SUNXI_MMC_RINT_RESP_CRC_ERROR |	\
	 SUNXI_MMC_RINT_DATA_CRC_ERROR |	\
	 SUNXI_MMC_RINT_RESP_TIMEOUT |		\
	 SUNXI_MMC_RINT_DATA_TIMEOUT |		\
	 SUNXI_MMC_RINT_VOLTAGE_CHANGE_DONE |	\
	 SUNXI_MMC_RINT_FIFO_RUN_ERROR |	\
	 SUNXI_MMC_RINT_HARD_WARE_LOCKED |	\
	 SUNXI_MMC_RINT_START_BIT_ERROR |	\
	 SUNXI_MMC_RINT_END_BIT_ERROR) /* 0xbfc2 */
#define SUNXI_MMC_RINT_INTERRUPT_DONE_BIT	\
	(SUNXI_MMC_RINT_AUTO_COMMAND_DONE |	\
	 SUNXI_MMC_RINT_DATA_OVER |		\
	 SUNXI_MMC_RINT_COMMAND_DONE |		\
	 SUNXI_MMC_RINT_VOLTAGE_CHANGE_DONE)

#define SUNXI_MMC_STATUS_RXWL_FLAG		(0x1 << 0)
#define SUNXI_MMC_STATUS_TXWL_FLAG		(0x1 << 1)
#define SUNXI_MMC_STATUS_FIFO_EMPTY		(0x1 << 2)
#define SUNXI_MMC_STATUS_FIFO_FULL		(0x1 << 3)
#define SUNXI_MMC_STATUS_CARD_PRESENT		(0x1 << 8)
#define SUNXI_MMC_STATUS_CARD_DATA_BUSY		(0x1 << 9)
#define SUNXI_MMC_STATUS_DATA_FSM_BUSY		(0x1 << 10)

#define SUNXI_MMC_NTSR_MODE_SEL_NEW		(0x1 << 31)

#define SUNXI_MMC_IDMAC_RESET		(0x1 << 0)
#define SUNXI_MMC_IDMAC_FIXBURST	(0x1 << 1)
#define SUNXI_MMC_IDMAC_ENABLE		(0x1 << 7)

#define SUNXI_MMC_IDIE_TXIRQ		(0x1 << 0)
#define SUNXI_MMC_IDIE_RXIRQ		(0x1 << 1)

#define SUNXI_MMC_COMMON_CLK_GATE		(1 << 16)
#define SUNXI_MMC_COMMON_RESET			(1 << 18)

/* Capabilities */
#define SUNXI_SD_CLK_MAX	52000000
#define SUNXI_SD_CLK_MIN	400000

extern uint32_t *sunxi_sd_clk_cfg[];

void sunxi_sd_detect_card(uint8_t mmc_no);
void sunxi_sd_set_clock(uint8_t mmc_no, uint32_t clock);
void sunxi_sd_set_width(uint8_t mmc_no, uint8_t width);
bool sunxi_sd_card_busy(uint8_t mmc_no);
void sunxi_sd_ctrl_init(uint8_t mmc_no);

#define mmc_hw_slot_reset()		do { } while (0)
#define mmc_hw_card_detect()		sunxi_sd_detect_card(MMC_BOOT_ID)
#define mmc_hw_set_clock(clock)		sunxi_sd_set_clock(MMC_BOOT_ID, clock)
#define mmc_hw_set_width(width)		sunxi_sd_set_width(MMC_BOOT_ID, width)
#define mmc_hw_card_busy()		sunxi_sd_card_busy(MMC_BOOT_ID)
#define mmc_hw_ctrl_init()		sunxi_sd_ctrl_init(MMC_BOOT_ID)
void mmc_hw_send_command(uint8_t cmd, uint32_t arg);
void mmc_hw_recv_response(uint8_t *resp, uint8_t size);
void mmc_hw_tran_data(uint8_t *dat, uint32_t len, uint16_t cnt);

#ifdef SYS_REALTIME
void sunxi_sd_irq_poll(void);
#define mmc_hw_irq_poll()		sunxi_sd_irq_poll()
#else
void sunxi_sd_irq_init(void);
#define mmc_hw_irq_init()		sunxi_sd_irq_init()
#endif

#endif /* __SD_SUNXI_H_INCLUDE__ */
