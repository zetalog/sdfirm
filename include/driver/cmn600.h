/*
 * Arm SCP/MCP Software
 * Copyright (c) 2017-2022, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)cmn600.h: ARM CoreLink CMN-600 coherent mesh network header
 * $Id: cmn600.h,v 1.1 2023-09-07 16:37:00 zhenglv Exp $
 */

#ifndef __CMN600_H_INCLUDE__
#define __CMN600_H_INCLUDE__

#include <target/irq.h>

/* The mach layer should provide the followings:
 * 1. CMN_PERIPH_BASE: base address of CFGM node
 * 2. CMN_MESH_DIMEN_X: X dimension
 * 3. CMN_MESH_DIMEN_Y: Y dimension
 * 4. CMN_HND_NID: NID of HN_D
 * 5. CMN_MAX_NODES: maximum mesh nodes
 * 6. CMN_MAX_DTCS: maximum DTC domains
 */

/* CMN node types */
#define CMN_INVAL		0x0
#define CMN_DVM			0x1
#define CMN_CFG			0x2
#define CMN_DTC			0x3
#define CMN_HNI			0x4
#define CMN_HNF			0x5
#define CMN_XP			0x6
#define CMN_SBSX		0x7
#define CMN_RNI			0xA
#define CMN_RND			0xD
#define CMN_RN_SAM		0xF
#define CMN_RNF			0x10
#define CMN_SNF			0x11
/* CML components */
#define CMN_CML			0x100
#define CMN_CXRA		(CMN_CML + 0x0)
#define CMN_CXHA		(CMN_CML + 0x1)
#define CMN_CXLA		(CMN_CML + 0x2)
#define CMN_CML_MAX		CMN_CXLA

/* XP device types */
#define CMN_MXP_INVAL		0x00
#define CMN_MXP_RNI		0x01
#define CMN_MXP_RND		0x02
#define CMN_MXP_RNF_CHIB	0x04
#define CMN_MXP_RNF_CHIB_ESAM	0x05
#define CMN_MXP_RNF_CHIA	0x06
#define CMN_MXP_RNF_CHIA_ESAM	0x07
#define CMN_MXP_HNT		0x08
#define CMN_MXP_HNI		0x09
#define CMN_MXP_HND		0x0A
#define CMN_MXP_SNF		0x0C
#define CMN_MXP_SBSX		0x0D
#define CMN_MXP_HNF		0x0E
#define CMN_MXP_CXHA		0x11
#define CMN_MXP_CXRA		0x12
#define CMN_MXP_CXRH		0x13

#define CMN_MAX_PORTS			2
#define CMN_MAX_DIMENS			8

#define CMN_MAX_MXP_COUNT		(CMN_MESH_DIMEN_X * CMN_MESH_DIMEN_Y)
#ifdef CMN_CXG_COUNT
#define CMN_MAX_CXG_COUNT		CMN_CXG_COUNT
#else
#define CMN_MAX_CXG_COUNT		2
#endif
#ifdef CMN_HNF_COUNT
#define CMN_MAX_HNF_COUNT		CMN_HNF_COUNT
#else
#define CMN_MAX_HNF_COUNT		32
#endif
#ifdef CMN_RND_COUNT
#define CMN_MAX_RND_COUNT		CMN_RND_COUNT
#else
#define CMN_MAX_RND_COUNT		32
#endif
#ifdef CMN_RNF_COUNT
#define CMN_MAX_RNF_COUNT		CMN_RNF_COUNT
#else
#define CMN_MAX_RNF_COUNT		32
#endif
#ifdef CMN_RNI_COUNT
#define CMN_MAX_RNI_COUNT		CMN_RNI_COUNT
#else
#define CMN_MAX_RNI_COUNT		32
#endif
#ifdef CMN_HNI_COUNT
#define CMN_MAX_HNI_COUNT		CMN_RNI_COUNT
#else
#define CMN_MAX_HNI_COUNT		32
#endif
#ifdef CMN_DTC_COUNT
#define CMN_MAX_DTC_COUNT		CMN_DTC_COUNT
#else
#define CMN_MAX_DTC_COUNT		4
#endif
#ifdef CMN_SBSX_COUNT
#define CMN_MAX_SBSX_COUNT		CMN_SBSX_COUNT
#else
#define CMN_MAX_SBSX_COUNT		32
#endif
#ifdef CMN_RN_SAM_INT_COUNT
#define CMN_MAX_RN_SAM_INT_COUNT	CMN_RN_SAM_INT_COUNT
#else
#define CMN_MAX_RN_SAM_INT_COUNT	32
#endif
#ifdef CMN_RN_SAM_EXT_COUNT
#define CMN_MAX_RN_SAM_EXT_COUNT	CMN_RN_SAM_EXT_COUNT
#else
#define CMN_MAX_RN_SAM_EXT_COUNT	32
#endif
#ifdef CMN_HA_MMAP_COUNT
#define CMN_MAX_HA_MMAP_COUNT		CMN_HA_MMAP_COUNT
#else
#define CMN_MAX_HA_MMAP_COUNT		4
#endif

/* NID and RNP (root node pointer) encodings */
#define CMN_NID_DeviceID_OFFSET		0
#define CMN_NID_DeviceID_MASK		REG_2BIT_MASK
#define CMN_DID(nid)			_GET_FV(CMN_NID_DeviceID, nid)
#define cmn_did(did)			_SET_FV(CMN_NID_DeviceID, did)
#define CMN_NID_Port_OFFSET		2
#define CMN_NID_Port_MASK		REG_1BIT_MASK
#define CMN_PID(nid)			_GET_FV(CMN_NID_Port, nid)
#define cmn_pid(pid)			_SET_FV(CMN_NID_Port, pid)
#define CMN_ROOT_NODE_POINTER_PID_OFFSET	0
#define CMN_ROOT_NODE_POINTER_PID_MASK		REG_2BIT_MASK
#define CMN_ROOT_NODE_POINTER_PID(pid)		_SET_FV(CMN_ROOT_NODE_POINTER_PID, pid)
#if CMN_MESH_DIMEN_X > 4 || CMN_MESH_DIMEN_Y > 4
#define CMN_NID_BITS			9
#define CMN_NID_MASK			REG_9BIT_MASK
#define CMN_NID_Y_OFFSET		3
#define CMN_NID_Y_MASK			REG_3BIT_MASK
#define CMN_Y(nid)			_GET_FV(CMN_NID_Y, nid)
#define cmn_y(y)			_SET_FV(CMN_NID_Y, y)
#define CMN_NID_X_OFFSET		6
#define CMN_NID_X_MASK			REG_3BIT_MASK
#define CMN_X(nid)			_GET_FV(CMN_NID_X, nid)
#define cmn_x(x)			_SET_FV(CMN_NID_X, x)
#define CMN_ROOT_NODE_POINTER_Y_OFFSET	6
#define CMN_ROOT_NODE_POINTER_Y_MASK	REG_3BIT_MASK
#define CMN_ROOT_NODE_POINTER_Y(y)	_SET_FV(CMN_ROOT_NODE_POINTER_Y, y)
#define CMN_ROOT_NODE_POINTER_X_OFFSET	9
#define CMN_ROOT_NODE_POINTER_X_MASK	REG_3BIT_MASK
#define CMN_ROOT_NODE_POINTER_X(x)	_SET_FV(CMN_ROOT_NODE_POINTER_X, x)
#else /* CMN_MESH_DIMEN_X <=4 && CMN_MESH_DIMEN_Y <= 4 */
#define CMN_NID_BITS			7
#define CMN_NID_MASK			REG_7BIT_MASK
#define CMN_NID_Y_OFFSET		3
#define CMN_NID_Y_MASK			REG_2BIT_MASK
#define CMN_Y(nid)			_GET_FV(CMN_NID_Y, nid)
#define CMN_NID_X_OFFSET		5
#define CMN_NID_X_MASK			REG_2BIT_MASK
#define CMN_X(nid)			_GET_FV(CMN_NID_X, nid)
#define CMN_ROOT_NODE_POINTER_Y_OFFSET	6
#define CMN_ROOT_NODE_POINTER_Y_MASK	REG_2BIT_MASK
#define CMN_ROOT_NODE_POINTER_Y(y)	_SET_FV(CMN_ROOT_NODE_POINTER_Y, y)
#define CMN_ROOT_NODE_POINTER_X_OFFSET	8
#define CMN_ROOT_NODE_POINTER_X_MASK	REG_2BIT_MASK
#define CMN_ROOT_NODE_POINTER_X(x)	_SET_FV(CMN_ROOT_NODE_POINTER_X, x)
#endif /* CMN_MESH_DIMEN_X > 4 || CMN_MESH_DIMEN_Y > 4 */
#define __CMN_ROOT_NODE_POINTER(x, y, pid)	\
	(CMN_ROOT_NODE_POINTER_X(x) |		\
	 CMN_ROOT_NODE_POINTER_Y(y) |		\
	 CMN_ROOT_NODE_POINTER_PID(pid))
#define CMN_ROOT_NODE_POINTER(nid)		\
	__CMN_ROOT_NODE_POINTER(CMN_X(nid),	\
				CMN_Y(nid),	\
				CMN_PID(nid))
#define CMN_NID(x, y, pid, did)		\
	(cmn_did(did) | cmn_pid(pid) | cmn_y(y) | cmn_x(x))

#if NR_CMN_NODES < 256
typedef uint8_t cmn_id_t;
#elif NR_CMN_NODES < 65536
typedef uint16_t cmn_id_t;
#else
typedef uint32_t cmn_id_t;
#endif

#ifdef ARCH_HAVE_XP_TABLE
#define CMN_XP_PID(x, y, pid)		\
	(((x) * 2) + ((y) * CMN_MESH_DIMEN_X * 2)  + (pid))
#define CMN_XP_X(xp_pid)		\
	(((xp_pid) >> 1) % CMN_MESH_DIMEN_X)
#define CMN_XP_Y(xp_pid)		\
	(((xp_pid) >> 1) / CMN_MESH_DIMEN_X)
#define CMN_XP_P(xp_pid)		\
	((xp_pid) & 0x01)
#define CMN_XP_PID2NID(xp_pid, dev)	\
	CMN_NID(CMN_XP_X(xp_pid), CMN_XP_Y(xp_pid), CMN_XP_P(xp_pid), dev)
#endif

#define NR_CMN_NODES			CMN_MAX_NODES
#define CMN_CFGM_ID			0
#define CMN_INVAL_ID			CMN_MAX_NODES

#define CMN_MAX_SCGS			4

#define CMN_CFGM_BASE			(cmn_bases[CMN_CFGM_ID])
#define CMN_HNF_BASE(id)		(cmn_bases[id])
#define CMN_RND_BASE(id)		(cmn_bases[id])
#define CMN_RNI_BASE(id)		(cmn_bases[id])
#define CMN_RN_SAM_INT_BASE(id)		(cmn_bases[id])
#define CMN_RN_SAM_EXT_BASE(id)		(cmn_bases[id])
/* TODO: Support D2D + C2C */
#define CMN_CXRA_BASE			(cmn_bases[cmn_cxra_ids[0]])
#define CMN_CXLA_BASE			(cmn_bases[cmn_cxla_ids[0]])
#define CMN_CXHA_BASE			(cmn_bases[cmn_cxha_ids[0]])

#define __CMN_HNF_BASE(i)		(cmn_bases[cmn_hnf_ids[i]])
#define __CMN_RND_BASE(i)		(cmn_bases[cmn_rnd_ids[i]])
#define __CMN_RNI_BASE(i)		(cmn_bases[cmn_rni_ids[i]])
#define __CMN_HNI_BASE(i)		(cmn_bases[cmn_hni_ids[i]])
#define __CMN_RN_SAM_INT_BASE(i)	(cmn_bases[cmn_rn_sam_int_ids[i]])
#define __CMN_RN_SAM_EXT_BASE(i)	(cmn_bases[cmn_rn_sam_ext_ids[i]])

#define CMN_12BIT_INDEX(n)		REG64_16BIT_INDEX(n)
#define CMN_12BIT_OFFSET(n)		\
	(((((uint64_t)(n)) & ULL( 3)) << 4) -	\
	 ((((uint64_t)(n)) & ULL( 3)) << 2))
#define CMN_8BIT_OFFSET(n)		\
	((((uint64_t)(n)) & ULL(3)) << 3)

#define CMN_REG(base, offset)		((base) + (offset))
#define CMN_CFGM_REG(offset)		CMN_REG(CMN_CFGM_BASE, offset)
#define CMN_8BIT_REG(base, offset, n)	\
	CMN_REG((base), (offset) + (REG64_8BIT_INDEX(n) << 3))
#define CMN_12BIT_REG(base, offset, n)	\
	CMN_REG((base), (offset) + (CMN_12BIT_INDEX(n) << 3))
#define CMN_16BIT_REG(base, offset, n)	\
	CMN_REG((base), (offset) + (REG64_16BIT_INDEX(n) << 3))
