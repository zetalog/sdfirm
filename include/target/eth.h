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
 * @(#)eth.h: Ethernet MAC/PHY controller interface
 * $Id: eth.h,v 1.0 2020-12-22 12:30:00 zhenglv Exp $
 */

#ifndef __ETH_H_INCLUDE__
#define __ETH_H_INCLUDE__

#include <target/generic.h>
#include <target/jiffies.h>

#define ETH_ALEN			6

#define PMA_MMD				0x1
#define XS_DTE_MMD			0x5
#define XS_PHY_MMD			0x4
#define PCS_MMD				0x3
#define AN_MMD				0x7

/* Or MII_ADDR_C45 into regnum for read/write on mii_bus to enable the 21
 * bit IEEE 802.3ae clause 45 addressing mode used by 10GIGE phy chips.
 */
#define MII_ADDR_C45			_BV(30)
#define MII_DEVADDR_C45_OFFSET		16
#define MII_REGADDR_C45_OFFSET		0
#define MII_REGADDR_C45_MASK		REG_16BIT_MASK
#define MII_REGADDR_C45(value)		_GET_FV(MII_REGADDR_C45, value)

#define MII_PHYADDR_MAX			32
#define MII_DEVADDR_MAX			32

/* Generic MII registers. */
#define MII_BMCR		0x00	/* Basic mode control register */
#define MII_BMSR		0x01	/* Basic mode status register  */
#define MII_PHYSID1		0x02	/* PHYS ID 1                   */
#define MII_PHYSID2		0x03	/* PHYS ID 2                   */
#define MII_ADVERTISE		0x04	/* Advertisement control reg   */
#define MII_LPA			0x05	/* Link partner ability reg    */
#define MII_EXPANSION		0x06	/* Expansion register          */
#define MII_CTRL1000		0x09	/* 1000BASE-T control          */
#define MII_STAT1000		0x0a	/* 1000BASE-T status           */
#define	MII_MMD_CTRL		0x0d	/* MMD Access Control Register */
#define	MII_MMD_DATA		0x0e	/* MMD Access Data Register */
#define MII_ESTATUS		0x0f	/* Extended Status             */
#define MII_DCOUNTER		0x12	/* Disconnect counter          */
#define MII_FCSCOUNTER		0x13	/* False carrier counter       */
#define MII_NWAYTEST		0x14	/* N-way auto-neg test reg     */
#define MII_RERRCOUNTER		0x15	/* Receive error counter       */
#define MII_SREVISION		0x16	/* Silicon revision            */
#define MII_RESV1		0x17	/* Reserved...                 */
#define MII_LBRERROR		0x18	/* Lpback, rx, bypass error    */
#define MII_PHYADDR		0x19	/* PHY address                 */
#define MII_RESV2		0x1a	/* Reserved...                 */
#define MII_TPISTATUS		0x1b	/* TPI status for 10mbps       */
#define MII_NCONFIG		0x1c	/* Network interface config    */

/* PHY mode */
#define PHY_INTFC_NONE		0
#define PHY_INTFC_MII		1
#define PHY_INTFC_GMII		2
#define PHY_INTFC_SGMII		3
#define PHY_INTFC_RMII		4
#define PHY_INTFC_RGMII		5
#define PHY_INTFC_SMII		6
#define PHY_INTFC_XGMII		7
#define PHY_INTFC_XLGMII	8
#define PHY_INTFC_QSGMII	9
#define PHY_INTFC_RXAUI		10
#define PHY_INTFC_XAUI		11
#define PHY_INTFC_USXGMII	12
#define PHY_INTFC_100BASEX	100
#define PHY_INTFC_1000BASEX	101
#define PHY_INTFC_2500BASEX	102
#define PHY_INTFC_5000BASER	103
#define PHY_INTFC_10000BASER	104
#define PHY_INTFC_25000BASER	105
#define PHY_INTFC_1000BASE_KX	70 /* Clause 70 */
#define PHY_INTFC_10000BASE_KX4	71 /* Clause 71 */
#define PHY_INTFC_10000BASE_KR	72 /* Clause 72 */

#include <driver/eth.h>

static inline void eth_random_addr(uint8_t *addr)
{
	get_random_bytes(addr, ETH_ALEN);
	addr[0] &= 0xfe;	/* clear multicast bit */
	addr[0] |= 0x02;	/* set local assignment bit (IEEE802) */
}

static inline void eth_broadcast_addr(uint8_t *addr)
{
	memset(addr, 0xff, ETH_ALEN);
}

static inline void eth_zero_addr(uint8_t *addr)
{
	memset(addr, 0x00, ETH_ALEN);
}

#define mdio_read(a, r, d)	eth_hw_mdio_read(a, r, d)
#define mdio_write(a, r, d)	eth_hw_mdio_write(a, r, d)

#endif /* __ETH_H_INCLUDE__ */
