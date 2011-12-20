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
 * @(#)msd_bbb.h: mass storage bulk-only protocol interfaces
 * $Id: msd_bbb.h,v 1.22 2011-02-11 09:38:22 zhenglv Exp $
 */

#ifndef __MSD_BBB_H_INCLUDE__
#define __MSD_BBB_H_INCLUDE__

#ifndef MSD_INTERFACE_PROTOCOL
#define MSD_INTERFACE_PROTOCOL		USB_INTERFACE_PROTOCOL_BBB
#else
#error "Multiple MSD protocol defined"
#endif

/*
 * MSD class requests
 */

#define MSD_REQ_BBB_MASS_STORAGE_RESET	0xFF
#define MSD_REQ_BBB_GET_MAX_LUN		0xFE

#define NR_MSD_ENDPS			2
#define MSD_BBB_ENDP_IN			0
#define MSD_BBB_ENDP_OUT		1

#define MSD_BBB_CBW_SIGNATURE		0x43425355
#define MSD_BBB_CSW_SIGNATURE		0x53425355

/* command block wrapper */
struct msd_cbw {
	uint32_t dCBWSignature;
	uint32_t dCBWDataTransferLength;
	uint8_t bmCBWFlags;
#define MSD_BBB_CBW2DIR(flags)	USB_ADDR2DIR(flags)
	uint8_t bCBWLUN;
	uint8_t bCBWCBLength;
#define MSD_BBB_CB_SIZE		16
	uint8_t CBWCB[MSD_BBB_CB_SIZE];
};
#define MSD_BBB_CBW_SIZE	31

/* command status wrapper */
struct msd_csw {
	uint32_t dCSWTag;
	uint32_t dCSWDataResidue;
	uint8_t bCSWStatus;
#define MSD_BBB_STATUS_COMMAND_PASSED	0x00
#define MSD_BBB_STATUS_COMMAND_FAILED	0x01
	/* Phase error means Host/Device Disagreements */
#define MSD_BBB_STATUS_PHASE_ERROR	0x02
};
#define MSD_BBB_CSW_SIZE	13

#define MSD_BBB_STATE_CBW	0x00
#define MSD_BBB_STATE_DATA	0x01
#define MSD_BBB_STATE_CSW	0x02

#endif /* __MSD_BBB_H_INCLUDE__ */