#define CMN_32BIT_REG(base, offset, n)	\
	CMN_REG((base), (offset) + (REG64_32BIT_INDEX(n) << 3))

#define CMN_SAM_GRANU			SZ_64M

/* Common to all nodes */
#define CMN_node_info(base)		CMN_REG(base, 0x0000)
#define CMN_child_info(base)		CMN_REG(base, 0x0080)

/* 3.2.1 Configuration master register summary */
#define CMN_MAX_cfgm_periph_ids		8
#define CMN_MAX_cfgm_component_ids	4
#define CMN_cfgm_periph_id(n)		CMN_32BIT_REG(CMN_CFGM_BASE, 0x0008, (n))
#define CMN_cfgm_component_id(n)	CMN_32BIT_REG(CMN_CFGM_BASE, 0x0028, (n))

/* 3.2.3 Debug and trace register summary */
#define CMN_dt_dtc_ctl(base)		CMN_REG(base, 0x0A00)
#define CMN_dt_trigger_status(base)	CMN_REG(base, 0x0A10)
#define CMN_dt_trigger_status_clr(base)	CMN_REG(base, 0x0A20)
#define CMN_dt_trace_control(base)	CMN_REG(base, 0x0A30)
#define CMN_dt_traceid(base)		CMN_REG(base, 0x0A40)
#define CMN_dt_pmevcntAB(base)		CMN_REG(base, 0x2000)
#define CMN_dt_pmevcntCD(base)		CMN_REG(base, 0x2010)
#define CMN_dt_pmevcntEF(base)		CMN_REG(base, 0x2020)
#define CMN_dt_pmevcntGH(base)		CMN_REG(base, 0x2030)
#define CMN_dt_pmccntr(base)		CMN_REG(base, 0x2040)
#define CMN_dt_pmevcntsrAB(base)	CMN_REG(base, 0x2050)
#define CMN_dt_pmevcntsrCD(base)	CMN_REG(base, 0x2060)
#define CMN_dt_pmevcntsrEF(base)	CMN_REG(base, 0x2070)
#define CMN_dt_pmevcntsrGH(base)	CMN_REG(base, 0x2080)
#define CMN_dt_pmccntrsr(base)		CMN_REG(base, 0x2090)
#define CMN_dt_pmcr(base)		CMN_REG(base, 0x2100)
#define CMN_dt_pmovsr(base)		CMN_REG(base, 0x2118)
#define CMN_dt_pmovsr_clr(base)		CMN_REG(base, 0x2120)
#define CMN_dt_pmssr(base)		CMN_REG(base, 0x2128)
#define CMN_dt_pmsrr(base)		CMN_REG(base, 0x2130)

/* 3.2.4 HN-F register summary */
#define CMN_MAX_hnf_slcway_partitions	4
#define CMN_MAX_hnf_sam_memregions	2
#define CMN_MAX_hnf_phys_ids		64
#define CMN_hnf_unit_info(base)		CMN_REG(base, 0x0900)
#define CMN_hnf_cfg_ctl(base)		CMN_REG(base, 0x0A00)
#define CMN_hnf_aux_ctl(base)		CMN_REG(base, 0x0A08)
#define CMN_hnf_r2_aux_ctl(base)	CMN_REG(base, 0x0A10)
#define CMN_hnf_ppu_pwpr(base)		CMN_REG(base, 0x1000)
#define CMN_hnf_ppu_pwsr(base)		CMN_REG(base, 0x1008)
#define CMN_hnf_ppu_misr(base)		CMN_REG(base, 0x1014)
#define CMN_hnf_ppu_idr0(base)		CMN_REG(base, 0x1FB0)
#define CMN_hnf_ppu_idr1(base)		CMN_REG(base, 0x1FB4)
#define CMN_hnf_ppu_iidr(base)		CMN_REG(base, 0x1FC8)
#define CMN_hnf_ppu_aidr(base)		CMN_REG(base, 0x1FCC)
#define CMN_hnf_ppu_dyn_ret_threshold(base)		\
					CMN_REG(base, 0x1100)
#define CMN_hnf_rn_starvation(base)	CMN_REG(base, 0x0A90)
#define CMN_hnf_slc_lock_ways(base)	CMN_REG(base, 0x0C00)
#define CMN_hnf_slc_lock_base(base, n)	CMN_REG(base, 0x0C08 + ((n) << 3))
#define CMN_hnf_rni_region_vec(base)	CMN_REG(base, 0x0C30)
#define CMN_hnf_rnf_region_vec(base)	CMN_REG(base, 0x0C38)
#define CMN_hnf_rnd_region_vec(base)	CMN_REG(base, 0x0C40)
#define CMN_hnf_slcway_partition_rnf_vec(base, n)	\
					CMN_REG(base, 0x0C48 + ((n) << 3))
#define CMN_hnf_rnf_region_vec1(base)	CMN_REG(base, 0x0C28)
#define CMN_hnf_slcway_partition_rnf_vec1(base, n)	\
					CMN_REG(base, 0x0CB0 + ((n) << 3))
#define CMN_hnf_slcway_partition_rni_vec(base, n)	\
					CMN_REG(base, 0x0C68 + ((n) << 3))
#define CMN_hnf_slcway_partition_rnd_vec(base, n)	\
					CMN_REG(base, 0x0C88 + ((n) << 3))
#define CMN_hnf_rn_region_lock(base)	CMN_REG(base, 0x0CA8)
#define CMN_hnf_sam_control(base)	CMN_REG(base, 0x0D00)
#define CMN_hnf_sam_memregion(base, n)	CMN_REG(base, 0x0D08 + ((n) << 3))
#define CMN_hnf_sam_sn_properties(base)	CMN_REG(base, 0x0D18)
#define CMN_hnf_sam_6sn_nodeid(base)	CMN_REG(base, 0x0D20)
#define CMN_hnf_rn_phys_id(base, n)	CMN_32BIT_REG(base, 0x0D28, (n))

/* MXP DTM registers */
#define CMN_dtm_control(base)		CMN_REG(base, 0x2100)
#define CMN_dtm_fifo_entry_ready(base)	CMN_REG(base, 0x2118)
/* n is 0...3 */
#define CMN_dtm_fifo_entry_0(base, n)	CMN_REG(base, 0x2120 + ((3 * (n)) << 3))
#define CMN_dtm_fifo_entry_1(base, n)	CMN_REG(base, 0x2128 + ((3 * (n)) << 3))
#define CMN_dtm_fifo_entry_2(base, n)	CMN_REG(base, 0x2130 + ((3 * (n)) << 3))
#define CMN_dtm_wp_config(base, n)	CMN_REG(base, 0x21A0 + ((3 * (n)) << 3))
#define CMN_dtm_wp_val(base, n)		CMN_REG(base, 0x21A8 + ((3 * (n)) << 3))
#define CMN_dtm_wp_mask(base, n)	CMN_REG(base, 0x21B0 + ((3 * (n)) << 3))
#define CMN_dtm_pmsicr(base)		CMN_REG(base, 0x2200)
#define CMN_dtm_pmsirr(base)		CMN_REG(base, 0x2208)
#define CMN_dtm_pmu_config(base)	CMN_REG(base, 0x2210)
#define CMN_dtm_pmevcnt(base, n)	CMN_16BIT_REG(base, 0x2220, n)
#define CMN_dtm_pmevcntsr(base)		CMN_16BIT_REG(base, 0x2240, n)

//abf
#define hnf_abf_lo_addr(base)		(__raw_readq(CMN_hnf_abf_lo_addr(base)))
#define hnf_abf_hi_addr(base)		(__raw_readq(CMN_hnf_abf_hi_addr(base)))
#define hnf_abf_pr(base)		(__raw_readq(CMN_hnf_abf_pr(base)))
#define hnf_abf_sr(base)		(__raw_readq(CMN_hnf_abf_sr(base)))
#define CMN_hnf_abf_lo_addr(base)	CMN_REG(base, 0xF50)
#define CMN_hnf_abf_hi_addr(base)	CMN_REG(base, 0xF58)
#define CMN_hnf_abf_pr(base)		CMN_REG(base, 0xF60)
#define CMN_hnf_abf_sr(base)		CMN_REG(base, 0xF68)

#define hnf_cfg_ctl(base)		(__raw_readq(CMN_hnf_cfg_ctl(base)))
#define hnf_ppu_pwpr(base)		(__raw_readq(CMN_hnf_ppu_pwpr(base)))
#define hnf_ppu_pwsr(base)		(__raw_readq(CMN_hnf_ppu_pwsr(base)))
#define hnf_slc_lock_ways(base)		(__raw_readq(CMN_hnf_slc_lock_ways(base)))

#define CMN_abf_mode_OFFSET		1
#define CMN_abf_mode_MASK		REG_2BIT_MASK
#define CMN_abf_mode(value)		_SET_FV_ULL(CMN_abf_mode, value)
#define CMN_abf_enable_OFFSET		0
#define CMN_abf_enable_MASK		REG_1BIT_MASK
#define CMN_abf_enable(value)		_SET_FV_ULL(CMN_abf_enable, value)
#define CMN_abf_lo_addr_OFFSET		0
#define CMN_abf_lo_addr_MASK		REG_48BIT_MASK
#define CMN_abf_lo_addr(value)		_SET_FV_ULL(CMN_abf_lo_addr, value)
#define CMN_abf_hi_addr_OFFSET		0
#define CMN_abf_hi_addr_MASK		REG_48BIT_MASK
#define CMN_abf_hi_addr(value)		_SET_FV_ULL(CMN_abf_hi_addr, value)

/* CMN_hnf_cfg_ctl */
#define CMN_hnf_ecc_disable			_BV_ULL(0)
#define CMN_hnf_pois_dis			_BV_ULL(2)
#define CMN_hnf_slc_tag_ecc_scrub_disable	_BV_ULL(4)
#define CMN_hnf_sf_ecc_scrub_disable		_BV_ULL(6)
#define CMN_hnf_ncdevcmo_mc_comp		_BV_ULL(8)
#define CMN_hnf_ocm_en				_BV_ULL(9)
#define CMN_hnf_ocm_always_en			_BV_ULL(10)
#define CMN_hnf_dmt_disable			_BV_ULL(11)


/* 3.2.6 XP register summary */
#define CMN_MAX_mxp_device_ports		2
#define CMN_mxp_device_port_connect_info(base, n)	\
					CMN_REG(base, 0x0008 + ((n) << 3))
#define CMN_mxp_mesh_port_connect_info_east(base)	\
					CMN_REG(base, 0x0018)
#define CMN_mxp_mesh_port_connect_info_north(base)	\
					CMN_REG(base, 0x0020)
#define CMN_mxp_p_info(base, n)		CMN_REG(base, 0x0900 + ((n) << 3))
#define CMN_mxp_aux_ctl(base)		CMN_REG(base, 0x0A00)

#define CMN_mxp_num_dev_OFFSET			0
#define CMN_mxp_num_dev_MASK			REG_3BIT_MASK
#define CMN_mxp_num_dev(value)			_GET_FV_ULL(CMN_mxp_num_dev, value)
#define CMN_mxp_poison_en			_BV_ULL(4)
#define CMN_mxp_datacheck_en			_BV_ULL(5)
#define CMN_mxp_ext_sam_en			_BV_ULL(7)
#define CMN_mxp_rxbuf_num_entries_OFFSET	8
#define CMN_mxp_rxbuf_num_entries_MASK		REG_3BIT_MASK
#define CMN_mxp_rxbuf_num_entries(value)	_GET_FV_ULL(CMN_mxp_rxbuf_num_entries, value)
#define CMN_mxp_a4s_num_OFFSET			11
#define CMN_mxp_a4s_num_MASK			REG_3BIT_MASK
#define CMN_mxp_a4s_num(value)			_GET_FV_ULL(CMN_mxp_a4s_num, value)
#define CMN_mxp_a4s_logicalid_base_OFFSET	16
#define CMN_mxp_a4s_logicalid_base_MASK		REG_8BIT_MASK
#define CMN_mxp_a4s_logicalid_base(value)	_GET_FV_ULL(CMN_mxp_a4s_logicalid_base, value)

/* 3.2.8 RN-I register summary */
#define CMN_rni_s_port_control(base, n)		CMN_REG(base, 0x0A10 + ((n) << 3))

/* 3.2.9 RN SAM register summary */
#define CMN_MAX_rnsam_sys_cache_groups		4
#define CMN_MAX_rnsam_non_hash_mem_regions	20
#define CMN_MAX_rnsam_non_hash_tgt_nodes	20
#define CMN_MAX_rnsam_sys_cache_grp_hn_nodes	32
#define CMN_rnsam_status(base)		CMN_REG(base, 0x0C00)
/* CMN_rnsam_non_hash_mem_region0: 0xC08
 * CMN_rnsam_non_hash_mem_region1: 0xC10
 * CMN_rnsam_non_hash_mem_region2: 0xC18
 * CMN_rnsam_non_hash_mem_region3: 0xC20
 * CMN_rnsam_non_hash_mem_region4: 0xC28
 * CMN_rnsam_non_hash_mem_region5: 0xCA0
 * CMN_rnsam_non_hash_mem_region6: 0xCA8
 * CMN_rnsam_non_hash_mem_region7: 0xCB0
 * CMN_rnsam_non_hash_mem_region8: 0xCB8
 * CMN_rnsam_non_hash_mem_region9: 0xCC0
 */
