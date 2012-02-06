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
 * @(#)ifd.h: ISO/IEC 7816-3 interface device interfaces
 * $Id: ifd.h,v 1.85 2011-11-07 03:45:30 zhenglv Exp $
 */

#ifndef __IFD_H_INCLUDE__
#define __IFD_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/scs.h>

typedef uint8_t ifd_sid_t;
typedef uint16_t ifd_event_t;

typedef void (*ifd_atr_cb)(uint8_t y);

#include <driver/ifd.h>

#ifdef CONFIG_IFD_MAX_SLOTS
#define NR_IFD_SLOTS			CONFIG_IFD_MAX_SLOTS
#else
#define NR_IFD_SLOTS			1
#endif
#define INVALID_IFD_SID			NR_IFD_SLOTS

#ifdef CONFIG_IFD_WT_RATIO
#define IFD_WT_RATIO			CONFIG_IFD_WT_RATIO
#else
#define IFD_WT_RATIO			1
#endif

#define NR_IFD_PROTOS			2

/* T=0, header is 5 bytes
 * T=1, header+tail is 4-5 bytes
 */
#define IFD_HEADER_SIZE			5
#define IFD_BUF_SIZE			256+IFD_HEADER_SIZE

#define NR_IFD_FIS			14
#define NR_IFD_DIS			10

#define IFD_FI_MIN			372

#define IFD_ETU_IMPLICIT		0x11	/* F = 372, D = 1 */
/* #define IFD_WT_ATR			(40000/372+1) */
#define IFD_WT_PPS			9600

#define IFD_CLASS_AUTO			0x00	/* auto voltage selection */
#define IFD_CLASS_A			0x01	/* voltage = 5V */
#define IFD_CLASS_B			0x02	/* voltage = 3V */
#define IFD_CLASS_C			0x03	/* voltage = 1.8V */
#define IFD_CLASS_NONE			0xFF

#define IFD_FD_IMPLICIT			0x11
#define IFD_N_IMPLICIT			0x00
#define IFD_N_MINIMUM			0xFF

#define IFD_PROTO_DEF			SCS_PROTO_T0
#define IFD_GLOBAL_T15			0x0F
#define IFD_PROTO_INVALID		0xFF

#define IFD_CONV_DIRECT			0x3B
#define IFD_CONV_INVERSE		0x3F

#define IFD_CLOCK_RUNNING		0x00
#define IFD_CLOCK_STOPPED_L		0x01
#define IFD_CLOCK_STOPPED_H		0x02
#define IFD_CLOCK_STOPPED_U		0x03

#define IFD_SLOT_STATE_NOTPRESENT	0x00
#define IFD_SLOT_STATE_PRESENT		0x01
#define IFD_SLOT_STATE_HWERROR		0x02
#define IFD_SLOT_STATE_ATR_READY	0x03
#define IFD_SLOT_STATE_PPS_READY	0x04
#define IFD_SLOT_STATE_SELECTED		0x05
#define IFD_SLOT_STATE_ACTIVATED	0x06

#define IFD_SLOT_EVENT_ICC_IN		(1<<0)
#define IFD_SLOT_EVENT_ICC_OUT		(1<<1)
#define IFD_SLOT_EVENT_CLS_CHG		(1<<2)
#define IFD_SLOT_EVENT_HW_ERR		(1<<3)
#define IFD_SLOT_EVENT_PWR_OFF		(1<<4)	/* check if icc in */
#define IFD_SLOT_EVENT_PWR_OK		(1<<5)
#define IFD_SLOT_EVENT_ATR_OK		(1<<6)
#define IFD_SLOT_EVENT_PPS_OK		(1<<7)
#define IFD_SLOT_EVENT_WARM_RST		(1<<8)
#define IFD_SLOT_EVENT_XFR_SYNC		(1<<15)

#define IFD_HW_FREQ_DEF			(uint32_t)4000
#define IFD_HW_DATA_DEF			((uint32_t)((IFD_HW_FREQ_DEF * 1000) / IFD_FI_MIN))

uint8_t ifd_cf_nr_freq(void);
uint32_t ifd_cf_get_freq(uint8_t index);
uint32_t ifd_get_freq(void);
void ifd_set_freq(uint32_t freq);

/* Error number defined refers to the CCID specification */
#define IFD_ERR_BAD_PCK			(SCS_ERR_SANITY|8)
#define IFD_ERR_BAD_PPSR		(SCS_ERR_SANITY|7)
#define IFD_ERR_BAD_PPSS		(SCS_ERR_SANITY|6)
#define IFD_ERR_BAD_FD			(SCS_ERR_SANITY|5)
#define IFD_ERR_BAD_PROTO		(SCS_ERR_SANITY|4)
#define IFD_ERR_BAD_TCK			(SCS_ERR_SANITY|3)
#define IFD_ERR_BAD_TS			(SCS_ERR_SANITY|2)
#define IFD_ERR_BAD_CLASS		(SCS_ERR_SANITY|1)

#define IFD_SEQ_AUTO_SEQ		0x00
#define IFD_SEQ_POWER_ON		0x01
#define IFD_SEQ_POWER_OFF		0x02
#define IFD_SEQ_SET_PARAM		0x03
/* #define IFD_SEQ_RST_PARAM		0x04 */
#define IFD_SEQ_INTERCHANGE		0x05
#define IFD_SEQ_IDLE			0xFF

#ifdef CONFIG_IFD_CLASS_SELECT_DESCEND
#define IFD_CLASS_INIT			IFD_CLASS_A
#endif
#ifdef CONFIG_IFD_CLASS_SELECT_ASCEND
#define IFD_CLASS_INIT			IFD_CLASS_C
#endif

struct ifd_slot {
	uint8_t status;

	/* applied class */
	uint8_t	clazz;
	/* applied Fi, f, Di */
	uint8_t fd;
	/* applied extra GT, TC1 */
	uint8_t n;
	/* applied protocol */
	uint8_t proto;

	/* TS, read only */
	uint8_t conv;

	/* TA1 */
	uint8_t ind_fd;
	/* T of TD1: first offered protocol */
	uint8_t first_proto;
	/* T of TDi (i != 1) */

	/* card supported protocols */
	uint16_t ind_protos;

	/* TA2 */
#define IFD_INFO_XCHG_SPECIFIC		0x00
#define IFD_INFO_XCHG_NEGO_PPS		0x01
#define IFD_INFO_XCHG_NEGO_1ST		0x02
	uint8_t info_xchg;
	uint8_t flags;
	/* GT's R should use FD indications in TA1 */
#define IFD_FLAG_GT_USE_IND_FD		0x01
	/* warm reset if specific mode's parameters not supported */
#define IFD_FLAG_SPEC_CAP_CHANGE	0x02

	/* Bit 5 of TA2 indicates the definition of parameters F & D:
	 * if bit 5 is set to 0, then Fi, Di defined by TA1 shall apply
	 * if bit 5 is set to 1, then the implicit value shall apply
	 */
	uint8_t spec_fd;
	/* T of TD2: specific protocol */
	uint8_t spec_proto;

	/* TA15 */
#define IFD_CLASS_IND_IMPLICITY		0x00
#define IFD_CLASS_A_IND			1<<IFD_CLASS_A
#define IFD_CLASS_B_IND			1<<IFD_CLASS_B
#define IFD_CLASS_C_IND			1<<IFD_CLASS_C
#define IFD_CLASS_MASK			0x07
	uint8_t	ind_cls;

#define IFD_CLOCK_STOP_NONE		0x00
#define IFD_CLOCK_STOP_BOTH		0xC0
#define IFD_CLOCK_STOP_STATE_L		0x40
#define IFD_CLOCK_STOP_STATE_H		0x80
#define IFD_CLOCK_STOP_MASK		0xC0
	uint8_t clk_stop;

	/* negotiation */
	uint8_t	nego_clazz;
	uint8_t nego_fd;
	uint8_t nego_proto;

	scs_off_t tx;
	scs_off_t rx;
	scs_size_t nc;
	scs_size_t ne;
#ifdef CONFIG_IFD_PANIC_BUF
	scs_size_t nr_valid;
#endif
	scs_cmpl_cb cmpl;
	uint32_t freq;

	uint16_t cgt;
	uint16_t bgt;
	uint32_t cwt;
	uint32_t bwt;
};

struct ifd_xchg {
	uint8_t state;
	scs_err_t error;
	scs_size_t nr;
};

struct ifd_user {
	uint8_t seq;
};

#define TA_EXISTS			0x10
#define TB_EXISTS			0x20
#define TC_EXISTS			0x40
#define TD_EXISTS			0x80

#define TD_Y(td)			((td) & 0xF0)
#define TD_T(td)			((td) & 0x0F)
#define T0_K(td)			((td) & 0x0F)
#define PPS_Y(pps0)			((pps0) & 0x70)
#define PPS_T(pps0)			((pps0) & 0x0F)

struct ifd_proto {
	uint8_t id;
	uint8_t hdr;
	ifd_atr_cb atr_parse;
	scs_txfr_cb xchg_init;
	scs_tvoid_cb xchg_aval;
	scs_tvoid_cb xchg_cmpl;
	scs_tvoid_cb ne_expire;
	scs_tvoid_cb reset_param;
	scs_tvoid_cb conf_param;
	scs_tvoid_cb nego_pps;
};
__TEXT_TYPE__(struct ifd_proto, ifd_proto_t);

struct ifd_xfr {
	scs_off_t tx_start;
	scs_off_t rx_start;
	scs_size_t nc;
	scs_size_t ne;
	scs_size_t tx;
	scs_size_t rx;
};

#define IFD_XCHG_EVENT_WT_EXPIRE	(1<<0)
#define IFD_XCHG_EVENT_XFR_CMPL		(1<<1)
#define IFD_XCHG_EVENT_ICC_OUT		(1<<2)
#define IFD_XCHG_EVENT_PARITY_ERR	(1<<3)
#define IFD_XCHG_EVENT_POWER_ERR	(1<<4)
#define IFD_XCHG_EVENT_CONV_ERR		(1<<5)
#define IFD_XCHG_EVENT_XFR_OVER		(1<<6)
/*#define IFD_XCHG_EVENT_XFR_MARK		(1<<7)*/

#define IFD_XCHG_STATE_IDLE		0x00
#define IFD_XCHG_STATE_BUSY		0x01
#define IFD_XCHG_STATE_ERROR		0x02

struct ifd_atr_parser {
	uint8_t ta;
	uint8_t tb;
	uint8_t tc;
	uint8_t td;
	uint8_t td_last;
	uint8_t len;
	uint8_t csum;
	boolean tck_exists;
};

struct ifd_xchg_atr {
	uint8_t atr_state;
#define IFD_ATR_TS	0x00
#define IFD_ATR_T0	0x01
#define IFD_ATR_TD	0x02
#define IFD_ATR_TK	0x03
#define IFD_ATR_TCK	0x04
	boolean atr_tck;
	uint8_t atr_k;
	uint8_t atr_y;
	uint8_t atr_td_last;
	scs_off_t atr_rx;
};

struct ifd_xchg_pps {
	uint8_t pps_req[4];		/* PPS1, PPS2, PPS3 */
	scs_size_t pps_size;
	scs_size_t ppsr_size;
	boolean pps_cmpl;
};

void ifd_xchg_start(void);
void ifd_xchg_stop(uint8_t event);
void ifd_xchg_run(void);
#ifdef CONFIG_IFD_PANIC_BUF
void ifd_xfr_block(scs_off_t tx, scs_size_t nc,
		   scs_off_t rx, scs_size_t ne,
		   scs_size_t nr_valid);
#else
#define ifd_xfr_block(tx, nc, rx, ne, nr_valid) \
	__ifd_xfr_block(tx, nc, rx, ne)
#endif
void __ifd_xfr_reset(void);
void __ifd_xfr_block(scs_off_t tx, scs_size_t nc,
		     scs_off_t rx, scs_size_t ne);
void ifd_xfr_txaval(void);
uint8_t ifd_xfr_read(scs_off_t idx);
uint8_t ifd_xfr_read_last(void);

/*=========================================================================
 * slot interfaces
 *=======================================================================*/
#if NR_IFD_SLOTS > 1
ifd_sid_t ifd_sid_save(ifd_sid_t sid);
void ifd_sid_restore(ifd_sid_t sid);
extern ifd_sid_t ifd_slid;
extern __near__ struct ifd_slot ifd_slots[NR_IFD_SLOTS];
#define ifd_slot_ctrl ifd_slots[ifd_slid]
#else
#define ifd_sid_save(sid)		0
#define ifd_sid_restore(sid)
#define ifd_slid			0
extern __near__ struct ifd_slot ifd_slot_ctrl;
#endif
#define ifd_sid_select(sid)		ifd_sid_restore(sid)
uint8_t ifd_slot_get_state(void);

void ifd_detect_icc_out(void);
void ifd_detect_icc_in(void);
boolean ifd_detect_icc_power(void);
void ifd_delay_clock(uint8_t cycle);

/*=========================================================================
 * contact interfaces
 *=======================================================================*/
#define ICC_CONTACT_CLK	3
#define ICC_CONTACT_IO	7
#define ifd_get_contact(c)		ifd_hw_get_contact(c)
#define ifd_set_contact(c, high)	ifd_hw_set_contact(c, high)

/*=========================================================================
 * xchg interfaces
 *=======================================================================*/
uint8_t ifd_xchg_get_state(void);
void ifd_xchg_set_state(uint8_t state);
void ifd_xchg_set_error(scs_err_t errno);
scs_err_t ifd_xchg_get_error(void);

void ifd_xchg_move(scs_off_t to, scs_off_t from, scs_size_t cnt);
void ifd_xchg_write(scs_off_t index, uint8_t byte);
uint8_t ifd_xchg_read(scs_off_t index);
void ifd_xchg_end(scs_size_t nr);
void ifd_xchg_begin(scs_off_t tx, scs_size_t nc,
		    scs_off_t rx, scs_size_t ne,
		    scs_size_t nr_valid);
void ifd_xchg_raise(uint8_t event);
boolean ifd_xchg_raised_any(void);

/*=========================================================================
 * user interfaces
 *=======================================================================*/
void ifd_register_completion(scs_cmpl_cb completion);

scs_err_t ifd_write_byte(scs_off_t index, uint8_t byte);
scs_err_t ifd_write_byte_mask(scs_off_t index,
			      uint8_t byte, uint8_t mask);
uint8_t ifd_read_byte(scs_off_t index);
scs_err_t ifd_xchg_block(scs_size_t nc, scs_size_t ne);
void ifd_xchg_abort(ifd_sid_t sid);
scs_size_t ifd_xchg_avail(void);

scs_err_t ifd_power_on(uint8_t clazz);
scs_err_t ifd_power_off(void);
scs_err_t ifd_set_param(void);
scs_err_t ifd_reset_param(void);
scs_err_t ifd_set_freq_data(uint32_t clock, uint32_t rate);

#ifdef CONFIG_IFD_SLOT
scs_err_t ifd_slot_activation(boolean cmpl);
void ifd_slot_completion(scs_err_t err);
void ifd_slot_synchronization(uint8_t state);
#else
#define ifd_slot_activation(cmpl)
#define ifd_slot_completion(err)
#define ifd_slot_synchronization(state)
#endif

/* Mechanical */
#ifdef CONFIG_IFD_MECHA_CONTROL
scs_err_t ifd_lock_card(void);
scs_err_t ifd_unlock_card(void);
#else
#define ifd_lock_card()		SCS_ERR_UNSUPPORT
#define ifd_unlock_card()	SCS_ERR_UNSUPPORT
#endif

/* IccClock */
#ifdef CONFIG_IFD_CLOCK_CONTROL
uint8_t ifd_clock_status(void);
scs_err_t ifd_restart_clock(void);
scs_err_t ifd_stop_clock(void);
#else
#define ifd_clock_status()	IFD_CLOCK_RUNNING
#define ifd_restart_clock()	SCS_ERR_UNSUPPORT
#define ifd_stop_clock()	SCS_ERR_UNSUPPORT
#endif

/* ClockFreq & DataRate */
uint32_t ifd_get_data(void);
uint8_t ifd_get_convention();

/*=========================================================================
 * report interfaces
 *=======================================================================*/
uint32_t ifd_cf_get_data(uint32_t freq,
			 uint8_t di, uint8_t fi);
uint8_t ifd_cf_nr_data(void);

/*=========================================================================
 * atr_parse interfaces
 *=======================================================================*/
uint8_t ifd_atr_get_ta(void);
uint8_t ifd_atr_get_tb(void);
uint8_t ifd_atr_get_tc(void);

/*=========================================================================
 * protocol interfaces
 *=======================================================================*/
uint8_t ifd_register_protocol(ifd_proto_t *cproto);
uint8_t ifd_proto_pid(uint8_t proto);
uint8_t ifd_get_proto(void);
ifd_proto_t *ifd_get_protocol(void);
ifd_proto_t *__ifd_get_protocol(uint8_t proto);

uint16_t ifd_get_F(uint8_t fi);
uint8_t ifd_get_D(uint8_t di);

#include <target/ifd_t0.h>
#include <target/ifd_t1.h>

#endif /* __IFD_H_INCLUDE__ */
