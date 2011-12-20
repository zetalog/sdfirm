#ifndef __DATAFLASH_H_INCLUDE__
#define __DATAFLASH_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/spi.h>
#include <target/mtd.h>

typedef uint8_t dataflash_bid_t;

#include <driver/dataflash.h>

/* Status Register Bit Definitions (MSB Shift Out)
 * +----------+---------+---+---+---+---+-----+-----+
 * |    7     |    6    | 5 | 4 | 3 | 2 |  1  |  0  |
 * +----------+---------+---+---+---+---+-----+-----+
 * | RDY/BUSY | Compare | Density Code  | Reserved  |
 * +----------+---------+---+---+---+---+-----+-----+
 */
#define DATAFLASH_STATUS_DENSITY_MASK		0x3C
#define DATAFLASH_STATUS_RDY_BUSY		0x80
#define DATAFLASH_STATUS_COMPARE		0x40

/* Commands for Inactive Clock Polarity SCK Modes */
#define DATAFLASH_ICP_ARRAY_READ		0x68
#define DATAFLASH_ICP_PAGE_READ			0x52
#define DATAFLASH_ICP_BUFFER1_READ		0x54
#define DATAFLASH_ICP_BUFFER2_READ		0x56
#define DATAFLASH_ICP_STATUS_READ		0x57
/* Commands for SPI SCK Modes */
#define DATAFLASH_SPI_ARRAY_READ		0xE8
#define DATAFLASH_SPI_ARRAY_HIGH_READ		0x0B
#define DATAFLASH_SPI_PAGE_READ			0xD2
#define DATAFLASH_SPI_BUFFER1_READ		0xD4
#define DATAFLASH_SPI_BUFFER2_READ		0xD6
#define DATAFLASH_SPI_STATUS_READ		0xD7

#define DATAFLASH_ANY_BUFFER1_WRITE		0x84
#define DATAFLASH_ANY_BUFFER2_WRITE		0x87
#define DATAFLASH_ANY_BUFFER1_PAGE_PRG_ERASE	0x83
#define DATAFLASH_ANY_BUFFER2_PAGE_PRG_ERASE	0x86
#define DATAFLASH_ANY_BUFFER1_PAGE_PRG		0x88
#define DATAFLASH_ANY_BUFFER2_PAGE_PRG		0x89
#define DATAFLASH_ANY_PAGE_ERASE		0x81
#define DATAFLASH_ANY_BLOCK_ERASE		0x50
#define DATAFLASH_ANY_PAGE_BUFFER1_PRG		0x82
#define DATAFLASH_ANY_PAGE_BUFFER2_PRG		0x85
#define DATAFLASH_ANY_PAGE_BUFFER1_XFR		0x53
#define DATAFLASH_ANY_PAGE_BUFFER2_XFR		0x55
#define DATAFLASH_ANY_PAGE_BUFFER1_CMP		0x60
#define DATAFLASH_ANY_PAGE_BUFFER2_CMP		0x61
#define DATAFLASH_ANY_PAGE_REWRITE_BUFFER1	0x58
#define DATAFLASH_ANY_PAGE_REWRITE_BUFFER2	0x59
#define DATAFLASH_MAIN_MEMORY_PAGE_READ		0xD2

#ifdef CONFIG_DATAFLASH_MAX_BANKS
#define DATAFLASH_MAX_BANKS			CONFIG_DATAFLASH_MAX_BANKS
#else
#define DATAFLASH_MAX_BANKS			2
#endif

#define INVALID_DATAFLASH_BID			DATAFLASH_MAX_BANKS

uint8_t dataflash_status(void);
uint8_t dataflash_waitready(void);
mtd_t dataflash_register_bank(uint8_t chip);
void dataflash_select(dataflash_bid_t bid);
void dataflash_deselect(void);

#endif /* __DATAFLASH_H_INCLUDE__ */
