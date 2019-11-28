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
 * @(#)scs.h: Smard card slot interfaces
 * $Id: scs.h,v 1.21 2011-02-21 02:12:57 zhenglv Exp $
 */

#ifndef __SCS_H_INCLUDE__
#define __SCS_H_INCLUDE__

#include <target/generic.h>

#ifdef CONFIG_SCS_DEBUG
#define scs_debug(tag, val)		dbg_print((tag), (val))
#define scs_debug_sl_state(state)					\
	do {								\
		if (ifd_slot_get_state() != state)			\
			scs_debug(SCS_DEBUG_SLOT_STATE, state);		\
	} while (0)
#define scs_debug_sl_event(event)					\
	do {								\
		if (!bits_raised(ifd_slot_event, event))		\
			scs_debug(SCS_DEBUG_SLOT_EVENT, __ffs16(event));\
	} while (0)
#define scs_debug_xg_state(state)					\
	do {								\
		if (state != ifd_xchg_get_state())			\
			scs_debug(SCS_DEBUG_XCHG_STATE, state);		\
	} while (0)
#define scs_debug_xg_event(event)					\
	do {								\
		if (!bits_raised(ifd_xchg_event, event))		\
			scs_debug(SCS_DEBUG_XCHG_EVENT, event);		\
	} while (0)
#else
#define scs_debug(type, val)
#define scs_debug_sl_state(state)
#define scs_debug_sl_event(event)
#define scs_debug_xg_state(state)
#define scs_debug_xg_event(event)
#endif

/* XXX: Faster Interrupt Handler
 * Such dumpers are called from interrupt handler.  Enabling this will
 * reduce throughput of the interrupt handler.
 */
#ifdef CONFIG_SCS_DEBUG_DUMP
#define scs_debug_dump(b)		debug_dump(b)
#else
#define scs_debug_dump(b)
#endif

#define SCS_ATR_MAX			33
#ifdef CONFIG_SCS_APDU_SIZE
#define SCS_APDU_MAX			CONFIG_SCS_APDU_SIZE
#else
#define SCS_APDU_MAX			256
#endif

/* TODO: Remove overhead of APDU extra size. */
#if SCS_APDU_MAX <= 256
typedef uint16_t scs_size_t;
typedef uint16_t scs_off_t;
#else
typedef uint32_t scs_size_t;
typedef uint32_t scs_off_t;
#endif
typedef uint8_t scs_err_t;

typedef void (*scs_cmpl_cb)(void);
typedef scs_err_t (*scs_txfr_cb)(scs_size_t nc, scs_size_t ne);
typedef void (*scs_tvoid_cb)(void);

/* Error number defined refers ccid */
#define SCS_ERR_WT_EXTEN		0xFF
#define SCS_ERR_SANITY			0x20
#define SCS_ERR_ABORTED			0x0B
#define SCS_ERR_PROGRESS		0x0A
#define SCS_ERR_UNSUPPORT		0x09
#define SCS_ERR_BAD_POWER		0x08
#define SCS_ERR_PARITY_ERR		0x07
#define SCS_ERR_OVERRUN			0x06
#define SCS_ERR_BUSY_SLOT		0x05
#define SCS_ERR_BUSY_AUTO		0x04
#define SCS_ERR_TIMEOUT			0x03
#define SCS_ERR_NOTPRESENT		0x02
#define SCS_ERR_HW_ERROR		0x01
#define SCS_ERR_SUCCESS			0x00

#define SCS_PROTO_T0			0x00
#define SCS_PROTO_T1			0x01
#define SCS_PROTO_MAX			0x0E
#define SCD_PROTO_T15			0x0F

#endif /* __SCS_H_INCLUDE__ */
