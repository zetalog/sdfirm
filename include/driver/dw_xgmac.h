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
 * @(#)dw_xgmac.h: Synopsys DesignWare 10G ethernet MAC interface
 * $Id: dw_xgmac.h,v 1.0 2020-12-17 17:20:00 zhenglv Exp $
 */

#ifndef __DW_XGMAC_H_INCLUDE__
#define __DW_XGMAC_H_INCLUDE__

#define DWCXG_CORE			0
#define DWCXG_MTL			0x1000
#define DWCXG_MTL_TCQ(n)		(0x1100 + (n) << 7)
#define DWCXG_DMA			0x3000
#define DWCXG_DMA_CH(c)			(0x3100 + (n) << 7)

#define XGMAC_REG(offset)		(DW_XGMAC_BASE + (offset))
#define DWCXG_CORE_REG(offset)		XGMAC_REG(DWCXG_CORE + (offset))
#define DWCXG_MTL_REG(offset)		XGMAC_REG(DWCXG_MTL + (offset))
#define DWCXG_MTL_TCQ_REG(q, offset)	XGMAC_REG(DWCXG_MTL_TCQ(q) + (offset))
#define DWCXG_DMA_REG(offset)		XGMAC_REG(DWCXG_DMA + (offset))
#define DWCXG_DMA_CH_REG(c, offset)	XGMAC_REG(DWCXG_DMA_CH(c) + (offset))

/* MAC Protocol-Core Registers */
#define MAC_Tx_Configuration			DWCXG_CORE_REG(0x000)
#define MAC_Rx_Configuration			DWCXG_CORE_REG(0x004)
#define MAC_Packet_Filter			DWCXG_CORE_REG(0x008)
#define MAC_Watchdog_Timeout			DWCXG_CORE_REG(0x00C)
#define MAC_Hash_Table(n)			DWCXG_CORE_REG(0x010 + (n) << 2)
#define MAC_VLAN_Tag				DWCXG_CORE_REG(0x050)
#define MAC_VLAN_Tag_Ctrl			DWCXG_CORE_REG(0x050) /* redefined */
#define MAC_VLAN_Tag_Data			DWCXG_CORE_REG(0x054)
#define MAC_VLAN_Tag_Filter(n)			DWCXG_CORE_REG(0x054 + (n) << 2)
#define MAC_VLAN_Hash_Table			DWCXG_CORE_REG(0x058)
#define MAC_VLAN_Incl(n)			DWCXG_CORE_REG(0x060 + (n) << 2)
#define MAC_Inner_VLAN_Incl			DWCXG_CORE_REG(0x064)
#define MAC_Rx_Eth_Type_Match			DWCXG_CORE_REG(0x06C)
#define MAC_Q_Tx_Flow_Ctrl(n)			DWCXG_CORE_REG(0x070 + (n) << 2)
#define MAC_Rx_Flow_Ctrl			DWCXG_CORE_REG(0x090)
#define MAC_RxQ_Ctrl4				DWCXG_CORE_REG(0x094)
#define MAC_RxQ_Ctrl0				DWCXG_CORE_REG(0x0A0)
#define MAC_RxQ_Ctrl1				DWCXG_CORE_REG(0x0A4)
#define MAC_RxQ_Ctrl2				DWCXG_CORE_REG(0x0A8)
#define MAC_RxQ_Ctrl3				DWCXG_CORE_REG(0x0AC)
#define MAC_Interrupt_Status			DWCXG_CORE_REG(0x0B0)
#define MAC_Interrupt_Enable			DWCXG_CORE_REG(0x0B4)
#define MAC_Rx_Tx_Status			DWCXG_CORE_REG(0x0B8)
#define MAC_PMT_Control_Status			DWCXG_CORE_REG(0x0C0)
/* RWK_Filter */
#define MAC_LPI_Control_Status			DWCXG_CORE_REG(0x0D0)
#define MAC_LPI_Timers_Control			DWCXG_CORE_REG(0x0D4)
#define MAC_LPI_Auto_Entry_Timer		DWCXG_CORE_REG(0x0D8)
#define MAC_1US_Tic_Counter			DWCXG_CORE_REG(0x0DC)
#define MAC_Tunnel_Identifier			DWCXG_CORE_REG(0x0E0)
#define MAC_Version				DWCXG_CORE_REG(0x110)
#define MAC_Debug				DWCXG_CORE_REG(0x114)
#define MAC_HW_Feature0				DWCXG_CORE_REG(0x11C)
#define MAC_HW_Feature1				DWCXG_CORE_REG(0x120)
#define MAC_HW_Feature2				DWCXG_CORE_REG(0x124)
#define MAC_HW_Feature3				DWCXG_CORE_REG(0x128)
#define MAC_Extended_Configuration		DWCXG_CORE_REG(0x140)
#define MDIO_Single_Command_Address		DWCXG_CORE_REG(0x200)
#define MDIO_Single_Command_Control_Data	DWCXG_CORE_REG(0x204)
#define MDIO_Continuous_Write_Address		DWCXG_CORE_REG(0x208)
#define MDIO_Continuous_Write_Data		DWCXG_CORE_REG(0x20C)
#define MDIO_Continuous_Scan_Port_Enable	DWCXG_CORE_REG(0x210)
#define MDIO_Interrupt_Status			DWCXG_CORE_REG(0x214)
#define MDIO_Interrupt_Enable			DWCXG_CORE_REG(0x218)
#define MDIO_Port_Connect_Disconnect_Status	DWCXG_CORE_REG(0x21C)
#define MDIO_Clause_22_Port			DWCXG_CORE_REG(0x220)
#define MDIO_Port_Nx4_Indirect_Control		DWCXG_CORE_REG(0x224)
#define MDIO_PortNx4P0_Device_In_Use		DWCXG_CORE_REG(0x230)
#define MDIO_PortNx4P0_Link_Status		DWCXG_CORE_REG(0x234)
#define MDIO_PortNx4P0_Alive_Status		DWCXG_CORE_REG(0x238)
#define MDIO_PortNx4P1_Device_In_Use		DWCXG_CORE_REG(0x240)
#define MDIO_PortNx4P1_Link_Status		DWCXG_CORE_REG(0x244)
#define MDIO_PortNx4P1_Alive_Status		DWCXG_CORE_REG(0x248)
#define MDIO_PortNx4P2_Device_In_Use		DWCXG_CORE_REG(0x250)
#define MDIO_PortNx4P2_Link_Status		DWCXG_CORE_REG(0x254)
#define MDIO_PortNx4P2_Alive_Status		DWCXG_CORE_REG(0x258)
#define MDIO_PortNx4P3_Device_In_Use		DWCXG_CORE_REG(0x260)
#define MDIO_PortNx4P3_Link_Status		DWCXG_CORE_REG(0x264)
#define MDIO_PortNx4P3_Alive_Status		DWCXG_CORE_REG(0x268)
#define MAC_GPIO_Control			DWCXG_CORE_REG(0x278)
#define MAC_GPIO_Status				DWCXG_CORE_REG(0x27C)
#define MAC_Address_High(n)		DWCXG_CORE_REG(0x300 + ((n) << 3))
#define MAC_Address_Low(n)		DWCXG_CORE_REG(0x304 + ((n) << 3))
#define MMC_Control				DWCXG_CORE_REG(0x800)
#define MMC_Rx_Interrupt			DWCXG_CORE_REG(0x804)
#define MMC_Tx_Interrupt			DWCXG_CORE_REG(0x808)
#define MMC_Receive_Interrupt_Enable		DWCXG_CORE_REG(0x80C)
#define MMC_Transmit_Interrupt_Enable		DWCXG_CORE_REG(0x810)
/* Errors, counters */
#define Tx_Octet_Count_Good_Bad_Low			DWCXG_CORE_REG(0x814)
#define Tx_LPI_Tran_Cntr				DWCXG_CORE_REG(0x8A8)
#define Priority_Interrupt_Status			DWCXG_CORE_REG(0x8CC)
#define Tx_Per_Priority_Status				DWCXG_CORE_REG(0x8D0)
#define Tx_Per_Priority_Pkt_Good_Bad			DWCXG_CORE_REG(0x8D4)
#define Tx_Per_Priority_PFC_Pkt_Good_Bad		DWCXG_CORE_REG(0x8D8)
#define Tx_Per_Priority_GPFC_Pkt_Good_Bad		DWCXG_CORE_REG(0x8DC)
#define Tx_Per_Priority_Octet_Good_Bad_Low		DWCXG_CORE_REG(0x8E0)
#define Tx_Per_Priority_Octet_Good_Bad_High		DWCXG_CORE_REG(0x8E4)
#define Rx_Packet_Count_Good_Bad_Low			DWCXG_CORE_REG(0x900)
#define Rx_Packet_Count_Good_Bad_High			DWCXG_CORE_REG(0x904)
#define Rx_Octet_Count_Good_Bad_Low			DWCXG_CORE_REG(0x908)
#define Rx_Octet_Count_Good_Bad_High			DWCXG_CORE_REG(0x90C)
#define Rx_Octet_Count_Good_Low				DWCXG_CORE_REG(0x910)
#define Rx_Octet_Count_Good_High			DWCXG_CORE_REG(0x914)
#define Rx_Broadcast_Packets_Good_Low			DWCXG_CORE_REG(0x918)
#define Rx_Broadcast_Packets_Good_High			DWCXG_CORE_REG(0x91C)
#define Rx_Multicast_Packets_Good_Low			DWCXG_CORE_REG(0x920)
#define Rx_Multicast_Packets_Good_High			DWCXG_CORE_REG(0x924)
#define Rx_CRC_Error_Packets_Low			DWCXG_CORE_REG(0x928)
#define Rx_CRC_Error_Packets_High			DWCXG_CORE_REG(0x92C)
#define Rx_Runt_Error_Packets				DWCXG_CORE_REG(0x930)
#define Rx_Jabber_Error_Packets				DWCXG_CORE_REG(0x934)
#define Rx_Undersize_Packets_Good			DWCXG_CORE_REG(0x938)
#define Rx_Oversize_Packets_Good			DWCXG_CORE_REG(0x93c)
#define Rx_64Octets_Packets_Good_Bad_Low		DWCXG_CORE_REG(0x940)
#define Rx_64Octets_Packets_Good_Bad_High		DWCXG_CORE_REG(0x944)
#define Rx_65To127Octets_Packets_Good_Bad_Low		DWCXG_CORE_REG(0x948)
#define Rx_65To127Octets_Packets_Good_Bad_High		DWCXG_CORE_REG(0x94C)
#define Rx_128To255Octets_Packets_Good_Bad_Low		DWCXG_CORE_REG(0x950)
#define Rx_128To255Octets_Packets_Good_Bad_High		DWCXG_CORE_REG(0x954)
#define Rx_256To511Octets_Packets_Good_Bad_Low		DWCXG_CORE_REG(0x958)
#define Rx_256To511Octets_Packets_Good_Bad_High		DWCXG_CORE_REG(0x95C)
#define Rx_512To1023Octets_Packets_Good_Bad_Low		DWCXG_CORE_REG(0x960)
#define Rx_512To1023Octets_Packets_Good_Bad_High	DWCXG_CORE_REG(0x964)
#define Rx_1024ToMaxOctets_Packets_Good_Bad_Low		DWCXG_CORE_REG(0x968)
#define Rx_1024ToMaxOctets_Packets_Good_Bad_High	DWCXG_CORE_REG(0x96C)
#define Rx_Unicast_Packets_Good_Low			DWCXG_CORE_REG(0x970)
#define Rx_Unicast_Packets_Good_High			DWCXG_CORE_REG(0x974)
#define Rx_Length_Error_Packets_Low			DWCXG_CORE_REG(0x978)
#define Rx_Length_Error_Packets_High			DWCXG_CORE_REG(0x97C)
#define Rx_OutofRange_Packets_Low			DWCXG_CORE_REG(0x980)
#define Rx_OutofRange_Packets_High			DWCXG_CORE_REG(0x984)
#define Rx_Pause_Packets_Low				DWCXG_CORE_REG(0x988)
#define Rx_Pause_Packets_High				DWCXG_CORE_REG(0x98C)
#define Rx_FIFOOverflow_Packets_Low			DWCXG_CORE_REG(0x990)
#define Rx_FIFOOverflow_Packets_High			DWCXG_CORE_REG(0x994)
#define Rx_VLAN_Packets_Good_Bad_Low			DWCXG_CORE_REG(0x998)
#define Rx_VLAN_Packets_Good_Bad_High			DWCXG_CORE_REG(0x99C)
#define Rx_Watchdog_Error_Packets			DWCXG_CORE_REG(0x9A0)
#define Rx_LPI_USEC_Cntr				DWCXG_CORE_REG(0x9A4)
#define Rx_LPI_Tran_Cntr				DWCXG_CORE_REG(0x9A8)
#define Rx_Discard_Packets_Good_Bad_Low			DWCXG_CORE_REG(0x9AC)
#define Rx_Discard_Packets_Good_Bad_High		DWCXG_CORE_REG(0x9B0)
#define Rx_Discard_Octets_Good_Bad_Low			DWCXG_CORE_REG(0x9B4)
#define Rx_Discard_Octets_Good_Bad_High			DWCXG_CORE_REG(0x9B8)
#define Rx_Alignment_Error_Packets			DWCXG_CORE_REG(0x9BC)
#define Rx_Per_Priority_Status				DWCXG_CORE_REG(0x9D0)
#define Rx_Per_Priority_Pkt_Good_Bad			DWCXG_CORE_REG(0x9D4)
#define Rx_Per_Priority_Pkt_Bad				DWCXG_CORE_REG(0x9D8)
#define Rx_Per_Priority_PFC_Pkt_Good_Bad		DWCXG_CORE_REG(0x9DC)
#define Rx_Per_Priority_Octet_Good_Bad_Low		DWCXG_CORE_REG(0x9E0)
#define Rx_Per_Priority_Octet_Good_Bad_High		DWCXG_CORE_REG(0x9E4)
#define Rx_Per_Priority_Discard_Good_Bad		DWCXG_CORE_REG(0x9E8)
/* L3/L4 */
#define MAC_L3_L4_Address_Control		DWCXG_CORE_REG(0xC00)
#define MAC_Layer3_Addr2(n)			DWCXG_CORE_REG(0xC04)
#define MAC_Layer3_Addr3(n)			DWCXG_CORE_REG(0xC04)
#define MAC_Layer4_Address(n)			DWCXG_CORE_REG(0xC04)
#define MAC_L3_L4_Control(n)			DWCXG_CORE_REG(0xC04)
#define MAC_L3_L4_Data				DWCXG_CORE_REG(0xC04)
#define MAC_Layer3_Addr0(n)			DWCXG_CORE_REG(0xC04)
#define MAC_Layer3_Addr1(n)			DWCXG_CORE_REG(0xC04)
#define MAC_ARP_Address				DWCXG_CORE_REG(0xC10)
#define MAC_RSS_Control				DWCXG_CORE_REG(0xC80)
#define MAC_RSS_Address				DWCXG_CORE_REG(0xC88)
#define RSS_Lookup_Data(n)			DWCXG_CORE_REG(0xC8C)
#define RSS_Hash_Key(n)				DWCXG_CORE_REG(0xC8C)
#define MAC_RSS_Data				DWCXG_CORE_REG(0xC8C)
#define MAC_Timestamp_Control			DWCXG_CORE_REG(0xD00)
#define MAC_Sub_Second_Increment		DWCXG_CORE_REG(0xD04)
#define MAC_System_Time_Seconds			DWCXG_CORE_REG(0xD08)
#define MAC_System_Time_Nanoseconds		DWCXG_CORE_REG(0xD0C)
#define MAC_System_Time_Seconds_Update		DWCXG_CORE_REG(0xD10)
#define MAC_System_Time_Nanoseconds_Update	DWCXG_CORE_REG(0xD14)
#define MAC_Timestamp_Addend			DWCXG_CORE_REG(0xD18)
#define MAC_System_Time_Higher_Word_Seconds	DWCXG_CORE_REG(0xD1C)
#define MAC_Timestamp_Status			DWCXG_CORE_REG(0xD20)
#define MAC_Rx_PCH_CRC_Mismatch_Counter		DWCXG_CORE_REG(0xD2C)
#define MAC_Tx_Timestamp_Status_Nanoseconds	DWCXG_CORE_REG(0xD30)
#define MAC_Tx_Timestamp_Status_Seconds		DWCXG_CORE_REG(0xD34)
#define MAC_Tx_Timestamp_Status_PktID		DWCXG_CORE_REG(0xD38)
#define MAC_Auxiliary_Control			DWCXG_CORE_REG(0xD40)
#define MAC_Auxiliary_Timestamp_Nanoseconds	DWCXG_CORE_REG(0xD48)
#define MAC_Auxiliary_Timestamp_Seconds		DWCXG_CORE_REG(0xD4C)
#define MAC_Timestamp_Ingress_Asym_Corr		DWCXG_CORE_REG(0xD50)
#define MAC_Timestamp_Egress_Asym_Corr		DWCXG_CORE_REG(0xD54)
#define MAC_Timestamp_Ingress_Corr_Nanosecond		DWCXG_CORE_REG(0xD58)
#define MAC_Timestamp_Ingress_Corr_Subnanosecond	DWCXG_CORE_REG(0xD5C)
#define MAC_Timestamp_Egress_Corr_Nanosecond		DWCXG_CORE_REG(0xD60)
#define MAC_Timestamp_Egress_Corr_Subnanosecond		DWCXG_CORE_REG(0xD64)
#define MAC_PPS_Control				DWCXG_CORE_REG(0xD70)
#define MAC_PPS0_Target_Time_Seconds		DWCXG_CORE_REG(0xD80)
#define MAC_PPS0_Target_Time_Nanoseconds	DWCXG_CORE_REG(0xD84)
#define MAC_PPS0_Interval			DWCXG_CORE_REG(0xD88)
#define MAC_PPS0_Width				DWCXG_CORE_REG(0xD8C)
#define MAC_PPS1_Target_Time_Seconds		DWCXG_CORE_REG(0xD90)
#define MAC_PPS1_Target_Time_Nanoseconds	DWCXG_CORE_REG(0xD94)
#define MAC_PPS1_Interval			DWCXG_CORE_REG(0xD98)
#define MAC_PPS1_Width				DWCXG_CORE_REG(0xD9C)
#define MAC_PPS2_Target_Time_Seconds		DWCXG_CORE_REG(0xDA0)
#define MAC_PPS2_Target_Time_Nanoseconds	DWCXG_CORE_REG(0xDA4)
#define MAC_PPS2_Interval			DWCXG_CORE_REG(0xDA8)
#define MAC_PPS2_Width				DWCXG_CORE_REG(0xDAC)
#define MAC_PPS3_Target_Time_Seconds		DWCXG_CORE_REG(0xDB0)
#define MAC_PPS3_Target_Time_Nanoseconds	DWCXG_CORE_REG(0xDB4)
#define MAC_PPS3_Interval			DWCXG_CORE_REG(0xDB8)
#define MAC_PPS3_Width				DWCXG_CORE_REG(0xDBC)
#define MAC_PTO_Control				DWCXG_CORE_REG(0xDC0)
#define MAC_Source_Port_Identity0		DWCXG_CORE_REG(0xDC4)
#define MAC_Source_Port_Identity1		DWCXG_CORE_REG(0xDC8)
#define MAC_Source_Port_Identity2		DWCXG_CORE_REG(0xDCC)
#define MAC_Log_Message_Interval		DWCXG_CORE_REG(0xDD0)

/* MDIO_Single_Command_Address */
#define MDIO_DA_OFFSET			21
#define MDIO_DA_MASK			REG_5BIT_MASK
#define MDIO_DA(value)			_SET_FV(MDIO_DA, value)
#define MDIO_PA_OFFSET			16
#define MDIO_PA_MASK			REG_5BIT_MASK
#define MDIO_PA(value)			_SET_FV(MDIO_PA, value)
#define MDIO_RA_OFFSET			0
#define MDIO_RA_MASK			REG_16BIT_MASK
#define MDIO_RA(value)			_SET_FV(MDIO_RA, value)
/* MDIO_Single_Command_Control_Data */
#define MDIO_CRS			_BV(31)
#define MDIO_PSE			_BV(30)
#define MDIO_SBusy			_BV(22)
#define MDIO_CR_OFFSET			19
#define MDIO_CR_MASK			REG_3BIT_MASK
#define MDIO_CR(value)			_SET_FV(MDIO_CR, value)
#define MDIO_SAADR			_BV(18)
#define MDIO_CMD_OFFSET			16
#define MDIO_CMD_MASK			REG_2BIT_MASK
#define MDIO_CMD(value)			_SET_FV(MDIO_CMD, value)
#define MDIO_SINGLE_READ		3
#define MDIO_SINGLE_WRITE		1
#define MDIO_POST_INCREMENT_READ	2
#define MDIO_SDATA_OFFSET		0
#define MDIO_SDATA_MASK			REG_16BIT_MASK
#define MDIO_SDATA(value)		_SET_FV(MDIO_SDATA, value)
#define mdio_sdata(value)		_GET_FV(MDIO_SDATA, value)
#ifdef CONFIG_DW_XGMAC_MDIO_FAST
#define MDIO_DATA_DEFAULT		(MDIO_SBusy | MDIO_CRS)
#else
#define MDIO_DATA_DEFAULT		MDIO_SBusy
#endif

#define dw_xgmac_mdio_busy()		\
	(!!(__raw_readl(MDIO_Single_Command_Control_Data) & MDIO_SBusy))

/* Mac Transaction Layer (MTL) Common/General Registers */
#define MTL_Operation_Mode		DWCXG_MTL_REG(0x000)
#define MTL_Debug_Control		DWCXG_MTL_REG(0x008)
#define MTL_Debug_Status		DWCXG_MTL_REG(0x00C)
#define MTL_FIFO_Debug_Data		DWCXG_MTL_REG(0x010)
#define MTL_Interrupt_Status		DWCXG_MTL_REG(0x020)
#define MTL_RxQ_DMA_Map0		DWCXG_MTL_REG(0x030)
#define MTL_RxQ_DMA_Map1		DWCXG_MTL_REG(0x034)
#define MTL_RxQ_DMA_Map2		DWCXG_MTL_REG(0x038)
#define MTL_TC_Prty_Map0		DWCXG_MTL_REG(0x040)
#define MTL_TC_Prty_Map1		DWCXG_MTL_REG(0x044)

/* MAC_Address_High */
#define MAC_AE				_BV(31)
#define MAC_DCS_OFFSET			16
#define MAC_DCS_MASK			REG_15BIT_MASK
#define MAC_DCS(value)			_SET_FV(MAC_DCS, value)

/* MTL Traffic Class/Queue 0 Registers */
#define MTL_TxQ0_Operation_Mode(n)		DWCXG_MTL_TCQ_REG(n, 0x000)
#define MTL_TxQ0_Underflow(n)			DWCXG_MTL_TCQ_REG(n, 0x004)
#define MTL_TxQ0_Debug(n)			DWCXG_MTL_TCQ_REG(n, 0x008)
#define MTL_TC0_ETS_Control(n)			DWCXG_MTL_TCQ_REG(n, 0x010)
#define MTL_TC0_ETS_Status(n)			DWCXG_MTL_TCQ_REG(n, 0x014)
#define MTL_TC0_Quantum_Weight(n)		DWCXG_MTL_TCQ_REG(n, 0x018)
#define MTL_RxQ0_Operation_Mode(n)		DWCXG_MTL_TCQ_REG(n, 0x040)
#define MTL_RxQ0_Missed_Pkt_Overflow_Cnt(n)	DWCXG_MTL_TCQ_REG(n, 0x044)
#define MTL_RxQ0_Debug(n)			DWCXG_MTL_TCQ_REG(n, 0x048)
#define MTL_RxQ0_Control(n)			DWCXG_MTL_TCQ_REG(n, 0x04C)
#define MTL_RxQ0_Flow_Control(n)		DWCXG_MTL_TCQ_REG(n, 0x050)
#define MTL_Q0_Interrupt_Enable(n)		DWCXG_MTL_TCQ_REG(n, 0x070)
#define MTL_Q0_Interrupt_Status(n)		DWCXG_MTL_TCQ_REG(n, 0x074)

/* DMA Common/General Registers */
#define DMA_Mode				DWCXG_DMA_REG(0x000)
#define DMA_SysBus_Mode				DWCXG_DMA_REG(0x004)
#define DMA_Interrupt_Status			DWCXG_DMA_REG(0x008)
#define AXI_Tx_AR_ACE_Control			DWCXG_DMA_REG(0x010)
#define AXI_Rx_AW_ACE_Control			DWCXG_DMA_REG(0x018)
#define AXI_TxRx_AWAR_ACE_Control		DWCXG_DMA_REG(0x01C)
#define DMA_Debug_Status0			DWCXG_DMA_REG(0x020)
#define DMA_Debug_Status1			DWCXG_DMA_REG(0x024)
#define DMA_Debug_Status2			DWCXG_DMA_REG(0x028)
#define DMA_Debug_Status3			DWCXG_DMA_REG(0x02C)
#define DMA_Debug_Status4			DWCXG_DMA_REG(0x030)
#define DMA_Tx_EDMA_Control			DWCXG_DMA_REG(0x040)
#define DMA_Rx_EDMA_Control			DWCXG_DMA_REG(0x044)
#define AXI_LPI_Entry_Interval			DWCXG_DMA_REG(0x050)

/* DMA Channel Registers */
#define DMA_CH_Control(n)			DWCXG_DMA_CH_REG(n, 0x000)
#define DMA_CH_TX_Control(n)			DWCXG_DMA_CH_REG(n, 0x004)
#define DMA_CH_RX_Control(n)			DWCXG_DMA_CH_REG(n, 0x008)
#define DMA_CH_Slot_Function_Control_Status(n)	DWCXG_DMA_CH_REG(n, 0x00C)
#define DMA_CH_TxDesc_List_HAddress(n)		DWCXG_DMA_CH_REG(n, 0x010)
#define DMA_CH_TxDesc_List_LAddress(n)		DWCXG_DMA_CH_REG(n, 0x014)
#define DMA_CH_RxDesc_List_HAddress(n)		DWCXG_DMA_CH_REG(n, 0x018)
#define DMA_CH_RxDesc_List_LAddress(n)		DWCXG_DMA_CH_REG(n, 0x01C)
#define DMA_CH_TxDesc_Tail_HPointer(n)		DWCXG_DMA_CH_REG(n, 0x020)
#define DMA_CH_TxDesc_Tail_LPointer(n)		DWCXG_DMA_CH_REG(n, 0x024)
#define DMA_CH_RxDesc_Tail_HPointer(n)		DWCXG_DMA_CH_REG(n, 0x028)
#define DMA_CH_RxDesc_Tail_LPointer(n)		DWCXG_DMA_CH_REG(n, 0x02C)
#define DMA_CH_TxDesc_Ring_Length(n)		DWCXG_DMA_CH_REG(n, 0x030)
#define DMA_CH_RxDesc_Ring_Length(n)		DWCXG_DMA_CH_REG(n, 0x034)
#define DMA_CH_Interrupt_Enable(n)		DWCXG_DMA_CH_REG(n, 0x038)
#define DMA_CH_Rx_Interrupt_Watchdog_Timer(n)	DWCXG_DMA_CH_REG(n, 0x03C)
#define DMA_CH_Current_App_TxDesc_H(n)		DWCXG_DMA_CH_REG(n, 0x040)
#define DMA_CH_Current_App_TxDesc_L(n)		DWCXG_DMA_CH_REG(n, 0x044)
#define DMA_CH_Current_App_RxDesc_H(n)		DWCXG_DMA_CH_REG(n, 0x048)
#define DMA_CH_Current_App_RxDesc_L(n)		DWCXG_DMA_CH_REG(n, 0x04C)
#define DMA_CH_Current_App_TxBuffer_H(n)	DWCXG_DMA_CH_REG(n, 0x050)
#define DMA_CH_Current_App_TxBuffer_L(n)	DWCXG_DMA_CH_REG(n, 0x054)
#define DMA_CH_Current_App_RxBuffer_H(n)	DWCXG_DMA_CH_REG(n, 0x058)
#define DMA_CH_Current_App_RxBuffer_L(n)	DWCXG_DMA_CH_REG(n, 0x05C)
#define DMA_CH_Status(n)			DWCXG_DMA_CH_REG(n, 0x060)
#define DMA_CH_Debug Status(n)			DWCXG_DMA_CH_REG(n, 0x064)
#define DMA_CH_Desc_Mem_Cache_Fill_Level(n)	DWCXG_DMA_CH_REG(n, 0x068)
#define DMA_CH_Miss_Packet_Cnt(n)		DWCXG_DMA_CH_REG(n, 0x06C)
#define DMA_CH_Tx_Data_Xfer_Ring_Offset(n)	DWCXG_DMA_CH_REG(n, 0x070)
#define DMA_CH_Rx_Data_Xfer_Ring_Offset(n)	DWCXG_DMA_CH_REG(n, 0x074)
#define DMA_CH_Tx_Desc_Write_Ring_Offset(n)	DWCXG_DMA_CH_REG(n, 0x078)
#define DMA_CH_Rx_Desc_Write_Ring_Offset(n)	DWCXG_DMA_CH_REG(n, 0x07C)

/* 8.4 Transmit Descriptor */
/* 8.4.1 Transmit Normal Descriptor (Read Format) */
/* TDES0 -> XGMAC_TDES0_BUF1AP[31:0]
 * TDES1 -> XGMAC_TDES1_BUF2AP[31:0] or XGMAC_TDES1_BUF1AP[64:32]
 */
#define XGMAC_TDES2_IOC			_BV(31)
#define XGMAC_TDES2_TTSE		_BV(30)
#define XGMAC_TDES2_TMWD		_BV(30)
#define XGMAC_TDES2_B2L_OFFSET		16
#define XGMAC_TDES2_B2L_MASK		REG_14BIT_MASK
#define XGMAC_TDES2_B2L(value)		_SET_FV(XGMAC_TDES2_B2L, value)
#define XGMAC_TDES2_VTIR_OFFSET		14
#define XGMAC_TDES2_VTIR_MASK		REG_2BIT_MASK
#define XGMAC_TDES2_VTIR(value)		_SET_FV(XGMAC_TDES2_VTIR, value)
#define XGMAC_TDES2_HL_OFFSET		0
#define XGMAC_TDES2_HL_MASK		REG_14BIT_MASK
#define XGMAC_TDES2_HL(value)		_SET_FV(XGMAC_TDES2_HL, value)
#define XGMAC_TDES2_B1L(value)		XGMAC_TDES2_HL(value)
#define XGMAC_TDES3_OWN			_BV(31)
#define XGMAC_TDES3_CTXT		_BV(30)
#define XGMAC_TDES3_FD			_BV(29)
#define XGMAC_TDES3_LD			_BV(28)
#define XGMAC_TDES3_CPC_OFFSET		26
#define XGMAC_TDES3_CPC_MASK		REG_2BIT_MASK
#define XGMAC_TDES3_CPC(value)		_SET_FV(XGMAC_TDES3_CPC, value)
#define XGMAC_TDES3_SAIC_OFFSET		23
#define XGMAC_TDES3_SAIC_MASK		REG_3BIT_MASK
#define XGMAC_TDES3_SAIC(value)		_SET_FV(XGMAC_TDES3_SAIC, value)
#define XGMAC_TDES3_VNP(value)		XGMAC_TDES3_SAIC(value)
#define XGMAC_TDES3_THL_OFFSET		19
#define XGMAC_TDES3_THL_MASK		REG_4BIT_MASK
#define XGMAC_TDES3_THL(value)		_SET_FV(XGMAC_TDES3_THL, value)
#define XGMAC_TDES3_SLOTNUM(value)	XGMAC_TDES3_THL(value)
#define XGMAC_TDES3_TSE			_BV(18)
#define XGMAC_TDES3_CIC_OFFSET		16
#define XGMAC_TDES3_CIC_MASK		REG_2BIT_MASK
#define XGMAC_TDES3_CIC(value)		_SET_FV(XGMAC_TDES3_CIC, value)
#define XGMAC_CIC_CRC_PAD_INSERTION	0
#define XGMAC_CIC_CRC_INSERTION		1
#define XGMAC_CIC_NONE			2
#define XGMAC_CIC_CRC_REPLACEMENT	3
#define XGMAC_TDES3_FL_OFFSET		0
#define XGMAC_TDES3_FL_MASK		REG_15BIT_MASK
#define XGMAC_TDES3_FL(value)		_SET_FV(XGMAC_TDES3_FL)
#define XGMAC_TDES3_TPL_OFFSET		0
#define XGMAC_TDES3_TPL_MASK		REG_17BIT_MASK
#define XGMAC_TDES3_TPL(value)		_SET_FV(XGMAC_TDES3_TPL, value)
/* 8.4.2 Transmit Normal Descriptor (Write-Back Format) */
#if 0
#define XGMAC_TDES3_OWN			_BV(31)
#define XGMAC_TDES3_CTXT		_BV(30)
#define XGMAC_TDES3_FD			_BV(29)
#define XGMAC_TDES3_LD			_BV(28)
#endif
#define XGMAC_TDES3_DERR		_BV(27)
/* 8.4.3 Transmit Context Descriptor */
/* TDES0 -> XGMAC_TDES0_TTSL[31:0]
 * TDES1 -> XGMAC_TDES1_TTSH[31:0]
 */
#define XGMAC_TDES2_IVT_OFFSET		16
#define XGMAC_TDES2_IVT_MASK		REG_16BIT_MASK
#define XGMAC_TDES2_IVT(value)		_SET_FV(XGMAC_TDES2_IVT, value)
#define XGMAC_TDES2_MSS_OFFSET		0
#define XGMAC_TDES2_MSS_MASK		REG_14BIT_MASK
#define XGMAC_TDES2_MSS(value)		_SET_FV(XGMAC_TDES2_MSS, value)
#if 0
#define XGMAC_TDES3_OWN			_BV(31)
#define XGMAC_TDES3_CTXT		_BV(30)
#endif
#define XGMAC_TDES3_CDE			_BV(29)
#define XGMAC_TDES3_OSTC		_BV(27)
#define XGMAC_TDES3_TCMSSV		_BV(26)
#define XGMAC_TDES3_PIDV		_BV(20)
#define XGMAC_TDES3_IVTIR_OFFSET	18
#define XGMAC_TDES3_IVTIR_MASK		REG_2BIT_MASK
#define XGMAC_TDES3_IVTIR(value)	_SET_FV(XGMAC_TDES3_IVTIR, value)
#define XGMAC_TDES3_IVLTV		_BV(17)
#define XGMAC_TDES3_VLTV		_BV(16)
#define XGMAC_TDES3_VT_OFFSET		0
#define XGMAC_TDES3_VT_MASK		REG_16BIT_MASK
#define XGMAC_TDES3_VT(value)		_SET_FV(XGMAC_TDES3_VT, value)

/* 8.5 Receive Descriptor */
/* 8.5.1 Receive Normal Descriptor (Read Format) */
/* RDES0 -> XGMAC_RDES0_BUF1AP[31:0]
 * RDES1 -> XGMAC_RDES1_BUF1AP[63:32]
 * RDES2 -> XGMAC_RDES2_BUF2AP[31:0]
 */
#define XGMAC_RDES3_OWN			_BV(31)
#define XGMAC_RDES3_IOC			_BV(30)
#define XGMAC_RDES3_BUF2APH_OFFSET	0
#define XGMAC_RDES3_BUF2APH_MASK	REG_30BIT_MASK
#define XGMAC_RDES3_BUF2APH(value)	_SET_FV(XGMAC_RDES3_BUF2APH, value)
/* 8.5.2 Receive Normal Descriptor (Write-Back Format) */
/* RDES0 -> XGMAC_RDES0_ELRD
 * RDES1 -> XGMAC_RDES1_RSSH
 */
#define XGMAC_RDES0_IVT_OFFSET		16
#define XGMAC_RDES0_IVT_MASK		REG_16BIT_MASK
#define XGMAC_RDES0_IVT(value)		_GET_FV(XGMAC_RDES0_IVT, value)
#define XGMAC_RDES0_OVT_OFFSET		0
#define XGMAC_RDES0_OVT_MASK		REG_16BIT_MASK
#define XGMAC_RDES0_OVT(value)		_GET_FV(XGMAC_RDES0_OVT, value)
#define XGMAC_RDES0_VNID_OFFSET		8
#define XGMAC_RDES0_VNID_MASK		REG_24BIT_MASK
#define XGMAC_RDES0_VNID(value)		_GET_FV(XGMAC_RDES0_VNID, value)
#define XGMAC_RDES0_VSID(value)		XGMAC_RDES0_VNID(value)
#define XGMAC_RDES0_OL2L3_OFFSET	0
#define XGMAC_RDES0_OL2L3_MASK		REG_3BIT_MASK
#define XGMAC_RDES0_OL2L3(value)	_GET_FV(XGMAC_RDES0_OL2L3, value)
#define XGMAC_RDES2_L3L4FM_OFFSET	29
#define XGMAC_RDES2_L3L4FM_MASK		REG_3BIT_MASK
#define XGMAC_RDES2_L3L4FM(value)	_GET_FV(XGMAC_RDES2_L3L4FM, value)
#define XGMAC_RDES2_L4FM		_BV(28)
#define XGMAC_RDES2_L3FM		_BV(27)
#define XGMAC_RDES2_MADRM_OFFSET	19
#define XGMAC_RDES2_MADRM_MASK		REG_8BIT_MASK
#define XGMAC_RDES2_MADRM(value)	_GET_FV(XGMAC_RDES2_MADRM, value)
#define XGMAC_RDES2_HF			_BV(18)
#define XGMAC_RDES2_DAF			_BV(17)
#define XGMAC_RDES2_SAF			_BV(16)
#define XGMAC_RDES2_VF			_BV(15)
#define XGMAC_RDES2_RPNG		_BV(14)
#define XGMAC_RDES2_IOS			_BV(13)
#define XGMAC_RDES2_ELD			_BV(12)
#define XGMAC_RDES2_TNP			_BV(11)
#define XGMAC_RDES2_HL_OFFSET		2
#define XGMAC_RDES2_HL_MASK		REG_8BIT_MASK
#define XGMAC_RDES2_HL(value)		_GET_FV(XGMAC_RDES2_HL, value)
#define XGMAC_RDES2_AVTDP		_BV(1)
#define XGMAC_RDES2_AVTCP		_BV(0)
#define XGMAC_RDES3_OWN			_BV(31)
#define XGMAC_RDES3_CTXT		_BV(30)
#define XGMAC_RDES3_FD			_BV(29)
#define XGMAC_RDES3_LD			_BV(28)
#define XGMAC_RDES3_CDA			_BV(27)
#define XGMAC_RDES3_RSV			_BV(26)
#define XGMAC_RDES3_ISP			_BV(25)
#define XGMAC_RDES3_ETM			_BV(24)
#define XGMAC_RDES3_NCP			_BV(24)
#define XGMAC_RDES3_L34T_OFFSET		20
#define XGMAC_RDES3_L34T(value)		_GET_FV(XGMAC_RDES3_L34T, value)
#define XGMAC_RDES3_ET_OFFSET		16
#define XGMAC_RDES3_ET_MASK		REG_4BIT_MASK
#define XGMAC_RDES3_ET(value)		_GET_FV(XGMAC_RDES3_ET, value)
#define XGMAC_RDES3_LT(value)		XGMAC_RDES3_ET(value)
#define XGMAC_RDES3_ES			_BV(15)
#define XGMAC_RDES3_PL_OFFSET		0
#define XGMAC_RDES3_PL_MASK		REG_14BIT_MASK
#define XGMAC_RDES3_PL(value)		_GET_FV(XGMAC_RDES3_PL, value)
/* 8.5.3 Receive Context Descriptor */
/* RDES0 -> XGMAC_RDES0_RTSL
 * RDES1 -> XGMAC_RDES1_RTSH
 */
#if 0
#define XGMAC_RDES3_OWN			_BV(31)
#define XGMAC_RDES3_CTXT		_BV(30)
#endif
#define XGMAC_RDES3_TSD			_BV(6)
#define XGMAC_RDES3_PRPNG		_BV(5)
#define XGMAC_RDES3_TSA			_BV(4)
#define XGMAC_RDES3_PMT_OFFSET		0
#define XGMAC_RDES3_PMT_MASK		REG_4BIT_MASK
#define XGMAC_RDES3_PMT(value)		_GET_FV(XGMAC_RDES3_PMT, value)

/* Rx IPC status */
enum rx_frame_status {
	rx_good_frame = 0x0,
	rx_discard_frame = 0x1,
	rx_not_ls = 0x2,
	rx_dma_own = 0x4,
};

/* Tx status */
enum tx_frame_status {
	tx_done = 0x0,
	tx_not_ls = 0x1,
	tx_err = 0x2,
	tx_dma_own = 0x4,
};

struct dw_xgmac_desc {
	uint32_t des0;
	uint32_t des1;
	uint32_t des2;
	uint32_t des3;
};

#ifdef CONFIG_DW_XGMAC
#define dw_xgmac_get_tx_len(p)			\
	((p)->des2 & XGMAC_TDES2_B1L)
#define dw_xgmac_get_tx_owner(p)		\
	(((p)->des3 & XGMAC_TDES3_OWN) > 0)
#define dw_xgmac_set_tx_owner(p)		\
	((p)->des3 |= XGMAC_TDES3_OWN)
#define dw_xgmac_set_rx_owner(p)		\
	((p)->des3 |= XGMAC_RDES3_OWN)
#define dw_xgmac_enable_rx_ic(p)		\
	((p)->des3 |= XGMAC_RDES3_IOC)
#define dw_xgmac_disable_rx_ic(p)		\
	((p)->des3 &= ~XGMAC_RDES3_IOC)
#define dw_xgmac_get_tx_ls(p)			\
	(((p)->des3 & XGMAC_RDES3_LD) > 0)
#define dw_xgmac_get_rx_frame_len(p)		\
	((p)->des3 & XGMAC_RDES3_PL)
int dw_xgmac_get_tx_status(struct dw_xgmac_desc *p);
int dw_xgmac_get_rx_status(struct dw_xgmac_desc *p);
void dw_xgmac_init_rx_desc(struct dw_xgmac_desc *p);
void dw_xgmac_init_tx_desc(struct dw_xgmac_desc *p);
void dw_xgmac_prepare_tx_desc(struct dw_xgmac_desc *p, bool is_fs,
			      int len, bool csum_flag, bool tx_own, bool ls);

void dw_xgmac_init_ctrl(void);
bool dw_xgmac_mdio_read(uint16_t phyaddr, uint32_t phyreg,
			uint16_t *phydata);
bool dw_xgmac_mdio_write(uint16_t phyaddr, uint32_t phyreg,
			 uint16_t phydata);
#else
#define dw_xgmac_get_tx_status(p)		tx_err
#define dw_xgmac_get_rx_status(p)		rx_discard_frame
#define dw_xgmac_get_tx_owner(p)		false
#define dw_xgmac_set_tx_owner(p)		do { } while (0)
#define dw_xgmac_set_rx_owner(p)		do { } while (0)
#define dw_xgmac_enable_rx_ic(p)		do { } while (0)
#define dw_xgmac_disable_rx_ic(p)		do { } while (0)
#define dw_xgmac_get_tx_len(p)			0
#define dw_xgmac_get_tx_ls(p)			false
#define dw_xgmac_get_rx_frame_len(p)		0
#define dw_xgmac_init_rx_desc(p)		do { } while (0)
#define dw_xgmac_init_tx_desc(p)		do { } while (0)
#define dw_xgmac_prepare_tx_desc(p, is_fs, len, csum_flag, tx_own, ls)	\
	do { } while (0)
#define dw_xgmac_init_ctrl()			do { } while (0)
#define dw_xgmac_mdio_read(addr, reg)		0
#define dw_xgmac_mdio_write(addr, reg, data)	do { } while (0)
#endif

#endif /* __DW_XGMAC_H_INCLUDE__ */
