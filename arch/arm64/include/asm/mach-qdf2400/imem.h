/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
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
 * @(#)imem.h: internal memory (imem) interfaces
 * $Id: imem.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __QDF2400_IMEM_H_INCLUDE__
#define __QDF2400_IMEM_H_INCLUDE__

#include <target/generic.h>
#include <target/irq.h>
#include <target/time.h>

#define SYS_IMEM_BASE				ULL(0xFF0C500000)
#define SYS_IMEM_IMEM_IMEM_CSRS_REG_BASE	(SYS_IMEM_BASE + 0x00000000)
#define MSG_IMEM_BASE				ULL(0xFF0C600000)
#define MSG_IMEM_IMEM_IMEM_CSRS_REG_BASE	(MSG_IMEM_BASE + 0x00000000)
#define IMC_BASE				ULL(0xFF6F800000)
#define RAM_0_IMEM_IMEM_IMEM_CSRS_REG_BASE	(IMC_BASE + 0x00500000)
#define RAM_1_IMEM_IMEM_IMEM_CSRS_REG_BASE	(IMC_BASE + 0x00540000)
#define RAM_2_IMEM_IMEM_IMEM_CSRS_REG_BASE	(IMC_BASE + 0x00580000)
#define RAM_3_IMEM_IMEM_IMEM_CSRS_REG_BASE	(IMC_BASE + 0x005c0000)
#define RAM_4_IMEM_IMEM_IMEM_CSRS_REG_BASE	(IMC_BASE + 0x00600000)
#define RAM_5_IMEM_IMEM_IMEM_CSRS_REG_BASE	(IMC_BASE + 0x00640000)

/* imem can be one of the followings:
 * SYS_IMEM:   2.43. SYS_imem_dec
 * MSG_IMEM:   2.22. MSG_imem_dec
 * RAM_0_IMEM: 2.16.1.10. ram_0
 * RAM_1_IMEM: 2.16.1.11. ram_1
 * RAM_2_IMEM: 2.16.1.12. ram_2
 * RAM_3_IMEM: 2.16.1.13. ram_3
 * RAM_4_IMEM: 2.16.1.14. ram_4
 * RAM_5_IMEM: 2.16.1.15. ram_5
 */
#define IMEM_REG(imem, offset)		\
	(imem##_IMEM_IMEM_CSRS_REG_BASE + (offset))

#define IMEM_HW_ID(imem)		IMEM_REG(imem, 0x0000)
#define IMEM_HW_VERSION(imem)		IMEM_REG(imem, 0x0004)
#define IMEM_ERR_STATUS(imem)		IMEM_REG(imem, 0x0008)
#define IMEM_ERR_CLR(imem)		IMEM_REG(imem, 0x000C)
#define IMEM_ERR_CTRL(imem)		IMEM_REG(imem, 0x0010)
#define IMEM_INT0_CTRL(imem)		IMEM_REG(imem, 0x0014)
#define IMEM_INT1_CTRL(imem)		IMEM_REG(imem, 0x0018)
#define IMEM_ERR_INJ(imem)		IMEM_REG(imem, 0x001C)
#define IMEM_ERR_CAP_CNTL(imem)		IMEM_REG(imem, 0x0020)
#define IMEM_ERR_CAP_0(imem)		IMEM_REG(imem, 0x0024)
#define IMEM_ERR_CAP_1(imem)		IMEM_REG(imem, 0x0028)
#define IMEM_ERR_CAP_2(imem)		IMEM_REG(imem, 0x002C)
#define IMEM_ERR_CAP_3(imem)		IMEM_REG(imem, 0x0030)
#define IMEM_ERR_CAP_4(imem)		IMEM_REG(imem, 0x0034)
#define IMEM_SCRUB_STATUS(imem)		IMEM_REG(imem, 0x0038)
#define IMEM_SCRUB_CTRL(imem)		IMEM_REG(imem, 0x003C)
#define IMEM_SCRUB_ADDR_RD_0(imem)	IMEM_REG(imem, 0x0040)
#define IMEM_SCRUB_ADDR_RD_1(imem)	IMEM_REG(imem, 0x0044)
#define IMEM_SCRUB_ADDR_WR_0(imem)	IMEM_REG(imem, 0x0048)
#define IMEM_SCRUB_ADDR_WR_1(imem)	IMEM_REG(imem, 0x004C)
#define IMEM_SCRUB_DATA_WR_0(imem)	IMEM_REG(imem, 0x0050)
#define IMEM_SCRUB_DATA_WR_1(imem)	IMEM_REG(imem, 0x0054)
#define IMEM_CONFIG(imem)		IMEM_REG(imem, 0x0058)
#define IMEM_AXI_BASE_ADDR_OFFSET(imem)	IMEM_REG(imem, 0x005C)
#define IMEM_CLK_ON_CFG(imem)		IMEM_REG(imem, 0x0064)
#define IMEM_PSBCGC_TIMERS(imem)	IMEM_REG(imem, 0x006C)
#define IMEM_PSBCGC_CONTROL(imem)	IMEM_REG(imem, 0x0070)
#define IMEM_ERR_COUNT(imem)		IMEM_REG(imem, 0x0074)

/* x.1.3.3. ERR_STATUS (0x0FF XXXX 0008)
 * x.1.3.4. ERR_CLR (0x0FF XXXX 000C)
 * x.1.3.5. ERR_CTRL (0x0FF XXXX 0010)
 * x.1.3.6. INT0_CTRL (0x0FF XXXX 0014)
 * x.1.3.7. INT1_CTRL (0x0FF XXXX 0018)
 * x.1.3.8. ERR_INJ (0x0FF XXXX 001C)
 */
#define IMEM_QSB_ADDR_DECODE_ERR	_BV(9)
#define IMEM_ERR_POISON_SCRUB		_BV(8)
#define IMEM_ECC_UE_SCRUB		_BV(7)
#define IMEM_ECC_CE_SCRUB		_BV(6)
#define IMEM_ECC_POISON_RMW		_BV(5)
#define IMEM_ECC_UE_RMW			_BV(4)
#define IMEM_ECC_CE_RMW			_BV(3)
#define IMEM_ECC_POISON_QSB_RD		_BV(2)
#define IMEM_ECC_UE_QSB_RD		_BV(1)
#define IMEM_ECC_CE_QSB_RD		_BV(0)
#define IMEM_ERR_ALL			\
	(IMEM_QSB_ADDR_DECODE_ERR |	\
	 IMEM_ERR_POISON_SCRUB |	\
	 IMEM_ECC_UE_SCRUB |		\
	 IMEM_ECC_CE_SCRUB |		\
	 IMEM_ECC_UE_RMW |		\
	 IMEM_ECC_CE_RMW |		\
	 IMEM_ECC_POISON_QSB_RD |	\
	 IMEM_ECC_UE_QSB_RD |		\
	 IMEM_ECC_CE_QSB_RD)
#define IMEM_ERR_NONE			0

/* x.1.3.9. ERR_CAP_CNTL (0x0FF XXXX 0020) */
#define IMEM_CLEAR_UE_COUNT		_BV(4)
#define IMEM_CLEAR_CE_COUNT		_BV(3)
#define IMEM_CONTINUOUS_CAPTURE		_BV(2)
#define IMEM_CAPTURE_FIRST_ERROR	0
#define IMEM_CAPTURE_CONTINUOUS		IMEM_CONTINUOUS_CAPTURE
#define IMEM_CLEAR_ERR_CAPTURED		_BV(1)
#define IMEM_ERR_CAPTURED		_BV(0)

/* x.1.3.10. ERR_CAP_0 (0x0FF XXXX 0024)
 * x.1.3.11. ERR_CAP_1 (0x0FF XXXX 0028)
 *
 * 1. CAP_0
 * 0                                                               31
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | ERR_ADDR_LO                                                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 2. CAP_1
 * 0                       12      16              24              31
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | ERR_ADDR_HI           |       | SYNDROME_FIRST| SYNDROME_LAST |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define IMEM_ERR_ADDR_BITS		48
#define IMEM_ERR_ADDR_HI_OFFSET		0
#define IMEM_ERR_ADDR_HI_MASK		0xFFF
#define IMEM_ERR_ADDR_HI(value)		_GET_FV(IMEM_ERR_ADDR_HI, value)

#define IMEM_ERR_SYNDROME_FIRST_OFFSET	16
#define IMEM_ERR_SYNDROME_FIRST_MASK	0xFF
#define IMEM_ERR_SYNDROME_FIRST(value)	_GET_FV(IMEM_ERR_SYNDROME_FIRST, value)
#define IMEM_ERR_SYNDROME_LAST_OFFSET	24
#define IMEM_ERR_SYNDROME_LAST_MASK	0xFF
#define IMEM_ERR_SYNDROME_LAST(value)	_GET_FV(IMEM_ERR_SYNDROME_LAST, value)

/* Encoded syndromes */
#define IMEM_ERR_NO_ERR			0x00
#define IMEM_ERR_ECC_CE_READ		0x01
#define IMEM_ERR_ECC_UE_READ		0x02
#define IMEM_ERR_ECC_POISON_READ	0x03
#define IMEM_ERR_ECC_CE_WRITE		0x04
#define IMEM_ERR_ECC_UE_WRITE		0x05
#define IMEM_ERR_ECC_POISON_WRITE	0x06
#define IMEM_ERR_ECC_CE_SCRUB		0x07
#define IMEM_ERR_ECC_UE_SCRUB		0x08
#define IMEM_ERR_ECC_POISION_SCRUB	0x09
#define IMEM_ERR_QSB_ADDR_DECODE_ERR	0x0A
#define IMEM_SYNDROME_NUM		(IMEM_ERR_QSB_ADDR_DECODE_ERR + 1)

/* x.1.3.12. ERR_CAP_2 (0x0FF XXXX 002C)
 * 3. CAP_2
 * 0               8         13    16        2122        2728      31
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | MID           | PID     | BID | TID     |X| ALEN    |W|B|     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * X: AEXCLUSIVE
 * W: AWRITE
 * B: ABURST
 */
#define IMEM_ERR_MID_OFFSET		0
#define IMEM_ERR_MID_MASK		0xFF
#define IMEM_ERR_MID(value)		_GET_FV(IMEM_ERR_MID, value)
#define IMEM_ERR_PID_OFFSET		8
#define IMEM_ERR_PID_MASK		0x1F
#define IMEM_ERR_PID(value)		_GET_FV(IMEM_ERR_PID, value)
#define IMEM_ERR_BID_OFFSET		13
#define IMEM_ERR_BID_MASK		0x07
#define IMEM_ERR_BID(value)		_GET_FV(IMEM_ERR_BID, value)
#define IMEM_ERR_TID_OFFSET		16
#define IMEM_ERR_TID_MASK		0x1F
#define IMEM_ERR_TID(value)		_GET_FV(IMEM_ERR_TID, value)
#define IMEM_ERR_AEXCLUSIVE		_BV(21)
#define IMEM_ERR_ALEN_OFFSET		22
#define IMEM_ERR_ALEN_MASK		0x1F
#define IMEM_ERR_ALEN(value)		_GET_FV(IMEM_ERR_ALEN, value)
#define IMEM_ERR_AWRITE			_BV(27)
#define IMEM_ERR_ABURST			_BV(28)

/* x.1.3.13. ERR_CAP_3 (0x0FF XXXX 0030)
 * 4. CAP_3
 * 0               8 9 10              18                          31
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | BEAT_INDEX    |N|M| VMID          |                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * N: NSPROT
 * M: MSA
 */
#define IMEM_ERR_BEAT_INDEX_OFFSET	0
#define IMEM_ERR_BEAT_INDEX_MASK	0xFF
#define IMEM_ERR_BEAT_INDEX(value)	_GET_FV(IMEM_BEAT_INDEX, value)
#define IMEM_ERR_NSPROT			_BV(8)
#define IMEM_ERR_MSA			_BV(9)
#define IMEM_ERR_VMID_OFFSET		10
#define IMEM_ERR_VMID_MASK		0xFF
#define IMEM_ERR_VMID(value)		_GET_FV(IMEM_VMID, value)

/* x.1.3.14. ERR_CAP_4 (0x0FF XXXX 0034)
 * 5. CAP_4
 * 0               8               16              24              31
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | ECC_SYNDROME  | ECC_GEN_BITS  | ECC_CHK_BITS  |S|             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * S: SCRUB_REQ
 */
#define IMEM_ERR_ECC_SYNDROME_OFFSET	0
#define IMEM_ERR_ECC_SYNDROME_MASK	0xFF
#define IMEM_ERR_ECC_SYNDROME(value)	_GET_FV(IMEM_ERR_ECC_SYNDROME, value)
#define IMEM_ERR_ECC_GEN_BITS_OFFSET	8
#define IMEM_ERR_ECC_GEN_BITS_MASK	0xFF
#define IMEM_ERR_ECC_GEN_BITS(value)	_GET_FV(IMEM_ERR_ECC_GEN_BITS, value)
#define IMEM_ERR_ECC_CHK_BITS_OFFSET	16
#define IMEM_ERR_ECC_CHK_BITS_MASK	0xFF
#define IMEM_ERR_ECC_CHK_BITS(value)	_GET_FV(IMEM_ERR_ECC_CHK_BITS, value)
#define IMEM_ERR_SCRUB_REQ		_BV(24)

/* x.1.3.28. ERR_COUNT (0x0FF XXXX 0074) */
#define IMEM_ERR_UE_COUNT_OFFSET	16
#define IMEM_ERR_UE_COUNT_MASK		0xFFFF
#define IMEM_ERR_UE_COUNT(value)	_GET_FV(IMEM_ERR_UE_COUNT, value)
#define IMEM_ERR_CE_COUNT_OFFSET	0
#define IMEM_ERR_CE_COUNT_MASK		0xFFFF
#define IMEM_ERR_CE_COUNT(value)	_GET_FV(IMEM_ERR_CE_COUNT, value)

struct imem_err {
	int imem_type;
	uint32_t hw_version;
	uint32_t status;
	uint64_t address;
	uint32_t syndrome;
	uint32_t cap0;
	uint32_t cap1;
	uint32_t cap2;
	uint32_t cap3;
	uint32_t cap4;
};

#define IMEM_TYPE_RAM_0_IMEM	0
#define IMEM_TYPE_RAM_1_IMEM	1
#define IMEM_TYPE_RAM_2_IMEM	2
#define IMEM_TYPE_RAM_3_IMEM	3
#define IMEM_TYPE_RAM_4_IMEM	4
#define IMEM_TYPE_RAM_5_IMEM	5
#define IMEM_TYPE_SYS_IMEM	6
#define IMEM_TYPE_MSG_IMEM	7
#define IMEM_TYPE_NUM		(IMEM_TYPE_MSG_IMEM + 1)

#define __imem_clear_err(name, err)	\
	__raw_writel((err), IMEM_ERR_CLR(name))
#define __imem_mask_err(name, err)	\
	__raw_setl((err), IMEM_ERR_CTRL(name))
#define __imem_unmask_err(name, err)	\
	__raw_clearl((err), IMEM_ERR_CTRL(name))
#define __imem_inject_err(name, err)	\
	__raw_setl((err), IMEM_ERR_INJ(name))
#define __imem_enable_int0(name, err)	\
	__raw_setl(err, IMEM_INT0_CTRL(name))
#define __imem_disable_int0(name, err)	\
	__raw_clearl(err, IMEM_INT0_CTRL(name))
#define __imem_enable_int1(name, err)	\
	__raw_setl(err, IMEM_INT1_CTRL(name))
#define __imem_disable_int1(name, err)	\
	__raw_clearl(err, IMEM_INT1_CTRL(name))
#define __imem_config_err_cap(name, continuous)			\
	__raw_writel(((continuous ? IMEM_CAPTURE_CONTINUOUS :	\
				    IMEM_CAPTURE_FIRST_ERROR) |	\
		      IMEM_CLEAR_UE_COUNT |			\
		      IMEM_CLEAR_CE_COUNT |			\
		      IMEM_CLEAR_ERR_CAPTURED),			\
		     IMEM_ERR_CAP_CNTL(name))
#define __imem_clear_err_cap(name)				\
	__raw_setl(IMEM_CLEAR_ERR_CAPTURED,			\
		   IMEM_ERR_CAP_CNTL(name))
#define __imem_config_err(name, int0, continuous, handler)	\
	do {							\
		irq_t imem_irq = IRQ_##name;			\
		__raw_writel(0, IMEM_ERR_CTRL(name));		\
		if (int0)					\
			__imem_enable_int0(name, IMEM_ERR_ALL);	\
		else						\
			__imem_enable_int1(name, IMEM_ERR_ALL);	\
		__imem_config_err_cap(name, continuous);	\
		irqc_configure_irq(imem_irq, 0,			\
				   IRQ_EDGE_TRIGGERED);		\
		irq_register_vector(imem_irq, handler);	\
		irqc_enable_irq(imem_irq);			\
	} while (0)
#define __imem_err_address(cap0, cap1)				\
	(((uint64_t)(cap0)) |					\
	 ((uint64_t)IMEM_ERR_ADDR_HI(cap1)) << 32)
#define __imem_handle_err(name, err)				\
	do {							\
		(err)->imem_type = IMEM_TYPE_##name;		\
		(err)->status =					\
			__raw_readl(IMEM_ERR_STATUS(name));	\
		(err)->hw_version =				\
			__raw_readl(IMEM_HW_VERSION(name));	\
		(err)->cap0 = __raw_readl(IMEM_ERR_CAP_0(name));\
		(err)->cap1 = __raw_readl(IMEM_ERR_CAP_1(name));\
		(err)->cap2 = __raw_readl(IMEM_ERR_CAP_2(name));\
		(err)->cap3 = __raw_readl(IMEM_ERR_CAP_3(name));\
		(err)->cap4 = __raw_readl(IMEM_ERR_CAP_4(name));\
		(err)->address =				\
			__imem_err_address((err)->cap0,		\
					   (err)->cap1);	\
		(err)->syndrome =				\
			IMEM_ERR_SYNDROME_FIRST((err)->cap1);	\
		imem_print_err(err);				\
		__imem_clear_err(name, IMEM_ERR_ALL);		\
		__imem_clear_err_cap(name);			\
	} while (0)

void imem_print_err(struct imem_err *err);

#define QDF2400_IMEM(sym, name)					\
static inline void sym##_inject_err(uint32_t err, uint64_t addr)\
{								\
	__imem_inject_err(name, err);				\
}								\
static inline void sym##_handle_err(irq_t irq)			\
{								\
	struct imem_err e;					\
	__imem_handle_err(name, &e);				\
}								\
static inline void sym##_init(void)				\
{								\
	__imem_config_err(name, true, false, sym##_handle_err);	\
}

#endif /* __QDF2400_IMEM_H_INCLUDE__ */
