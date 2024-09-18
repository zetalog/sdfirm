#ifndef __SPD_EE1004_H_INCLUDE__
#define __SPD_EE1004_H_INCLUDE__

#include <target/i2c.h>

#ifndef ARCH_HAVE_SPD_EEPROM
#define ARCH_HAVE_SPD_EEPROM 1
#else
#error "Multiple DDR SPD eeprom types defined"
#endif

#define SPD_EE_PAGE_SIZE	256
#define SPD_EE_PAGE_CNT		2
#define SPD_EE_SIZE		(SPD_EE_PAGE_SIZE * SPD_EE_PAGE_CNT)

#define SPD_EE_PAGE_WRITE_SIZE	16

/* Device Select Code */
#ifdef CONFIG_SPD_TSE2004
#define SPD_DSC_TR(lsa)		(0x18 | (lsa)) /* Read/Write temperature register */
#endif /* CONFIG_SPD_TSE2004 */
#define SPD_DSC_SPD(lsa)	(0x50 | (lsa)) /* Read/Write SPD EE memory */
#define SPD_DSC_WP0		0x31 /* Set/Read Write Protection Status */
#define SPD_DSC_WP1		0x34
#define SPD_DSC_WP2		0x35
#define SPD_DSC_WP3		0x30
#define SPD_DSC_CWP		0x33 /* Clear All Write Protection */
#define SPD_DSC_PA0		0x36 /* Set/Read SPD Page Address */
#define SPD_DSC_PA1		0x37

int spd_hw_read_bytes(uint8_t lsa, uint16_t offset,
		      uint8_t *buffer, int len);

#endif /* __SPD_EE1004_H_INCLUDE__ */
