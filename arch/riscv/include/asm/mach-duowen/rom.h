/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)rom.h: DUOWEN ROM configuration definitions
 * $Id: rom.h,v 1.1 2021-04-01 09:25:00 zhenglv Exp $
 */

#ifndef __ROM_DUOWEN_H_INCLUDE__
#define __ROM_DUOWEN_H_INCLUDE__

/* ROM Configuration Protocol
 *
 * ROM uses SCSR SW_MSG to pass shared configurables.
 *
 * SW_MSG_0:
 * 31      2          1          0
 *  +------+----------+----------+
 *  | RSVD | PLICCNTL | CHIPLINK |
 *  +------+----------+----------+
 * SW_MSG_1:
 * 32               24    23         18    17    16                  0
 *  +----------------+-----+----------+-----+-----+------------------+
 *  | PCIe Link Mode | NOC | BOOTHART | IMC | APC | APC Partial Good |
 *  +----------------+-----+----------+-----+-----+------------------+
 */

#define __ROM_GBL_STATUS	(__SCSR_BASE + ____SCSR_SW_MSG(0))
#define __ROM_SOC_STATUS	(__SCSR_BASE + ____SCSR_SW_MSG(1))
#define ROM_GBL_STATUS		SCSR_SW_MSG(0)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define ROM_SOC_STATUS(soc)	__SCSR_SW_MSG(soc, 1)
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define ROM_SOC_STATUS(soc)	SCSR_SW_MSG(1)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* ROM_GBL_STATUS */
#define ROM_CHIPLINK_READY	_BV(0)
#define ROM_PLICCNTL_DONE	_BV(1)
/* ROM_SOC_STATUS */
#define ROM_APC_PG_OFFSET	0
#define ROM_APC_PG_MASK		REG_16BIT_MASK
#define ROM_GET_APC_PG(value)	_GET_FV(ROM_APC_PG, value)
#define ROM_SET_APC_PG(value)	_SET_FV(ROM_APC_PG, value)
#define ROM_APC_VALID		_BV(16)
#define ROM_IMC_VALID		_BV(17)
#define ROM_BOOTHART_OFFSET	18
#define ROM_BOOTHART_MASK	REG_4BIT_MASK
#define ROM_SET_BOOTHART(value)	_SET_FV(ROM_BOOTHART, value)
#define ROM_GET_BOOTHART(value)	_GET_FV(ROM_BOOTHART, value)
#define ROM_NOC_INIT		23
#define ROM_PCIE_LM_OFFSET	24
#define ROM_PCIE_LM_MASK	REG_8BIT_MASK
#define ROM_GET_PCIE_LM(value)	_GET_FV(ROM_PCIE_LM, value)
#define ROM_SET_PCIE_LM(value)	_SET_FV(ROM_PCIE_LM, value)

/* global status */
#define rom_get_chiplink_ready()				\
	(!!(__raw_readl(ROM_GBL_STATUS) & ROM_CHIPLINK_READY))
#define rom_set_chiplink_ready()				\
	__raw_setl(ROM_CHIPLINK_READY, ROM_GBL_STATUS)
#define rom_get_pliccntl_done()					\
	(!!(__raw_readl(ROM_GBL_STATUS) & ROM_PLICCNTL_DONE))
#define rom_set_pliccntl_done()					\
	__raw_setl(ROM_PLICCNTL_DONE, ROM_GBL_STATUS)

#define soc_chip_link()						\
	(imc_chip_link() && rom_get_chiplink_ready())

#ifndef __ASSEMBLY__
/* socket specific status */
uint16_t rom_get_s0_apc_map(void);
uint16_t rom_get_s1_apc_map(void);
void rom_set_apc_map(uint16_t map);
uint32_t rom_get_apc_map(void);
uint8_t rom_get_cluster_num(void);
uint8_t rom_get_cluster_map(void);
bool rom_get_noc_configured(void);
void rom_set_noc_configured(void);
uint8_t rom_get_pcie_link_mode(void);
void rom_set_pcie_link_mode(uint8_t mode);
#endif /* __ASSEMBLY__ */

#endif /* __ROM_DUOWEN_H_INCLUDE__ */
