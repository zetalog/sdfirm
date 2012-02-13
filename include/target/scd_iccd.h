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
 * @(#)scd_iccd.h: integrated circuit card devices (ICCD) interfaces
 * $Id: scd_iccd.h,v 1.32 2011-11-10 07:02:37 zhenglv Exp $
 */

#ifndef __SCD_ICCD_H_INCLUDE__
#define __SCD_ICCD_H_INCLUDE__

#include <target/scs_slot.h>
#include <target/cos.h>

#if NR_SCS_SLOTS == 0
#error "SCD device is not defined"
#endif

#ifdef CONFIG_ICCD_OLDVER
#define SCD_VERSION			0x100
#define ICCD_FEATURE_SPECIFIC		0x00000000
#else
#define SCD_VERSION			0x110
#define ICCD_FEATURE_SPECIFIC		0x00000800
#endif

#define USB_INTERFACE_PROTOCOL_ICCD_A	0x01
#define USB_INTERFACE_PROTOCOL_ICCD_B	0x02

#ifndef CONFIG_SCD_BULK
#ifdef CONFIG_SCD_INTERRUPT
#define USB_INTERFACE_PROTOCOL_SCD	USB_INTERFACE_PROTOCOL_ICCD_B
#else
#define USB_INTERFACE_PROTOCOL_SCD	USB_INTERFACE_PROTOCOL_ICCD_A
#endif
#endif

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
#define NR_SCD_USB_SLOTS		1
#define NR_SCD_USB_QUEUES		NR_SCD_USB_SLOTS
#define ICCD_SID_USB			(NR_SCD_USB_SLOTS-1)

#define ICCD_BUF_SIZE			261
#define ICCD_MESSAGE_SIZE		(ICCD_BUF_SIZE + SCD_HEADER_SIZE)

#ifdef CONFIG_ICCD_COS
#define scd_slot_select(sid)
#define scd_get_slot_error()		cos_get_error()
#define scd_get_slot_status()		cos_get_status()
#define scd_read_byte(idx)		cos_xchg_read(idx)
#define scd_write_byte(idx, b)		cos_xchg_write(idx, b)
#define scd_xchg_avail()		cos_xchg_avail()
#define scd_xchg_block(nc, ne)		cos_xchg_block(nc, ne)
#define scd_power_on(cls)		cos_power_on()
#define scd_power_off()			cos_power_off()
#define scd_register_completion(cb) 	cos_register_completion(cb)
#else
#define scd_slot_select(sid)		scs_slot_select(scd_qid)
#define scd_get_slot_error()		scs_get_slot_error()
#define scd_get_slot_status()		scs_get_slot_status()
#define scd_read_byte(idx)		scs_slot_xchg_read(idx)	
#define scd_write_byte(idx, b)		scs_slot_xchg_write(idx, b)
#define scd_xchg_avail()		scs_slot_xchg_avail()
#define scd_xchg_block(nc, ne)		scs_slot_xchg_block(nc, ne)
#define scd_power_on(cls)		scs_slot_power_on()
#define scd_power_off()			scs_slot_power_off()
#define scd_register_completion(cb)	scs_slot_register_completion(cb)
#endif

#endif /* __SCD_ICCD_H_INCLUDE__ */
