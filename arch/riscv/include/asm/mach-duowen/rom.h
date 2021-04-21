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
 * 31     6          5        4        3        2        1          0
 * +------+----------+--------+--------+--------+--------+----------+
 * | RSVD | PLICCNTL | IMC_S1 | IMC_S0 | APC_S1 | APC_S0 | CHIPLINK |
 * +------+----------+--------+--------+--------+--------+----------+
 * SW_MSG_1:
 * +-----------------------+-----------------------+
 * | Socket 1 Partial Good | Socket 0 Partial Good |
 * +-----------------------+-----------------------+
 */

#define ROM_STATUS		SCSR_SW_MSG(0)
#define ROM_APC_MAP		SCSR_SW_MSG(1)

/* ROM_STATUS */
#define ROM_CHIPLINK_READY	_BV(0)
#define ROM_S0_APC_VALID	_BV(1)
#define ROM_S1_APC_VALID	_BV(2)
#define ROM_S0_IMC_VALID	_BV(3)
#define ROM_S1_IMC_VALID	_BV(4)
#define ROM_PLICCNTL_DONE	_BV(5)
/* ROM_APC_MAP */
#define ROM_S0_APC_OFFSET	0
#define ROM_S0_APC_MASK		REG_16BIT_MASK
#define ROM_GET_S0_APC(value)	_GET_FV(ROM_S0_APC, value)
#define ROM_SET_S0_APC(value)	_SET_FV(ROM_S0_APC, value)
#define ROM_S1_APC_OFFSET	16
#define ROM_S1_APC_MASK		REG_16BIT_MASK
#define ROM_GET_S1_APC(value)	_GET_FV(ROM_S1_APC, value)
#define ROM_SET_S1_APC(value)	_SET_FV(ROM_S1_APC, value)

#define rom_get_chiplink_ready()				\
	(!!(__raw_readl(ROM_STATUS) & ROM_CHIPLINK_READY))
#define rom_set_chiplink_ready()				\
	__raw_setl(ROM_CHIPLINK_READY, ROM_STATUS)
#define rom_get_pliccntl_done()					\
	(!!(__raw_readl(ROM_STATUS) & ROM_PLICCNTL_DONE))
#define rom_set_pliccntl_done()					\
	__raw_setl(ROM_PLICCNTL_DONE, ROM_STATUS)

#define soc_chip_link()						\
	(imc_chip_link() && rom_get_chiplink_ready())

#ifndef __ASSEMBLY__
uint16_t rom_get_s0_apc_map(void);
uint16_t rom_get_s1_apc_map(void);
void rom_set_s0_apc_map(uint16_t map);
void rom_set_s1_apc_map(uint16_t map);
uint32_t rom_get_apc_map(void);
uint8_t rom_get_s0_cluster_map(void);
uint8_t rom_get_s1_cluster_map(void);
uint8_t rom_get_cluster_num(void);
uint8_t rom_get_cluster_map(void);
#endif /* __ASSEMBLY__ */

#endif /* __ROM_DUOWEN_H_INCLUDE__ */
