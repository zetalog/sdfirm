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
 * @(#)usb_iccd.h: integrated circuit card devices (ICCD) interfaces
 * $Id: usb_iccd.h,v 1.32 2011-11-10 07:02:37 zhenglv Exp $
 */

#ifndef __SCD_ICCD_H_INCLUDE__
#define __SCD_ICCD_H_INCLUDE__

#include <target/scs_slot.h>
#include <target/cos.h>

#if NR_SCS_SLOTS == 0
#error "SCD device is not defined"
#endif

#define USB_INTERFACE_PROTOCOL_ICCD_A	0x01
#define USB_INTERFACE_PROTOCOL_ICCD_B	0x02

#ifdef CONFIG_ICCD_PROTO_BULK
#define USB_INTERFACE_PROTOCOL_ICCD	USB_INTERFACE_PROTOCOL_SCD
#elif defined(CONFIG_ICCD_PROTO_A)
#define USB_INTERFACE_PROTOCOL_ICCD	USB_INTERFACE_PROTOCOL_ICCD_A
#elif defined(CONFIG_ICCD_PROTO_B)
#define USB_INTERFACE_PROTOCOL_ICCD	USB_INTERFACE_PROTOCOL_ICCD_B
#endif

#define ICCD_RDR2PC_NOTIFYSLOTCHANGE	0x50

#ifdef CONFIG_ICCD_COS
#define NR_SCD_SLOTS			1
#define scd_sid				0
typedef uint8_t				scd_sid_t;
#else
#define NR_SCD_SLOTS			NR_SCS_SLOTS
#define scd_sid				scs_sid
typedef scs_sid_t			scd_sid_t;
#endif
#define NR_SCD_QUEUES			NR_SCS_SLOTS
#define ICCD_SID_USB			(ICCD_MAX_BUSY_SLOT-1)

#define ICCD_BUF_SIZE			261
#define ICCD_MESSAGE_SIZE		(ICCD_BUF_SIZE + SCD_HEADER_SIZE)

struct iccd_hwerr {
	/* hwerr interrupt is pending */
	uint8_t bState;
#define ICCD_HWERR_PENDING		0x01
#define ICCD_HWERR_RUNNING		0x02
	uint8_t bSeq;
#define ICCD_HWERR_OVERCURRENT		0x01
	uint8_t bHardwareErrorCode;
	uint8_t bPendingSeq;
	uint8_t bPendingCode;
};
#define CCID_IRQ_HWERR_SIZE		0x04

#define ICCD_INTR_RUNNING_SET		0x00
#define ICCD_INTR_RUNNING_UNSET		0x01
#define ICCD_INTR_PENDING_SET		0x02
#define ICCD_INTR_PENDING_UNSET		0x03
#define ICCD_INTR_ICC_PRESENT		0x04
#define ICCD_INTR_ICC_NOTPRESENT	0x05

#define ICCD_ERROR_ICC_MUTE		0xFE
#define ICCD_ERROR_XFR_OVERRUN		0xFC
#define ICCD_ERROR_HW_ERROR		0xFB
#define ICCD_ERROR_USER_DEFINED		0xC0
#define ICCD_ERROR_USER(e)		(ICCD_ERROR_USER_DEFINED-e)
#define ICCD_ERROR_RESERVED		0x80
#define ICCD_ERROR_CMD_UNSUPPORT	0x00

/* Windows CCID requires this, though these should not be an ICCD command */
#define ICCD_PC2RDR_GETPARAMETERS	0x6C

#ifdef CONFIG_ICCD_COS
#define __iccd_get_error()		cos_get_error()
#define scd_read_byte(idx)		cos_xchg_read(idx)
#define scd_write_byte(idx, b)		cos_xchg_write(idx, b)
#define scd_xchg_avail()		cos_xchg_avail()
#define scd_xchg_block(nc, ne)		cos_xchg_block(nc, ne)
#define __iccd_power_off()		cos_power_off()
#define __iccd_power_on()		cos_power_on()
#define __iccd_reg_handlers(cb1, cb2) 		\
	do {					\
		cos_register_handlers(cb2);	\
		cb1();				\
	} while (0)
#else
#define __iccd_get_error()		scs_get_slot_error()
#define scd_read_byte(idx)		scs_slot_xchg_read(idx)	
#define scd_write_byte(idx, b)		scs_slot_xchg_write(idx, b)
#define scd_xchg_avail()		scs_slot_xchg_avail()
#define scd_xchg_block(nc, ne)		scs_slot_xchg_block(nc, ne)
#define __iccd_power_off()		scs_slot_power_off()
#define __iccd_power_on()		scs_slot_power_on()
#define __iccd_reg_handlers(cb1, cb2) 	scd_notify_slot(cb1, cb2)
#endif

#endif /* __SCD_ICCD_H_INCLUDE__ */