#define CMN_rnsam_non_hash_mem_region(base, n)		\
	CMN_32BIT_REG(base, 0x0C08, ((n) >= 10) ? ((n) + 28) : (n))
/* CMN_rnsam_non_hash_tgt_nodeid0: 0xC30
 * CMN_rnsam_non_hash_tgt_nodeid1: 0xC38
 * CMN_rnsam_non_hash_tgt_nodeid2: 0xC40
 * CMN_rnsam_non_hash_tgt_nodeid3: 0xCE0
 * CMN_rnsam_non_hash_tgt_nodeid4: 0xCE8
 */
#define CMN_rnsam_non_hash_tgt_nodeid(base, n)		\
	CMN_12BIT_REG(base, 0x0C30, ((n) >= 12) ? ((n) + 76): (n))
#define CMN_rnsam_sys_cache_grp_region(base, n)		\
					CMN_32BIT_REG(base, 0x0C48, n)
#define CMN_rnsam_sys_cache_grp_secondary_region(base, n)		\
					CMN_32BIT_REG(base, 0x0F00, n)
#define CMN_rnsam_sys_cache_grp_hn_nodeid(base, n)	\
					CMN_12BIT_REG(base, 0x0C58, n)
#define CMN_rnsam_sys_cache_grp_nonhash_nodeid(base, n)	\
					CMN_12BIT_REG(base, 0x0C98, n)
#define CMN_rnsam_sys_cache_group_hn_count(base, n)	\
					CMN_8BIT_REG(base, 0x0D00, n)
#define CMN_rnsam_sys_cache_grp_sn_nodeid(base, n)	\
					CMN_12BIT_REG(base, 0x0D08, n)
#define CMN_rnsam_sys_cache_grp_cal_mode(base)		\
					CMN_REG(base, 0x0F10)

/* RAS register */
#define CMN_cfgm_errgsr(ns, grp, rec)	CMN_REG(CMN_CFGM_BASE, 0x3000 + ((ns) << 8) + ((rec) << 3) + ((grp) << 7))
#define CMN_error_ns_0			0
#define CMN_error_ns_1			1
#define CMN_error_group_error		0
#define CMN_error_group_fault		1
#define CMN_error_record_mxp		0
#define CMN_error_record_hni		1
#define CMN_error_record_hnf		2
#define CMN_error_record_sbsx		3
#define CMN_error_record_cxg		4

/* HN-F, HN-I, XP, SBSX, CXG  */
#define CMN_errfr(base)			CMN_REG(base, 0x3000)
#define CMN_errctlr(base)		CMN_REG(base, 0x3008)
#define CMN_errstatus(base)		CMN_REG(base, 0x3010)
#define CMN_errmisc(base)		CMN_REG(base, 0x3020)
#define CMN_errfr_NS(base)		CMN_REG(base, 0x3100)
#define CMN_errctlr_NS(base)		CMN_REG(base, 0x3108)
#define CMN_errstatus_NS(base)		CMN_REG(base, 0x3110)
#define CMN_errmisc_NS(base)		CMN_REG(base, 0x3120)

/* HN-F, HN-I, SBSX, CXG */
#define CMN_erraddr(base)		CMN_REG(base, 0x3018)
#define CMN_erraddr_NS(base)		CMN_REG(base, 0x3118)

/* CXHA, CXRA, CXLA, HN-F XP */
#define CMN_pmu_event_sel(base)		CMN_REG(base, 0x2000)

/* HN-F */
#define CMN_hnf_err_inj(base)		CMN_REG(base, 0x3030)
#define CMN_hnf_err_inj_srcid_OFFSET		16
#define CMN_hnf_err_inj_srcid_MASK		REG_11BIT_MASK
#define CMN_hnf_err_inj_srcid(value)		_SET_FV_ULL(CMN_hnf_err_inj_srcid, value)
#define CMN_hnf_err_inj_lpid_OFFSET		4
#define CMN_hnf_err_inj_lpid_MASK		REG_5BIT_MASK
#define CMN_hnf_err_inj_lpid(value)		_SET_FV_ULL(CMN_hnf_err_inj_lpid, value)
#define CMN_hnf_err_inj_en			_BV(0)

/* HN-F, XP */
#define CMN_hnf_byte_par_err_inj(base)		CMN_REG(base, 0x3038)
#define CMN_mxp_byte_par_err_inj(base, p)	CMN_REG(base, 0x3030 + ((p) << 3))

/* ERRMISC */
#define CMN_xp_errmisc_tgtid_OFFSET		48
#define CMN_xp_errmisc_tgtid_MASK		REG_11BIT_MASK
#define CMN_xp_errmisc_tgtid(value)		_GET_FV_ULL(CMN_xp_errmisc_tgtid, value)
#define CMN_xp_errmisc_opcode_OFFSET		16
#define CMN_xp_errmisc_opcode_MASK		REG_6BIT_MASK
#define CMN_xp_errmisc_opcode(value)		_GET_FV_ULL(CMN_xp_errmisc_opcode, value)
#define CMN_xp_errmisc_srcid_OFFSET		4
#define CMN_xp_errmisc_srcid_MASK		REG_11BIT_MASK
#define CMN_xp_errmisc_srcid(value)		_GET_FV_ULL(CMN_xp_errmisc_srcid, value)
#define CMN_xp_errmisc_errsrc_OFFSET		0
#define CMN_xp_errmisc_errsrc_MASK		REG_3BIT_MASK
#define CMN_xp_errmisc_errsrc(value)		_GET_FV_ULL(CMN_xp_errmisc_errsrc, value)

#define CMN_hni_errmisc_size_OFFSET		28
#define CMN_hni_errmisc_size_MASK		REG_3BIT_MASK
#define CMN_hni_errmisc_size(value)		_GET_FV_ULL(CMN_hni_errmisc_size, value)
#define CMN_hni_errmisc_memattr_OFFSET		24
#define CMN_hni_errmisc_memattr_MASK		REG_4BIT_MASK
#define CMN_hni_errmisc_memattr(value)		_GET_FV_ULL(CMN_hni_errmisc_memattr, value)
#define CMN_hni_errmisc_opcode_OFFSET		16
#define CMN_hni_errmisc_opcode_MASK		REG_6BIT_MASK
#define CMN_hni_errmisc_opcode(value)		_GET_FV_ULL(CMN_hni_errmisc_opcode, value)
#define CMN_hni_errmisc_srcid_OFFSET		4
#define CMN_hni_errmisc_srcid_MASK		REG_11BIT_MASK
#define CMN_hni_errmisc_srcid(value)		_GET_FV_ULL(CMN_hni_errmisc_srcid, value)
#define CMN_hni_errmisc_errsrc_OFFSET		0
#define CMN_hni_errmisc_errsrc_MASK		REG_4BIT_MASK
#define CMN_hni_errmisc_errsrc(value)		_GET_FV_ULL(CMN_hni_errmisc_errsrc, value)

#define CMN_hnf_errmisc_optype_OFFSET		16
#define CMN_hnf_errmisc_optype_MASK		REG_2BIT_MASK
#define CMN_hnf_errmisc_optype(value)		_GET_FV_ULL(CMN_hnf_errmisc_optype, value)
#define CMN_hnf_errmisc_srcid_OFFSET		4
#define CMN_hnf_errmisc_srcid_MASK		REG_11BIT_MASK
#define CMN_hnf_errmisc_srcid(value)		_GET_FV_ULL(CMN_hnf_errmisc_srcid, value)
#define CMN_hnf_errmisc_errsrc_OFFSET		0
#define CMN_hnf_errmisc_errsrc_MASK		REG_4BIT_MASK
#define CMN_hnf_errmisc_errsrc(value)		_GET_FV_ULL(CMN_hnf_errmisc_errsrc, value)

#define CMN_sbsx_errmisc_size_OFFSET		28
#define CMN_sbsx_errmisc_size_MASK		REG_3BIT_MASK
#define CMN_sbsx_errmisc_size(value)		_GET_FV_ULL(CMN_sbsx_errmisc_size, value)
#define CMN_sbsx_errmisc_memattr_OFFSET		24
#define CMN_sbsx_errmisc_memattr_MASK		REG_4BIT_MASK
#define CMN_sbsx_errmisc_memattr(value)		_GET_FV_ULL(CMN_sbsx_errmisc_memattr, value)
#define CMN_sbsx_errmisc_optype			_BV(16)
#define CMN_sbsx_errmisc_srcid_OFFSET		4
#define CMN_sbsx_errmisc_srcid_MASK		REG_11BIT_MASK
#define CMN_sbsx_errmisc_srcid(value)		_GET_FV_ULL(CMN_sbsx_errmisc_srcid, value)

#define CMN_cxha_errmisc_srcraid_OFFSET		4
#define CMN_cxha_errmisc_srcraid_MASK		REG_6BIT_MASK
#define CMN_cxha_errmisc_srcraid(value)		_GET_FV_ULL(CMN_cxha_errmisc_srcraid, value)
#define CMN_cxha_errmisc_errsrc_OFFSET		0
#define CMN_cxha_errmisc_errsrc_MASK		REG_2BIT_MASK
#define CMN_cxha_errmisc_errsrc(value)		_GET_FV_ULL(CMN_cxha_errmisc_errsrc, value)

/* CXG link register summary: common to CXHA and CXRA */
#define CMN_cxg_cxprtcl_link_ctl(base, n)			\
					CMN_REG(base, 0x1000 + ((n) << 4))
#define CMN_cxg_cxprtcl_link_status(base, n)			\
					CMN_REG(base, 0x1008 + ((n) << 4))

/* 3.2.11 CXHA register summary */
#define CMN_cxg_ha_id(base)		CMN_REG(base, 0x0008)
#define CMN_cxg_ha_aux_ctl(base)	CMN_REG(base, 0x0A08)
#define CMN_cxg_ha_secure_register_groups_override(base)	\
					CMN_REG(base, 0x0980)
#define CMN_cxg_ha_unit_info(base)	CMN_REG(base, 0x0900)
#define CMN_cxg_ha_rnf_raid_to_ldid(base, n)			\
					CMN_8BIT_REG(base, 0x0C00, n)
#define CMN_cxg_ha_agentid_to_linkid(base, n)			\
					CMN_8BIT_REG(base, 0x0C40, n)
#define CMN_cxg_ha_agentid_to_linkid_val(base)			\
					CMN_REG(base, 0x0D00)
#define CMN_cxg_ha_rnf_raid_to_ldid_val(base)			\
					CMN_REG(base, 0x0D08)

/* 3.2.12 CXRA register summary */
#define CMN_cxg_ra_secure_register_groups_override(base)	\
					CMN_REG(base, 0x0980)
#define CMN_cxg_ra_unit_info(base)	CMN_REG(base, 0x0900)
#define CMN_cxg_ra_cfg_ctl(base)	CMN_REG(base, 0x0A00)
#define CMN_cxg_ra_aux_ctl(base)	CMN_REG(base, 0x0A08)
#define CMN_cxg_ra_sam_addr_region(base, n)			\
					CMN_REG(base, 0x0DA8 + ((n) << 3))
#define CMN_cxg_ra_agentid_to_linkid(base, n)			\
					CMN_8BIT_REG(base, 0x0E60, n)
#define CMN_cxg_ra_rnf_ldid_to_raid(base, n)			\
					CMN_8BIT_REG(base, 0x0EA0, n)
#define CMN_cxg_ra_rni_ldid_to_raid(base, n)			\
					CMN_8BIT_REG(base, 0x0EE0, n)
#define CMN_cxg_ra_rnd_ldid_to_raid(base, n)			\
					CMN_8BIT_REG(base, 0x0F00, n)
#define CMN_cxg_ra_agentid_to_linkid_val(base)			\
					CMN_REG(base, 0x0F20)
#define CMN_cxg_ra_rnf_ldid_to_raid_val(base)			\
					CMN_REG(base, 0x0F28)
#define CMN_cxg_ra_rni_ldid_to_raid_val(base)			\
					CMN_REG(base, 0x0F30)
#define CMN_cxg_ra_rnd_ldid_to_raid_val(base)			\
					CMN_REG(base, 0x0F38)

/* 3.2.13 CXLA register summary */
#define CMN_cxla_secure_register_groups_override(base)		\
					CMN_REG(base, 0x0980)
#define CMN_cxla_unit_info(base)	CMN_REG(base, 0x0900)
#define CMN_cxla_aux_ctl(base)		CMN_REG(base, 0x0A08)
#define CMN_cxla_ccix_prop_capabilities(base)			\
					CMN_REG(base, 0x0C00)
#define CMN_cxla_ccix_prop_configured(base)			\
					CMN_REG(base, 0x0C08)
#define CMN_cxla_tx_cxs_attr_capabilities(base)			\
					CMN_REG(base, 0x0C10)
