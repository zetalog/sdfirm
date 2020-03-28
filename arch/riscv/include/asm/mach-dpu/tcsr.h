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
 * @(#)tcsr.h: DPU top control/status register definitions
 * $Id: tcsr.h,v 1.1 2020-03-09 17:29:00 zhenglv Exp $
 */

#ifndef __TCSR_DPU_H_INCLUDE__
#define __TCSR_DPU_H_INCLUDE__

#define AXI_AXSIZE_BYTES		32

#include <target/amba.h>

#define TCSR_REG(offset)		(TCSR_BASE + (offset))
#define TCSR_SOC_HW_VERSION		TCSR_REG(0x00)
#define TCSR_BOOT_MODE			TCSR_REG(0x04)
#define TCSR_BOOT_ADDR			TCSR_REG(0x10)
#define TCSR_CORE_ID			TCSR_REG(0x14)
#define TCSR_CLUSTER_ID			TCSR_REG(0x18)
#define TCSR_CLOCK_EN			TCSR_REG(0x1C)
#define TCSR_SHUTDN_REQ			TCSR_REG(0x80)
#define TCSR_BRINGUP_REQ		TCSR_REG(0x84)
#define TCSR_SHUTDN_ACK			TCSR_REG(0x88)
#define TCSR_BRINGUP_ACK		TCSR_REG(0x8C)
#define TCSR_LP_STATUS			TCSR_REG(0x90)

/* SOC_HW_VERSION */
#define IMC_MAJOR_OFFSET		8
#define IMC_MAJOR_MASK			REG_8BIT_MASK
#define IMC_MAJOR(value)		_GET_FV(IMC_MAJOR, value)
#define IMC_MINOR_OFFSET		0
#define IMC_MINOR_MASK			REG_8BIT_MASK
#define IMC_MINOR(value)		_GET_FV(IMC_MINOR, value)
/* BOOT_MODE */
#define IMC_BOOT_MODE_OFFSET		0
#define IMC_BOOT_MODE_MASK		REG_2BIT_MASK
#define IMC_BOOT_MODE(value)		_GET_FV(IMC_BOOT_MODE, value)
#define IMC_BOOT_ROM			0x00
#define IMC_BOOT_FLASH			0x01
#define IMC_BOOT_USE_BOOT_ADDR		0x02
/* CORE_ID */
#define IMC_CORE_ID_OFFSET		0
#define IMC_CORE_ID_MASK		REG_4BIT_MASK
#define IMC_CORE_ID(value)		_GET_FV(IMC_CORE_ID, value)
/* CLUSTER_ID */
#define IMC_CLUSTER_ID_OFFSET		0
#define IMC_CLUSTER_ID_MASK		REG_6BIT_MASK
#define IMC_CLUSTER_ID(value)		_GET_FV(IMC_CLUSTER_ID, value)
/* CLOCK_EN */
#ifdef CONFIG_DPU_TCSR_SIM_FINISH
#define IMC_SIM_PASS			_BV(31)
#define IMC_SIM_FAIL			_BV(30)
#endif
#define IMC_CLOCK_EN			_BV(0)
/* LP_STATUS */
#define IMC_I2C2_EN			_BV(6)
#define IMC_I2C1_EN			_BV(5)
#define IMC_I2C0_EN			_BV(4)
#define IMC_UART_LP_REQ_SCLK		_BV(3)
#define IMC_UART_LP_REQ_PCLK		_BV(2)
#define IMC_SSI_SLEEP			_BV(1)
#define IMC_SSI_BUSY			_BV(0)

#define imc_soc_major()			\
	IMC_MAJOR(__raw_readl(TCSR_SOC_HW_VERSION))
#define imc_soc_minor()			\
	IMC_MINOR(__raw_readl(TCSR_SOC_HW_VERSION))
#define imc_core_id()			\
	IMC_CORE_ID(__raw_readl(TCSR_CORE_ID))
#define imc_cluster_id()		\
	IMC_CLUSTER_ID(__raw_readl(TCSR_CLUSTER_ID))
#define imc_boot_mode()			\
	IMC_BOOT_MODE(__raw_readl(TCSR_BOOT_MODE))
#define imc_boot_addr()			__raw_readl(TCSR_BOOT_ADDR)
#define imc_enable_clock()		\
	__raw_setl(IMC_CLOCK_EN, TCSR_CLOCK_EN)
#define imc_disable_clock()		\
	__raw_clearl(IMC_CLOCK_EN, TCSR_CLOCK_EN)
#ifdef CONFIG_DPU_TCSR_SIM_FINISH
#define imc_sim_finish(pass)		\
	__raw_setl((pass) ? IMC_SIM_PASS : IMC_SIM_FAIL, TCSR_CLOCK_EN)
#else
#define imc_sim_finish(pass)		do { } while (0)
#endif
#define imc_lp_status(ip)		\
	(__raw_readl(IMC_LP_STATUS) & (ip))

#ifndef __ASSEMBLY__
typedef uint16_t imc_at_attr_t;
#endif

#ifdef CONFIG_DPU_TCSR_ADDR_TRANS
/* IMC address translation
 *
 * When IMC is accessing the addresses that are external to the IMC, the
 * bus transactions are delivered to SLAVE1, and ADDR_TRANS fabric converts
 * the 32-bit IMC addresses to the 44-bit APC addresses, and relays the
 * transactions to the fabric.
 * The translation algorithm is:
 * +-------------------------------------------------+
 * |if (in_addr[31:24] == ADDR_I[31:24])             |
 * |  out_addr[43:0] = {ADDR_O[43:24], in_addr[23:0]}|
 * +-------------------------------------------------+
 * If no address translation entry is matched, the default rule is:
 * +-----------------------------------------------+
 * |out_addr[43:0] = {12'h0FF, 1'b0, in_addr[30:0]}|
 * +-----------------------------------------------+
 */
#define IMC_ADDR_TRANS_LO(n)		TCSR_REG(0x40 + (n) * 0x08)
#define IMC_ADDR_TRANS_HI(n)		TCSR_REG(0x44 + (n) * 0x08)

#define IMC_AT_ENTRIES			6
/* IMC_ADDR_TRANS_LO */
#define IMC_AT_IN_31_24_OFFSET		0
#define IMC_AT_IN_31_24_MASK		REG_8BIT_MASK
#define IMC_AT_IN_31_24(value)		_SET_FV(IMC_AT_IN_31_24, (value))
/* IMC_ADDR_TRANS_HI */
#define IMC_AT_VALID			_BV(31)
#define IMC_AT_BURST_OFFSET		28
#define IMC_AT_BURST_MASK		REG_2BIT_MASK
#define IMC_AT_BURST(value)		_SET_FV(IMC_AT_BURST, (value))
#define IMC_AT_CACHE_OFFSET		24
#define IMC_AT_CACHE_MASK		REG_4BIT_MASK
#define IMC_AT_CACHE(value)		_SET_FV(IMC_AT_CACHE, (value))
#define IMC_AT_PROT_OFFSET		20
#define IMC_AT_PROT_MASK		REG_3BIT_MASK
#define IMC_AT_PROT(value)		_SET_FV(IMC_AT_PROT, (value))
#define IMC_AT_OUT_43_24_OFFSET		0
#define IMC_AT_OUT_43_24_MASK		REG_20BIT_MASK
#define IMC_AT_OUT_43_24(value)		_SET_FV(IMC_AT_OUT_43_24, (value))

/* IMC_AT attributes, see AXI_AXPROT/AXI_AXCACHE/AXBURST
 *
 * As ri5cy doesn't have MMU implemented, thus these signals need to be
 * filled by AT or software.
 */
#define IMC_AT_ATTR_PROT_OFFSET		0
#define IMC_AT_ATTR_PROT_MASK		IMC_AT_PROT_MASK
#define IMC_AT_ATTR_SET_PROT(value)	_SET_FV(IMC_AT_ATTR_PROT, (value))
#define IMC_AT_ATTR_GET_PROT(value)	_GET_FV(IMC_AT_ATTR_PROT, (value))
#define IMC_AT_ATTR_CACHE_OFFSET	4
#define IMC_AT_ATTR_CACHE_MASK		IMC_AT_CACHE_MASK
#define IMC_AT_ATTR_SET_CACHE(value)	_SET_FV(IMC_AT_ATTR_CACHE, (value))
#define IMC_AT_ATTR_GET_CACHE(value)	_GET_FV(IMC_AT_ATTR_CACHE, (value))
#define IMC_AT_ATTR_BURST_OFFSET	8
#define IMC_AT_ATTR_BURST_MASK		IMC_AT_BURST_MASK
#define IMC_AT_ATTR_SET_BURST(value)	_SET_FV(IMC_AT_ATTR_BURST, (value))
#define IMC_AT_ATTR_GET_BURST(value)	_GET_FV(IMC_AT_ATTR_BURST, (value))
#define IMC_AT_ATTR(burst, cache)		\
	(IMC_AT_ATTR_SET_BURST((burst)) |	\
	 IMC_AT_ATTR_SET_CACHE((cache)) |	\
	 AXI_AXPROT_NON_PRIVILEDGED |		\
	 AXI_AXPROT_SECURE | AXI_AXPROT_DATA)
#define IMC_AT_ATTR_NORMAL		\
	IMC_AT_ATTR(AXI_AXCACHE_CACHEABLE, AXI_AXBURST_INCR)
#define IMC_AT_ATTR_DEVICE		\
	IMC_AT_ATTR(AXI_AXCACHE_NON_CACHEABLE, AXI_AXBURST_WRAP)
#define IMC_AT_ATTR_DEFAULT		IMC_AT_ATTR_NORMAL
#define imc_addr_set_valid(n)		\
	__raw_setl(IMC_AT_VALID, IMC_ADDR_TRANS_HI(n));
#define imc_addr_set_invalid(n)		\
	__raw_clearl(IMC_AT_VALID, IMC_ADDR_TRANS_HI(n));
#define imc_addr_set_attr(n, attr)					\
	do {								\
		uint32_t burst = IMC_AT_ATTR_GET_BURST(attr);		\
		uint32_t cache = IMC_AT_ATTR_GET_CACHE(attr);		\
		uint32_t prot = IMC_AT_ATTR_GET_PROT(attr);		\
		__raw_writel_mask(IMC_AT_BURST(burst) |			\
				  IMC_AT_CACHE(cache) |			\
				  IMC_AT_PROT(prot),			\
				  IMC_AT_BURST(IMC_AT_BURST_MASK) |	\
				  IMC_AT_CACHE(IMC_AT_CACHE_MASK) |	\
				  IMC_AT_PROT(IMC_AT_PROT_MASK),	\
				  IMC_ADDR_TRANS_HI(n));		\
	} while (0)
#define IMC_AT_ADDR_I_OFFSET		24
#define IMC_AT_ADDR_I_MASK		IMC_AT_IN_31_24_MASK
#define IMC_AT_ADDR_I(value)		_GET_FV(IMC_AT_ADDR_I, (value))
#define IMC_AT_ADDR_O_OFFSET		24
#define IMC_AT_ADDR_O_MASK		IMC_AT_OUT_43_24_MASK
#define IMC_AT_ADDR_O(value)		_GET_FV_ULL(IMC_AT_ADDR_O, (value))
#define imc_addr_set_addr_i(n, addr)				\
	do {							\
		uint32_t in = IMC_AT_ADDR_I(addr);		\
		__raw_writel_mask(IMC_AT_IN_31_24(in),		\
			IMC_AT_IN_31_24(IMC_AT_ADDR_I_MASK),	\
			IMC_ADDR_TRANS_LO(n));			\
	} while (0)
#define imc_addr_set_addr_o(n, addr)				\
	do {							\
		uint32_t out = IMC_AT_ADDR_O(addr);		\
		__raw_writel_mask(IMC_AT_OUT_43_24(out),	\
			IMC_AT_OUT_43_24(IMC_AT_ADDR_O_MASK),	\
			IMC_ADDR_TRANS_HI(n));			\
	} while (0)

#ifndef __ASSEMBLY__
void imc_addr_trans(int n, uint32_t in_addr, uint64_t out_addr,
		    imc_at_attr_t attr);
#endif
#else
#define imc_addr_trans(n, in_addr, out_addr, attr)		\
	do { } while (0)
#endif

#ifdef CONFIG_DPU_TCSR_BUS_TRANS
/* SHUTDN_REQ/ACK */
#define IMC_DDR1_CTRL			15
#define IMC_DDR1			14
#define IMC_DDR0_CTRL			13
#define IMC_DDR0			12
#define IMC_PCIE_X4_1_DBI		11
#define IMC_PCIE_X4_0_DBI		10
#define IMC_PCIE_X8_DBI			9
#define IMC_PCIE_X16_DBI		8
#define IMC_PCIE_X4_1_SLV		7
#define IMC_PCIE_X4_1_MST		6
#define IMC_PCIE_X4_0_SLV		5
#define IMC_PCIE_X4_0_MST		4
#define IMC_PCIE_X8_SLV			3
#define IMC_PCIE_X8_MST			2
#define IMC_PCIE_X16_SLV		1
#define IMC_PCIE_X16_MST		0

#define IMC_BUS_REQ(bus)		_BV(bus)
#define IMC_BUS_ACTIVE(bus)		_BV((bus) + 16)
#define IMC_BUS_ACK(bus)		_BV(bus)

#define imc_bus_active(bus)		\
	(__raw_readl(IMC_SHUTDN_ACK) & IMC_BUS_ACTIVE(bus))
#define imc_bus_shutdn_req(bus)		\
	__raw_setl(IMC_BUS_REQ(bus), IMC_SHUTDN_REQ)
#define imc_bus_shutdn_ack(bus)		\
	__raw_setl(IMC_BUS_ACK(bus), IMC_SHUTDN_ACK)
#endif

#endif /* __TCSR_DPU_H_INCLUDE__ */
