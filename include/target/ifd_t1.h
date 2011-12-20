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
 * @(#)iso7816_t1.h: ISO/IEC 7816-3 T=1 protocol interfaces
 * $Id: ifd_t1.h,v 1.17 2010-11-05 03:58:54 zhenglv Exp $
 */

#ifndef __ISO7816_T1_H_INCLUDE__
#define __ISO7816_T1_H_INCLUDE__

struct ifd_conf_t1 {
	uint8_t tccks;	/* LRC is default */
#define IFD_T1_GI_IMPLICIT	22
#define IFD_T1_WI_IMPLICIT	0xD4
#define IFD_T1_BWI_IMPLICIT	0x04
#define IFD_T1_CWI_IMPLICIT	0x0D
#define IFD_T1_IFS_IMPLICIT	0x20
#define IFD_T1_IFS_MAXIMUM	0xFE
	/* unnegotiatables */
	uint8_t wi;
	uint8_t ifsc;
	uint8_t nad;

	uint8_t ifsd;
 	uint8_t nego_ifsd;
#define IFD_T1_GET_SAD(nad)		(LOBYTE(nad) & 0x07)
#define IFD_T1_GET_DAD(nad)		(HIBYTE(nad) & 0x07)
#define IFD_T1_SET_SAD(nad, sad)	(((nad) &= ~0x0f), (nad) |= ((sad) & 0x07))
#define IFD_T1_SET_DAD(nad, dad)	(((nad) &= ~0xf0), (nad) |= (((dad) & 0x07) << 4))
};

#define IFD_T1_PROLOGUE_SIZE	3
#define IFD_T1_SAFE_IFSD	1

struct ifd_xchg_t1 {
	boolean cmpl;

#ifdef CONFIG_IFD_XCHG_APDU
#define IFD_T1_SAVE_SIZE		8
	uint8_t saved_buf[IFD_T1_SAVE_SIZE];
	uint8_t saved_len;

	uint8_t ns;
	uint8_t nr;
	boolean tx_cmpl;
	boolean rx_cmpl;
	/* PCB of the sending/sended block */
	uint8_t pcb;
	uint8_t ack_type;
	uint8_t ack_info;
	uint8_t wtx;

	uint8_t error;
	uint8_t resync_level;
#define IFD_T1_RETRY_RETRNS	0x03
#define IFD_T1_RETRY_RESYNC	0x02
#define IFD_T1_RETRY_WRESET	0x01
#define IFD_T1_RETRY_FAILED	0x00
	uint8_t resync_retry;
#define	IFD_T1_MAX_RETRY	3
#endif

	/* start of the sending APDU */
	scs_off_t tx;
	/* start of the recving APDU */
	scs_off_t rx;
	/* count of the sending APDU */
	scs_size_t nc;
	/* count of the receiving APDU */
	scs_size_t ne;

	/* start of the sending TPDU */
	scs_off_t tx_blk;
	/* start of the receiving TPDU */
	scs_off_t rx_blk;
	/* count of the received TPDU */
	uint8_t rx_len;
	/* count of the sending TPDU */
	uint8_t ifsc;
	/* count of the receiving TPDU */
	uint8_t ifsd;

	/* count of the sending i-block */
	uint8_t ack_ifsc;

	scs_size_t nr_valid;
};

#ifdef CONFIG_IFD_T1_MAX_IFSD
#define IFD_T1_MAX_IFSD		CONFIG_IFD_T1_MAX_IFSD
#else
#define IFD_T1_MAX_IFSD		IFD_T1_IFS_MAXIMUM
#endif
#define IFD_T1_SAFE_IFSD	1

#define IFD_T1_PARAM_FD		0x00
#define IFD_T1_PARAM_TCCKS	0x01
#define IFD_T1_PARAM_GT		0x02
#define IFD_T1_PARAM_WI		0x03
#define IFD_T1_PARAM_CLOCKSTOP	0x04
#define IFD_T1_PARAM_IFSC	0x05
#define IFD_T1_PARAM_NAD	0x06

/* T=1 protocol constants */
#define IFD_T1_I_BLOCK		0x00
#define IFD_T1_R_BLOCK		0x80
#define IFD_T1_S_BLOCK		0xC0

/* I block */
#define IFD_T1_I_N_SHIFT	6
#define IFD_T1_I_M_SHIFT	5
#define IFD_T1_I_N_BIT		(1<<IFD_T1_I_N_SHIFT)
#define IFD_T1_I_M_BIT		(1<<IFD_T1_I_M_SHIFT)

/* R block */
#define IFD_T1_R_CODE(pcb)	((pcb) & 0x0F)
#define IFD_T1_R_NOERR		0x00
#define IFD_T1_R_PARITY		0x01
#define IFD_T1_R_OTHER		0x02
#define IFD_T1_R_N_SHIFT	4
#define IFD_T1_R_N_BIT		(1<<IFD_T1_R_N_SHIFT)

/* S block stuff */
#define IFD_T1_S_CODE(pcb)	((pcb) & 0x0F)
#define IFD_T1_S_R_SHIFT	5
#define IFD_T1_S_R_BIT		(1<<IFD_T1_S_R_SHIFT)
#define IFD_T1_S_RESYNC		0x00
#define IFD_T1_S_IFS		0x01
#define IFD_T1_S_ABORT		0x02
#define IFD_T1_S_WTX		0x03

#ifdef CONFIG_IFD_T1
scs_err_t ifd_set_t1_param(uint8_t fd, uint8_t tccks,
			   uint8_t gt, uint8_t wi, boolean clk_stop,
			   uint8_t ifsc, uint8_t nad);
uint8_t ifd_get_t1_param(uint8_t what);
void ifd_t1_init(void);
#else
#define ifd_set_t1_param(fd, tccks, gt, wt, clock_stop, ifsc, nad)	\
	SCS_ERR_UNSUPPORT
#define ifd_get_t1_param(what)						0
#define ifd_t1_init()
#endif

#endif /* __ISO7816_T1_H_INCLUDE__ */