#define CMN_cxla_rx_cxs_attr_capabilities(base)			\
					CMN_REG(base, 0x0C18)
#define CMN_cxla_agentid_to_linkid(base, n)			\
					CMN_8BIT_REG(base, 0x0C30, n)
#define CMN_cxla_agentid_to_linkid_val(base)			\
					CMN_REG(base, 0x0C70)
#define CMN_cxla_linkid_to_pcie_bus_num(base, n)			\
					CMN_16BIT_REG(base, 0x0C78, n)
#define CMN_cxla_tlp_hdr_fields(base)	CMN_REG(base, 0x0C80)
#define CMN_cxla_permsg_pyld(base, n)	CMN_REG(base, 0x0D00 + ((n) << 3))
#define CMN_cxla_permsg_ctl(base)	CMN_REG(base, 0x0D20)
#define CMN_cxla_err_agent_id(base)	CMN_REG(base, 0x0D28)
#define CMN_cxla_pmu_config(base)	CMN_8BIT_REG(base, 0x2210, n)
#define CMN_cxla_pmevent(base, n)	CMN_16BIT_REG(base, 0x2220, n)
#define CMN_cxla_pmevcntsr(base, n)	CMN_16BIT_REG(base, 0x2240, n)

typedef uint16_t cmn_nid_t;
typedef uint16_t cmn_lid_t;
typedef uint8_t cmn_pid_t;
typedef uint8_t cmn_did_t;

/* Reigster field definitions */
/* CMN_node_info */
#define CMN_node_type_OFFSET		0
#define CMN_node_type_MASK		REG_16BIT_MASK
#define CMN_node_type(value)		_GET_FV_ULL(CMN_node_type, value)
#define CMN_node_id_OFFSET		16
#define CMN_node_id_MASK		REG_16BIT_MASK
#define CMN_node_id(value)		_GET_FV_ULL(CMN_node_id, value)
#define CMN_logical_id_OFFSET		32
#define CMN_logical_id_MASK		REG_16BIT_MASK
#define CMN_logical_id(value)		_GET_FV_ULL(CMN_logical_id, value)

/* CMN_child_info */
#define CMN_child_count_OFFSET		0
#define CMN_child_count_MASK		REG_16BIT_MASK
#define CMN_child_count(value)		_GET_FV_ULL(CMN_child_count, value)
#define CMN_child_ptr_offset_OFFSET	16
#define CMN_child_ptr_offset_MASK	REG_16BIT_MASK
#define CMN_child_ptr_offset(value)	_GET_FV_ULL(CMN_child_ptr_offset, value)

/* CMN_pmu_event_sel */
#define CMN_pmu_event_id_OFFSET(n)	REG64_8BIT_OFFSET(n)
#define CMN_pmu_event_id_MASK		REG_8BIT_MASK
#define CMN_pmu_event_id(n, value)	_SET_FV_ULLn(n, CMN_pmu_event_id, value)
#define CMN_pmu_occupl_id_OFFSET	32
#define CMN_pmu_occupl_id_MASK		REG_3BIT_MASK
#define CMN_pmu_occupl_id(value)	_SET_FV_ULL(CMN_pmu_occupl_id, value)
#define CMN_hnf_occupl_all		0
#define CMN_hnf_occupl_read		1
#define CMN_hnf_occupl_write		2
#define CMN_hnf_occupl_atomic		3
#define CMN_hnf_occupl_stash		4
#define CMN_dn_occupl_all		0
#define CMN_dn_occupl_dvm_ops		1
#define CMN_dn_occupl_dvm_syncs		2

/* CMN_cfgm_periph_id */
#define CMN_periph_id_OFFSET(n)		REG64_32BIT_OFFSET(n)
#define CMN_periph_id_MASK		REG_32BIT_MASK
#define CMN_periph_id(n, value)		(_GET_FV_ULLn(n, CMN_periph_id, value) & REG_8BIT_MASK)

/* CMN_cfgm_periph_id_2 */
#define CMN_JEP106_id_code_6_4_OFFSET	0
#define CMN_JEP106_id_code_6_4_MASK	REG_3BIT_MASK
#define CMN_JEP106_id_code_6_4(value)	_GET_FV(CMN_JEP106_id_code_6_4, value)
#define CMN_JEDED_JEP106		_BV_ULL(3)
#define CMN_revision_OFFSET		4
#define CMN_revision_MASK		REG_4BIT_MASK
#define CMN_revision(value)		_GET_FV_ULL(CMN_revision, value)
#define CMN_r1p0			0
#define CMN_r1p1			1
#define CMN_r1p2			2
#define CMN_r1p3			3
#define CMN_r2p0			4
#define CMN_r3p0			5
#define CMN_r3p1			6
#define CMN_r3p2			7

/* CMN_cfgm_child_info */
#define CMN_child_external			_BV_ULL(31)
#define CMN_child_address_offset_OFFSET		0
#define CMN_child_address_offset_MASK		REG_28BIT_MASK
#define CMN_child_address_offset(value)		_GET_FV_ULL(CMN_child_address_offset, value)
#define CMN_child_node_pointer_OFFSET		14
#define CMN_child_node_pointer_MASK		REG_14BIT_MASK
#define CMN_child_node_pointer(value)		_GET_FV_ULL(CMN_child_node_pointer, value)
#define CMN_child_register_offset_OFFSET	0
#define CMN_child_register_offset_MASK		REG_14BIT_MASK
#define CMN_child_register_offset(value)	_GET_FV_ULL(CMN_child_register_offset, value)

/* CMN_dt_dtc_ctl */
#define CMN_dt_cg_disable			_BV_ULL(10)
#define CMN_cross_trigger_count_OFFSET		4
#define CMN_cross_trigger_count_MASK		REG_6BIT_MASK
#define CMN_cross_trigger_count(value)		_SET_FV_ULL(CMN_cross_trigger_count, value)
#define CMN_dt_wait_for_trigger			_BV_ULL(3)
#define CMN_atbtrigger_en			_BV_ULL(2)
#define CMN_dbgtrigger_en			_BV_ULL(1)
#define CMN_dt_en				_BV_ULL(0)

/* CMN_dt_pmcr */
#define CMN_ovfl_intr_en			_BV_ULL(6)
#define CMN_cntr_rst				_BV_ULL(5)
#define CMN_cntcfg_OFFSET			1
#define CMN_cntcfg_MASK				REG_4BIT_MASK
#define CMN_cntcfg(value)			_SET_FV_ULL(CMN_cntcfg, value)
#define CMN_pmu_en				_BV_ULL(0)

/* CMN_hnf_sam_control */
#define CMN_hn_cfg_sn_nodeid_OFFSET(n)		CMN_12BIT_OFFSET(n)
#define CMN_hn_cfg_sn_nodeid_MASK		REG_12BIT_MASK
#define CMN_hn_cfg_sn_nodeid(n, value)		(_SET_FV_ULLn(n, CMN_hn_cfg_sn_nodeid, value) & REG_11BIT_MASK)
#define CMN_hn_cfg_three_sn_en			_BV_ULL(36)
#define CMN_hn_cfg_six_sn_en			_BV_ULL(37)
#define CMN_hn_cfg_sam_top_address_bit_OFFSET(n)	\
						(REG64_8BIT_OFFSET(n) + 40)
#define CMN_hn_cfg_sam_top_address_bit_MASK	REG64_8BIT_MASK
#define CMN_hn_cfg_sam_top_address_bit(n, value)	\
	_SET_FV_ULLn(n, CMN_hn_cfg_sam_top_address_bit, value)
#define CMN_hn_cfg_sam_inv_top_address_bit	_BV_ULL(63)

/* CMN_hnf_rn_phys_id */
#define CMN_nodeid_ra_OFFSET		0
#define CMN_nodeid_ra_MASK		REG_11BIT_MASK
#define CMN_nodeid_ra(value)		_SET_FV_ULL(CMN_nodeid_ra, value)
#define CMN_remote_ra			_BV_ULL(16)
#define CMN_cpa_grp_ra_OFFSET		17
#define CMN_cpa_grp_ra_MASK		REG_2BIT_MASK
#define CMN_cpa_grp_ra(value)		_SET_FV_ULL(CMN_cpa_grp_ra, value)
#define CMN_cpa_en_ra			_BV_ULL(30)
#define CMN_valid_ra			_BV_ULL(31)
#define CMN_phys_id_OFFSET(n)		REG64_32BIT_OFFSET(n)
#define CMN_phys_id_MASK		REG_32BIT_MASK
#define CMN_phys_id(n, value)		_SET_FV_ULLn(n, CMN_phys_id, value)

/* CMN_mxp_device_port_connect_info */
#define CMN_device_type_OFFSET		0
#define CMN_device_type_MASK		REG_5BIT_MASK
#define CMN_device_type(value)		_GET_FV_ULL(CMN_device_type, value)

/* CMN_rnsam_non_hash_tgt_nodeid
 * CMN_rnsam_sys_cache_grp_hn_nodeid
 * CMN_rnsam_sys_cache_grp_sn_nodeid
 */
#define CMN_nodeid_OFFSET(n)		CMN_12BIT_OFFSET(n)
#define CMN_nodeid_MASK			REG_12BIT_MASK
#define CMN_nodeid(n, value)		_SET_FV_ULLn(n, CMN_nodeid, value)

#define CMN_scg_hnf_num_OFFSET(n)	CMN_8BIT_OFFSET(n)
#define CMN_scg_hnf_num_MASK		REG_8BIT_MASK
#define CMN_scg_hnf_num(n, value)	_SET_FV_ULLn(n, CMN_scg_hnf_num, value)

/* CMN_hnf_slc_lock_ways */
#define CMN_hnf_slc_lock_ways_ways_OFFSET		0
#define CMN_hnf_slc_lock_ways_ways_MASK			REG_4BIT_MASK
#define CMN_hnf_slc_lock_ways_ways(value)		_SET_FV_ULL(CMN_hnf_slc_lock_ways_ways, value)
#define CMN_hnf_slc_lock_ways_num_hnf_OFFSET		8
#define CMN_hnf_slc_lock_ways_num_hnf_MASK		REG_7BIT_MASK
#define CMN_hnf_slc_lock_ways_num_hnf(value)		_SET_FV_ULL(CMN_hnf_slc_lock_num_hnf, value)

/* CMN_hnf_slc_lock_base(base, n) */
#define CMN_hnf_slc_lock_basen_OFFSET		0
#define CMN_hnf_slc_lock_basen_MASK		REG_48BIT_MASK
#define CMN_hnf_slc_lock_basen(value)		_SET_FV_ULL(CMN_hnf_slc_lock_basen, value)
#define CMN_hnf_slc_lock_basen_vld		_BV_ULL(63)
/* CMN_hnf_ppu_pwpr */
#define CMN_ppu_policy_OFFSET			0
#define CMN_ppu_policy_MASK			REG_4BIT_MASK
#define CMN_ppu_policy(value)			_SET_FV_ULL(CMN_ppu_policy, value)
#define CMN_ppu_policy_OFF			0
#define CMN_ppu_policy_MEM_RET			2
#define CMN_ppu_policy_FUNC_RET			7
#define CMN_ppu_policy_ON			8
#define CMN_ppu_op_mode_OFFSET			4
#define CMN_ppu_op_mode_MASK			REG_4BIT_MASK
#define CMN_ppu_op_mode(value)			_SET_FV_ULL(CMN_ppu_op_mode, value)
#define CMN_ppu_op_mode_NOSFSLC			0
#define CMN_ppu_op_mode_SFONLY			1
#define CMN_ppu_op_mode_HAM			2
#define CMN_ppu_op_mode_FAM			3
#define CMN_ppu_dyn_en				_BV_ULL(8)

/* CMN_hnf_ppu_pwsr */
#define CMN_ppu_pow_status_OFFSET		0
#define CMN_ppu_pow_status_MASK			REG_4BIT_MASK
#define CMN_ppu_pow_status(value)		_SET_FV_ULL(CMN_ppu_pow_status, value)
#define CMN_ppu_pow_status_OFF			0
#define CMN_ppu_pow_status_MEM_RET		2
#define CMN_ppu_pow_status_FUNC_RET		7
#define CMN_ppu_pow_status_ON			8
#define CMN_ppu_op_mode_status_OFFSET		4
#define CMN_ppu_op_mode_status_MASK		REG_4BIT_MASK
#define CMN_ppu_op_mode_status(value)		_GET_FV_ULL(CMN_ppu_op_mode_status, value)
#define CMN_ppu_op_mode_status_NOSFSLC		0
#define CMN_ppu_op_mode_status_SFONLY		1
#define CMN_ppu_op_mode_status_HAM		2
#define CMN_ppu_op_mode_status_FAM		3
#define CMN_ppu_dyn_en_status			_BV_ULL(8)	

