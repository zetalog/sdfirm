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

/* The mach layer should provide the followings:
 * 1. CMN_PERIPH_BASE: base address of CFGM node
 * 2. CMN_MESH_DIMEN_X: X dimension
 * 3. CMN_MESH_DIMEN_Y: Y dimension
 * 4. CMN_HND_NID: NID of HN_D
 * 5. CMN_MAX_NODES: maximum mesh nodes
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

/* NID and RNP (root node pointer) encodings */
#define CMN_NID_DeviceID_OFFFSET	0
#define CMN_NID_DeviceID_MASK		REG_2BIT_MASK
#define CMN_DID(nid)			_GET_FV(CMN_NID_DeviceID, nid)
#define CMN_NID_Port_OFFSET		2
#define CMN_NID_Port_MASK		REG_1BIT_MASK
#define CMN_PID(nid)			_GET_FV(CMN_NID_Port, nid)
#define CMN_ROOT_NODE_POINTER_PID_OFFSET	0
#define CMN_ROOT_NODE_POINTER_PID_MASK		REG_2BIT_MASK
#define CMN_ROOT_NODE_POINTER_PID(pid)		_SET_FV(CMN_ROOT_NODE_POINTER_PID, pid)
#if CMN_MESH_DIMEN_X > 4 || CMN_MESH_DIMEN_Y > 4
#define CMN_NID_BITS			9
#define CMN_NID_MASK			REG_9BIT_MASK
#define CMN_NID_Y_OFFSET		3
#define CMN_NID_Y_MASK			REG_3BIT_MASK
#define CMN_Y(nid)			_GET_FV(CMN_NID_Y, nid)
#define CMN_NID_X_OFFSET		6
#define CMN_NID_X_MASK			REG_3BIT_MASK
#define CMN_X(nid)			_GET_FV(CMN_NID_X, nid)
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

typedef uint8_t cmn_id_t;
#define NR_CMN_NODES			CMN_MAX_NODES
#define CMN_CFGM_ID			0
#define CMN_INVAL_ID			CMN_MAX_NODES

#define CMN_CFGM_BASE			(cmn_bases[CMN_CFGM_ID])
#define CMN_HNF_BASE(id)		(cmn_bases[id])
#define CMN_RND_BASE(id)		(cmn_bases[id])
#define CMN_RNI_BASE(id)		(cmn_bases[id])
#define CMN_RN_SAM_INT_BASE(id)		(cmn_bases[id])
#define CMN_RN_SAM_EXT_BASE(id)		(cmn_bases[id])
#define CMN_CXRA_BASE			(cmn_bases[cmn_cxra_id])
#define CMN_CXLA_BASE			(cmn_bases[cmn_cxla_id])
#define CMN_CXHA_BASE			(cmn_bases[cmn_cxha_id])

#define __CMN_HNF_BASE(i)		(cmn_bases[cmn_hnf_ids[i]])
#define __CMN_RND_BASE(i)		(cmn_bases[cmn_rnd_ids[i]])
#define __CMN_RNI_BASE(i)		(cmn_bases[cmn_rni_ids[i]])
#define __CMN_RN_SAM_INT_BASE(i)	(cmn_bases[cmn_rn_sam_int_ids[i]])
#define __CMN_RN_SAM_EXT_BASE(i)	(cmn_bases[cmn_rn_sam_ext_ids[i]])

#define CMN_12BIT_INDEX(n)		REG64_16BIT_INDEX(n)
#define CMN_12BIT_OFFSET(n)		\
	(((((uint64_t)(n)) & ULL( 3)) << 4) -	\
	 ((((uint64_t)(n)) & ULL( 3)) << 2))

#define CMN_REG(base, offset)		((base) + (offset))
#define CMN_CFGM_REG(offset)		CMN_REG(CMN_CFGM_BASE, offset)
#define CMN_12BIT_REG(base, offset, n)	\
	CMN_REG((base), (offset) + CMN_12BIT_INDEX(n))
#define CMN_32BIT_REG(base, offset, n)	\
	CMN_REG((base), (offset) + REG64_32BIT_INDEX(n))

/* Common to all nodes */
#define CMN_node_info(base)		CMN_REG(base, 0x0)
#define CMN_child_info(base)		CMN_REG(base, 0x80)

