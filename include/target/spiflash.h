#ifndef __SPIFLASH_H_INCLUDE__
#define __SPIFLASH_H_INCLUDE__

#include <target/mtd.h>
#include <target/spi.h>

typedef uint8_t spiflash_bid_t;

#include <driver/spiflash.h>

/* Status registers 1 */
#define SF_BUSY		_BV(0) /* S0: Busy */
#define SF_WEL		_BV(1) /* S1: Write Enable Latch */
#define SF_BP0		_BV(2) /* S2: Block Protect */
#define SF_BP1		_BV(3) /* S3: Block Protect */
#define SF_BP2		_BV(4) /* S4: Block Protect */
#define SF_TB		_BV(5) /* S5: Top/Bottom Block Protect */
#define SF_SEC		_BV(6) /* S6: Sector/Block Protect */
#define SF_SRP0		_BV(7) /* S7: Status Register Protect */
/* Status registers 2 */
#define SF_SRP1		_BV(0) /* S8: Status Register Protect */
#define SF_QE		_BV(1) /* S9: Quad Enable */

/* Instruction set */
#define SF_WRITE_ENABLE			0x06 /* Write Enable */
#define SF_WRITE_DISABLE		0x04 /* Write Disable */
#define SF_READ_STATUS_1		0x05 /* Read Status Reg-1 S0-S7 */
#define SF_READ_STATUS_2		0x35 /* Read Status Reg-2 S8-S15 */
#define SF_WRITE_STATUS			0x01 /* Write Status Reg */
#define SF_PAGE_PROGRAM			0x02 /* Page Program */
#define SF_QUAD_PAGE_PROGRAM		0x32 /* Quad Page Program */
#define SF_BLOCK_ERASE_64K		0xD8 /* Block Erase (64KB) */
#define SF_BLOCK_ERASE_32K		0x52 /* Block Erase (32KB) */
#define SF_SECTOR_ERASE_4K		0x20 /* Sector Erase (4KB) */
#define SF_CHIP_ERASE			0xC7 /* Chip Erase */
/* #define SF_CHIP_ERASE			0x60 */
#define SF_ERASE_SUSPEND		0x75 /* Erase Suspend */
#define SF_ERASE_RESUME			0x7A /* Erase Resume */
#define SF_POWER_DOWN			0xB9 /* Power-down */
#define SF_HIGH_PERFORMANCE_MODE	0xA3 /* High Performance Mode */
#define SF_MODE_BIT_RESET		0xFF /* Mode Bit Reset */
#define SF_RELEASE_POWER_DOWN		0xAB /* Release Power down or HPM/Device ID */
#define SF_VENDOR_DEVICE_ID		0x90 /* Manufacturer/Device ID */
#define SF_READ_UNIQUE_ID		0x4B /* Read Unique ID */
#define SF_JEDEC_ID			0x9F /* JEDEC ID */
#define SF_READ_DATA			0x03 /* Read Data */
#define SF_FAST_READ			0x0B /* Fast Read */
#define SF_FAST_READ_DUAL_O		0x3B /* Fast Read Dual Output */
#define SF_FAST_READ_DUAL_IO		0xBB /* Fast Read Dual I/O */
#define SF_FAST_READ_QUAD_O		0x6B /* Fast Read Quad Output */
#define SF_FAST_READ_QUAD_IO		0xEB /* Fast Read Quad I/O */

#ifdef CONFIG_SPIFLASH_MAX_BANKS
#define SPIFLASH_MAX_BANKS			CONFIG_SPIFLASH_MAX_BANKS
#else
#define SPIFLASH_MAX_BANKS			1
#endif

#define INVALID_SPIFLASH_BID			SPIFLASH_MAX_BANKS
void spiflash_select(spiflash_bid_t bid);
void spiflash_deselect(void);

#endif /* __SPIFLASH_H_INCLUDE__ */
