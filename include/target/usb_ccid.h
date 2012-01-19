/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2009
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
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
 * @(#)usb_ccid.h: circuit chip interface card interfaces
 * $Id: usb_ccid.h,v 1.79 2011-02-21 02:27:09 zhenglv Exp $
 */

#ifndef __USB_CCID_H_INCLUDE__
#define __USB_CCID_H_INCLUDE__

#include <target/ifd.h>

typedef ifd_sid_t	ccid_qid_t;
typedef uint8_t		ccid_seq_t;

#define USB_INTERFACE_CLASS_CCID	11

#define USB_INTERFACE_PROTOCOL_CCID	0x00

#define CCID_REQ_ABORT			0x01
#define CCID_REQ_GET_CLOCK_FREQS	0x02
#define CCID_REQ_GET_DATA_RATES		0x03

#define CCID_MESSAGE_SIZE		(IFD_BUF_SIZE + SCD_HEADER_SIZE)

struct ccid_slot {
	uint8_t state;
	/* sync ABORT messages for CTRL and BULK_OUT */
#define CCID_ABORT_NONE			0x00
#define CCID_ABORT_CTRL			0x01
#define CCID_ABORT_BULK			0x02
	uint8_t aborting;
	uint8_t bulk_seq;
	uint8_t ctrl_seq;
};

struct ccid_hwerr {
	/* hwerr interrupt is pending */
	uint8_t bState;
#define CCID_HWERR_PENDING	0x01
#define CCID_HWERR_RUNNING	0x02
	uint8_t bSeq;
#define CCID_HWERR_OVERCURRENT	0x01
	uint8_t bHardwareErrorCode;
	uint8_t bPendingSeq;
	uint8_t bPendingCode;
};
#define CCID_IRQ_HWERR_SIZE	0x04

#ifdef CONFIG_SCD_INTERRUPT
#define NR_CCID_ENDPS		3
#else
#define NR_CCID_ENDPS		2
#endif

/* maximum busy slots in CCID layer */
#define NR_SCD_SLOTS			NR_IFD_SLOTS
/*
 * QID = SID+1: additional QID for BULK_OUT
 */
#define NR_CCID_QUEUES			NR_IFD_SLOTS + 1
#define INVALID_CCID_SID		NR_IFD_SLOTS
#define INVALID_CCID_QID		NR_CCID_QUEUES
#define INVALID_CCID_SEQ		NR_CCID_QUEUES

#define CCID_STATE_IDLE			0x00

#define CCID_ENDP_BULK_IN		0x00
#define CCID_ENDP_BULK_OUT		0x01
#define CCID_ENDP_INTR_IN		0x02

#define CCID_INTR_RUNNING_SET		0x00
#define CCID_INTR_RUNNING_UNSET		0x01
#define CCID_INTR_PENDING_SET		0x02
#define CCID_INTR_PENDING_UNSET		0x03
#define CCID_INTR_ICC_PRESENT		0x04
#define CCID_INTR_ICC_NOTPRESENT	0x05

#define CCID_ERROR_CMD_ABORTED			0xFF
#define CCID_ERROR_ICC_MUTE			0xFE
#define CCID_ERROR_XFR_PARITY_ERROR		0xFD
#define CCID_ERROR_XFR_OVERRUN			0xFC
#define CCID_ERROR_HW_ERROR			0xFB
#define CCID_ERROR_BAD_ATR_TS			0xF8
#define CCID_ERROR_BAD_ATR_TCK			0xF7
#define CCID_ERROR_PROTO_UNSUPPORT		0xF6
#define CCID_ERROR_BAD_T0_PB			0xF4
#define CCID_ERROR_PROTO_DEACTIVATE		0xF3
#define CCID_ERROR_BUSY_AUTO_SEQ		0xF2
#define CCID_ERROR_PIN_TIMEOUT			0xF0
#define CCID_ERROR_PIN_CANCELLED		0xEF
#define CCID_ERROR_CMD_SLOT_BUSY		0xE0
#define CCID_ERROR_USER_DEFINED			0xC0
#define CCID_ERROR_USER(e)			(CCID_ERROR_USER_DEFINED-e)
#define CCID_ERROR_RESERVED			0x80
#define CCID_ERROR_CMD_UNSUPPORT		0x00

/* clock status for RDR2PC_SlotStatus */
#define CCID_CLOCK_RUNNING		0x00
#define CCID_CLOCK_STOPPED_L		0x01
#define CCID_CLOCK_STOPPED_H		0x02
#define CCID_CLOCK_STOPPED_U		0x03

#define CCID_PC2RDR_SETPARAMETERS	0x61
#define CCID_PC2RDR_ICCPOWERON		0x62
#define CCID_PC2RDR_ICCPOWEROFF		0x63
#define CCID_PC2RDR_GETSLOTSTATUS	0x65
#define CCID_PC2RDR_SECURE		0x69
#define CCID_PC2RDR_T0APDU		0x6A
#define CCID_PC2RDR_ESCAPE		0x6B
#define CCID_PC2RDR_GETPARAMETERS	0x6C
#define CCID_PC2RDR_RESETPARAMETERS	0x6D
#define CCID_PC2RDR_ICCCLOCK		0x6E
#define CCID_PC2RDR_XFRBLOCK		0x6F
#define CCID_PC2RDR_MECHANICAL		0x71
#define CCID_PC2RDR_ABORT		0x72
#define CCID_PC2RDR_SETDATAANDFREQ	0x73

#define CCID_RDR2PC_DATABLOCK		0x80
#define CCID_RDR2PC_SLOTSTATUS		0x81
#define CCID_RDR2PC_PARAMETERS		0x82
#define CCID_RDR2PC_ESCAPE		0x83
#define CCID_RDR2PC_DATARATEANDCLOCK	0x84

#define CCID_RDR2PC_NOTIFYSLOTCHANGE	0x50
#define CCID_RDR2PC_HARDWAREERROR	0x51	

/* CCID_PC2RDR_ICCCLOCK */
#define CCID_CLOCK_RESTART		0x00
#define CCID_CLOCK_STOP			0x01

/* CCID_PC2RDR_MECHANICAL */
#define CCID_MECHA_ACCEPT_CARD		0x01
#define CCID_MECHA_EJECT_CARD		0x02
#define CCID_MECHA_CAPTURE_CARD		0x03
#define CCID_MECHA_LOCK_CARD		0x04
#define CCID_MECHA_UNLOCK_CARD		0x05

void ccid_CmdResponse_cmp(void);
void ccid_ScsSequence_cmp(scs_err_t err);
void ccid_XfrBlock_cmp(void);

void ccid_CmdOffset_cmp(uint8_t offset);
void ccid_SlotNotExist_cmp(void);
void ccid_Parameters_cmp(scs_err_t err);
void ccid_DataBlock_cmp(scs_err_t err);

void __ccid_XfrBlock_out(scs_size_t hdr_size, scs_size_t blk_size);

void ccid_SlotStatus_in(void);
void ccid_DataBlock_in(void);

void ccid_display_slot(void);
void ccid_display_default(void);

#endif /* __USB_CCID_H_INCLUDE__ */
