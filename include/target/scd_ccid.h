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
 * @(#)scd_ccid.h: circuit chip interface card interfaces
 * $Id: scd_ccid.h,v 1.79 2011-02-21 02:27:09 zhenglv Exp $
 */

#ifndef __SCD_CCID_H_INCLUDE__
#define __SCD_CCID_H_INCLUDE__

#include <target/ifd.h>

#define SCD_VERSION			0x100

typedef ifd_sid_t			scd_sid_t;
typedef uint8_t				ccid_seq_t;

#define CCID_REQ_ABORT			0x01
#define CCID_REQ_GET_CLOCK_FREQS	0x02
#define CCID_REQ_GET_DATA_RATES		0x03

#define CCID_MESSAGE_SIZE		(IFD_BUF_SIZE + SCD_HEADER_SIZE)

struct ccid_abort {
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

/* DataRate and ClockFreq */
struct ccid_fd_param {
	uint32_t dwClockFrequency;
	uint32_t dwDataRate;
};

#include <target/ccid_spe.h>

/* maximum busy slots in CCID layer */
#define NR_SCD_SLOTS			NR_IFD_SLOTS
#define scd_sid				ifd_slid
#ifdef CONFIG_CCID_ABORT
/*
 * QID = SID+1: additional QID for BULK_OUT
 */
#define NR_SCD_QUEUES			(NR_SCD_SLOTS + 1)
#else
#define NR_SCD_QUEUES			NR_SCD_SLOTS
#endif
#define NR_SCD_USB_SLOTS		NR_SCD_SLOTS
#define NR_SCD_USB_QUEUES		NR_SCD_QUEUES

#define scd_read_byte(i)		ifd_read_byte(i)
#define scd_write_byte(i, v)		ifd_write_byte((i), (v))
#define scd_xchg_avail()		ifd_xchg_avail()
#define scd_xchg_block(nc, ne)		ifd_xchg_block((nc), (ne))
#define scd_power_on(cls)		ifd_power_on(cls)
#define scd_power_off()			ifd_power_off()

#define INVALID_CCID_SEQ		NR_SCD_QUEUES

#define CCID_STATE_IDLE			0x00

#define CCID_ERROR_BAD_ATR_TS			0xF8
#define CCID_ERROR_BAD_ATR_TCK			0xF7
#define CCID_ERROR_PROTO_UNSUPPORT		0xF6
#define CCID_ERROR_BAD_T0_PB			0xF4
#define CCID_ERROR_PROTO_DEACTIVATE		0xF3
#define CCID_ERROR_BUSY_AUTO_SEQ		0xF2
#define CCID_ERROR_PIN_TIMEOUT			0xF0
#define CCID_ERROR_PIN_CANCELLED		0xEF
#define CCID_ERROR_XFR_PARITY_ERROR		0xFD
#define CCID_ERROR_CMD_SLOT_BUSY		0xE0
#define CCID_ERROR_CMD_ABORTED			0xFF

/* clock status for RDR2PC_SlotStatus */
#define CCID_CLOCK_RUNNING		0x00
#define CCID_CLOCK_STOPPED_L		0x01
#define CCID_CLOCK_STOPPED_H		0x02
#define CCID_CLOCK_STOPPED_U		0x03

#define CCID_PC2RDR_SETPARAMETERS	0x61
#define CCID_PC2RDR_SECURE		0x69
#define CCID_PC2RDR_T0APDU		0x6A
#define CCID_PC2RDR_RESETPARAMETERS	0x6D
#define CCID_PC2RDR_ICCCLOCK		0x6E
#define CCID_PC2RDR_MECHANICAL		0x71
#define CCID_PC2RDR_ABORT		0x72
#define CCID_PC2RDR_SETDATAANDFREQ	0x73

#define CCID_RDR2PC_PARAMETERS		0x82
#define CCID_RDR2PC_DATARATEANDCLOCK	0x84

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

void ccid_Parameters_cmp(scs_err_t err);

void ccid_display_slot(void);
void ccid_display_default(void);

#endif /* __SCD_CCID_H_INCLUDE__ */
