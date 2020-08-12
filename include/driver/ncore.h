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
 * @(#)ncore.h: Arteris Ncore interconnect interface
 * $Id: ncore.h,v 1.0 2020-08-07 12:20:00 zhenglv Exp $
 */

#ifndef __NCORE_NOC_H_INCLUDE__
#define __NCORE_NOC_H_INCLUDE__

#include <target/generic.h>

/* Structure units */
#define NCORE_REG(offset)		(NCORE_BASE + (offset))
#define NCORE_SU(n)			(NCORE_BASE + ((n) << 12))
#define NCORE_SU_CAIU			0
#define NCORE_SU_NCBU			3
#define NCORE_SU_DIRU			4
#define NCORE_SU_CMIU			6
#define ncore_su_t2i(type)		((type) << 5)
#define ncore_su_i2t(id)		((id) >> 5)
#define ncore_su_caiu(n)		(ncore_su_t2i(NCORE_SU_CAIU) + (n))
#define ncore_su_ncbu(n)		(ncore_su_t2i(NCORE_SU_NCBU) + (n))
#define ncore_su_diru(n)		(ncore_su_t2i(NCORE_SU_DIRU) + (n))
#define ncore_su_cmiu(n)		(ncore_su_t2i(NCORE_SU_CMIU) + (n))
#define NCORE_MAX_SUS			224
#define NCORE_CAIU(n)			NCORE_SU(ncore_su_t2i(NCORE_SU_CAIU) + (n))
#define NCORE_NCBU(n)			NCORE_SU(ncore_su_t2i(NCORE_SU_NCBU) + (n))
#define NCORE_DIRU(n)			NCORE_SU(ncore_su_t2i(NCORE_SU_DIRU) + (n))
#define NCORE_CMIU(n)			NCORE_SU(ncore_su_t2i(NCORE_SU_CMIU) + (n))
#define NCORE_CS			NCORE_REG(0xFF000)

#define SU_REG(n, offset)		(NCORE_SU(n) + (offset))
#define CS_REG(offset)			(NCORE_CS + (offset))

/* ===========================================================================
 * NCORE Common Register and Fields
 * =========================================================================== */
#define SUTTR(n)			SU_REG(n, 0x008)
#define SUCECR(n)			SU_REG(n, 0x100)
#define SUCESR(n)			SU_REG(n, 0x104)
#define SUCELR0(n)			SU_REG(n, 0x108)
#define SUCELR1(n)			SU_REG(n, 0x10C)
#define SUCESAR(n)			SU_REG(n, 0x124)
#define SUUECR(n)			SU_REG(n, 0x140)
#define SUUESR(n)			SU_REG(n, 0x144)
#define SUUELR0(n)			SU_REG(n, 0x148)
#define SUUELR1(n)			SU_REG(n, 0x14C)
#define SUUESAR(n)			SU_REG(n, 0x164)
#define SUIDR(n)			SU_REG(n, 0xFFC)

/* CAIUCECR Correctable Error Control Register
 * CAIUUECR Uncorrectable Error Control Register
 * NCBUCECR Correctable Error Control Register
 * NCBUUECR Uncorrectable Error Control Register
 * DIRUCECR Correctable Error Control Register
 * DIRUUECR Uncorrectable Error Control Register
 */
#define SU_ErrDetEn			_BV(0)
#define SU_ErrIntEn			_BV(1)
#define SU_ErrThreshold_OFFSET		4
#define SU_ErrThreshold_MASK		REG_8BIT_MASK
#define SU_ErrThreshold(value)		_SET_FV(SU_ErrThreshold, value)
/* CAIUCESR Correctable Error Status Register
 * CAIUCESAR Correctable Error Status Alias Register
 * CAIUUESR Uncorrectable Error Status Register
 * CAIUUESAR Uncorrectable Error Status Alias Register
 * NCBUCESR Correctable Error Status Register
 * NCBUCESAR Correctable Error Status Alias Register
 * NCBUUESR Uncorrectable Error Status Register
 * NCBUUESAR Uncorrectable Error Status Alias Register
 * DIRUCESR Correctable Error Status Register
 * DIRUCESAR Correctable Error Status Alias Register
 * DIRUUESR Uncorrectable Error Status Register
 * DIRUUESAR Uncorrectable Error Status Alias Register
 */
#define SU_ErrVld			_BV(0)
#define SU_ErrOvf			_BV(1)
#define SU_ErrCount_OFFSET		4
#define SU_ErrCount_MASK		REG_8BIT_MASK
#define SU_ErrCount(value)		_GET_FV(SU_ErrCount, value)
#define SU_ErrType_OFFSET		12
#define SU_ErrType_MASK			REG_4BIT_MASK
#define SU_ErrType(value)		_GET_FV(SU_ErrType, value)
#define SU_ErrInfo_OFFSET		16
#define SU_ErrInfo_MASK			REG_8BIT_MASK
#define SU_ErrInfo(value)		_GET_FV(SU_ErrInfo, value)
/* CAIUCELR0 Correctable Error Location Register
 * CAIUUELR0 Uncorrectable Error Location Register
 * NCBUCELR0 Correctable Error Location Register
 * NCBUUELR0 Uncorrectable Error Location Register
 * DIRUCELR0 Correctable Error Location Register
 * DIRUUELR0 Uncorrectable Error Location Register
 */
#define SU_ErrEntry_OFFSET		0
#define SU_ErrEntry_MASK		REG_20BIT_MASK
#define SU_ErrEntry(value)		_GET_FV(CAIU_ErrEntry, value)
#define SU_ErrWay_OFFSET		20
#define SU_ErrWay_MASK			REG_6BIT_MASK
#define SU_ErrWay(value)		_GET_FV(SU_ErrWay, value)
#define SU_ErrWord_OFFSET		26
#define SU_ErrWord_MASK			REG_6BIT_MASK
#define SU_ErrWord(value)		_GET_FV(SU_ErrWord, value)
/* CAIUCELR1 Correctable Error Location Register
 * CAIUUELR1 Uncorrectable Error Location Register
 * NCBUCELR1 Correctable Error Location Register
 * NCBUUELR1 Uncorrectable Error Location Register
 * DIRUCELR1 Correctable Error Location Register
 * DIRUUELR1 Uncorrectable Error Location Register
 */
#define SU_ErrAddr_OFFSET		0
#define SU_ErrAddr_MASK			REG_12BIT_MASK
#define SU_ErrAddr(value)		_GET_FV(SU_ErrAddr, value)

/* ===========================================================================
 * NCBUPC/DIRUSF Common Registers and Fields
 * =========================================================================== */
#define SUMCR(n)			SU_REG(n, 0x080)
#define SUMAR(n)			SU_REG(n, 0x084)
#define SUMLR0(n)			SU_REG(n, 0x088)
#define SUMLR1(n)			SU_REG(n, 0x08C)

/* NCBUPCMCR Proxy Cache Maintenance Control Register
 * DIRUSFMCR Snoop Filter Maintenance Control Register
 * CMIUCMCMCR Coherent Memory Cache Maintenance Control Register
 */
#define SU_MntOp_OFFSET			0
#define SU_MntOp_MASK			REG_4BIT_MASK
#define SU_MntOp(value)			_SET_FV(SU_MntOp, value)
#define SU_MntOp_InitAll		0x0
#define SU_MntOp_FlushAll		0x4
#define SU_MntOp_FlushSetWay		0x5
#define SU_MntOp_FlushAddr		0x6
#define SU_MntId_OFFSET			16
#define SU_MntId_MASK			REG_5BIT_MASK
#define SU_MntId(value)			_SET_FV(SU_MntId, value)
#define SU_MntSecAttr			_BV(21)
#define DIRU_MntOp_FlushAllVictim	0x8
/* Identification varies from different Us */
#define NCBU_MntArrId			_BV(16)
#define CMIU_MntArrId			_BV(16)
/* NCBUPCMAR Proxy Cache Maintenance Activity Register
 * DIRUSFMAR Snoop Filter Maintenance Activity Register
 * CMIUCMCMAR Coherent Memory Cache Maintenace Activity Register
 */
#define SU_MntOpActv			_BV(0)
/* NCBUPCMLR0 Proxy Cache Maintenance Location Register
 * DIRUSFMLR0 Snoop Filter Maintenance Location Register
 * CMIUCMCMLR0 Coherent Memory Cache Maintenance Location Register
 */
#define SU_MntSet_OFFSET		0
#define SU_MntSet_MASK			REG_20BIT_MASK
#define SU_MntSet(value)		_SET_FV(SU_MntSet, value)
#define SU_MntWay_OFFSET		20
#define SU_MntWay_MASK			REG_6BIT_MASK
#define SU_MntWay(value)		_GET_FV(SU_MntWay, value)
#define SU_MntWord_OFFSET		26
#define SU_MntWord_MASK			REG_6BIT_MASK
#define SU_MntWord(value)		_GET_FV(SU_MntWord, value)
/* NCBUPCMLR1 Proxy Cache Maintenance Location Register
 * DIRUSFMLR1 Snoop Filter Maintenance Location Register
 * CMIUCMCMLR1 Coherent Memory Cache Maintenance Location Register
 */
#define SU_MntAddr_OFFSET		0
#define SU_MntAddr_MASK			REG_12BIT_MASK
#define SU_MntAddr(value)		_GET_FV(SU_MntAddr, value)

/* CAIUIDR Identification Register
 * NCBUIDR Identification Register
 * DIRUIDR Identification Register
 * CMIUIDR Identification Register
 */
#define SU_ImplVer_OFFSET		0
#define SU_ImplVer_MASK			REG_8BIT_MASK
#define SU_ImplVer(value)		_GET_FV(SU_ImplVer, value)

/* ===========================================================================
 * CAIU Specific Registers and Fields
 * =========================================================================== */
/* CAIU Coherent Agent Interface Unit */
#define CAIUTCR(n)			SU_REG(0x000)
#define CAIUTAR(n)			SU_REG(0x004)

/* CAIUTCR Transaction Control Register */
#define CAIU_TransEn			_BV(0)
#define CAIU_IsolEn			_BV(1)
/* CAIUTAR Transaction Activity Register */
#define CAIU_TransActv			_BV(0)
#define CAIU_SnpActv			_BV(1)
#define CAIU_CohActv			_BV(2)
/* CAIUIDR Identification Register */
#define CAIU_CaiId_OFFSET		8
#define CAIU_CaiId_MASK			REG_7BIT_MASK
#define CAIU_CaiId(value)		_GET_FV(CAIU_CaiId, value)
#define CAIU_Ca				_BV(15)
#define CAIU_Type_OFFSET		16
#define CAIU_Type_MASK			REG_4BIT_MASK
#define CAIU_Type(value)		_GET_FV(CAIU_Type, value)
#define CAIU_Type_CAI_ACE_DVM		0
#define CAIU_Type_CAI_ACELite_DVM	1
#define CAIU_Type_CAI_ACE		2
#define CAIU_Type_CAI_ACELite		3
#define CAIU_SfId_OFFSET		20
#define CAIU_SfId_MASK			REG_5BIT_MASK
#define CAIU_SfId(value)		_GET_FV(CAIU_SfId, value)

/* NCBU Non-coherent Bridge Unit */
#define NCBUTCR(n)			SU_REG(n, 0x000)
#define NCBUTAR(n)			SU_REG(n, 0x004)
#define NCBUPCTCR(n)			SU_REG(n, 0x010)
#define NCBUPCTAR(n)			SU_REG(n, 0x014)
#define NCBUPCIDR(n)			SU_REG(n, 0xFF8)

/* NCBUTCR Transaction Control Register */
#define NCBU_TransEn			_BV(0)
/* NCBUTAR Transaction Activity Register */
#define NCBU_TransActv			_BV(0)
#define NCBU_SnpActv			_BV(1)
#define NCBU_CohActv			_BV(2)
/* NCBUPCTCR Proxy Cache Maintenance Control Register */
#define NCBUPC_LookupEn			_BV(0)
#define NCBUPC_FillEn			_BV(1)
#define NCBUPC_AllocPtlRdMiss		_BV(8)
#define NCBUPC_AllocPtlWrMiss		_BV(9)
#define NCBUPC_AllocFullRdMiss		_BV(10)
#define NCBUPC_AllocFullWrMiss		_BV(11)
/* NCBUPCTAR Proxy Cache Transaction Activity Register */
#define NCBUPC_EvictActv		_BV(0)
#define NCBUPC_FillActv			_BV(1)
/* NCBUPCIDR Proxy Cache Identification Register */
#define NCBUPC_NumSets_OFFSET		0
#define NCBUPC_NumSets_MASK		REG_20BIT_MASK
#define NCBUPC_NumSets(value)		_GET_FV(NCBUPC_NumSets, value)
#define NCBUPC_NumWays_OFFSET		20
#define NCBUPC_NumWays_MASK		REG_6BIT_MASK
#define NCBUPC_NumWays(value)		_GET_FV(NCBUPC_NumWays, value)
#define NCBUPC_Type_OFFSET		26
#define NCBUPC_Type_MASK		REG_3BIT_MASK
#define NCBUPC_Type(value)		_GET_FV(NCBUPC_Type, value)
#define NCBUPC_Type_UNIMPL		0x0
#define NCBUPC_Type_MSI			0x1
#define NCBUPC_Type_MEI			0x2
/* NCBUIDR Identification Register */
#define NCBU_NcbId_OFFSET		8
#define NCBU_NcbId_MASK			REG_7BIT_MASK
#define NCBU_NcbId(value)		_GET_FV(NCBU_NcbId, value)
#define NCBU_Ca				_BV(15)
#define NCBU_Type_OFFSET		16
#define NCBU_Type_MASK			REG_4BIT_MASK
#define NCBU_Type(value)		_GET_FV(NCBU_Type, value)
#define NCBU_TYPE_NCB			3
#define NCBU_SfId_OFFSET		20
#define NCBU_SfId_MASK			REG_5BIT_MASK
#define NCBU_SfId(value)		_GET_FV(NCBU_SfId, value)

/* DIRU Directory Unit */
#define DIRUTCR(n)			SU_REG(n, 0x000)
#define DIRUTAR(n)			SU_REG(n, 0x004)
#define DIRUSFER(n)			REG_1BIT_ADDR(SU_REG(n, 0x010), 0)
#define DIRUCASER(n, b)			REG_1BIT_ADDR(SU_REG(n, 0x040), (b))
#define DIRUCASAR(n, b)			REG_1BIT_ADDR(SU_REG(n, 0x050), (b))
#define DIRUMRHER(n)			REG_1BIT_ADDR(SU_REG(n, 0x070), 0)

/* DIRUTAR Transaction Activity Register */
#define DIRU_TransActv			_BV(0)

/* CMIU Coherent Memory Interface Unit */
#define CMIUTCR(n)			SU_REG(n, 0x000)
#define CMIUTAR(n)			SU_REG(n, 0x004)
#define CMIUCMCTCR(n)			SU_REG(n, 0x010)
#define CMIUCMCTAR(n)			SU_REG(n, 0x014)
#define CMIUCMCIDR(n)			SU_REG(n, 0xFF8)

/* CMIUTCR Transaction Control Register */
#define CMIU_TransActv			_BV(0)

/* CMIUCMCTCR Coherent Memory Cache Transaction Control Register */
#define CMIUCMC_LookupEn		_BV(0)
#define CMIUCMC_FillEn			_BV(1)

/* CMIUCMCTAR Coherent Memory Cache Transaction Activity Register */
#define CMIUCMC_EvictActv		_BV(0)
#define CMIUCMC_FillActv		_BV(1)

/* CMIUCMCIDR Coherent Memory Cache Identification Register */
#define CMIUCMC_NumSets_OFFSET		0
#define CMIUCMC_NumSets_MASK		REG_20BIT_MASK
#define CMIUCMC_NumSets(value)		_GET_FV(CMIUCMC_NumSets, value)
#define CMIUCMC_NumWays_OFFSET		20
#define CMIUCMC_NumWays_MASK		REG_6BIT_MASK
#define CMIUCMC_NumWays(value)		_GET_FV(CMIUCMC_NumWays, value)
#define CMIUCMC_Type_OFFSET		26
#define CMIUCMC_Type_MASK		REG_3BIT_MASK
#define CMIUCMC_Type(value)		_GET_FV(CMIUCMC_Type, value)
#define CMIUCMC_Type_UNIMPL		0x0
#define CMIUCMC_Type_HINT		0x1
#define CMIUCMC_Type_VICTIM		0x2
/* CMIUIDR Identification Register */
#define CMIU_CmiId_OFFSET		8
#define CMIU_CmiId_MASK			REG_4BIT_MASK
#define CMIU_CmiId(value)		_GET_FV(CMIU_CmiId, value)
#define CMIU_HntCap			_BV(13)
#define CMIU_Cmc			_BV(31)

/* Coherent Subsystem Register */
#define CSADSER(n)			REG_1BIT_ADDR(CS_REG(0x040), (n))
#define CSADSAR(n)			REG_1BIT_ADDR(CS_REG(0x050), (n))
#define CSCEISR(n)			REG_1BIT_ADDR(CS_REG(0x100), (n))
#define CSUEISR(n)			REG_1BIT_ADDR(CS_REG(0x140), (n))
#define CSSFIDR(n)			CS_REG(0xF00 + ((n) << 2))
#define CSUIDR				CS_REG(0xFF8)
#define CSIDR				CS_REG(0xFFC)

/* CSSFIDR0-31 Snoop FIlter Identification Register */
#define CS_NumSets_OFFSET		0
#define CS_NumSets_MASK			REG_20BIT_MASK
#define CS_NumSets(value)		_GET_FV(CS_NumSets, value)
#define CS_NumWays_OFFSET		20
#define CS_NumWays_MASK			REG_6BIT_MASK
#define CS_NumWays(value)		_GET_FV(CS_NumWays, value)
#define CS_Type_OFFSET			26
#define CS_Type_MASK			REG_3BIT_MASK
#define CS_Type(value)			_GET_FV(CS_Type, value)
#define CS_Type_UNIMPL			0
#define CS_Type_NULL			1
#define CS_Type_TAG_PRESENCE		2
#define CS_Type_TAG_OWNER_SHARER	3
/* CSUIDR Unit Identification Register */
#define CS_NumCaius_OFFSET		0
#define CS_NumCaius_MASK		REG_7BIT_MASK
#define CS_NumCaius(value)		_GET_FV(CS_NumCaius, value)
#define CS_NumNcbus_OFFSET		8
#define CS_NumNcbus_MASK		REG_6BIT_MASK
#define CS_NumNcbus(value)		_GET_FV(CS_NumNcbus, value)
#define CS_NumDirus_OFFSET		16
#define CS_NumDirus_MASK		REG_6BIT_MASK
#define CS_NumDirus(value)		_GET_FV(CS_NumDirus, value)
#define CS_NumCmius_OFFSET		24
#define CS_NumCmius_MASK		REG_6BIT_MASK
#define CS_NumCmius(value)		_GET_FV(CS_NumCmius, value)
/* CSIDR Identification Register */
#define CS_RelVer_OFFSET		0
#define CS_RelVer_MASK			REG_8BIT_MASK
#define CS_RelVer(value)		_GET_FV(CS_RelVer, value)
#define CS_DirClOffset_OFFSET		8
#define CS_DirClOffset_MASK		REG_3BIT_MASK
#define CS_DirClOffset(value)		_GET_FV(CS_DirClOffset, value)
#define CS_NumSfs_OFFSET		18
#define CS_NumSfs_MASK			REG_5BIT_MASK
#define CS_NumSfs(value)		_GET_FV(CS_NumSfs, value)

#define NCORE_SU_ID(su)			REG_1BIT_OFFSET(su)

/* CSIDR */
#define ncore_release_version()		CS_RelVer(__raw_readl(CSIDR))
#define ncore_dir_cacheline_offset()	CS_DirClOffset(__raw_readl(CSIDR))
#define ncore_num_sfs()			CS_NumSfs(__raw_readl(CSIDR))

/* CSUIDR */
#define ncore_num_caius()		CS_NumCaius(__raw_readl(CSUIDR))
#define ncore_num_ncbus()		CS_NumNcbus(__raw_readl(CSUIDR))
#define ncore_num_dirus()		CS_NumDirus(__raw_readl(CSUIDR))
#define ncore_num_cmius()		CS_NumCmius(__raw_readl(CSUIDR))

/* CSSFIDR */
#define ncore_sf_type(n)		CS_Type(__raw_readl(CSSFIDR(n)))
#define ncore_sf_num_sets(n)		CS_NumSets(__raw_readl(CSSFIDR(n)))
#define ncore_sf_num_ways(n)		CS_NumWays(__raw_readl(CSSFIDR(n)))

/* CSADSER/CSADSAR */
#define ncore_dvm_enable(su)				\
	do {						\
		uint32_t sid = NCORE_SU_ID(su);		\
		__raw_setl(_BV(sid), CSADSER(su));	\
	} while (0)
#define ncore_dvm_disable(su)				\
	do {						\
		uint32_t sid = NCORE_SU_ID(su);		\
		__raw_clearl(_BV(sid), CSADSER(su));	\
	} while (0)
#define ncore_dvm_active(su)		(__raw_readl(CSADSAR(su)) & _BV(NCORE_SU_ID(su)))

/* CSCEISR/CSUEISR */
#define ncore_ce_irq_status(su)		(__raw_readl(CSCEISR(su)) & _BV(NCORE_SU_ID(su)))
#define ncore_ue_irq_status(su)		(__raw_readl(CSUEISR(su)) & _BV(NCORE_SU_ID(su)))

/* SUIDR */
#define ncore_su_impl_ver(su)		SU_ImplVer(__raw_readl(SUIDR(su)))
#define ncore_caiu_id(su)		CAIU_CaiId(__raw_readl(SUIDR(su)))
#define ncore_caiu_type(su)		CAIU_Type(__raw_readl(SUIDR(su)))
#define ncore_caiu_ca(su)		(!!(__raw_readl(SUIDR(su)) & CAIU_Ca))
#define ncore_caiu_sf_id(su)		CAIU_SfId(__raw_readl(SUIDR(su)))
#define ncore_ncbu_id(su)		NCBU_NcbId(__raw_readl(SUIDR(su)))
#define ncore_ncbu_type(su)		NCBU_Type(__raw_readl(SUIDR(su)))
#define ncore_ncbu_ca(su)		(!!(__raw_readl(SUIDR(su)) & NCBU_Ca))
#define ncore_ncbu_sf_id(su)		NCBU_SfId(__raw_readl(SUIDR(su)))
#define ncore_cmiu_id(su)		CMIU_CmiId(__raw_readl(SUIDR(su)))
#define ncore_cmiu_hnt_cap(su)		(!!(__raw_readl(SUIDR(su)) & CMIU_HntCap))
#define ncore_cmiu_cmc(su)		(!!(__raw_readl(SUIDR(su)) & CMIU_Cmc))

/* NCBUPCIDR/CMIUCMCIDR */
#define ncore_ncbu_pc_type(su)		NCBUPC_Type(__raw_readl(NCBUPCIDR(su)))
#define ncore_ncbu_pc_num_sets(su)	NCBUPC_NumSets(__raw_readl(NCBUPCIDR(su)))
#define ncore_ncbu_pc_num_ways(su)	NCBUPC_NumWays(__raw_readl(NCBUPCIDR(su)))
#define ncore_cmiu_cmc_type(su)		CMIUCMC_Type(__raw_readl(CMIUCMCIDR(su)))
#define ncore_cmiu_cmc_num_sets(su)	CMIUCMC_NumSets(__raw_readl(CMIUCMCIDR(su)))
#define ncore_cmiu_cmc_num_ways(su)	CMIUCMC_NumWays(__raw_readl(CMIUCMCIDR(su)))

/* SUMCR/SUMAR/SUMLR */
#define ncore_su_mnt_init_all(su, id)		\
	__raw_writel(SU_MntOp(SU_MntOp_InitAll) | SU_MntId(id), SUMCR(su))
#define ncore_su_mnt_flush_all(su, id)		\
	__raw_writel(SU_MntOp(SU_MntOp_FlushAll) | SU_MntId(id), SUMCR(su))
#define ncore_su_mnt_wait_active(su)		\
	while (!(__raw_readl(SUMAR(su)) & SU_MntOpActv))

/* DIRUSFER */
#define ncore_diru_enable_sf(diru, su)				\
	do {							\
		uint32_t sid = NCORE_SU_ID(su);			\
		__raw_setl(_BV(sid),				\
			   DIRUSFER(ncore_su_diru(diru)));	\
	} while (0)
#define ncore_diru_enable_sfs(diru, sus)			\
	__raw_writel((su_mask), DIRUSFER(ncore_su_diru(diru)))
/* DIRUCASER */
#define ncore_diru_enable_cas(diru, su)				\
	do {							\
		uint32_t sid = NCORE_SU_ID(su);			\
		__raw_setl(_BV(sid),				\
			   DIRUCASER(ncore_su_diru(diru), su));	\
	} while (0)
#define ncore_diru_enable_cas_group(diru, casg, su_mask)	\
	__raw_writel((su_mask), DIRUCASER(ncore_su_diru(diru), (casg)))
/* DIRUCASAR */
#define ncore_diru_cas_active(diru, su)				\
	(__raw_readl(DIRUCASAR(ncore_su_diru(diru), su)) & _BV(NCORE_SU_ID(su)))

#ifdef CONFIG_NCORE
void ncore_init(uint8_t ncais, uint8_t nncbs, uint8_t ndirs, uint8_t ncmis);
#else
#define ncore_init(ncais, nncbs, ndirs, ncmis)	do { } while (0)
#endif

#endif /* __NCORE_NOC_H_INCLUDE__ */