/* CMN_dtm_pmu_config */
#define CMN_pmu_en				_BV_ULL(0)
#define CMN_pmevcnt01_combined			_BV_ULL(1)
#define CMN_pmevcnt23_combined			_BV_ULL(2)
#define CMN_pmevcntall_combined			_BV_ULL(3)
#define CMN_pmevcnt_paired(n)			_BV_ULL(4 + (n))
#define CMN_pmevcnt_global_num_OFFSET(n)	REG64_4BIT_OFFSET(n)
#define CMN_pmevcnt_global_num_MASK		REG_4BIT_MASK
#define CMN_pmevcnt_global_num(n, value)	_SET_FV_ULL(n, CMN_pmevcnt_global_num, value)
#define CMN_pmevcnt_input_sel_OFFSET(n)		REG64_8BIT_OFFSET((n) + 4)
#define CMN_pmevcnt_input_sel_MASK		REG_8BIT_MASK
#define CMN_pmevcnt_input_sel(n, value)		_SET_FV_ULL(n, CMN_pmevcnt_input_sel, value)
#define CMN_pmevcnt_input_sel_wp(n)		(n)
#define CMN_pmevcnt_input_sel_xp(e)		(0x4 + (e))
#define CMN_pmevcnt_input_sel_dev(p, d, e)	(0x10 + ((p) * 0x10) + ((d) * 0x04) + (e))
/* CMN_dtm_pmevcnt/CMN_dtm_pmevcntsr */
#define CMN_pmevcnt_OFFSET(n)		REG64_16BIT_OFFSET(n)
#define CMN_pmevcnt_MASK		REG_16BIT_MASK
#define CMN_pmevcnt(n, value)		_SET_FV_ULL(n, CMN_pmevcnt, value)

/* CMN_rnsam_status */
#define CMN_sam_use_default_node	_BV_ULL(0)
#define CMN_sam_nstall_req_OFFSET	1
#define CMN_sam_nstall_req_MASK		REG_1BIT_MASK
#define CMN_sam_nstall_req(value)	_SET_FV_ULL(CMN_sam_nstall_req, value)
#define CMN_sam_stall_req		0
#define CMN_sam_unstall_req		1

/* CMN_rnsam_sys_cache_grp_cal_mode */
#define CMN_scg_hnf_cal_mode_en(n)	(_BV_ULL(0) << ((n) << 4))

/* CMN_rnsam_sys_cache_grp_region */
#define CMN_region_nonhash_reg_en	_BV_ULL(1)
#define CMN_region_OFFSET(n)		REG64_32BIT_OFFSET(n)
#define CMN_region_MASK			REG_32BIT_MASK
#define CMN_region(n, value)		_SET_FV_ULLn(n, CMN_region, value)

/* CMN_rnsam_sys_cache_grp_region
 * CMN_rnsam_non_hash_mem_region
 */
#define CMN_region_valid		_BV_ULL(0)
#define CMN_region_target_type_OFFSET	2
#define CMN_region_target_type_MASK	REG_2BIT_MASK
#define CMN_region_target_type(value)	_SET_FV_ULL(CMN_region_target_type, value)
#define CMN_region_target_HNF		0
#define CMN_region_target_HNI		1
#define CMN_region_target_CXRA		2
#define CMN_region_size_OFFSET		4
#define CMN_region_size_MASK		REG_5BIT_MASK
#define CMN_region_size(value)		_SET_FV_ULL(CMN_region_size, value)
#define CMN_region_base_addr_OFFSET	9
#define CMN_region_base_addr_MASK	REG_22BIT_MASK
#define CMN_region_base_addr(value)	_SET_FV_ULL(CMN_region_base_addr, value)

#define CMN_valid_region(t, b, s)		\
	(CMN_region_valid |			\
	 CMN_region_target_type(t) |		\
	 CMN_region_base_addr(b >> 26) |	\
	 CMN_region_size(__ilog2_u64((s) / CMN_SAM_GRANU)))

#define CMN_valid_nonhash_region(t, b, s)	\
	(CMN_region_valid |			\
	 CMN_region_nonhash_reg_en |		\
	 CMN_region_target_type(t) |		\
	 CMN_region_base_addr(b >> 26) |	\
	 CMN_region_size(__ilog2_u64((s) / CMN_SAM_GRANU)))

/* CMN_hnf_sam_memregion */
#define CMN_sam_range_nodeid_OFFSET	0
#define CMN_sam_range_nodeid_MASK	REG_11BIT_MASK
#define CMN_sam_range_nodeid(value)	_SET_FV_ULL(CMN_sam_range_nodeid, value)
#define CMN_sam_range_size_OFFSET	12
#define CMN_sam_range_size_MASK		REG_5BIT_MASK
#define CMN_sam_range_size(value)	_SET_FV_ULL(CMN_sam_range_size, value)
#define CMN_sam_range_base_addr_OFFSET	26
#define CMN_sam_range_base_addr_MASK	REG_22BIT_MASK
#define CMN_sam_range_base_addr(value)	_SET_FV_ULL(CMN_sam_range_base_addr, value)
#define CMN_sam_range_valid		_BV_ULL(63)

/* CMN_cxg_ha_cxprtcl_link_ctl
 * CMN_cxg_ra_cxprtcl_link_ctl
 */
#define CMN_lnk_link_en			_BV_ULL(0)
#define CMN_lnk_link_req		_BV_ULL(1)
#define CMN_lnk_link_up			_BV_ULL(2)

/* CMN_cxg_ha_cxprtcl_link_ctl */
#define CMN_lnk_snoopdomain_req		_BV_ULL(3)
#define CMN_lnk_num_reqcrds_OFFSET	4
#define CMN_lnk_num_reqcrds_MASK	REG_4BIT_MASK
#define CMN_lnk_num_reqcrds(value)	_SET_FV_ULL(CMN_lnk_num_reqcrds, value)
#define CMN_lnk_num_datcrds_OFFSET	8
#define CMN_lnk_num_datcrds_MASK	REG_4BIT_MASK
#define CMN_lnk_num_datcrds(value)	_SET_FV_ULL(CMN_lnk_num_datcrds, value)

/* CMN_cxg_ra_cxprtcl_link_ctl */
#define CMN_lnk_dvmdomain_req		_BV_ULL(3)
#define CMN_lnk_num_snpcrds_OFFSET	4
#define CMN_lnk_num_snpcrds_MASK	REG_4BIT_MASK
#define CMN_lnk_num_snpcrds(value)	_SET_FV_ULL(CMN_lnk_num_snpcrds, value)
#define CMN_lnk_dis_cpuevent_prop	_BV_ULL(8)

/* CMN_cxg_ha_cxprtcl_link_status
 * CMN_cxg_ra_cxprtcl_link_status
 */
#define CMN_lnk_link_ack		_BV_ULL(0)
#define CMN_lnk_link_down		_BV_ULL(1)

/* CMN_cxg_ha_cxprtcl_link_status */
#define CMN_lnk_snoopdomain_ack		_BV_ULL(2)

/* CMN_cxg_ra_cxprtcl_link_status */
#define CMN_lnk_dvmdomain_ack		_BV_ULL(2)
#define CMN_lnk_ot_cbkwr		_BV_ULL(3)

/* CMN_cxg_ha_id */
#define CMN_ccix_haid_OFFSET		0
#define CMN_ccix_haid_MASK		REG_6BIT_MASK
#define CMN_ccix_haid(value)		_GET_FV(CMN_ccix_haid, value)

/* CMN_cxg_ha_aux_ctl
 * CMN_cxg_ra_aux_ctl
 */
#define CMN_cg_disable			_BV_ULL(0)

/* CMN_cxg_ha_aux_ctl */
#define CMN_ha_smp_mode_en		_BV_ULL(16)
#define CMN_ha_disable_cg_flopen	_BV_ULL(5)
#define CMN_ha_disable_secure_access	_BV_ULL(4)
#define CMN_ha_early_compack_en		_BV_ULL(3)
#define CMN_ha_remote_chia_rnf_present	_BV_ULL(2)
#define CMN_ha_snoop_dataret_disable	_BV_ULL(1)

/* CMN_cxg_ra_aux_ctl */
#define CMN_ra_smp_mode_en		_BV_ULL(16)
#define CMN_ra_dis_cxsa_early_write_comp	\
					_BV_ULL(10)
#define CMN_ra_dis_dbiddispq_rsp	_BV_ULL(9)
#define CMN_ra_dis_wrreqchain		_BV_ULL(8)
#define CMN_ra_dis_rdreqchain		_BV_ULL(7)
#define CMN_ra_r2byp_en			_BV_ULL(6)
#define CMN_ra_dis_rem_secure_access	_BV_ULL(5)
#define CMN_ra_sameaddr_ord_wfc		_BV_ULL(4)
#define CMN_ra_devnr_ord_wfc		_BV_ULL(3)
#define CMN_ra_early_compack_en		_BV_ULL(2)
#define CMN_ra_early_rdrcpt_en		_BV_ULL(1)

/* CMN_cxla_aux_ctl */
#define CMN_la_tlp_timeout_en		_BV_ULL(6)
#define CMN_la_idle_timeout_th_OFFSET	8
#define CMN_la_idle_timeout_th_MASK	REG_3BIT_MASK
#define CMN_la_idle_timeout_th(value)	_SET_FV_ULL(CMN_la_idle_timeout_th, value)
#define CMN_la_rspdat_timeout_th_OFFSET	12
#define CMN_la_rspdat_timeout_th_MASK	REG_3BIT_MASK
#define CMN_la_rspdat_timeout_th(value)	_SET_FV_ULL(CMN_la_rspdat_timeout_th, value)
#define CMN_la_reqrsp_timeout_th_OFFSET	16
#define CMN_la_reqrsp_timeout_th_MASK	REG_3BIT_MASK
#define CMN_la_reqrsp_timeout_th(value)	_SET_FV_ULL(CMN_la_reqrsp_timeout_th, value)
#define CMN_la_snpdat_timeout_th_OFFSET	20
#define CMN_la_snpdat_timeout_th_MASK	REG_3BIT_MASK
#define CMN_la_snpdat_timeout_th(value)	_SET_FV_ULL(CMN_la_snpdat_timeout_th, value)
#define CMN_la_snprsp_timeout_th_OFFSET	24
#define CMN_la_snprsp_timeout_th_MASK	REG_3BIT_MASK
#define CMN_la_snprsp_timeout_th(value)	_SET_FV_ULL(CMN_la_snprsp_timeout_th, value)
#define CMN_la_reqdat_timeout_th_OFFSET	28
#define CMN_la_reqdat_timeout_th_MASK	REG_3BIT_MASK
#define CMN_la_reqdat_timeout_th(value)	_SET_FV_ULL(CMN_la_reqdat_timeout_th, value)
#define CMN_la_snp_timeout_th_OFFSET	32
#define CMN_la_snp_timeout_th_MASK	REG_3BIT_MASK
#define CMN_la_snp_timeout_th(value)	_SET_FV_ULL(CMN_la_snp_timeout_th, value)
#define CMN_la_misc_timeout_th_OFFSET	36
#define CMN_la_misc_timeout_th_MASK	REG_3BIT_MASK
#define CMN_la_misc_timeout_th(value)	_SET_FV_ULL(CMN_la_misc_timeout_th, value)
#define CMN_la_crdgnt_crd_count_th_OFFSET	40
#define CMN_la_crdgnt_crd_count_th_MASK		REG_3BIT_MASK
#define CMN_la_crdgnt_crd_count_th(value)	_SET_FV_ULL(CMN_la_crdgnt_crd_count_th, value)
#define CMN_la_crdgnt_crd_cycle_th_OFFSET	44
#define CMN_la_crdgnt_crd_cycle_th_MASK		REG_3BIT_MASK
#define CMN_la_crdgnt_crd_cycle_th(value)	_SET_FV_ULL(CMN_la_crdgnt_crd_cycle_th, value)
#define CMN_la_smp_mode_en		_BV_ULL(47)

/* CMN_cxg_ha_secure_register_groups_override */
#define CMN_ha_ldid_ctl			_BV_ULL(0)
#define CMN_ha_linkid_ctl		_BV_ULL(1)

/* CMN_cxg_ra_secure_register_groups_override */
#define CMN_ra_cfg_ctl			_BV_ULL(0)
#define CMN_ra_rasam_ctl		_BV_ULL(1)
#define CMN_ra_linkid_ctl		_BV_ULL(2)
#define CMN_ra_ldid_ctl			_BV_ULL(3)

/* CMN_cxla_secure_register_groups_override */
#define CMN_la_cfg_ctl			_BV_ULL(0)
#define CMN_la_linkid_ctl		_BV_ULL(2)

