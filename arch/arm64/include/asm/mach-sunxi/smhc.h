#ifndef __SMHC_SUNXI_H_INCLUDE__
#define __SMHC_SUNXI_H_INCLUDE__

#define SMHC_REG(n, offset)	(SMHC_BASE(0) + (offset))

struct sunxi_mmc {
	uint32_t gctrl;		/* 0x00 global control */
	uint32_t clkcr;		/* 0x04 clock control */
	uint32_t timeout;	/* 0x08 time out */
	uint32_t width;		/* 0x0c bus width */
	uint32_t blksz;		/* 0x10 block size */
	uint32_t bytecnt;	/* 0x14 byte count */
	uint32_t cmd;		/* 0x18 command */
	uint32_t arg;		/* 0x1c argument */
	uint32_t resp0;		/* 0x20 response 0 */
	uint32_t resp1;		/* 0x24 response 1 */
	uint32_t resp2;		/* 0x28 response 2 */
	uint32_t resp3;		/* 0x2c response 3 */
	uint32_t imask;		/* 0x30 interrupt mask */
	uint32_t mint;		/* 0x34 masked interrupt status */
	uint32_t rint;		/* 0x38 raw interrupt status */
	uint32_t status;	/* 0x3c status */
	uint32_t ftrglevel;	/* 0x40 FIFO threshold watermark*/
	uint32_t funcsel;	/* 0x44 function select */
	uint32_t cbcr;		/* 0x48 CIU byte count */
	uint32_t bbcr;		/* 0x4c BIU byte count */
	uint32_t dbgc;		/* 0x50 debug enable */
	uint32_t res0;		/* 0x54 reserved */
	uint32_t a12a;		/* 0x58 Auto command 12 argument */
	uint32_t ntsr;		/* 0x5c	New timing set register */
	uint32_t res1[8];
	uint32_t dmac;		/* 0x80 internal DMA control */
	uint32_t dlba;		/* 0x84 internal DMA descr list base address */
	uint32_t idst;		/* 0x88 internal DMA status */
	uint32_t idie;		/* 0x8c internal DMA interrupt enable */
	uint32_t chda;		/* 0x90 */
	uint32_t cbda;		/* 0x94 */
	uint32_t res2[26];
#if defined(CONFIG_SUN50I) || defined(CONFIG_SUN8I) || defined(CONFIG_SUN6I)
	uint32_t res3[64];
#endif
	uint32_t fifo;		/* 0x100 / 0x200 FIFO access address */
};
#define SUNXI_SMHC(n)		((struct sunxi_mmc *)SMHC_BASE(n))

#endif
