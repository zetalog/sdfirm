/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2022
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
 * @(#)reg.h: K1MAX specific register layout definitions
 * $Id: reg.h,v 1.1 2022-10-15 12:10:00 zhenglv Exp $
 */

#ifndef __REG_K1MAX_H_INCLUDE__
#define __REG_K1MAX_H_INCLUDE__

#ifdef CONFIG_K1M_SOC
#define BROM_BASE		ULL(0x00000000)
#define TIMER_BASE		ULL(0x10000000)
#define UART_BASE		ULL(0x10010000)
#define GPIO_BASE		ULL(0x10011000)
#define SYSREG_BASE		ULL(0x10012000)
#define DRAMC_BASE		ULL(0x10020000)
#define DMAC_BASE		ULL(0x10030000)
#define SSI_BASE		ULL(0x10031000)
#define EMMC_BASE		ULL(0x10032000)
#define SDIO_BASE		ULL(0x10036000)
#define USB_BASE		ULL(0x10040000)
#define CCI_BASE		ULL(0x10100000)
#define PIC_BASE		ULL(0x30000000)
#define PLIC_REG_BASE		PIC_BASE
#define CLINT_BASE		(PIC_BASE + ULL(0x4000000))

#define SRAM_BASE		ULL(0x20000000)
#define DRAM_BASE		ULL(0x40000000)
#define TCM_BASE		ULL(0x38000000)

#define BROM_SIZE		ULL(0x00010000)
#define SRAM_SIZE		ULL(0x04000000)
#define __DRAM_SIZE		ULL(0xC0000000)
#define TCM_SIZE		ULL(0x02000000)
#define FINISH_ADDR		ULL(0x10011000)
#endif /* CONFIG_K1M_SOC */
#ifdef CONFIG_K1M_CPU
#define SRAM_BASE		ULL(0x00000000)
#define SRAM_SIZE		CONFIG_K1M_MEM_SIZE
#define CLINT_BASE		ULL(0x94000000)
#define UART_BASE		ULL(0x9FF00000)
#define FINISH_ADDR		ULL(0x9F0F2100)
#endif /* CONFIG_K1M_CPU */
#define k1max_finish(code)	__raw_writel(code, FINISH_ADDR)

#ifdef CONFIG_K1M_SRAM
#define ROM_BASE		SRAM_BASE
#define ROM_SIZE		SRAM_SIZE
#define RAM_BASE		SRAM_BASE
#define RAM_SIZE		SRAM_SIZE
#endif /* CONFIG_K1M_SRAM */

#ifdef CONFIG_K1M_DDR_SIZE_CUSTOM
#define DRAM_SIZE               CONFIG_K1M_MEM_SIZE
#else /* CONFIG_K1M_DDR_SIZE_CUSTOM */
#ifdef CONFIG_K1M_S2C
/* XXX: DRAM Size
 * Actually supports up to 8G with dual ranks, 4G in single rank. But due
 * to the address space limitation, at least up to 3G in single rank.
 */
#define DRAM_SIZE		__DRAM_SIZE
#endif /* CONFIG_K1M_S2C */
#ifdef CONFIG_K1M_PZ1
#define DRAM_SIZE		SZ_256M
#endif /* CONFIG_K1M_PZ1 */
#ifndef DRAM_SIZE
#define DRAM_SIZE		CONFIG_K1M_MEM_SIZE
#endif /* DRAM_SIZE */
#endif /* CONFIG_K1M_DDR_SIZE_CUSTOM */

#ifdef CONFIG_K1M_DDR
#define ROM_BASE		DRAM_BASE
#define ROM_SIZE		DRAM_SIZE
#define RAM_BASE		DRAM_BASE
#define RAM_SIZE		DRAM_SIZE
#endif /* CONFIG_K1M_DDR */

#ifdef CONFIG_K1M_ZSBL
#define ROM_BASE		BROM_BASE
#define ROM_SIZE		BROM_SIZE
#define RAM_BASE		SRAM_BASE
#define RAM_SIZE		SRAM_SIZE
#endif /* CONFIG_K1M_ZSBL */

#define ROMEND			(ROM_BASE + ROM_SIZE)
#define RAMEND			(RAM_BASE + RAM_SIZE)

#endif /* __REG_K1MAX_H_INCLUDE__ */