/* CMN_cxg_ha_unit_info */
#define CMN_ha_smp_mode					_BV_ULL(63)
#define CMN_ha_snoop_compack_hazbuf_depth_OFFSET	54
#define CMN_ha_snoop_compack_hazbuf_depth_MASK		REG_9BIT_MASK
#define CMN_ha_snoop_compack_hazbuf_depth(value)	_GET_FV_ULL(CMN_ha_snoop_compack_hazbuf_depth, value)
#define CMN_ha_snoop_databuffer_depth_OFFSET		45
#define CMN_ha_snoop_databuffer_depth_MASK		REG_9BIT_MASK
#define CMN_ha_snoop_databuffer_depth(value)		_GET_FV_ULL(CMN_ha_snoop_databuffer_depth, value)
#define CMN_ha_snoop_tracker_depth_OFFSET		36
#define CMN_ha_snoop_tracker_depth_MASK			REG_9BIT_MASK
#define CMN_ha_snoop_tracker_depth(value)		_GET_FV_ULL(CMN_snoop_tracker_depth, value)
#define CMN_ha_snoop_request_sinkbuffer_depth_OFFSET	27
#define CMN_ha_snoop_request_sinkbuffer_depth_MASK	REG_9BIT_MASK
#define CMN_ha_snoop_request_sinkbuffer_depth(value)	_GET_FV_ULL(CMN_snoop_request_sinkbuffer_depth, value)
#define CMN_ha_wdb_depth_OFFSET				18
#define CMN_ha_wdb_depth_MASK				REG_9BIT_MASK
#define CMN_ha_wdb_depth(value)				_GET_FV_ULL(CMN_wdb_depth, value)
#define CMN_ha_rdb_depth_OFFSET				9
#define CMN_ha_rdb_depth_MASK				REG_9BIT_MASK
#define CMN_ha_rdb_depth(value)				_GET_FV_ULL(CMN_rdb_depth, value)
#define CMN_ha_request_tracker_depth_OFFSET		0
#define CMN_ha_request_tracker_depth_MASK		REG_9BIT_MASK
#define CMN_ha_request_tracker_depth(value)		_GET_FV_ULL(CMN_request_tracker_depth, value)

/* CMN_cxg_ra_unit_info */
#define CMN_num_mem_regions_OFFSET			0
#define CMN_num_mem_regions_MASK			REG_5BIT_MASK
#define CMN_num_mem_regions(value)			_GET_FV_ULL(CMN_num_mem_regions, value)
#define CMN_num_mem_region_limit_enabled		_BV_ULL(6)
#define CMN_a4s_logicalid_OFFSET			8
#define CMN_a4s_logicalid_MASK				REG_8BIT_MASK
#define CMN_a4s_logicalid(value)			_GET_FV_ULL(CMN_a4s_logicalid, value)
#define CMN_ra_request_tracker_depth_OFFSET		16
#define CMN_ra_request_tracker_depth_MASK		REG_9BIT_MASK
#define CMN_ra_request_tracker_depth(value)		_GET_FV_ULL(CMN_ra_request_tracker_depth, value)
#define CMN_ra_rdb_depth_OFFSET				25
#define CMN_ra_rdb_depth_MASK				REG_9BIT_MASK
#define CMN_ra_rdb_depth(value)				_GET_FV_ULL(CMN_ra_rdb_depth, value)
#define CMN_ra_wdb_depth_OFFSET				34
#define CMN_ra_wdb_depth_MASK				REG_9BIT_MASK
#define CMN_ra_wdb_depth(value)				_GET_FV_ULL(CMN_ra_wdb_depth, value)
#define CMN_ra_snoop_tracker_depth_OFFSET		43
#define CMN_ra_snoop_tracker_depth_MASK			REG_9BIT_MASK
#define CMN_ra_snoop_tracker_depth(value)		_GET_FV_ULL(CMN_ra_snoop_tracker_depth, value)
#define CMN_ra_snoop_databuffer_depth_OFFSET		52
#define CMN_ra_snoop_databuffer_depth_MASK		REG_9BIT_MASK
#define CMN_ra_snoop_databuffer_depth(value)		_GET_FV_ULL(CMN_ra_snoop_databuffer_depth, value)
#define CMN_ra_smp_mode					_BV_ULL(61)

/* CMN_cxla_unit_info */
#define CMN_la_db_present				_BV_ULL(0)
#define CMN_la_db_lcrd_count_OFFSET			1
#define CMN_la_db_lcrd_count_MASK			REG_4BIT_MASK
#define CMN_la_db_lcrd_count(value)			_GET_FV_ULL(CMN_la_db_lcrd_count, value)
#define CMN_la_tlp_count_tx_OFFSET			8
#define CMN_la_tlp_count_tx_MASK			REG_3BIT_MASK
#define CMN_la_tlp_count_tx(value)			_GET_FV_ULL(CMN_la_tlp_count_tx, value)
#define CMN_la_tlp_count_rx_OFFSET			12
#define CMN_la_tlp_count_rx_MASK			REG_3BIT_MASK
#define CMN_la_tlp_count_rx(value)			_GET_FV_ULL(CMN_la_tlp_count_rx, value)
#define CMN_la_db_fifo_depth_OFFSET			16
#define CMN_la_db_fifo_depth_MASK			REG_6BIT_MASK
#define CMN_la_db_fifo_depth(value)			_GET_FV_ULL(CMN_la_db_fifo_depth, value)

/* CMN_cxg_ra_cfg_ctl */
#define CMN_ra_cxsa_mode_en				_BV_ULL(9)
#define CMN_ra_en_cxla_pmucmd_prop			_BV_ULL(8)
#define CMN_ra_pcie_nonqos15_entry_rsv_en		_BV_ULL(2)
#define CMN_ra_qos15_entry_rsv_en			_BV_ULL(1)
#define CMN_ra_pcie_qos15_entry_rsv_en			_BV_ULL(0)

/* CMN_cxla_ccix_prop_capabilities
 * CMN_cxla_ccix_prop_configured
 */
#define CMN_la_nocompack				_BV_ULL(0)
#define CMN_la_partialcachestates			_BV_ULL(1)
#define CMN_la_cachelinesize				_BV_ULL(2)
#define CMN_la_addrwidth_OFFSET				3
#define CMN_la_addrwidth_MASK				REG_3BIT_MASK
#define CMN_la_addrwidth(value)				_SET_FV_ULL(CMN_la_addrwidth, value)
#define CMN_LA_ADDRWIDTH(value)				_GET_FV_ULL(CMN_la_addrwidth, value)
#define CMN_la_pktheader				_BV_ULL(6)
#define CMN_la_maxpacketsize_OFFSET			7
#define CMN_la_maxpacketsize_MASK			REG_3BIT_MASK
#define CMN_la_maxpacketsize(value)			_SET_FV_ULL(CMN_la_maxpacketsize, value)
#define CMN_LA_MAXPACKETSIZE(value)			_GET_FV_ULL(CMN_la_maxpacketsize, value)
#define CMN_la_nomessagepack				_BV_ULL(10)

/* CMN_cxla_tx_cxs_attr_capabilities */
/* CMN_cxla_rx_cxs_attr_capabilities */

/* CMN_cxg_ra_sam_addr_region */
#define CMN_reg_size_OFFSET				0
#define CMN_reg_size_MASK				REG_6BIT_MASK
#define CMN_reg_size(value)				\
	_SET_FV_ULL(CMN_reg_size, __ilog2_u64((value) / _BV(CMN_reg_base_addr_OFFSET)))
#define CMN_reg_base_addr_OFFSET			16
#define CMN_reg_base_addr_MASK				REG_32BIT_MASK
#define CMN_reg_base_addr(value)			\
	_SET_FV_ULL(CMN_reg_base_addr, (value) >> CMN_reg_base_addr_OFFSET)
#define CMN_reg_ha_tgtid_OFFSET				52
#define CMN_reg_ha_tgtid_MASK				REG_6BIT_MASK
#define CMN_reg_ha_tgtid(value)				_SET_FV_ULL(CMN_reg_ha_tgtid, value)
#define CMN_reg_valid					_BV_ULL(63)

/* CMN_cxg_ha_rnf_raid_to_ldid */
#define CMN_ldid_OFFSET					0
#define CMN_ldid_MASK					REG_6BIT_MASK
#define CMN_ldid(value)					_SET_FV_ULL(CMN_ldid, value)
#define CMN_ldid_rnf_OFFSET				7
#define CMN_ldid_rnf_MASK				REG_1BIT_MASK
#define CMN_ldid_rnf(value)				_SET_FV_ULL(CMN_ldid_rnf, value)
#define CMN_rnf_raid_to_ldid(ldid, rnf)			\
	(CMN_ldid(ldid) | CMN_ldid_rnf(rnf))
/* CMN_cxg_ha_rnf_raid_to_ldid */
#define CMN_raid_ldid_OFFSET(n)				REG64_8BIT_OFFSET(n)
#define CMN_raid_ldid_MASK				REG_8BIT_MASK
#define CMN_raid_ldid(n, value)				_SET_FV_ULLn(n, CMN_raid_ldid, value)
/* CMN_cxg_ra_rnf_ldid_to_raid */
#define CMN_ldid_raid_OFFSET(n)				REG64_8BIT_OFFSET(n)
#define CMN_ldid_raid_MASK				REG_8BIT_MASK
#define CMN_ldid_raid(n, value)				_SET_FV_ULLn(n, CMN_ldid_raid, value)
/* CMN_cxg_ha_rnf_raid_to_ldid_val */
#define CMN_raid_ldid_valid(n)				_BV_ULL(n)
/* CMN_cxg_ra_rnf_ldid_to_raid_val
 * CMN_cxg_ra_rni_ldid_to_raid_val
 * CMN_cxg_ra_rnd_ldid_to_raid_val
 */
#define CMN_ldid_raid_valid(n)				_BV_ULL(n)

/* CMN_cxg_ha_agentid_to_linkid
 * CMN_cxg_ra_agentid_to_linkid
 * CMN_cxla_agentid_to_linkid
 */
#define CMN_agent_linkid_OFFSET(n)			REG64_8BIT_OFFSET(n)
#define CMN_agent_linkid_MASK				REG_8BIT_MASK
#define CMN_agent_linkid(n, value)			_SET_FV_ULLn(n, CMN_agent_linkid, value)
/* CMN_cxg_ha_agentid_to_linkid_val
 * CMN_cxg_ra_agentid_to_linkid_val
 * CMN_cxla_agentid_to_linkid_val
 */
#define CMN_agent_linkid_valid(n)			_BV_ULL(n)

/* CMN_cxla_linkid_to_pcie_bus_num */
#define CMN_link_pcie_bus_num_OFFSET			0
#define CMN_link_pcie_bus_num_MASK			REG_8BIT_MASK
#define CMN_link_pcie_bus_num(value)			_SET_FV_ULL(CMN_link_pcie_bus_num, value)
#define CMN_link_pcie_func_num_OFFSET			8
#define CMN_link_pcie_func_num_MASK			REG_3BIT_MASK
#define CMN_link_pcie_func_num(value)			_SET_FV_ULL(CMN_link_pcie_func_num, value)
#define CMN_link_pcie_dev_num_OFFSET			11
#define CMN_link_pcie_dev_num_MASK			REG_5BIT_MASK
#define CMN_link_pcie_dev_num(value)			_SET_FV_ULL(CMN_link_pcie_dev_num, value)
#define CMN_pcie_bdf(bus, dev, func)			\
	(CMN_link_pcie_bus_num(bus) |			\
	 CMN_link_pcie_dev_num(dev) |			\
	 CMN_link_pcie_func_num(func))
#define CMN_link_pcie_bdf_OFFSET(n)			REG64_16BIT_OFFSET(n)
#define CMN_link_pcie_bdf_MASK				REG_16BIT_MASK
#define CMN_link_pcie_bdf(n, value)			_SET_FV_ULLn(n, CMN_link_pcie_bdf, value)

/* CMN_cxla_tlp_hdr_fields */
#define CMN_tlp_pkt_type_OFFSET				0
#define CMN_tlp_pkt_type_MASK				REG_5BIT_MASK
#define CMN_tlp_pkt_type(value)				_SET_FV_ULL(CMN_tlp_pkt_type, value)
#define CMN_tlp_fmt_OFFSET				5
#define CMN_tlp_fmt_MASK				REG_3BIT_MASK
#define CMN_tlp_fmt(value)				_SET_FV_ULL(CMN_tlp_fmt, value)
#define CMN_tlp_attr_OFFSET				8
#define CMN_tlp_attr_MASK				REG_4BIT_MASK
#define CMN_tlp_attr(value)				_SET_FV_ULL(CMN_tlp_attr, value)
#define CMN_tlp_tc_OFFSET				12
#define CMN_tlp_tc_MASK					REG_3BIT_MASK
#define CMN_tlp_tc(value)				_SET_FV_ULL(CMN_tlp_tc, value)
#define CMN_tlp_at_OFFSET				15
#define CMN_tlp_at_MASK					REG_2BIT_MASK
#define CMN_tlp_at(value)				_SET_FV_ULL(CMN_tlp_at, value)
#define CMN_tlp_ep					_BV_ULL(17)
#define CMN_tlp_td					_BV_ULL(18)
#define CMN_tlp_messagecode_OFFSET			24
#define CMN_tlp_messagecode_MASK			REG_8BIT_MASK
#define CMN_tlp_messagecode(value)			_SET_FV_ULL(CMN_tlp_messagecode, value)
#define CMN_tlp_vendor_OFFSET				32
#define CMN_tlp_vendor_MASK				REG_16BIT_MASK
#define CMN_tlp_vendor(value)				_SET_FV_ULL(CMN_tlp_vendor, value)

