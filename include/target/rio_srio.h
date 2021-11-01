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
 * @(#)rio_srio.h: RapidIO LP-serial physical layer
 * $Id: rio_srio.h,v 1.0 2021-11-02 14:51:00 zhenglv Exp $
 */

#ifndef __RIO_SRIO_H_INCLUDE__
#define __RIO_SRIO_H_INCLUDE__

/* RIO link speed */
#define RIO_LINK_1_25		1 /* 1.25 GBaud */
#define RIO_LINK_2_5		2 /* 2.5 GBaud */
#define RIO_LINK_3_125		3 /* 3.125 GBaud */
#define RIO_LINK_5		4 /* 5.0 GBaud */
#define RIO_LINK_6_25		5 /* 6.25 GBaud */

/* SRIO link width */
#define SRIO_LINK_1X		SRIO_LANE_1x
#define SRIO_LINK_1XR		SRIO_LANE_1xR
#define SRIO_LINK_2X		SRIO_LANE_2x
#define SRIO_LINK_4X		SRIO_LANE_4x
#define SRIO_LINK_8X		SRIO_LANE_8x
#define SRIO_LINK_16X		SRIO_LANE_16x

/* Part 6: LP-Serial Physical Layer Specification */

/* Chaper 6 LP-Serial Registers */

/* 6.4 Capability Registers (CARs) */
#define SRIO_CAR_ProcessingElementFeatures	0x10

/* 6.6 LP-Serial Command and Status Registers (CSRs) */
#define SRIO_CSR_LPSerialRegisterBlockHeader	0x00
#define SRIO_CSR_PortLinkTimeoutControl		0x20
#define SRIO_CSR_PortResponseTimeoutControl	0x24
#define SRIO_CSR_PortGeneralControl		0x3C
#define SRIO_CSR_PortLinkMaintenanceRequest(n, m)	\
	(0x40 + (n) * (0x20 * (m)))
#define SRIO_CSR_PortLinkMaintenanceResponse(n, m)	\
	(0x44 + (n) * (0x20 * (m)))
#define SRIO_CSR_PortLocalackID(n, m)			\
	(0x48 + (n) * (0x20 * (m)))
#define SRIO_CSR_PortErrorandStatus(n, m)		\
	(0x58 + (n) * (0x20 * (m)))
#define SRIO_CSR_PortControl(n, m)			\
	(0x5C + (n) * (0x20 * (m)))
#define SRIO_CSR_PortControl2(n, m)			\
	(0x54 + (n) * (0x20 * (m)))

/* Short aliases */
#define SRIO_PE_FEAT_CAR		SRIO_CAR_ProcessingElementFeatures
#define SRIO_RB_HDR_CSR			SRIO_CSR_LPSerialRegisterBlockHeader
#define SRIO_PORT_LTC_CSR		SRIO_CSR_PortLinkTimeoutControl
#define SRIO_PORT_RTC_CSR		SRIO_CSR_PortResponseTimeoutControl
#define SRIO_PORT_GC_CSR		SRIO_CSR_PortGeneralControl
#define SRIO_PORT_LM_REQ_CSR(n, m)	SRIO_CSR_PortLinkMaintenanceRequest(n, m)
#define SRIO_PORT_LM_REP_CSR(n, m)	SRIO_CSR_PortLinkMaintenanceResponse(n, m)
#define SRIO_PORT_ACK_ID_CSR(n, m)	SRIO_CSR_PortLocalackID(n, m)
#define SRIO_PORT_ERR_STS_CSR(n, m)	SRIO_CSR_PortErrorandStatus(n, m)
#define SRIO_PORT_CTL_CSR(n, m)		SRIO_CSR_PortControl(n, m)
#define SRIO_PORT_CTL2_CSR(n, m)	SRIO_CSR_PortControl2(n, m)

/* 6.6.1 LP-Serial Register Block Header */
#define SRIO_EF_PTR_OFFSET		0
#define SRIO_EF_PTR_MASK		REG_16BIT_MASK
#define SRIO_EF_PTR(value)		_GET_FV(SRIO_EF_PTR, value)
#define SRIO_EF_ID_OFFSET		16
#define SRIO_EF_ID_MASK			REG_16BIT_MASK
#define SRIO_EF_ID(value)		_GET_FV(SRIO_EF_ID, value)

/* 6.6.2 Port Link Timeout Control CSR
 * 6.6.3 Port Response Timeout Control CSR
 */
#define SRIO_timeout_value_OFFSET	0
#define SRIO_timeout_value_MASK		REG_24BIT_MASK
#define SRIO_timeout_value(value)	_SET_FV(SRIO_timeout_value, value)

/* 6.6.4 Port General Control CSR */
#define SRIO_Host			_BV(0)
#define SRIO_MasterEnable		_BV(1)
#define SRIO_Discovered			_BV(2)

/* 6.6.5 Port n Link Maintenance Request CSRs */
#define SRIO_Command_OFFSET		29
#define SRIO_Command_MASK		REG_3BIT_MASK
#define SRIO_Command(value)		_SET_FV(SRIO_Command, value)

/* 6.6.6 Port n Link Maintenance Response CSRs */
#define SRIO_response_valid		_BV(0)
#define SRIO_ackID_status_OFFSET	21
#define SRIO_ackID_status_MASK		REG_6BIT_MASK
#define SRIO_ackID_status(value)	_GET_FV(SRIO_ackID_status, value)
#define SRIO_port_status_OFFSET		27
#define SRIO_port_status_MASK		REG_4BIT_MASK
#define SRIO_port_status(value)		_GET_FV(SRIO_port_status, value)

/* 6.6.7 Port n Local ackID CSRs */
#define SRIO_Clr_outstanding_ackIDs	_BV(0)
#define SRIO_Inbound_ackID_OFFSET	2
#define SRIO_Inbound_ackID_MASK		REG_6BIT_MASK
#define SRIO_Inbound_ackID(value)	_GET_FV(SRIO_Inbound_ackID, value)
#define SRIO_Outstanding_ackID_OFFSET	18
#define SRIO_Outstanding_ackID_MASK	REG_6BIT_MASK
#define SRIO_Outstanding_ackID(value)	_GET_FV(SRIO_Outstanding_ackID, value)
#define SRIO_Outbound_ackID_OFFSET	26
#define SRIO_Outbound_ackID_MASK	REG_6BIT_MASK
#define SRIO_Outbound_ackID(value)	_GET_FV(SRIO_Outbound_ackID, value)

/* 6.6.8 Port n Error and Status CSRs */
#define SRIO_IdleSequence2Support	_BV(0)
#define SRIO_IdleSequence2Enable	_BV(1)
#define SRIO_IdleSequence		_BV(2)
#define SRIO_FlowControlMode		_BV(4)
#define SRIO_OutputRetry_encountered	_BV(11)
#define SRIO_OutputRetried		_BV(12)
#define SRIO_OutputRetry_stopped	_BV(13)
#define SRIO_OutputError_encountered	_BV(14)
#define SRIO_OutputError_stopped	_BV(15)
#define SRIO_InputRetry_stopped		_BV(21)
#define SRIO_InputError_encountered	_BV(22)
#define SRIO_InputError_stopped		_BV(23)
#define SRIO_Port_writePending		_BV(27)
#define SRIO_PortUnavailable		_BV(28)
#define SRIO_PortError			_BV(29)
#define SRIO_PortOK			_BV(30)
#define SRIO_PortUninitialized		_BV(31)

/* 6.6.9 Port n Control CSR */
#define SRIO_PortWidthSupport_OFFSET		0
#define SRIO_PortWidthSupport_MASK		REG_2BIT_MASK
#define SRIO_PortWidthSupport(value)		\
	_GET_FV(SRIO_PortWidthSupport, value)
#define SRIO_PortWidth_2x			_BV(0)
#define SRIO_PortWidth_4x			_BV(1)
#define SRIO_InitializedPortWidth_OFFSET	2
#define SRIO_InitializedPortWidth_MASK		REG_3BIT_MASK
#define SRIO_InitializedPortWidth(value)	\
	_GET_FV(SRIO_InitializedPortWidth, value)
#define SRIO_LANE_1x				0
#define SRIO_LANE_1xR				1
#define SRIO_LANE_4x				2
#define SRIO_LANE_2x				3
#define SRIO_LANE_8x				4
#define SRIO_LANE_16x				5
#define SRIO_PortWidthOverride_OFFSET		5
#define SRIO_PortWidthOverride_MASK		REG_3BIT_MASK
#define SRIO_PortWidthOverride(value)		\
	_SET_FV(SRIO_PortWidthOverride, value)
#define SRIO_PortDisable			_BV(8)
#define SRIO_OutputPortEnable			_BV(9)
#define SRIO_InputPortEnable			_BV(10)
#define SRIO_ErrorCheckingDisable		_BV(11)
#define SRIO_Multicast_eventParticipant		_BV(12)
#define SRIO_EnumerationBoundary		_BV(14)
#define SRIO_ExtendedPortWidthOverride_OFFSET	16
#define SRIO_ExtendedPortWidthOverride_MASK	REG_2BIT_MASK
#define SRIO_ExtendedPortWidthOverride(value)	\
	_SET_FV(SRIO_ExtendedPortWidthOverride, value)
#define SRIO_ExtendedPortWidthSupport_OFFSET	18
#define SRIO_ExtendedPortWidthSupport_MASK	REG_2BIT_MASK
#define SRIO_ExtendedPortWidthSupport(value)	\
	_GET_FV(SRIO_ExtendedPortWidthSupport, value)
#define SRIO_PortType				_BV(31)

/* 6.6.10 Port n Control 2 CSRs */
#define SRIO_SelectedBaudrate_OFFSET		0
#define SRIO_SelectedBaudrate_MASK		REG_4BIT_MASK
#define SRIO_SelectedBaudrate(value)		\
	_SET_FV(SRIO_SelectedBaudrate, value)
#define SRIO_BaudrateDiscoverySuport		_BV(4)
#define SRIO_BaudrateDiscoveryEnable		_BV(5)
#define SRIO_1_25GBaudSupport			_BV(6)
#define SRIO_1_25GBaudEnable			_BV(7)
#define SRIO_2_5GBaudSupport			_BV(8)
#define SRIO_2_5GBaudEnable			_BV(9)
#define SRIO_3_125GBaudSupport			_BV(10)
#define SRIO_3_125GBaudEnable			_BV(11)
#define SRIO_5_0GBaudSupport			_BV(12)
#define SRIO_5_0GBaudEnable			_BV(13)
#define SRIO_6_25GBaudSupport			_BV(14)
#define SRIO_6_25GBaudEnable			_BV(15)
#define SRIO_EnableInactiveLanes		_BV(28)
#define SRIO_Data_scrambling_disable		_BV(29)
#define SRIO_RemoteTransmitEmphasisControlSupport	_BV(30)
#define SRIO_RemoteTransmitEmphasisControlEnable	_BV(31)

/* 6.7 LP-Serial Lane Extended Features Block */
#define SRIO_CSR_LaneStatus0(n, m)			\
	(0x10 + (n) * (0x20 * (m)))
#define SRIO_CSR_LaneStatus1(n, m)			\
	(0x14 + (n) * (0x20 * (m)))

/* 6.7.2.2 Lane n Status 0 CSRs */
#define SRIO_PortNumber_OFFSET			0
#define SRIO_PortNumber_MASK			REG_8BIT_MASK
#define SRIO_PortNumber(value)			_GET_FV(SRIO_PortNumber, value)
#define SRIO_LaneNumber_OFFSET			8
#define SRIO_LaneNumber_MASK			REG_4BIT_MASK
#define SRIO_LaneNumber(value)			_GET_FV(SRIO_LaneNumber, value)
#define SRIO_Transmitter_type			_BV(12)
#define SRIO_Transmitter_mode			_BV(13)
#define SRIO_Receiver_type_OFFSET		14
#define SRIO_Receiver_type_MASK			REG_2BIT_MASK
#define SRIO_Receiver_type(value)		\
	_GET_FV(SRIO_Receiver_type, value)
#define SRIO_Receiver_input_inverted		_BV(16)
#define SRIO_Receiver_trained			_BV(17)
#define SRIO_Receiver_lane_sync			_BV(18)
#define SRIO_Receiver_lane_ready		_BV(19)
#define SRIO_10B_10B_decoding_errors_OFFSET	20
#define SRIO_10B_10B_decoding_errors_MASK	REG_4BIT_MASK
#define SRIO_10B_10B_decoding_errors(value)	_GET_FV(SRIO_10B_10B_decoding_errors, value)
#define SRIO_Lane_sync_state_change		_BV(24)
#define SRIO_Rcvr_trained_state_change		_BV(25)
#define SRIO_Status_1_CSR_implemented		_BV(28)
#define SRIO_Status_2_7_CSR_implemented_OFFSET	29
#define SRIO_Status_2_7_CSR_implemented_MASK	REG_3BIT_MASK
#define SRIO_Status_2_7_CSR_implemented(value)	\
	_GET_FV(SRIO_Status_2_7_CSR_implemented, value)

/* 6.7.2.3 Lane n Status 1 CSRs */
#define SRIO_IDLE2_received				_BV(0)
#define SRIO_IDLE2_information_current			_BV(1)
#define SRIO_Values_changed				_BV(2)
#define SRIO_Implementation_defined			_BV(3)
#define SRIO_Connected_port_lane_receiver_trained	_BV(4)
#define SRIO_Receiver_port_width_OFFSET			5
#define SRIO_Receiver_port_width_MASK			REG_3BIT_MASK
#define SRIO_Receiver_port_width(value)			\
	_GET_FV(SRIO_Receiver_port_width, value)
#define SRIO_Lane_number_in_connected_port_OFFSET	8
#define SRIO_Lane_number_in_connected_port_MASK		REG_4BIT_MASK
#define SRIO_Lane_number_in_connected_port(value)	\
	_GET_FV(SRIO_Lane_number_in_connected_port, value)
#define SRIO_Connected_port_transmit_emphasisTapM1_status_OFFSET	12
#define SRIO_Connected_port_transmit_emphasisTapM1_status_MASK		\
	REG_2BIT_MASK
#define SRIO_Connected_port_transmit_emphasisTapM1_status(value)	\
	_GET_FV(SRIO_Connected_port_transmit_emphasisTapM1_status, value)
#define SRIO_Connected_port_transmit_emphasisTapP1_status_OFFSET	14
#define SRIO_Connected_port_transmit_emphasisTapP1_status_MASK		\
	REG_2BIT_MASK
#define SRIO_Connected_port_transmit_emphasisTapP1_status(value)	\
	_GET_FV(SRIO_Connected_port_transmit_emphasisTapP1_status, value)
#define SRIO_Connected_port_scrambling_descrambling_enabled		_BV(16)

/* 6.8 Virtual Channel Extended Features Block */

void srio_init_port(void);

#endif /* __RIO_SRIO_H_INCLUDE__ */
