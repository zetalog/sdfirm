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
 * @(#)ifd_t1.c: ISO/IEC 7816-3 T=1 protocol functions
 * $Id: ifd_t1.c,v 1.70 2011-10-26 10:09:44 zhenglv Exp $
 */

/* TODO:
 *   1. atr_build (warm reset) support
 */
#include "ifd_priv.h"
#include <target/crc16_ccitt.h>

#if NR_IFD_SLOTS > 1
__near__ struct ifd_conf_t1 ifd_t1_confs[NR_IFD_SLOTS];
#define ifd_t1_conf ifd_t1_confs[ifd_slid]
#else
__near__ struct ifd_conf_t1 ifd_t1_conf;
#endif

#ifdef CONFIG_IFD_XCHG_APDU
static void __ifd_t1_xfr_reset(void);
static void __ifd_t1_xfr_resync(void)
{
	ifd_t1_conf.ifsd = IFD_T1_IFS_IMPLICIT;
	ifd_t1_xchg.ns = ifd_t1_xchg.nr = 0;
}
#else
#define __ifd_t1_xfr_reset()
static void __ifd_t1_xfr_resync(void)
{
	ifd_t1_conf.ifsd = IFD_T1_MAX_IFSD;
}
#endif

static void ifd_t1_apply_wi(uint8_t wi)
{
	ifd_t1_conf.wi = wi;
}

static void ifd_t1_apply_ifsc(uint8_t ifsc)
{
	ifd_t1_conf.ifsc = ifsc;
}

uint8_t ifd_get_t1_param(uint8_t what)
{
	switch (what) {
	case IFD_T1_PARAM_FD:
		return ifd_slot_ctrl.fd;
	case IFD_T1_PARAM_TCCKS:
		return ifd_get_conv();
	case IFD_T1_PARAM_GT:
		return ifd_slot_ctrl.n;
	case IFD_T1_PARAM_WI:
		return ifd_t1_conf.wi;
	case IFD_T1_PARAM_CLOCKSTOP:
		return ifd_slot_ctrl.clk_stop;
	case IFD_T1_PARAM_IFSC:
		return ifd_t1_conf.ifsc;
	case IFD_T1_PARAM_NAD:
		return ifd_t1_conf.nad;
	}
	return 0;
}

#ifndef CONFIG_IFD_AUTO_PPS_PROP
scs_err_t ifd_set_t1_param(uint8_t fd, uint8_t tccks,
			   uint8_t gi, uint8_t wi,
			   boolean clk_stop,
			   uint8_t ifsc, uint8_t nad)
{
	scs_err_t err;

	err = ifd_valid_fd(fd);
	if (err != SCS_ERR_SUCCESS) return err;

	ifd_slot_ctrl.nego_fd = fd;
	ifd_slot_ctrl.nego_proto = IFD_PROTO_T1;

	/* not negotiatables */
	ifd_apply_n(gi);
	ifd_t1_apply_wi(wi);
	ifd_t1_apply_ifsc(ifsc);
	ifd_t1_conf.nad = nad;

	return ifd_set_param();
}
#endif

static void ifd_t1_reset_param(void)
{
	ifd_t1_apply_wi(IFD_T1_WI_IMPLICIT);
	ifd_t1_apply_ifsc(IFD_T1_IFS_IMPLICIT);
	ifd_t1_conf.nad = 0x00;
}

#define IFD_T1_WWT_FACTOR	(uint16_t)960
static void ifd_t1_conf_param(void)
{
	uint32_t cwt, bwt;

	ifd_apply_bgt(22);
	if (ifd_slot_ctrl.n == IFD_N_MINIMUM)
		ifd_apply_cgt(11);
	else
		ifd_apply_cgt(ifd_slot_ctrl.n+12);

	/* XXX:
	 * WI = BWI[7:4] CWI[3:0]
	 * CWT = 2**CWI + 11
	 * BWT = 2**BWI * 960 * D + 11
	 */
	cwt = ((uint32_t)1 << LOHALF(ifd_t1_conf.wi)) + 11; 
	bwt = mul16u((uint16_t)mul16u((uint16_t)1 << HIHALF(ifd_t1_conf.wi),
				      IFD_T1_WWT_FACTOR),
		     ifd_get_D(LOHALF(ifd_slot_ctrl.fd))) + 11;
	ifd_apply_cwt(cwt);
	ifd_apply_bwt(bwt);
	ifd_config_crep(false);

	/* default ifsd */
	__ifd_t1_xfr_reset();
	__ifd_t1_xfr_resync();
}

static void ifd_t1_atr_parse(uint8_t y)
{
	uint8_t tai = ifd_atr_get_ta();
	uint8_t tbi = ifd_atr_get_tb();
	uint8_t tci = ifd_atr_get_tc();

	ifd_t1_apply_ifsc(y & TA_EXISTS ? tai : IFD_T1_IFS_IMPLICIT);
	ifd_t1_apply_wi(y & TB_EXISTS ? tbi : IFD_T1_WI_IMPLICIT);

	if (y & TC_EXISTS)
		ifd_t1_conf.tccks |= (tci & 0x01);
	else
		ifd_t1_conf.tccks &= ~0x01;
}

uint8_t ifd_t1_epilogue_size(void)
{
	return 1 + (ifd_t1_conf.tccks & 0x01);
}

static void ifd_t1_xfr_block(void)
{
	ifd_xchg_begin(ifd_t1_xchg.tx_blk,
		       (scs_size_t)(ifd_t1_xchg.ifsc+
				    IFD_T1_PROLOGUE_SIZE+
				    ifd_t1_epilogue_size()),
		       ifd_t1_xchg.rx_blk,
		       IFD_T1_PROLOGUE_SIZE,
		       ifd_t1_xchg.nr_valid);
	ifd_t1_xchg.cmpl = false;
	ifd_xchg_run();
}

#ifdef CONFIG_IFD_XCHG_APDU
/* APDU exchange requires T=1 protocol stack embedded */
static uint8_t ifd_t1_t(uint8_t pcb)
{
	switch (pcb & 0xC0) {
	case IFD_T1_R_BLOCK:
		return IFD_T1_R_BLOCK;
	case IFD_T1_S_BLOCK:
		return IFD_T1_S_BLOCK;
	default:
		return IFD_T1_I_BLOCK;
	}
}

static boolean ifd_t1_r(uint8_t pcb)
{
	BUG_ON(ifd_t1_t(pcb) != IFD_T1_S_BLOCK);
	return IFD_T1_S_R_BIT & pcb;
}

#if 0
static uint8_t ifd_t1_e(uint8_t pcb)
{
	BUG_ON(ifd_t1_t(pcb) != IFD_T1_R_BLOCK);
	return IFD_T1_R_CODE(pcb);
}
#endif

static uint8_t ifd_t1_s(uint8_t pcb)
{
	BUG_ON(ifd_t1_t(pcb) != IFD_T1_S_BLOCK);
	return IFD_T1_S_CODE(pcb);
}

static boolean ifd_t1_m(uint8_t pcb)
{
	BUG_ON(ifd_t1_t(pcb) != IFD_T1_I_BLOCK);
	return IFD_T1_I_M_BIT & pcb;
}

static uint8_t ifd_t1_n(uint8_t pcb)
{
	if (ifd_t1_t(pcb) == IFD_T1_R_BLOCK) {
		return (uint8_t)((pcb & IFD_T1_R_N_BIT) >> IFD_T1_R_N_SHIFT);
	} else {
		BUG_ON(ifd_t1_t(pcb) != IFD_T1_I_BLOCK);
		return (uint8_t)((pcb & IFD_T1_I_N_BIT) >> IFD_T1_I_N_SHIFT);
	}
}

static uint8_t ifd_t1_xfr_nr(void)
{
	return (ifd_t1_xchg.nr & 0x01);
}

static uint8_t ifd_t1_xfr_ns(void)
{
	return (ifd_t1_xchg.ns & 0x01);
}

uint8_t ifd_t1_xfr_m(void)
{
	return (ifd_t1_xchg.nc > (ifd_t1_xchg.tx + ifd_t1_xchg.ack_ifsc)) ? 1 : 0;
}

static scs_err_t ifd_t1_xfr_parse(scs_size_t nc)
{
	ifd_t1_xchg.nc = nc;
	return SCS_ERR_SUCCESS;
}

void ifd_t1_save(scs_off_t index, uint8_t length)
{
	uint8_t i;

	ifd_t1_xchg.saved_len = length;
	BUG_ON(ifd_t1_xchg.saved_len > 8);
	for (i = 0; i < length; i++)
		ifd_t1_xchg.saved_buf[i] = ifd_xchg_read((scs_off_t)(index+i));
}

void ifd_t1_restore(scs_off_t index)
{
	uint8_t i;

	BUG_ON(ifd_t1_xchg.saved_len > 8);
	for (i = 0; i < ifd_t1_xchg.saved_len; i++)
		 ifd_xchg_write((scs_off_t)(index+i), ifd_t1_xchg.saved_buf[i]);
	ifd_t1_xchg.saved_len = 0;
}

/* @tx/rx: ifd_t1_xchg.tx/rx may be modified by ifd_t1_acked_iblock, tx/rx
 *         passed here should be the unmodified one.
 */
void ifd_t1_restore_buffer(uint8_t pcb, scs_off_t tx, scs_off_t rx)
{
	uint8_t safe;

	safe = IFD_T1_PROLOGUE_SIZE + ifd_t1_epilogue_size() +
	       ifd_t1_xchg.ifsd;

	if (tx < ifd_t1_xchg.nc &&
	    ifd_t1_xchg.tx < ifd_t1_xchg.nc) {
		scs_off_t save_end;

		save_end = safe + ifd_t1_xchg.ifsc + ifd_t1_epilogue_size();
		if (save_end > ifd_t1_xchg.nc)
			save_end = ifd_t1_xchg.nc;
		BUG_ON(save_end > IFD_BUF_SIZE);

		ifd_xchg_move(tx, safe, ifd_t1_xchg.ifsc);
		if (save_end > (tx + ifd_t1_xchg.ifsc)) {
			ifd_t1_restore((scs_off_t)(tx + ifd_t1_xchg.ifsc));
		}
	}
	if (ifd_t1_xchg.tx == ifd_t1_xchg.nc) {
		uint8_t saved;

		if ((IFD_BUF_SIZE - rx) < safe) {
			saved = safe - (IFD_BUF_SIZE - rx);
			ifd_t1_restore((scs_off_t)(rx-saved));
		}
	}
}

void ifd_t1_save_buffer(void)
{
	uint8_t safe;
	uint8_t saved;

	safe = IFD_T1_PROLOGUE_SIZE + ifd_t1_epilogue_size() +
	       ifd_t1_xchg.ifsd;

	if (ifd_t1_xchg.tx < ifd_t1_xchg.nc) {
		scs_off_t save_end;

		/* rx_blk:
		 *
		 * Always aligned to rx position.
		 */
		ifd_t1_xchg.rx_blk = ifd_t1_xchg.rx;

		/* tx_blk:
		 *
		 * I-Block:
		 * Aligned to 'safe' space to keep responses.
		 * Otherwise:
		 * Aligned to rx position.
		 */
		if (ifd_t1_t(ifd_t1_xchg.pcb) == IFD_T1_I_BLOCK) {
			ifd_t1_xchg.tx_blk = safe-IFD_T1_PROLOGUE_SIZE;
		} else {
			ifd_t1_xchg.tx_blk = ifd_t1_xchg.rx_blk;
		}
		BUG_ON(IFD_BUF_SIZE < (ifd_t1_xchg.tx_blk+ifd_t1_xchg.ifsd+
				       IFD_T1_PROLOGUE_SIZE+
				       ifd_t1_epilogue_size()));

		save_end = safe + ifd_t1_xchg.ifsc + ifd_t1_epilogue_size();
		if (save_end > ifd_t1_xchg.nc)
			save_end = ifd_t1_xchg.nc;
		BUG_ON(save_end > IFD_BUF_SIZE);

		/*
		 * ifsc >= (safe - tx)
		 * rx   tx  tx_blk                           save_end
		 * +----+---+-------+----+...........+.......+--+
		 * |    |AAA|AAAAAAA|AAAA|SSSSSSSSSSS|SSSSSSS|  |
		 * |    |   |PPPPPPP|AAAA|AAAAAAAAAAA|EEEEEEE|  |
		 * +----+---+-------+----+...........+.......+--+
		 * |    |   |       |    |           |       |  |
		 * |<--------------------nc-------------------->|
		 * |    |<-----ifsc----->|           |       |  |
		 * |<-----safe----->|    |           |       |  |
		 * |        |       |    |           |       |  |
		 * |        |       |    |<------saved------>|  |
		 * |        |<-pro->|<-----ifsc----->|<-epi->|  |
		 * save_pos = tx + ifsc;
		 */
		
		/*
		 * ifsc <= (safe - tx)
		 * rx   tx  tx_blk                   save_end
		 * +----+---+----+--+........+..+....+----------+
		 * |    |AAA|AAAA|SS|SSSSSSSS|SS|SSSS|          |
		 * |    |   |PPPP|PP|AAAAAAAA|EE|EEEE|          |
		 * +----+---+----+--+........+..+....+----------+
		 * |    |   |    |  |        |       |          |
		 * |<--------------------nc-------------------->|
		 * |    |<-ifsc->|  |        |       |          |
		 * |<-----safe----->|        |       |          |
		 * |             |  |        |       |          |
		 * |             |<------saved------>|          |
		 * |        |<-pro->|<-ifsc->|<-epi->|          |
		 * save_pos = tx + ifsc;
		 */

		/*
		 * (tx - rx) > safe
		 * rx       tx_blk       tx          save_end
		 * +--------+-------+----+---+----+..+----------+
		 * |                |    |AAA|AAAA|SS|          |
		 * |        |PPPPPPP|AAAA|AAA|EEEE|EE|          |
		 * +--------+-------+--------+----+..+----------+
		 * |        |       |    |   |    |  |          |
		 * |<--------------------nc-------------------->|
		 * |        |       |    |<-ifsc->|  |          |
		 * |<-----safe----->|        |    |  +------+   |
		 * |        |       |        |    |<-saved->|   |
		 * |        |       |        |    |  +------+   |
		 * |        |<-pro->|<-ifsc->|<-epi->|          |
		 * save_pos = tx + ifsc;
		 */

		if (save_end > (ifd_t1_xchg.tx + ifd_t1_xchg.ifsc)) {
			saved = save_end - (ifd_t1_xchg.tx + ifd_t1_xchg.ifsc);
			ifd_t1_save((scs_off_t)(ifd_t1_xchg.tx + ifd_t1_xchg.ifsc),
				    (uint8_t)saved);
		}
		ifd_xchg_move(safe, ifd_t1_xchg.tx, ifd_t1_xchg.ifsc);
	} else {

		if ((IFD_BUF_SIZE - ifd_t1_xchg.rx) < safe) {
			saved = safe - (IFD_BUF_SIZE - ifd_t1_xchg.rx);
			ifd_t1_save((scs_off_t)(ifd_t1_xchg.rx-saved), saved);
		} else {
			saved = 0;
		}
		ifd_t1_xchg.rx_blk = ifd_t1_xchg.rx - saved;
		ifd_t1_xchg.tx_blk = ifd_t1_xchg.rx_blk;
	}
}


#ifdef CONFIG_IFD_T1_LRC
static uint8_t ifd_t1_csum_lrc(void)
{
	uint8_t	lrc = 0;
	scs_off_t n;

	for (n = 0; n < ifd_t1_xchg.ifsc+IFD_T1_PROLOGUE_SIZE; n++)
		lrc ^= ifd_xchg_read((scs_off_t)(ifd_t1_xchg.tx_blk + n));

	return lrc;
}

static boolean ifd_t1_verify_lrc(void)
{
	uint8_t	lrc = 0;
	scs_off_t n;

	for (n = 0; n < ifd_t1_xchg.rx_len+IFD_T1_PROLOGUE_SIZE; n++)
		lrc ^= ifd_xchg_read((scs_off_t)(ifd_t1_xchg.rx_blk + n));

	return lrc == ifd_xchg_read((scs_off_t)(ifd_t1_xchg.rx_blk + ifd_t1_xchg.rx_len+IFD_T1_PROLOGUE_SIZE));
}

#else
#define ifd_t1_csum_lrc()	0
#define ifd_t1_verify_lrc()	false
#endif

#ifdef CONFIG_IFD_T1_CRC
static uint16_t ifd_t1_csum_crc(void)
{
	scs_off_t n;
	uint8_t byte;
	boolean msb = (ifd_get_convention() == IFD_CONV_INVERSE);

	crc_ccitt_init();
	for (n = 0; n < ifd_t1_xchg.ifsc+IFD_T1_PROLOGUE_SIZE; n++) {
		byte = ifd_xchg_read((scs_off_t)(ifd_t1_xchg.tx_blk + n));
		crc_ccitt_update(byte, msb);
	}
	return crc_ccitt_final();
}

static boolean ifd_t1_verify_crc(void)
{
	scs_off_t n;
	uint8_t byte;
	uint16_t crc;
	boolean msb = (ifd_get_convention() == IFD_CONV_INVERSE);

	crc_ccitt_init();
	for (n = 0; n < ifd_t1_xchg.rx_len+IFD_T1_PROLOGUE_SIZE; n++) {
		byte = ifd_xchg_read((scs_off_t)(ifd_t1_xchg.rx_blk + n));
		crc_ccitt_update(byte, msb);
	}
	crc = crc_ccitt_final();

	byte = ifd_xchg_read((scs_off_t)(ifd_t1_xchg.rx_blk + 
					  ifd_t1_xchg.rx_len +
					  IFD_T1_PROLOGUE_SIZE));
	if (HIBYTE(crc) == byte) {
		byte = ifd_xchg_read((scs_off_t)(ifd_t1_xchg.rx_blk +
						  ifd_t1_xchg.rx_len +
						  IFD_T1_PROLOGUE_SIZE + 1));
		return byte == LOBYTE(crc);
		
	}
	return false;
}
#else
#define ifd_t1_csum_crc()	0
#define ifd_t1_verify_crc()	false
#endif

void ifd_t1_compute_checksum(void)
{
	scs_off_t index = ifd_t1_xchg.ifsc + IFD_T1_PROLOGUE_SIZE;

	/* XXX: Save Checksum to Following Position
	 * ifd_t1_xchg.tx_blk+ifd_t1_xchg.nc
	 */
	if (ifd_t1_conf.tccks & 0x01) {
		uint16_t crc = ifd_t1_csum_crc();
		ifd_xchg_write((scs_off_t)(ifd_t1_xchg.tx_blk + index), HIBYTE(crc));
		ifd_xchg_write((scs_off_t)(ifd_t1_xchg.tx_blk + index+1), LOBYTE(crc));
	} else {
		uint8_t lrc = ifd_t1_csum_lrc();
		ifd_xchg_write((scs_off_t)(ifd_t1_xchg.tx_blk + index), lrc);
	}
}

boolean ifd_t1_verify_checksum(void)
{
	if (ifd_t1_conf.tccks & 0x01) {
		return ifd_t1_verify_crc();
	} else {
		return ifd_t1_verify_lrc();
	}
}

static void ifd_t1_write_prologue(void)
{
	ifd_xchg_write((scs_off_t)(ifd_t1_xchg.tx_blk+0), ifd_t1_conf.nad);
	ifd_xchg_write((scs_off_t)(ifd_t1_xchg.tx_blk+1), ifd_t1_xchg.pcb);
	ifd_xchg_write((scs_off_t)(ifd_t1_xchg.tx_blk+2), ifd_t1_xchg.ifsc);
}

void ifd_t1_build_sblock(uint8_t type, uint8_t inf, boolean resp)
{
	BUG_ON(type == IFD_T1_S_IFS && inf < IFD_T1_SAFE_IFSD);

	ifd_t1_xchg.ifsc = (type == IFD_T1_S_IFS || type == IFD_T1_S_WTX) ? 1 : 0;
	ifd_t1_xchg.ifsd = IFD_T1_SAFE_IFSD;
	ifd_t1_xchg.pcb = IFD_T1_S_BLOCK | type;
	if (resp)
		ifd_t1_xchg.pcb |= IFD_T1_S_R_BIT;

	ifd_t1_save_buffer();

	ifd_t1_write_prologue();
	if (ifd_t1_xchg.ifsc == 1)
		ifd_xchg_write((scs_off_t)(ifd_t1_xchg.tx_blk+3), inf);
	ifd_t1_compute_checksum();
}

void ifd_t1_build_iblock(uint8_t ifsc)
{
	ifd_t1_xchg.ack_ifsc = ifsc;
	ifd_t1_xchg.ifsc = ifsc;
	ifd_t1_xchg.ifsd = ifd_t1_conf.ifsd;
	ifd_t1_xchg.pcb = IFD_T1_I_BLOCK |
			 (ifd_t1_xfr_ns()<<IFD_T1_I_N_SHIFT) |
			 (ifd_t1_xfr_m()<<IFD_T1_I_M_SHIFT);

	ifd_t1_save_buffer();

	/* prepare prologue/epilogue */
	ifd_t1_write_prologue();
	/* inf field prepared after ifd_t1_save_buffer() */
	ifd_t1_compute_checksum();
}

void ifd_t1_build_rblock(void)
{
	ifd_t1_xchg.ifsc = 0;
	ifd_t1_xchg.ifsd = ifd_t1_conf.ifsd;
	ifd_t1_xchg.pcb = IFD_T1_R_BLOCK |
			 (ifd_t1_xfr_nr()<<IFD_T1_R_N_SHIFT);

	ifd_t1_save_buffer();

	/* prepare prologue/epilogue */
	ifd_t1_write_prologue();
	/* inf field prepared after ifd_t1_save_buffer() */
	ifd_t1_compute_checksum();
}

static void __ifd_t1_xfr_reset(void)
{
	ifd_t1_xchg.tx_cmpl = ifd_t1_xchg.rx_cmpl = true;
}

void ifd_t1_ack_iblock(boolean cmpl)
{
	ifd_t1_xchg.nr++;
	ifd_xchg_move(ifd_t1_xchg.rx,
		      (scs_off_t)(ifd_t1_xchg.rx_blk+3),
		      ifd_t1_xchg.rx_len);
	ifd_t1_xchg.rx += ifd_t1_xchg.rx_len;
	if (cmpl) {
		ifd_t1_xchg.rx_cmpl = true;
		ifd_xchg_end((scs_size_t)ifd_t1_xchg.rx);
	}
}

void ifd_t1_acked_iblock(void)
{
	if (!ifd_t1_xchg.tx_cmpl) {
		ifd_t1_xchg.ns++;
		ifd_t1_xchg.tx += ifd_t1_xchg.ack_ifsc;
		BUG_ON(ifd_t1_xchg.tx > ifd_t1_xchg.nc);
		if (ifd_t1_xchg.tx == ifd_t1_xchg.nc) {
			ifd_t1_xchg.tx_cmpl = true;
		}
	}
}

static void ifd_t1_resync_reset(void)
{
	ifd_t1_xchg.resync_level = IFD_T1_RETRY_RETRNS;
	ifd_t1_xchg.resync_retry = IFD_T1_MAX_RETRY;
}

void ifd_t1_xfr_reset(scs_size_t nc)
{
	ifd_t1_xchg.tx_blk = ifd_t1_xchg.rx_blk = 0;
	ifd_t1_xchg.tx = ifd_t1_xchg.rx = 0;
	ifd_t1_xchg.ifsc = ifd_t1_xchg.ifsd = 0;
	ifd_t1_xchg.wtx = 0;
	ifd_t1_xchg.tx_cmpl = ifd_t1_xchg.rx_cmpl = false;
	ifd_t1_xchg.saved_len = 0;
	ifd_t1_resync_reset();
}

scs_err_t ifd_t1_xchg_init(scs_size_t nc, scs_size_t ne)
{
	scs_err_t err;
	
	ifd_t1_xfr_reset(nc);

	/* calculate Nc & Ne */
	err = ifd_t1_xfr_parse(nc);
	if (err != SCS_ERR_SUCCESS) return err;

	ifd_t1_xchg.nr_valid = IFD_T1_PROLOGUE_SIZE+
			      ifd_t1_xchg.nc+
			      ifd_t1_epilogue_size();

	/* reset to IFD_T1_I_BLOCK */
	ifd_t1_xchg.pcb = 0;
	return SCS_ERR_PROGRESS;
}

uint8_t ifd_t1_safe_space(scs_off_t tx, scs_off_t rx)
{
	scs_size_t space = tx - rx;
	return min(space, IFD_T1_MAX_IFSD);
}

uint8_t ifd_t1_safe_size(void)
{
	return IFD_T1_PROLOGUE_SIZE + ifd_t1_epilogue_size() +
	       ifd_t1_xchg.ifsd;
}

void ifd_t1_xchg_aval(void)
{
	scs_size_t ifsc, ifsd;
	scs_size_t ifsd_rx;

	switch (ifd_t1_t(ifd_t1_xchg.pcb)) {
	case IFD_T1_I_BLOCK:
		ifsd_rx = ifd_t1_xchg.rx + IFD_T1_PROLOGUE_SIZE + ifd_t1_epilogue_size();
		ifsc = min(ifd_t1_conf.ifsc, ifd_t1_xchg.nc - ifd_t1_xchg.tx);
		if (ifd_t1_xchg.tx < ifd_t1_xchg.nc) {
			uint8_t safe;
			/* XXX: Safe IFSC Ensurance
			 *
			 * See ifd_t1_save_buffer(), command is aligned
			 * to the tx_blk (i.e., safe-IFD_T1_PROLOGUE_SIZE)
			 * position.
			 */
			safe = ifd_t1_safe_size();
			if (ifsc + safe + ifd_t1_epilogue_size() >= IFD_BUF_SIZE) {
				ifsc = IFD_BUF_SIZE-safe-ifd_t1_epilogue_size();
			}
			ifsd = max(ifd_t1_xchg.tx - ifsd_rx, IFD_T1_SAFE_IFSD);
			if (ifsd < (scs_size_t)ifd_t1_conf.ifsd) {
				ifd_t1_conf.ifsd = (uint8_t)ifsd;
				ifd_t1_build_sblock(IFD_T1_S_IFS, (uint8_t)ifsd, false);
			} else {
				ifd_t1_build_iblock((uint8_t)ifsc);
			}
		} else {
			ifsd = min(IFD_T1_MAX_IFSD,
				   IFD_BUF_SIZE - ifd_t1_xchg.rx);
			if (ifsd != ifd_t1_conf.ifsd) {
				ifd_t1_conf.ifsd = (uint8_t)ifsd;
				ifd_t1_build_sblock(IFD_T1_S_IFS, (uint8_t)ifsd, false);
			} else {
				ifd_t1_build_rblock();
			}
		}
		break;
	case IFD_T1_S_BLOCK:
		ifd_t1_build_sblock(ifd_t1_xchg.ack_type, ifd_t1_xchg.ack_info, true);
		break;
	case IFD_T1_R_BLOCK:
		ifd_t1_build_rblock();
		break;
	}

	ifd_t1_xfr_block();
}

uint8_t ifd_t1_xfr_read(scs_off_t idx)
{
	return ifd_xchg_read((scs_off_t)(ifd_t1_xchg.rx_blk+idx));
}

static boolean ifd_t1_block_valid(void)
{
	uint8_t pcb, len, info;

#ifdef CONFIG_IFD_T1_NAD
	uint8_t nad = ifd_t1_xfr_read(0);
	if (swapb(nad, 4) != ifd_t1_conf.nad) {
		return false;
	}
#endif
	if (!ifd_t1_verify_checksum()) {
		return false;
	}

	pcb = ifd_t1_xfr_read(1);
	len = ifd_t1_xfr_read(2);
	info = ifd_t1_xfr_read(3);

	/*
	 * Rule 7.3
	 * Want S-block response if TX S-block request.
	 */
	if (ifd_t1_t(ifd_t1_xchg.pcb) == IFD_T1_S_BLOCK &&
	    !ifd_t1_r(ifd_t1_xchg.pcb)) {
		if (ifd_t1_t(pcb) != IFD_T1_S_BLOCK ||
		    ifd_t1_s(pcb) != ifd_t1_s(ifd_t1_xchg.pcb) ||
		    !ifd_t1_r(pcb))
			return false;
	}

	switch (ifd_t1_t(pcb)) {
	case IFD_T1_R_BLOCK:
		/* ISO/IEC 7816-3 11.3 Block Frame
		 * 0-N(R)-xxxx: xxxx represents error code
		 */
		if (len != 0x00 || pcb & (1<<6))
			return false;
		/* Rule 2.2 - RX R-block
		 * R-block implies that the I-block we sent should not be
		 * the last I-block.
		 */
		if (ifd_t1_xfr_m() && ifd_t1_n(pcb) == ifd_t1_xfr_ns()) {
			return false;
		}
		if (ifd_t1_xchg.tx_cmpl)
			return false;
#if 0
		/* XXX: I-block should be resent on error indication */
		if (ifd_t1_e(pcb))
			return false;
#endif
		break;
	case IFD_T1_I_BLOCK:
		/* Rule 2.1 - RX I-block when TX I-block.
		 * which means:
		 * 1. Block we sent has M-Bit set
		 */
		if (ifd_t1_t(ifd_t1_xchg.pcb) == IFD_T1_I_BLOCK) {
			if (ifd_t1_m(ifd_t1_xchg.pcb) ||
			    (ifd_t1_n(pcb) != ifd_t1_xfr_nr())) {
				return false;
			}
			BUG_ON(ifd_t1_m(ifd_t1_xchg.pcb) && !ifd_t1_xchg.tx_cmpl);
		}
		break;
	case IFD_T1_S_BLOCK:
		/* ISO/IEC 7816-3 11.3 Block Frame */
		switch (ifd_t1_s(pcb)) {
		case IFD_T1_S_RESYNC:
		case IFD_T1_S_ABORT:
			if (len != 0x00)
				return false;
			break;
		case IFD_T1_S_IFS:
		case IFD_T1_S_WTX:
			if (len != 0x01)
				return false;
			break;
		default:
			/* unknown S-block type */
			return false;
		}

		/*
		 * Rule 7.3
		 * Don't want S-block response if !TX S-block request.
		 */
		if (ifd_t1_r(pcb) &&
		    ((ifd_t1_t(ifd_t1_xchg.pcb) != IFD_T1_S_BLOCK) ||
		     (ifd_t1_s(pcb) != ifd_t1_s(ifd_t1_xchg.pcb)) ||
		     (ifd_t1_r(ifd_t1_xchg.pcb)))) {
			return false;
		}

		switch (ifd_t1_s(pcb)) {
		case IFD_T1_S_RESYNC:
			/* Rule 6 - RX S(RESYNC, request)
			 * S(RESYNC, request) may be transmitted only by
			 * IFD.
			 */
			if (!ifd_t1_r(pcb))
				return false;
			break;
		case IFD_T1_S_IFS:
			if (ifd_t1_r(pcb) &&
			    ifd_t1_xfr_read(3) != ifd_t1_conf.ifsd)
				return false;
			break;
		case IFD_T1_S_WTX:
			if (ifd_t1_r(pcb))
				return false;
			break;
		}
		break;
	}
	return true;
}

/* XXX: ISO/IEC 7816-3 11.6.2 Error-free Operation */
void ifd_t1_error_free_operation(void)
{
	uint8_t pcb, info;
	/* XXX: Determine Next PCB
	 *
	 * When S(request) is received, PCB must be S(response) block.
	 * Otherwise, PCB should be default (I block).  When PCB is
	 * default, in ifd_t1_xchg_aval, we can determine whether
	 * I/R/S(request) block should be transmitted.
	 */
	uint8_t next_pcb = 0;
	/* XXX: Reserve Old TX/RX Offset
	 *
	 * When the TPDU we received shows a success of i-block
	 * transmission,
	 */
	scs_off_t old_tx = ifd_t1_xchg.tx;
	scs_off_t old_rx = ifd_t1_xchg.rx;

	ifd_t1_resync_reset();

	pcb = ifd_t1_xfr_read(1);
	info = ifd_t1_xfr_read(3);

	switch (ifd_t1_t(pcb)) {
	case IFD_T1_R_BLOCK:
		if (ifd_t1_xfr_ns() != ifd_t1_n(pcb))
			ifd_t1_acked_iblock();
		break;
	case IFD_T1_I_BLOCK:
		ifd_t1_acked_iblock();
		ifd_t1_ack_iblock((boolean)(!ifd_t1_m(pcb)));
		break;
	case IFD_T1_S_BLOCK:
		if (!ifd_t1_r(pcb)) {
			next_pcb = pcb;
			ifd_t1_xchg.ack_type = ifd_t1_s(pcb);
			ifd_t1_xchg.ack_info = info;

			switch (ifd_t1_s(pcb)) {
			case IFD_T1_S_ABORT:
				break;
			case IFD_T1_S_WTX:
				ifd_t1_xchg.wtx = info;
				break;
			case IFD_T1_S_IFS:
				ifd_t1_conf.ifsc = info;
				break;
			}
		} else {
			switch (ifd_t1_s(pcb)) {
			case IFD_T1_S_ABORT:
				break;
			case IFD_T1_S_RESYNC:
				__ifd_t1_xfr_resync();
				break;
			}
		}
		break;
	}

	ifd_t1_restore_buffer(pcb, old_tx, old_rx);
	ifd_t1_xchg.pcb = next_pcb;

	if (!ifd_t1_xchg.rx_cmpl)
		ifd_xchg_set_error(SCS_ERR_PROGRESS);
	else
		ifd_xchg_set_error(SCS_ERR_SUCCESS);
}

/* TODO: ISO/IEC 7816-3 11.6.3 Error-handling */
void ifd_t1_error_handling(void)
{
	/* XXX: Determine Next PCB
	 *
	 * When S(request) is received, PCB must be S(response) block.
	 * Otherwise, PCB should be default (I block).  When PCB is
	 * default, in ifd_t1_xchg_aval, we can determine whether
	 * I/R/S(request) block should be transmitted.
	 */
	uint8_t next_pcb = 0;

	if (ifd_t1_xchg.resync_level == IFD_T1_RETRY_RETRNS) {
		next_pcb = IFD_T1_R_BLOCK;
	} else if (ifd_t1_xchg.resync_level == IFD_T1_RETRY_RESYNC) {
		next_pcb = IFD_T1_S_BLOCK | IFD_T1_S_RESYNC;
	} else if (ifd_t1_xchg.resync_level == IFD_T1_RETRY_WRESET) {
		ifd_xchg_set_error(SCS_ERR_HW_ERROR);
		return;
	}

	if (ifd_t1_xchg.resync_retry == 0) {
		ifd_t1_xchg.resync_retry = IFD_T1_MAX_RETRY;
		ifd_t1_xchg.resync_level--;
	} else {
		ifd_t1_xchg.resync_retry--;
	}

	ifd_t1_restore_buffer(0, ifd_t1_xchg.tx, ifd_t1_xchg.rx);
	ifd_t1_xchg.pcb = next_pcb;
	if (!ifd_t1_xchg.rx_cmpl)
		ifd_xchg_set_error(SCS_ERR_PROGRESS);
	else
		ifd_xchg_set_error(SCS_ERR_SUCCESS);
}

void ifd_t1_xchg_cmpl(void)
{
	if (ifd_t1_xchg.rx_cmpl && ifd_t1_xchg.rx_cmpl)
		return;

	if (ifd_xchg_get_error() != SCS_ERR_SUCCESS ||
	    !ifd_t1_block_valid()) {
		ifd_t1_error_handling();
	} else {
		ifd_t1_error_free_operation();
	}
}

void __ifd_t1_ne_expire(void)
{
	ifd_xchg_stop(IFD_XCHG_EVENT_XFR_CMPL);
}
#else
scs_err_t ifd_t1_xchg_init(scs_size_t nc, scs_size_t ne)
{
	ifd_t1_xchg.ifsc = nc - IFD_T1_PROLOGUE_SIZE -
			  ifd_t1_epilogue_size();
	ifd_t1_xchg.ifsd = ifd_t1_conf.ifsd;
	ifd_t1_xchg.rx_blk = ifd_t1_xchg.tx_blk = 0;
	ifd_t1_xchg.nc = nc;
	ifd_t1_xchg.nr_valid = nc;
	return SCS_ERR_PROGRESS;
}

static void ifd_t1_xchg_aval(void)
{
	ifd_t1_xfr_block();
}

static void ifd_t1_xchg_cmpl(void)
{
	/* TPDU level T=1 does not handle transfers in task context */
}

static void __ifd_t1_ne_expire(void)
{
	ifd_xchg_end(ifd_t1_xchg.rx_blk+IFD_T1_PROLOGUE_SIZE+
		     ifd_t1_xchg.rx_len+ifd_t1_epilogue_size());
	ifd_xchg_stop(IFD_XCHG_EVENT_XFR_CMPL);
}
#endif

void ifd_t1_ne_expire(void)
{
	if (!ifd_t1_xchg.cmpl) {
		ifd_t1_xchg.rx_len = ifd_xfr_read(2);
		if (ifd_t1_xchg.rx_len > ifd_t1_xchg.ifsd) {
			ifd_xchg_stop(IFD_XCHG_EVENT_XFR_OVER);
			return;
		}
		ifd_xfr_block(0, 0,
			      (scs_off_t)(ifd_t1_xchg.rx_blk+IFD_T1_PROLOGUE_SIZE),
			      (scs_size_t)(ifd_t1_xchg.rx_len+ifd_t1_epilogue_size()),
			      ifd_t1_xchg.nr_valid);
		ifd_t1_xchg.cmpl = true;
	} else {
		__ifd_t1_ne_expire();
	}
}

#ifdef CONFIG_IFD_AUTO_ATR_BUILD
static uint8_t ifd_t1_atr_build(uint8_t seq)
{
	uint8_t n = 0;

	if (seq != 1)
		return 0;

	if (ifd_t1_conf.ifsc != IFD_T1_IFS_IMPLICIT) {
		ifd_atr_set_ta(ifd_t1_conf.ifsc);
		n++;
	}
	if (LOHALF(ifd_t1_conf.wi) != IFD_T1_CWI_IMPLICIT || 
	    HIHALF(ifd_t1_conf.wi) != IFD_T1_BWI_IMPLICIT) {
	    ifd_atr_set_tb(ifd_t1_conf.wi);
	    n++;
	}
	if (ifd_t1_conf.tccks) {
		ifd_atr_set_tc(ifd_t1_conf.tccks);
		n++;
	}
	return n;
}
#endif

static void ifd_t1_nego_pps(void)
{
 	//ifd_t1_apply_wi(ifd_t1_conf.wi);
}

ifd_proto_t ifd_t1 = {
	IFD_PROTO_T1,
	IFD_T1_PROLOGUE_SIZE,
	ifd_t1_atr_parse,
	ifd_t1_xchg_init,
	ifd_t1_xchg_aval,
	ifd_t1_xchg_cmpl,
	ifd_t1_ne_expire,
	ifd_t1_reset_param,
	ifd_t1_conf_param,
	ifd_t1_nego_pps,
};

void ifd_t1_init(void)
{
	ifd_register_protocol(&ifd_t1);
}