#define CCIX_VENDOR_ID					0x1E2C

#define CMN_errfr_ED_OFFSET				0
#define CMN_errfr_ED_MASK				REG_2BIT_MASK
#define CMN_errfr_ED(value)				_GET_FV_ULL(CMN_errfr_ED, value)
#define CMN_errfr_DE_OFFSET				2
#define CMN_errfr_DE_MASK				REG_2BIT_MASK
#define CMN_errfr_DE(value)				_GET_FV_ULL(CMN_errfr_DE, value)
#define CMN_errfr_UI_OFFSET				4
#define CMN_errfr_UI_MASK				REG_2BIT_MASK
#define CMN_errfr_UI(value)				_GET_FV_ULL(CMN_errfr_UI, value)
#define CMN_errfr_FI_OFFSET				6
#define CMN_errfr_FI_MASK				REG_2BIT_MASK
#define CMN_errfr_FI(value)				_GET_FV_ULL(CMN_errfr_FI, value)
#define CMN_errfr_CFI_OFFSET				10
#define CMN_errfr_CFI_MASK				REG_2BIT_MASK
#define CMN_errfr_CFI(value)				_GET_FV_ULL(CMN_errfr_CFI, value)
#define CMN_errfr_CEC_OFFSET				12
#define CMN_errfr_CEC_MASK				REG_3BIT_MASK
#define CMN_errfr_CEC(value)				_GET_FV_ULL(CMN_errfr_CEC, value)

#define CMN_errctlr_ED					_BV(0)
#define CMN_errctlr_DE					_BV(1)
#define CMN_errctlr_UI					_BV(2)
#define CMN_errctlr_FI					_BV(3)
#define CMN_errctlr_CFI					_BV(8)

#define CMN_errstatus_DE				_BV_ULL(23)
#define CMN_errstatus_CE				_BV_ULL(24)
#define CMN_errstatus_MV				_BV_ULL(26)
#define CMN_errstatus_OF				_BV_ULL(27)
#define CMN_errstatus_UE				_BV_ULL(29)
#define CMN_errstatus_V					_BV_ULL(30)
#define CMN_errstatus_AV				_BV_ULL(31)

/* CMN macros and APIs */
#define cmn_node_type(base)			\
	CMN_node_type(__raw_readq(CMN_node_info(base)))
#define cmn_node_id(base)			\
	((cmn_nid_t)(CMN_node_id(__raw_readq(CMN_node_info(base)))))
#define cmn_logical_id(base)			\
	((cmn_lid_t)(CMN_logical_id(__raw_readq(CMN_node_info(base)))))
#define cmn_child_count(base)			\
	CMN_child_count(__raw_readq(CMN_child_info(base)))
#define cmn_child_ptr_offset(base)		\
	CMN_child_ptr_offset(__raw_readq(CMN_child_info(base)))
#define cmn_node_x(base)	CMN_X(cmn_node_id(base))
#define cmn_node_y(base)	CMN_Y(cmn_node_id(base))
#define cmn_node_pid(base)	CMN_PID(cmn_node_id(base))
#define cmn_node_did(base)	CMN_DID(cmn_node_id(base))

#define cmn_child_ptr(base, index)		\
	((base) + cmn_child_ptr_offset(base) + ((index) << 3))
#define cmn_child_external(base, index)		\
	(__raw_readq(cmn_child_ptr(base, index)) & CMN_child_external)
#define cmn_child_address(base, index)		\
	CMN_child_address_offset(__raw_readq(cmn_child_ptr(base, index)))
#define cmn_child_node(base, index)		\
	(CMN_PERIPH_BASE + cmn_child_address(base, index))

#define cmn_child_node_id(base, index)		\
	cmn_node_id(cmn_child_node(base, index))
#define cmn_child_node_pid(base, index)		\
	cmn_node_pid(cmn_child_node_id(base, index))

#define cmn_periph_id(id)			\
	CMN_periph_id(id, __raw_readq(CMN_cfgm_periph_id(id)))
#define cmn_revision()				\
	((uint8_t)CMN_revision(cmn_periph_id(2)))
#define cmn_cal_supported()	(cmn_revision() >= CMN_r2p0)

#define cmn_mxp_device_type(base, pid)		\
	CMN_device_type(__raw_readq(CMN_mxp_device_port_connect_info(base, pid)))
#define cmn_mxp_datacheck(base, pid)		\
	(__raw_readq(CMN_mxp_p_info(base, pid)) & CMN_mxp_datacheck_en)

#define cmn_hnf_op_mode_status(base)		\
	CMN_ppu_op_mode_status(__raw_readq(CMN_hnf_ppu_pwsr(base)))

/* CXG capabilities */
#define cmn_ccix_request_credits()		\
	CMN_ra_request_tracker_depth(__raw_readq(CMN_cxg_ra_unit_info(CMN_CXRA_BASE)))
#define cmn_ccix_snoop_credits()		\
	CMN_ha_snoop_tracker_depth(__raw_readq(CMN_cxg_ha_unit_info(CMN_CXHA_BASE)))
#define cmn_ccix_data_credits()			\
	CMN_ha_wdb_depth(__raw_readq(CMN_cxg_ha_unit_info(CMN_CXHA_BASE)))
#define cmn_ccix_max_packet_size()		\
	CMN_LA_MAXPACKETSIZE(__raw_readq(CMN_cxla_ccix_prop_capabilities(CMN_CXLA_BASE)))
#define cmn_ccix_no_message_pack()		\
	(!!(__raw_readq(CMN_cxla_ccix_prop_capabilities(CMN_CXLA_BASE)) & CMN_la_nomessagepack))

/* RAS options */
#define cmn_ras_support_ed(base)		\
	(!!(CMN_errfr_ED(__raw_readq(CMN_errfr(base)))))
#define cmn_ras_enable_ed(base)			\
	__raw_setq(CMN_errctlr_ED, CMN_errctlr(base))
#define cmn_ras_disable_ed(base)		\
	__raw_clearq(CMN_errctlr_ED, CMN_errctlr(base))
#define cmn_ras_support_ed_NS(base)		\
	(!!(CMN_errfr_ED(__raw_readq(CMN_errfr_NS(base)))))
#define cmn_ras_enable_ed_NS(base)			\
	__raw_setq(CMN_errctlr_ED, CMN_errctlr_NS(base))
#define cmn_ras_disable_ed_NS(base)		\
	__raw_clearq(CMN_errctlr_ED, CMN_errctlr_NS(base))

#define cmn_ras_support_de(base)		\
	(!!(CMN_errfr_DE(__raw_readq(CMN_errfr(base)))))
#define cmn_ras_enable_de(base)			\
	__raw_setq(CMN_errctlr_DE, CMN_errctlr(base))
#define cmn_ras_disable_de(base)		\
	__raw_clearq(CMN_errctlr_DE, CMN_errctlr(base))
#define cmn_ras_support_de_NS(base)		\
	(!!(CMN_errfr_DE(__raw_readq(CMN_errfr_NS(base)))))
#define cmn_ras_enable_de_NS(base)			\
	__raw_setq(CMN_errctlr_DE, CMN_errctlr_NS(base))
#define cmn_ras_disable_de_NS(base)		\
	__raw_clearq(CMN_errctlr_DE, CMN_errctlr_NS(base))

#define cmn_ras_support_ui(base)		\
	(!!(CMN_errfr_UI(__raw_readq(CMN_errfr(base)))))
#define cmn_ras_enable_ui(base)			\
	__raw_setq(CMN_errctlr_UI, CMN_errctlr(base))
#define cmn_ras_disable_ui(base)		\
	__raw_clearq(CMN_errctlr_UI, CMN_errctlr(base))
#define cmn_ras_support_ui_NS(base)		\
	(!!(CMN_errfr_UI(__raw_readq(CMN_errfr_NS(base)))))
#define cmn_ras_enable_ui_NS(base)			\
	__raw_setq(CMN_errctlr_UI, CMN_errctlr_NS(base))
#define cmn_ras_disable_ui_NS(base)		\
	__raw_clearq(CMN_errctlr_UI, CMN_errctlr_NS(base))

#define cmn_ras_support_fi(base)		\
	(!!(CMN_errfr_FI(__raw_readq(CMN_errfr(base)))))
#define cmn_ras_enable_fi(base)			\
	__raw_setq(CMN_errctlr_FI, CMN_errctlr(base))
#define cmn_ras_disable_fi(base)		\
	__raw_clearq(CMN_errctlr_FI, CMN_errctlr(base))
#define cmn_ras_support_fi_NS(base)		\
	(!!(CMN_errfr_FI(__raw_readq(CMN_errfr_NS(base)))))
#define cmn_ras_enable_fi_NS(base)			\
	__raw_setq(CMN_errctlr_FI, CMN_errctlr_NS(base))
#define cmn_ras_disable_fi_NS(base)		\
	__raw_clearq(CMN_errctlr_FI, CMN_errctlr_NS(base))

#define cmn_ras_support_cfi(base)		\
	(!!(CMN_errfr_CFI(__raw_readq(CMN_errfr(base)))))
#define cmn_ras_enable_cfi(base)		\
	__raw_setq(CMN_errctlr_CFI, CMN_errctlr(base))
#define cmn_ras_disable_cfi(base)		\
	__raw_clearq(CMN_errctlr_CFI, CMN_errctlr(base))
#define cmn_ras_support_cfi_NS(base)		\
	(!!(CMN_errfr_CFI(__raw_readq(CMN_errfr_NS(base)))))
#define cmn_ras_enable_cfi_NS(base)		\
	__raw_setq(CMN_errctlr_CFI, CMN_errctlr_NS(base))
#define cmn_ras_disable_cfi_NS(base)		\
	__raw_clearq(CMN_errctlr_CFI, CMN_errctlr_NS(base))

#define CMN_ERRG_XP				0
#define CMN_ERRG_HNI				1
#define CMN_ERRG_HNF				2
#define CMN_ERRG_SBSX				3
#define CMN_ERRG_CXHA				4

/* Configuration interfaces */
#define CMN600_MEMORY_REGION_TYPE_IO			0
#define CMN600_MEMORY_REGION_TYPE_SYSCACHE		1
#define CMN600_REGION_TYPE_SYSCACHE_SUB			2
#define CMN600_REGION_TYPE_CCIX				3
#define CMN600_REGION_TYPE_SYSCACHE_NONHASH		4
#define CMN600_MEMORY_REGION_TYPE_SYSCACHE_SECONDARY	5

