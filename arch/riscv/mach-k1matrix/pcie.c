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
 * @(#)pcie.c: K1Matrix specific PCIe implementation
 * $Id: pcie.c,v 1.1 2023-08-01 11:41:01 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/delay.h>
#include <target/sbi.h>
#include <driver/pcie_designware.h>

void pcie_config_rc_ep_mode(void)
{
	if (sysreg_die_id() == 0) {
		printf("rc\n");
		__raw_writel(0x4, CCIX_APP_BASE);
	} else {
		printf("ep\n");
		__raw_writel(0x0, CCIX_APP_BASE);
	}
}

void pcie_link_ctrl_setup(void)
{
	uint32_t val;

	val = PORT_LINK_MODE_4_LANES | PORT_LINK_RATE(1) |
		PORT_FASK_LINK_MODE_ENABLE | PORT_DLL_LINK_ENABLE;
	__raw_writel(val, CCIX_DBI_BASE + PCIE_PORT_LINK_CONTROL);
}

void pcie_config_ltssm_enable(void)
{
	__raw_writel(0x1, CCIX_APP_BASE + 4);
}

int pcie_wait_linkup(void)
{
	int32_t timeout;

	timeout = 10000000;
	while (__raw_readl(CCIX_APP_BASE + 0x104) != 3) {
		if (timeout-- <= 0) {
			printf("wait linkup timeout\n");
			return -1;
		}
	}

	return 0;
}

void pcie_ccix_packet_mode_config(void)
{
	uint32_t reg;

	reg = __raw_readl(CCIX_DBI_BASE + CCIX_TL_CAP);
	reg &= ~CCIX_OPT_LTP_FMT_SUPPORT;
	__raw_writel(reg, CCIX_DBI_BASE + CCIX_TL_CAP);

	__raw_writel(0x0, CCIX_DBI_BASE + CCIX_TL_CNTL);
}

int pcie_ccix_wait_vc0_ready(void)
{
	int32_t timeout;

	timeout = 10000000;
	while (__raw_readl(CCIX_DBI_BASE + PORT_RESOURCE_STATUS_REG_VC0) != 0) {
		if (timeout-- <= 0) {
			printf("wait vc0 ready timeout\n");
			return -1;
		}
	}

	return 0;
}

int pcie_ccix_wait_vc1_ready(void)
{
	int32_t timeout;

	timeout = 10000000;
	while (__raw_readl(CCIX_DBI_BASE + PORT_RESOURCE_STATUS_REG_VC1) != 0) {
		if (timeout-- <= 0) {
			printf("wait vc1 ready timeout\n");
			return -1;
		}
	}

	return 0;
}

void pcie_vc_resource_config(void)
{
	uint32_t reg;

	reg = PORT_VC_ENABLE_VC0 | PORT_VC_TC_MAP_VC0_BIT1(0xFD);

	__raw_writel(reg, CCIX_DBI_BASE + PORT_RESOURCE_CON_REG_VC0);

	reg = PORT_VC_PORT_ARBI_TABLE_VC1(0x81) |
		PORT_VC_PORT_ARBI_CAP_VC1(0x2);
	__raw_writel(reg, CCIX_DBI_BASE + PORT_RESOURCE_CAP_REG_VC1);

	pcie_ccix_wait_vc0_ready();
	pcie_ccix_wait_vc1_ready();
}

void pcie_ccix_set_id(int id)
{
	__raw_writel(id, CCIX_DBI_BASE + 0xC20);
}

void pcie_ccix_linkup(void)
{
	sysreg_pcie0_cold_reset();
	sysreg_pcie0_button_reset();

	pcie_config_rc_ep_mode();

	sysreg_pcie0_warm_reset();

	pcie_link_ctrl_setup();

	pcie_config_ltssm_enable();

	pcie_wait_linkup();

	pcie_ccix_packet_mode_config();

	pcie_vc_resource_config();

	pcie_ccix_set_id(0);
}