/* 3.2.1 Configuration master register summery */
#define CMN_cfgm_periph_id(n)		CMN_32BIT_REG(CMN_CFGM_BASE, 0x8, (n))
#define CMN_cfgm_component_id(n)	CMN_32BIT_REG(CMN_CFGM_BASE, 0x28, (n))

/* 3.2.4 HN-F register summery */
#define CMN_hnf_unit_info(base)		CMN_REG(base, 0x900)
#define CMN_hnf_cfg_ctl(base)		CMN_REG(base, 0xA00)
#define CMN_hnf_aux_ctl(base)		CMN_REG(base, 0xA08)
#define CMN_hnf_r2_aux_ctl(base)	CMN_REG(base, 0xA10)
#define CMN_hnf_ppu_pwpr(base)		CMN_REG(base, 0x1000)
#define CMN_hnf_ppu_pwsr(base)		CMN_REG(base, 0x1008)
#define CMN_hnf_ppu_misr(base)		CMN_REG(base, 0x1014)
#define CMN_hnf_ppu_idr0(base)		CMN_REG(base, 0x1FB0)
#define CMN_hnf_ppu_idr1(base)		CMN_REG(base, 0x1FB4)
#define CMN_hnf_ppu_iidr(base)		CMN_REG(base, 0x1FC8)
#define CMN_hnf_ppu_aidr(base)		CMN_REG(base, 0x1FCC)
#define CMN_hnf_ppu_dyn_ret_threshold(base)		\
					CMN_REG(base, 0x1100)
#define CMN_hnf_rn_starvation(base)	CMN_REG(base, 0xA90)
#define CMN_hnf_slc_lock_ways(base)	CMN_REG(base, 0xC00)
#define CMN_hnf_slc_lock_base(base, n)	CMN_REG(base, 0xC08 + ((n) << 3))
#define CMN_hnf_rni_region_vec(base)	CMN_REG(base, 0xC30)
#define CMN_hnf_rnf_region_vec(base)	CMN_REG(base, 0xC38)
#define CMN_hnf_rnd_region_vec(base)	CMN_REG(base, 0xC40)
#define CMN_hnf_slcway_partition_rnf_vec(base, n)	\
					CMN_REG(base, 0xC48 + ((n) << 3))
#define CMN_hnf_rnf_region_vec1(base)	CMN_REG(base, 0xC28)
#define CMN_hnf_slcway_partition_rnf_vec1(base, n)	\
					CMN_REG(base, 0xCB0 + ((n) << 3))
#define CMN_hnf_slcway_partition_rni_vec(base, n)	\
					CMN_REG(base, 0xC68 + ((n) << 3))
#define CMN_hnf_slcway_partition_rnd_vec(base, n)	\
					CMN_REG(base, 0xC88 + ((n) << 3))
#define CMN_hnf_rn_region_lock(base)	CMN_REG(base, 0xCA8)
#define CMN_hnf_sam_control(base)	CMN_REG(base, 0xD00)
#define CMN_hnf_sam_memregion(base, n)	CMN_REG(base, 0xD08 + ((n) << 3))
#define CMN_hnf_sam_sn_properties(base)	CMN_REG(base, 0xD18)
#define CMN_hnf_sam_6sn_nodeid(base)	CMN_REG(base, 0xD20)

/* 3.2.6 XP register summery */
#define CMN_mxp_device_port_connect_info(base, n)	\
					CMN_REG(base, 0x8 + ((n) << 3))
#define CMN_mxp_mesh_port_connect_info_east(base)	\
					CMN_REG(base, 0x18)
#define CMN_mxp_mesh_port_connect_info_north(base)	\
					CMN_REG(base, 0x20)
#define CMN_mxp_p0_info(base)		CMN_REG(base, 0x900)
#define CMN_mxp_p1_info(base)		CMN_REG(base, 0x908)
#define CMN_mxp_aux_ctl(base)		CMN_REG(base, 0xA00)

/* 3.2.9 RN SAM register summery */
#define CMN_rnsam_status(base)		CMN_REG(base, 0xC00)
#define CMN_rnsam_non_hash_mem_region(base, n)		\
					CMN_32BIT_REG(base, 0xC08, n)
#define CMN_rnsam_non_hash_tgt_nodeid(base, n)		\
					CMN_12BIT_REG(base, 0xC30, n)
