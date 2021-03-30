/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)tsensor.h: DPU thermal sensor (TSENS) definitions
 * $Id: tsensor.h,v 1.1 2020-05-20 14:37:00 zhenglv Exp $
 */

#ifndef __TSENSOR_DPU_H_INCLUDE__
#define __TSENSOR_DPU_H_INCLUDE__

#define TSENS_REG(offset)		(TSENSOR_BASE + (offset))
#define TSENS_METS_CFG			0x5C
#define TSENS_CFG_BASE			TSENS_REG(TSENS_METS_CFG)
#define TSENS_CFG_REG(offset)		(TSENS_CFG_BASE + (offset))
#define REG_METS_CONFIG_RUN		0x0500
#define TC_PADDR_REG_PE16_METS_CONFIG_ADDR	TSENS_CFG_REG(0x00)
#define TC_PADDR_REG_VPU_METS_CONFIG_ADDR	TSENS_CFG_REG(0x04)
#define TC_PADDR_REG_DDR_METS_CONFIG_ADDR	TSENS_CFG_REG(0x08)
#define TC_PADDR_REG_PCIE_METS_CONFIG_ADDR	TSENS_CFG_REG(0x0C)
#define TC_PADDR_REG_NOC_METS_CONFIG_ADDR	TSENS_CFG_REG(0x10)

#ifdef CONFIG_DPU_SIM_TSENSOR_IRQ
void dpu_tsensor_irq_init(void);
#else
#define dpu_tsensor_irq_init()		do { } while (0)
#endif

#endif /* __TSENSOR_DPU_H_INCLUDE__ */
