#ifndef __FLASH_LM3S9B92_H_INCLUDE__
#define __FLASH_LM3S9B92_H_INCLUDE__

#ifndef ARCH_HAVE_FLASH
#define ARCH_HAVE_FLASH		1
#else
#error "Multiple Flash controller defined"
#endif

#include <target/config.h>
#include <target/mtd.h>
#include <asm/reg.h>

/*=========================================================================
 * flash registers
 *=======================================================================*/
/* Flash Memory Address */
#define FMA			FLASH(0x000)

/* Flash Memory Data */
#define FMD			FLASH(0x004)

/* Flash Memory Control */
#define FMC			FLASH(0x008)
#define COMT			3
#define MERASE			2
#define ERASE			1
#define WRITE			0
/* Flash Memory Control 2 */
#define FMC2			FLASH(0x020)
#define WRBUF			0
/* FMC/FMC2 Access Protection */
#define FLASH_WRITE_KEY		0xA442
#define WRKEY			16

/* Flash Controller Raw Interrupt Status */
#define FCRIS			FLASH(0x00C)
/* Flash Controller Interrupt Mask */
#define FCIM			FLASH(0x010)
/* Flash Controller Masked Interrupt Status and Clear */
#define FCMISC			FLASH(0x014)
#define PI			1
#define AI			0

/* Flash Write Buffer Valid */
#define FWBVAL			FLASH(0x030)

/* Flash Write Buffer n(0-31) */
#define FWB(n)			FLASH(0x100+n*4)

/* Flash Control */
#define FCTL			FLASH(0x0F8)
#define USDACK			1
#define USDREQ			0

/* ROM Control */
#define RMCTL			SYSTEM(0x0F0)
#define BA			0

/* Flash Memory Protection Read Enable 0 */
#define FMPRE0			SYSTEM(0x130)
/* Flash Memory Protection Program Enable 0 */
#define FMPPE0			SYSTEM(0x134)
/* Flash Memory Protection Read Enable n(0-3) */
#define FMPRE(n)		SYSTEM(0x200+n*4)
/* Flash Memory Protection Program Enable n(0-3) */
#define FMPPE(n)		SYSTEM(0x400+n*4)
#define FMPRE_COMT(n)		(0x00000000+n*2)
#define FMPPE_COMT(n)		(0x00000001+n*2)

/* USec Reload */
/* RW: Number of Processor Clocks per Micro-Second */
#define USECRL			SYSTEM(0x140)

/* Non-Volatile Memory Information */
#define NVMSTAT			SYSTEM(0x1A0)
#define NVFWB			0

/* Boot Configuration */
#define BOOTCFG			SYSTEM(0x1D0)
#define NW			31
#define PORT			13
#define PIN			10
#define POL			9
#define EN			8
#define DBG1			1
#define DBG0			0
#define BOOT_PORT_MASK		0x07
#define BOOT_PIN_MASK		0x07
#define BOOTCFG_COMT		0x75100000

/* User Register n(0-3) */
#define USER_REG(n)		SYSTEM(0x1E0+n*4)
#define USER_REG_COMT(0)	(0x80000000+n)

/*=========================================================================
 * flash functions
 *=======================================================================*/
boolean flash_hw_access_failure(void);
uint32_t flash_hw_get_user(uint8_t n);
void flash_hw_set_user(uint32_t val);

void flash_hw_op_perform(uint32_t addr, uint8_t op);
#define flash_hw_commit_user(n)		flash_hw_op_perform(USER_REG_COMT(n), COMT)

/*=========================================================================
 * flash driver
 *=======================================================================*/
#define FLASH_HW_PAGE_SIZE	0x00000400
#define FLASH_HW_WORD_WEIGHT	2
#define FLASH_HW_WORD_SIZE	(1<<FLASH_HW_WORD_WEIGHT)

typedef uint32_t flash_word_t;
void flash_hw_erase_page(mtd_addr_t addr);
void flash_hw_program_words(mtd_addr_t addr, mtd_size_t size,
			    flash_word_t *pdata);
void flash_hw_ctrl_init(void);

#endif /* __FLASH_LM3S9B92_H_INCLUDE__ */