#define CMN_rnsam_sys_cache_grp_region(base, n)		\
					CMN_32BIT_REG(base, 0xC48, n)
#define CMN_rnsam_sys_cache_grp_hn_nodeid(base, n)		\
					CMN_12BIT_REG(base, 0xC68, n)
#define CMN_rnsam_sys_cache_grp_nonhash_nodeid(base)	\
					CMN_REG(base, 0xC98)
#define CMN_rnsam_sys_cache_group_hn_count(base)	\
					CMN_REG(base, 0xD00)

typedef uint16_t cmn_nid_t;
typedef uint32_t cmn_lid_t;
typedef uint8_t cmn_pid_t;
typedef uint8_t cmn_did_t;

/* CMN_node_info */
#define CMN_node_type_OFFSET		0
#define CMN_node_type_MASK		REG_16BIT_MASK
#define CMN_node_type(value)		_GET_FV(CMN_node_type, value)
#define CMN_node_id_OFFSET		16
#define CMN_node_id_MASK		REG_16BIT_MASK
#define CMN_node_id(value)		_GET_FV(CMN_node_id, value)
#define CMN_logical_id_OFFSET		32
#define CMN_logical_id_MASK		REG_16BIT_MASK
#define CMN_logical_id(value)		_GET_FV_ULL(CMN_logical_id, value)

/* CMN_child_info */
#define CMN_child_count_OFFSET		0
#define CMN_child_count_MASK		REG_16BIT_MASK
#define CMN_child_count(value)		_GET_FV(CMN_child_count, value)
#define CMN_child_ptr_offset_OFFSET	16
#define CMN_child_ptr_offset_MASK	REG_16BIT_MASK
#define CMN_child_ptr_offset(value)	_GET_FV(CMN_child_ptr_offset, value)

/* 2.5.3 Child pointers */
#define CMN_child_external		_BV(31)
#define CMN_child_address_offset_OFFSET		0
#define CMN_child_address_offset_MASK		REG_28BIT_MASK
#define CMN_child_address_offset(value)		_GET_FV(CMN_child_address_offset, value)
#define CMN_child_node_pointer_OFFSET		14
#define CMN_child_node_pointer_MASK		REG_14BIT_MASK
#define CMN_child_node_pointer(value)		_GET_FV(CMN_child_node_pointer, value)
#define CMN_child_register_offset_OFFSET	0
#define CMN_child_register_offset_MASK		REG_14BIT_MASK
#define CMN_child_register_offset(value)	_GET_FV(CMN_child_register_offset, value)

/* CMN_mxp_device_port_connect_info */
#define CMN_device_type_OFFSET		0
#define CMN_device_type_MASK		REG_5BIT_MASK
#define CMN_device_type(device_port_connect_info)	\
	_GET_FV(CMN_device_type, device_port_connect_info)

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
#define cmn_child_ptr(base, index)		\
	((base) + cmn_child_ptr_offset(base) + ((index) << 3))
#define cmn_node_x(base)	CMN_X(cmn_node_id(base))
#define cmn_node_y(base)	CMN_Y(cmn_node_id(base))
#define cmn_node_pid(base)	CMN_PID(cmn_node_id(base))
#define cmn_node_did(base)	CMN_DID(cmn_node_id(base))

#define cmn_child_external(base, index)		\
	(cmn_child_ptr(base, index) & CMN_child_external)
#define cmn_child_address(base, index)		\
	CMN_child_address_offset(__raw_readq(cmn_child_ptr(base, index)))
#define cmn_child_node(base, index)		\
	(CMN_PERIPH_BASE + cmn_child_ptr(base, index))

#define cmn_child_node_id(base, index)		\
	cmn_node_id(cmn_child_node(base, index))
#define cmn_child_node_pid(base, index)		\
	cmn_node_pid(cmn_child_node_id(base, index))

#define cmn_mxp_device_type(base, pid)		\
	CMN_device_type(CMN_mxp_device_port_connect_info(base, pid))


extern caddr_t cmn600_bases[];
extern cmn_nid_t cmn_cxra_id;
extern cmn_nid_t cmn_cxla_id;
extern cmn_nid_t cmn_cxha_id;
extern bool cmn600_initialized;

void cmn600_init(void);

#endif /* __CMN600_H_INCLUDE__ */
