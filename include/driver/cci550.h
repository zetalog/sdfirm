/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)cci550.h: ARM CoreLink CCI-550 coherent interconnect header
 * $Id: cci550.h,v 1.1 2023-02-20 16:37:00 zhenglv Exp $
 */

#ifndef __CCI550_H_INCLUDE__
#define __CCI550_H_INCLUDE__

#define CCI_REG(offset)		(CCI_BASE + (offset))

#define CCI_ctrl_ovr		CCI_REG(0x00000)
#define CCI_secr_acc		CCI_REG(0x00008)
#define CCI_status		CCI_REG(0x0000C)
#define CCI_impr_err		CCI_REG(0x00010)
#define CCI_qos_threshold	CCI_REG(0x00014)
#define CCI_pmu_ctrl		CCI_REG(0x00100)
#define CCI_debug_ctrl		CCI_REG(0x00104)
#define CCI_peripheral_id4	CCI_REG(0x00FD0)
#define CCI_peripheral_id5	CCI_REG(0x00FD4)
#define CCI_peripheral_id6	CCI_REG(0x00FD8)
#define CCI_peripheral_id7	CCI_REG(0x00FDC)
#define CCI_peripheral_id0	CCI_REG(0x00FE0)
#define CCI_peripheral_id1	CCI_REG(0x00FE4)
#define CCI_peripheral_id2	CCI_REG(0x00FE8)
#define CCI_peripheral_id3	CCI_REG(0x00FEC)
#define CCI_component_id0	CCI_REG(0x00FF0)
#define CCI_component_id1	CCI_REG(0x00FF4)
#define CCI_component_id2	CCI_REG(0x00FF8)
#define CCI_component_id3	CCI_REG(0x00FFC)

/* n is 0...5 */
#define CCI_SLAVE_IF_REG(n, offset)	CCI_REG(((1 + (n)) << 12) + (offset))

#define CCI_snoop_ctrl(n)	CCI_SLAVE_IF_REG(n, 0x00000)
#define CCI_share_ovr(n)	CCI_SLAVE_IF_REG(n, 0x00004)
#define CCI_arqos_ovr(n)	CCI_SLAVE_IF_REG(n, 0x00100)
#define CCI_awqos_ovr(n)	CCI_SLAVE_IF_REG(n, 0x00104)
#define CCI_qos_max_ot(n)	CCI_SLAVE_IF_REG(n, 0x00110)

/* n is 0...7 */
#define CCI_PERF_CNT_REG(n, offset)	CCI_REG(((1 + (n)) << 16) + (offset))

#define CCI_evnt_sel(n)		CCI_PERF_CNT_REG(n, 0x00000)
#define CCI_ecnt_data(n)	CCI_PERF_CNT_REG(n, 0x00004)
#define CCI_ecnt_ctrl(n)	CCI_PERF_CNT_REG(n, 0x00008)
#define CCI_ecnt_clr_ovfl(n)	CCI_PERF_CNT_REG(n, 0x0000C)

#define CCI_slave_debug(n)	CCI_REG(0x90000 + ((n) << 2))
#define CCI_master_debug(n)	CCI_REG(0x90100 + ((n) << 2))

/* 3.3.1 Control Override Register */
#define CCI_snoop_disable		_BV(0)
#define CCI_dvm_message_disable		_BV(1)
#define CCI_disable_snoop_filter	_BV(2)
#define CCI_disable_clock_gating	_BV(3)
#define CCI_cd_layer2_ace_enable	_BV(4)

/* 3.3.2 Secure Access Register */
#define CCI_non_secure_register_access_override	_BV(0)
#define CCI_debug_monitor_security_override	_BV(1)
#define CCI_secure_observation_override		_BV(2)

/* 3.3.3 Status Register */
#define CCI_change_pending		_BV(0)
#define CCI_sf_ram_initialization	_BV(1)
#define CCI_sf_ram_state_OFFSET		2
#define CCI_sf_ram_state_MASK		REG_3BIT_MASK
#define CCI_sf_ram_state(state)		_SET_FV(CCI_sf_ram_state, state)
#define CCI_sf_ram_state_Off		0
#define CCI_sf_ram_state_Static		1
#define CCI_sf_ram_state_Dynamic	3
#define CCI_sf_ram_state_On		4
#define CCI_sf_ram_state_request_OFFSET	5
#define CCI_sf_ram_state_request_MASK	REG_3BIT_MASK
#define CCI_sf_ram_state_request(req)	_SET_FV(CCI_sf_ram_state_request, req)
#define CCI_sf_ram_state_change_pending	_BV(8)

/* 3.3.10 Snoop Control Registers */
#define CCI_enable_snoops			_BV(0)
#define CCI_enable_dvms				_BV(1)
#define CCI_hardware_snoop_enable_control	_BV(29)
#define CCI_support_snoops			_BV(30)
#define CCI_support_dvms			_BV(31)

/* CCI part number codes read from Peripheral ID registers 0 and 1 */
#define CCI400_PART_NUM			0x420
#define CCI500_PART_NUM			0x422
#define CCI550_PART_NUM			0x423
#define CCI400_SLAVE_PORTS		5
#define CCI500_SLAVE_PORTS		7
#define CCI550_SLAVE_PORTS		7

#define cci_change_pending()	\
	(__raw_readl(CCI_status) & CCI_change_pending)
#define cci_enable_snoops(n)	\
	__raw_setl(CCI_enable_snoops, CCI_snoop_ctrl(n))
#define cci_enable_dvms(n)	\
	__raw_setl(CCI_enable_dvms, CCI_snoop_ctrl(n))
#define cci_disable_snoops(n)	\
	__raw_clearl(CCI_enable_snoops, CCI_snoop_ctrl(n))
#define cci_disable_dvms(n)	\
	__raw_clearl(CCI_enable_dvms, CCI_snoop_ctrl(n))

void cci_enable_snoop_dvm_reqs(unsigned int master_id);
void cci_disable_snoop_dvm_reqs(unsigned int master_id);

#endif /* __CCI550_H_INCLUDE__ */
