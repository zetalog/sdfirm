/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)arch.h: machine specific definitions
 * $Id: arch.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __ARCH_QDF2400_H_INCLUDE__
#define __ARCH_QDF2400_H_INCLUDE__

#include <asm/io.h>

#define PERIPH_PCIE0		0
#define PERIPH_PCIE1		1
#define PERIPH_SYSFAB		2
#define PERIPH_HDMA		3
#define PERIPH_SATA		4

#ifdef CONFIG_QDF2400_PSEUDO_CPU_MASK
#ifdef CONFIG_QDF2400_CPU_48_CORES
#define ARCH_CPU_MASK		0xFFFFFFFFFFFF
#endif
#ifdef CONFIG_QDF2400_CPU_6_CORES
#define ARCH_CPU_MASK		0x3F
#endif
#ifdef CONFIG_QDF2400_CPU_USER_CORES
#define ARCH_CPU_MASK		CONFIG_QDF2400_CPU_MASK
#endif
#else
#define ARCH_CPU_MASK		0xFFFFFFFFFFFF
#endif

#ifdef CONFIG_QDF2400_PSEUDO_LLC_MASK
#ifdef CONFIG_QDF2400_LLC_24_INTERLEAVES
#define ARCH_LLC_MASK		0xFFFFFF
#endif
#ifdef CONFIG_QDF2400_LLC_USER_INTERLEAVES
#define ARCH_LLC_MASK		CONFIG_QDF2400_LLC_MASK
#endif
#else
#define ARCH_LLC_MASK		0xFFFFFF
#endif

#ifdef CONFIG_QDF2400_PSEUDO_DDR_MASK
#ifdef CONFIG_QDF2400_DDR_12_SLOTS
#define ARCH_DDR_MASK		0xFFF
#else
#define ARCH_DDR_MASK		CONFIG_QDF2400_DDR_MASK
#endif
#else
#define ARCH_DDR_MASK		0xFFF
#endif

#ifdef CONFIG_QDF2400_PSEUDO_PERIPH_MASK
#ifdef CONFIG_QDF2400_PERIPH_PCIE0
#define PERIPH_PCIE0_MASK	_BV(PERIPH_PCIE0)
#else
#define PERIPH_PCIE0_MASK	0
#endif
#ifdef CONFIG_QDF2400_PERIPH_PCIE1
#define PERIPH_PCIE1_MASK	_BV(PERIPH_PCIE1)
#else
#define PERIPH_PCIE1_MASK	0
#endif
#ifdef CONFIG_QDF2400_PERIPH_HDMA
#define PERIPH_HDMA_MASK	_BV(PERIPH_HDMA)
#else
#define PERIPH_HDMA_MASK	0
#endif
#ifdef CONFIG_QDF2400_PERIPH_SATA
#define PERIPH_SATA_MASK	_BV(PERIPH_SATA)
#else
#define PERIPH_SATA_MASK	0
#endif
#else
#define PERIPH_PCIE0_MASK	_BV(PERIPH_PCIE0)
#define PERIPH_PCIE1_MASK	_BV(PERIPH_PCIE1)
#define PERIPH_HDMA_MASK	_BV(PERIPH_HDMA)
#define PERIPH_SATA_MASK	_BV(PERIPH_SATA)
#endif

#define PERIPH_SYSFAB_MASK	_BV(PERIPH_SYSFAB)
#define ARCH_PERIPH_MASK	\
	(PERIPH_PCIE0_MASK |	\
	 PERIPH_PCIE1_MASK |	\
	 PERIPH_SYSFAB_MASK |	\
	 PERIPH_HDMA_MASK |	\
	 PERIPH_SATA_MASK)

#define CORE_TOP_CSR_BASE	ULL(0xFF02E00000)

/*===========================================================================
 * Security Control Core
 *===========================================================================*/
#ifndef SEC_CTL_CORE_BASE
#define SEC_CTL_CORE_BASE		0xFF0C700000
#endif
#define SEC_CTL_REG(off)		(SEC_CTL_CORE_BASE + (off))

