/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)smmu_common.h: Common interfaces for ARM SMMU and ARM SMMUv3
 * $Id: smmu_common.h,v 1.1 2023-11-20 16:54:00 zhenglv Exp $
 */

#ifndef __SMMU_COMMON_H_INCLUDE__
#define __SMMU_COMMON_H_INCLUDE__

#define NR_SMMU_STREAMS		NR_IOMMUS
#define INVALID_SMMU_SID	NR_IOMMUS

typedef uint16_t smmu_gr_t; /* smmu stream id */
typedef uint8_t smmu_cb_t; /* smmu context bank id */
typedef iommu_t smmu_sme_t; /* iommu device and smmu stream mapping group */

#define SMMU_SME_MASK(iommu, gr, sm)	MAKELLONG(MAKELONG(gr, sm), iommu)
#define SMMU_SME(iommu, gr)		SMMU_SME_MASK(iommu, gr, 0)
#define smmu_sme_dev(sme)		((iommu_dev_t)HIDWORD(sme))
#define smmu_sme_gr(sme)		LOWORD(LODWORD(sme))
#define smmu_sme_sm(sme)		HIWORD(LODWORD(sme))

#define SMMU_MAX_CBS			128
#define INVALID_SMMU_CB			SMMU_MAX_CBS
#define INVALID_SMMU_SME		SMMU_SME(INVALID_IOMMU_DEV, 0)

struct smmu_device {
	uint32_t features;
	smmu_gr_t gr;
	smmu_cb_t cb;
	SMMU_DEVICE_ATTR
};

struct smmu_group {
	iommu_grp_t grp;
	smmu_sme_t sme;		/* default stream */
	iommu_dom_t dom;	/* default domain */
	SMMU_GROUP_ATTR
};

struct smmu_stream {
	iommu_dev_t dev;
	iommu_grp_t grp;
	bool valid;
	int count;
	smmu_gr_t sid;
	smmu_sme_t sme;
	SMMU_STREAM_ATTR
};

struct smmu_domain {
	smmu_cb_t cb;
	SMMU_DOMAIN_ATTR
};

#define smmu_gr			smmu_device_ctrl.gr
#define smmu_cb			smmu_device_ctrl.cb

void smmu_domain_restore(smmu_cb_t cb);
smmu_cb_t smmu_domain_save(smmu_cb_t cb);
void smmu_domain_select(void);

extern struct smmu_device smmu_devices[];
extern struct smmu_group smmu_groups[];
extern struct smmu_domain smmu_domains[];
#define smmu_device_ctrl		smmu_devices[iommu_dev]
#define smmu_group_ctrl			smmu_groups[iommu_grp]
#define smmu_domain_ctrl		smmu_domains[iommu_dom]

#if NR_SMMU_STREAMS > 1
extern smmu_gr_t smmu_sid;
extern struct smmu_stream smmu_streams[NR_SMMU_STREAMS];
smmu_gr_t smmu_stream_save(smmu_gr_t gr);
void smmu_stream_restore(smmu_gr_t gr);
#define smmu_stream_ctrl		smmu_streams[smmu_sid]
#else
#define smmu_sid			0
#define smmu_stream_save(sid)		0
#define smmu_stream_restore(sid)
extern struct smmu_stream smmu_stream_ctrl;
#endif
void smmu_stream_select(void);

#ifdef CONFIG_SMMU_COMMON
smmu_sme_t smmu_alloc_sme(void);
void smmu_free_sme(smmu_gr_t sid);
smmu_cb_t smmu_alloc_cb(smmu_cb_t start, smmu_cb_t end);
void smmu_free_cb(smmu_cb_t cb);
#else
#define smmu_alloc_sme()			INVALID_SMMU_SME
#define smmu_free_sme(sme)			do { } while (0)
#endif


void smmu_device_exit(void);
void smmu_device_init(void);

#endif /* __SMMU_COMMON_H_INCLUDE__ */