/* PMU performance events */
#define CMN_PMU_NO_EVENT			0
/* HN-F performance events */
#define CMN_PMU_HN_CACHE_MISS			1
#define CMN_PMU_HN_SLC_SF_CACHE_ACCESS		2
#define CMN_PMU_HN_CACHE_FILL			3
#define CMN_PMU_HN_POCQ_RETRY			4
#define CMN_PMU_HN_POCQ_REQS_RECVD		5
#define CMN_PMU_HN_SF_HIT			6
#define CMN_PMU_HN_SF_EVICTIONS			7
#define CMN_PMU_HN_DIR_SNOOPS_SENT		8
#define CMN_PMU_HN_BRD_SNOOPS_SENT		9
#define CMN_PMU_HN_SLC_EVICTION			10
#define CMN_PMU_HN_SLC_FILL_INVALID_WAY		11
#define CMN_PMU_HN_MC_RETRIES			12
#define CMN_PMU_HN_MC_REQS			13
#define CMN_PMU_HN_QOS_HH_RETRY			14
#define CMN_PMU_HN_POCQ_OCCUPANCY		15
#define CMN_PMU_HN_POCQ_ADDRHAZ			16
#define CMN_PMU_HN_POCQ_ATOMICS_ADDRHAZ		17
#define CMN_PMU_HN_LD_ST_SWP_ADQ_FULL		18
#define CMN_PMU_HN_CMP_ADQ_FULL			19
#define CMN_PMU_HN_TXDAT_STALL			20
#define CMN_PMU_HN_TXRSP_STALL			21
#define CMN_PMU_HN_SEQ_FULL			22
#define CMN_PMU_HN_SEQ_HIT			23
#define CMN_PMU_HN_SNP_SENT			24
#define CMN_PMU_HN_SFBI_DIR_SNP_SENT		25
#define CMN_PMU_HN_SFBI_BRD_SNP_SENT		26
#define CMN_PMU_HN_SNP_SENT_UNTRK		27
#define CMN_PMU_HN_INTV_DIRTY			28
#define CMN_PMU_HN_STASH_SNP_SENT		29
#define CMN_PMU_HN_STASH_DATA_PULL		30
#define CMN_PMU_HN_SNP_FWDED			31
/* HN-I performance events */
#define CMN_PMU_HN_RRT_RD_OCCUPANCY_CNT_OVFL	32
#define CMN_PMU_HN_RRT_WR_OCCUPANCY_CNT_OVFL	33
#define CMN_PMU_HN_RDT_RD_OCCUPANCY_CNT_OVFL	34
#define CMN_PMU_HN_RDT_WR_OCCUPANCY_CNT_OVFL	35
#define CMN_PMU_HN_WDB_OCCUPANCY_CNT_OVFL	36
#define CMN_PMU_HN_RRT_RD_ALLOCATION		37
#define CMN_PMU_HN_RRT_WR_ALLOCATION		38
#define CMN_PMU_HN_RDT_RD_ALLOCATION		39
#define CMN_PMU_HN_RDT_WR_ALLOCATION		40
#define CMN_PMU_HN_WDB_ALLOCATION		41
#define CMN_PMU_HN_TXRSPFLITV_RETIRIED		42
#define CMN_PMU_HN_ARVALID_NO_ARREADY		43
#define CMN_PMU_HN_ARREADY_NO_ARVALID		44
#define CMN_PMU_HN_AWVALID_NO_AWREADY		45
#define CMN_PMU_HN_AWREADY_NO_AWVALID		46
#define CMN_PMU_HN_WVALID_NO_WREADY		47
#define CMN_PMU_HN_TXDATFLITV_NO_LINKCRD	48
#define CMN_PMU_HN_NONPCIE_SERIALIZED		49
#define CMN_PMU_HN_PCIE_SERIALIZED		50
/* RN-I/RN-D performance events */
#define CMN_PMU_RN_RDATABEATS_P0		1
#define CMN_PMU_RN_RDATABEATS_P1		2
#define CMN_PMU_RN_RDATABEATS_P2		3
#define CMN_PMU_RN_RXDATFLITV			4
#define CMN_PMU_RN_TXDATFLITV			5
#define CMN_PMU_RN_TXREQFLITV			6
#define CMN_PMU_RN_TXREQFLITV_RETRIED		7
#define CMN_PMU_RN_RRT_OCCUPANCY		8
#define CMN_PMU_RN_WRT_OCCUPANCY		9
#define CMN_PMU_RN_TXREQFLITV_REPLAYED		10
#define CMN_PMU_RN_WRCANCEL_SENT		11
#define CMN_PMU_RN_WDATABEAT_P0			12
#define CMN_PMU_RN_WDATABEAT_P1			13
#define CMN_PMU_RN_WDATABEAT_P2			14
#define CMN_PMU_RN_RRTALLOC			15
#define CMN_PMU_RN_WRTALLOC			16
#define CMN_PMU_RN_RDB_UNORD			17
#define CMN_PMU_RN_RDB_REPLAY			18
#define CMN_PMU_RN_RDB_HYBRID			19
#define CMN_PMU_RN_RDB_ORD			20
/* XP performance events */
#define CMN_PMU_XP_PC_OFFSET			5
#define CMN_PMU_XP_PC_MASK			REG_3BIT_MASK
#define CMN_PMU_XP_PC(value)			_SET_FV(CMN_PMU_XP_PC, value)
#define CMN_PMU_XP_REQ				0x0
#define CMN_PMU_XP_RSP				0x1
#define CMN_PMU_XP_SNP				0x2
#define CMN_PMU_XP_DAT				0x3
#define CMN_PMU_XP_IF_OFFSET			2
#define CMN_PMU_XP_IF_MASK			REG_3BIT_MASK
#define CMN_PMU_XP_IF(value)			_SET_FV(CMN_PMU_XP_IF, value)
#define CMN_PMU_XP_EAST				0x0
#define CMN_PMU_XP_WEST				0x1
#define CMN_PMU_XP_NORTH			0x2
#define CMN_PMU_XP_SOUTH			0x3
#define CMN_PMU_XP_P0				0x04
#define CMN_PMU_XP_P1				0x05
#define CMN_PMU_XP_SP_OFFSET			0
#define CMN_PMU_XP_SP_MASK			REG_2BIT_MASK
#define CMN_PMU_XP_SP(value)			_SET_FV(CMN_PMU_XP_SP, value)
#define CMN_PMU_XP_TXFLIT_VALID			1
#define CMN_PMU_XP_TXFLIT_STALL			2
/* Partial DAT flit:
 * 1. signaled when 128-bit DAT flits could not be merged into a 256-bit DAT flit;
 * 2. only applicable on the DAT PC on RN-F CHIA and RN-F CHIA ESAM ports.
 */
#define CMN_PMU_XP_PARTIAL_DAT_FLIT_VALID	3
#define CMN_PMU_XP_EVENT(spec, intf, PC)	\
	(CMN_PMU_XP_SP(spec) |			\
	 CMN_PMU_XP_IF(intf) |			\
	 CMN_PMU_XP_PC(PC))
/* SBSX performance events */
#define CMN_PMU_SBSX_RXDAT			1
#define CMN_PMU_SBSX_TXDAT			2
#define CMN_PMU_SBSX_CMO			3
#define CMN_PMU_SBSX_TXRSPFLITV_RETIRED		4
#define CMN_PMU_SBSX_TXDATFLITV_TOTAL		5
#define CMN_PMU_SBSX_TXRSPFLITV_TOTAL		6
#define CMN_PMU_SBSX_RRT_RD_OCCUPANCY_CNT_OVFL	17
#define CMN_PMU_SBSX_RRT_WR_OCCUPANCY_CNT_OVFL	18
#define CMN_PMU_SBSX_RRT_CMO_OCCUPANCY_CNT_OVFL	19
#define CMN_PMU_SBSX_WDB_OCCUPANCY_CNT_OVFL	20
#define CMN_PMU_SBSX_RDT_RD_OCCUPANCY_CNT_OVFL	21
#define CMN_PMU_SBSX_RDT_CMO_OCCUPANCY_CNT_OVFL	22
#define CMN_PMU_SBSX_ARVALID_NO_ARREADY		33
#define CMN_PMU_SBSX_AWVALID_NO_AWREADY		34
#define CMN_PMU_SBSX_WVALID_NO_WREADY		35
#define CMN_PMU_SBSX_TXDATFLITV_NO_LINKCRD	36
#define CMN_PMU_SBSX_TXRSPFLITV_NO_LINKCRD	37
/* DN performance events */
#define CMN_PMU_DN_RXREQ_DVMOP			1
#define CMN_PMU_DN_RXREQ_DVMSYNC		2
#define CMN_PMU_DN_RXREQ_DVMOP_VMID_FILTERED	3
#define CMN_PMU_DN_RXREQ_RETRIED		4
#define CMN_PMU_DN_TRK_OCCUPANCY		5
/* CXHA performance events */
/* CXRA performance events */
/* CXLA performance events */
/* DEVEVENT field in trace flits */
#define CMN_DEVEVENT_LINE_MISS_NO_SNOOP		0
#define CMN_DEVEVENT_LINE_MISS_DIRECTED_SNOOP	1
#define CMN_DEVEVENT_LINE_MISS_BROADCAST_SNOOP	2
#define CMN_DEVEVENT_LINE_HIT_NO_SNOOP		3

#ifdef CONFIG_CMN600_DEBUG
void cmn_writeq(uint64_t v, caddr_t a, const char *n, int i);
#define cmn_setq(v,a,n,i)				\
	do {						\
		uint64_t __v = __raw_readq(a);		\
		__v |= (v);				\
		cmn_writeq(__v, (a), (n), (i));		\
	} while (0)
#define cmn_clearq(v,a,n,i)				\
	do {						\
		uint64_t __v = __raw_readq(a);		\
		__v &= ~(v);				\
		cmn_writeq(__v, (a), (n), (i));		\
	} while (0)
#define cmn_writeq_mask(v,m,a,n,i)			\
	do {						\
		uint64_t __v = __raw_readq(a);		\
		__v &= ~(m);				\
		__v |= (v);				\
		cmn_writeq(__v, (a), (n), (i));		\
	} while (0)
#else
#define cmn_debug_init()		do { } while (0)
#define cmn_writeq(v,a,n,i)		__raw_writeq(v,a)
#define cmn_setq(v,a,n,i)		__raw_setq(v,a)
#define cmn_clearq(v,a,n,i)		__raw_clearq(v,a)
#define cmn_writeq_mask(v,m,a,n,i)	__raw_writeq_mask(v,m,a)
#endif

#define CMN600_CHIP_LOCAL		CMN_MAX_CHIPS

struct cmn600_memregion {
	caddr_t base;
	uint64_t size;
	uint16_t type;
	cmn_nid_t node_id;
	cmn_id_t chip_id;
};

struct cmn600_ccix_ha_mmap {
	cmn_id_t chip_id;
	caddr_t base;
	uint64_t size;
};

extern caddr_t cmn_bases[NR_CMN_NODES];
extern cmn_nid_t cmn_xp_ids[CMN_MAX_MXP_COUNT];
extern cmn_nid_t cmn_hnf_ids[CMN_MAX_HNF_COUNT];
extern cmn_nid_t cmn_hni_ids[CMN_MAX_HNI_COUNT];
extern cmn_nid_t cmn_rni_ids[CMN_MAX_RNI_COUNT];
extern cmn_nid_t cmn_rnd_ids[CMN_MAX_RND_COUNT];
extern cmn_nid_t cmn_dtc_ids[CMN_MAX_DTC_COUNT];
extern cmn_nid_t cmn_sbsx_ids[CMN_MAX_SBSX_COUNT];
extern cmn_nid_t cmn_cxra_ids[CMN_MAX_CXG_COUNT];
extern cmn_nid_t cmn_cxla_ids[CMN_MAX_CXG_COUNT];
extern cmn_nid_t cmn_cxha_ids[CMN_MAX_CXG_COUNT];
extern cmn_nid_t cmn_rn_sam_ext_ids[CMN_MAX_RN_SAM_EXT_COUNT];
extern cmn_nid_t cmn_rn_sam_int_ids[CMN_MAX_RN_SAM_INT_COUNT];
extern cmn_id_t cmn_xp_count;
extern cmn_id_t cmn_hnf_count;
extern cmn_id_t cmn_hni_count;
extern cmn_id_t cmn_rnf_count;
extern cmn_id_t cmn_rni_count;
extern cmn_id_t cmn_rnd_count;
extern cmn_id_t cmn_dtc_count;
extern cmn_id_t cmn_sbsx_count;
extern cmn_id_t cmn_snf_count;
extern cmn_id_t cmn_cxla_count;
extern cmn_id_t cmn_cxra_count;
extern cmn_id_t cmn_cxha_count;
extern cmn_id_t cmn_rn_sam_int_count;
extern cmn_id_t cmn_rn_sam_ext_count;
extern bool cmn600_initialized;

#ifdef CONFIG_CONSOLE_VERBOSE
const char *cmn600_node_type_name(uint16_t node_type);
const char *cmn600_mem_region_name(uint8_t type);
const char *cmn600_revision_name(uint8_t revision);
#endif

void cmn600_init(void);
uint16_t cmn600_node_type(cmn_nid_t nid);
bool cmn600_rnsam_is_rnd(cmn_nid_t nid);
bool cmn600_rnsam_is_rni(cmn_nid_t nid);
bool cmn600_rnsam_is_rnf(cmn_nid_t nid);
bool cmn600_rnsam_is_cxha(cmn_nid_t nid);
cmn_nid_t cmn600_local_rnf_nid(cmn_id_t id);
void cmn600_configure_rn_sam_ext(cmn_nid_t nid);
cmn_id_t cmn600_max_tgt_nodes(void);
cmn_id_t cmn600_nid2xp(cmn_nid_t nid);
#ifdef CONFIG_CMN600_CML
void cmn600_cml_detect_mmap(void);
int cmn600_cml_get_config(void);
void cmn600_cml_set_config(void);
void cmn600_cml_enable_sf(void);
void cmn600_cml_enable_dvm(void);
uint64_t cmn600_cml_base(caddr_t base, cmn_id_t chip_id, bool ccix);
void cmn600_cml_init(void);
#else
#define cmn600_cml_detect_mmap()			do { } while (0)
#define cmn600_cml_get_config()				(-ENODEV)
#define cmn600_cml_set_config()				do { } while (0)
#define cmn600_cml_enable_sf()				do { } while (0)
#define cmn600_cml_enable_dvm()				do { } while (0)
#define cmn600_cml_base(base, chip_id, ccix)		base
#define cmn600_cml_init()				do { } while (0)
#endif

#ifdef CONFIG_CMN600_RAS
void cmn600_ras_config(caddr_t base);
void cmn600_ras_report(caddr_t base);
void cmn600_ras_init(void);
#else
#define cmn600_ras_config(nid)		do { } while (0)
#define cmn600_ras_report(nid)		do { } while (0)
#define cmn600_ras_init()		do { } while (0)
#endif

#ifndef ARCH_HAVE_SNF_TABLE
#define cmn600_hw_scg_count()				1
#define cmn600_hw_snf_count()				1
#define cmn600_hw_snf_hashed(hnf)			cmn_snf_table[0]
#endif

#ifndef ARCH_HAVE_DTC_DOMAIN
#define cmn600_hw_dtc_domain(nid)			0
#define CMN_MAX_DTCS					1
#endif

#endif /* __CMN600_H_INCLUDE__ */