#define FEATURE_CONFIG0				SEC_CTL_REG(0x00003804)
#define FEATURE_PBL_SCRUB_DISABLE		_BV(22)
#define FEATURE_PRNG_TESTMODE_DISABLE		_BV(21)
#define FEATURE_MDDR_COMPRESSION_DISABLE	_BV(20)
#define FEATURE_TESTBUS_DISABLE			_BV(19)
#define FEATURE_FORCE_DLOAD_DISABLE		_BV(18)
#define FEATURE_PARTIAL_GOOD_DISABLE		_BV(16)
#define FEATURE_ATE_TEST_MODE_DISABLE		_BV(15)
#define FEATURE_APPS_CFGAESPMULLDISABLE		_BV(11)
#define FEATURE_APPS_CFGSHADISABLE		_BV(10)
#define FEATURE_APPS_CFGCRYPTODISABLE		_BV(9)
#define FEATURE_APPS_CP15SDISABLE		_BV(8)
#define FEATURE_APPS_CFGRSTSCTLREL3V		_BV(7)
#define FEATURE_APPS_CFGRSTSCTLREL3TE		_BV(6)
#define FEATURE_APPS_CFGRSTSCTLREL3EE		_BV(5)
#define FEATURE_BOOT_ROM_PATCH_DISABLE		_BV(0)

#define BOOT_CONFIG				SEC_CTL_REG(0x00003834)
#define BOOT_ROT_TRANSFER_GPIO			_BV(15)
#define BOOT_DEEP_RECOVERY_GPIO			_BV(14)
#define BOOT_FORCE_SD_RECOVERY_GPIO		_BV(13)
#define BOOT_RECOVERY_GPIO			_BV(12)
#define BOOT_ALL_USE_SERIAL_NUM			_BV(11)
#define BOOT_FORCE_MSA_AUTH_EN			_BV(10)
#define BOOT_PK_HASH_IN_FUSE			_BV(9)
#define BOOT_PK_HASH_INDEX_SOURCE		_BV(8)
#define BOOT_PBL_BOOT_SPEED_OFFSET		6
#define BOOT_PBL_BOOT_SPEED_MASK		0x3
#define BOOT_PBL_BOOT_SPEED(value)		\
	_SET_FV(BOOT_PBL_BOOT_SPEED, value)
#define BOOT_FROM_ROM				_BV(5)
#define BOOT_INTF_SELECT_OFFSET			1
#define BOOT_INTF_SELECT_MASK			0xF
#define BOOT_INTF_SELECT(value)			\
	_SET_FV(BOOT_INTF_SELECT, value)
#define BOOT_INTF_DEFAULT			0
#define BOOT_INTF_SPINOR_EMMC_SD		1
#define BOOT_INTF_EMMC				2
#define BOOT_INTF_SPINOR			3
#define BOOT_WDOG_EN				_BV(0)

#define JTAG_ID					SEC_CTL_REG(0x000038e0)
#define JTAG_ID_DIE_VERSION_OFFSET		28
#define JTAG_ID_DIE_VERSION_MASK		0x0F
#define JTAG_ID_DIE_VERSION(value)		\
	_GET_FV(JTAG_ID_DIE_VERSION, value)
#define JTAG_ID_JTAG_ID_OFFSET			0
#define JTAG_ID_JTAG_ID_MASK			0xFFFFFFF
#define JTAG_ID_JTAG_ID(value)			\
	_GET_FV(JTAG_ID_JTAG_ID, value)

#define BOOT_PARTITION_SELECT			SEC_CTL_REG(0x000038F0)
#define BOOT_SDI_BASE_ADDR_OFFSET		5
#define BOOT_SDI_BASE_ADDR_MASK			0x3FFFFFF
#define BOOT_SDI_BASE_ADDR(value)		\
	_GET_FV(BOOT_SDI_BASE_ADDR, value)
#define BOOT_WD_RESET				_BV(4)
#define BOOT_RECOVERY				_BV(3)
#define BOOT_DLOAD_MODE_OFFSET			1
#define BOOT_DLOAD_MODE_MASK			0x3
#define BOOT_DLOAD_MODE(value)			\
	_GET_FV(BOOT_DLOAD_MODE, value)
#define BOOT_DETECT_ABNORMAL_RESET		_BV(0)

/* PBL specific indications */
#define PBL_SKIP_ECC_SCRUB_IMEM			_BV(2)

#define QFPROM_CORR_CALIB_ROW22_LSB		SEC_CTL_REG(0x00004350)
#define QFPROM_CORR_CALIB_ROW22_MSB		SEC_CTL_REG(0x00004354)
#define PACKAGE_SUBSTRATE			QFPROM_CORR_CALIB_ROW22_MSB
#define PACKAGE_SUBSTRATE_REDUN			_BV(8)
#define PACKAGE_SUBSTRATE_REDUN_REV_OFFSET	6
#define PACKAGE_SUBSTRATE_REDUN_REV_MASK	0x03
#define PACKAGE_SUBSTRATE_REDUN_REV(value)	\
	_GET_FV(PACKAGE_SUBSTRATE_REDUN_REV, value)
#define PACKAGE_SUBSTRATE_REV_OFFSET		4
#define PACKAGE_SUBSTRATE_REV_MASK		0x03
#define PACKAGE_SUBSTRATE_REV(value)		\
	_GET_FV(PACKAGE_SUBSTRATE_REV, value)

#define TCSR_BASE		(CORE_TOP_CSR_BASE + 0x00010000)
#define TCSR_REG(off)		(TCSR_BASE + (off))
#define SOC_HW_VERSION		TCSR_REG(0x00030000)
#define SOC_FAMILY_MEMBER_OFFSET		28
#define SOC_FAMILY_MEMBER_MASK			0xF
#define SOC_FAMILY_MEMBER(value)		\
	_GET_FV(SOC_FAMILY_MEMBER, value)
#define SOC_DEVICE_MEMBER_OFFSET		16
#define SOC_DEVICE_MEMBER_MASK			0xFFF
#define SOC_DEVICE_MEMBER(value)		\
	_GET_FV(SOC_DEVICE_MEMBER, value)
#define SOC_MAJOR_VERSION_OFFSET		8
#define SOC_MAJOR_VERSION_MASK			0xF
#define SOC_MAJOR_VERSION(value)		\
	_GET_FV(SOC_MAJOR_VERSION, value)
#define SOC_MINOR_VERSION_OFFSET		0
#define SOC_MINOR_VERSION_MASK			0xF
#define SOC_MINOR_VERSION(value)		\
	_GET_FV(SOC_MINOR_VERSION, value)

#define RTLINFO_BASE		TCSR_REG(0x00090000)
#define RTLINFO_REG(off)	(RTLINFO_BASE + (off))
#define RTL_MAJOR_VERSION	RTLINFO_REG(0x30)
#define RTL_MINOR_VERSION	RTLINFO_REG(0x34)

/* 2.45.3. WARM_RESET_RETAIN_REGS */
#define TCSR_BOOT_MISC_DETECT	TCSR_REG(0x000B0000)

/* PBL specific indications */
/* #define PBL_SKIP_ECC_SCRUB_IMEM		_BV(2) */
#define PBL_JUMP_WDOG_DEBUG		_BV(0)

#include <asm/mach/pg.h>

#define soc_get_version(major, minor)					\
	do {								\
		major = SOC_MAJOR_VERSION(__raw_readl(SOC_HW_VERSION));	\
		minor = SOC_MINOR_VERSION(__raw_readl(SOC_HW_VERSION));	\
		if (major == 2) {					\
			uint8_t die;					\
			die = JTAG_ID_DIE_VERSION(__raw_readl(JTAG_ID));\
			if (die >= 3)					\
				minor = die - 2;			\
		}							\
	} while (0)

void qdf2400_gblct_init(void);

#endif /* __ARCH_QDF2400_INCLUDE__ */
