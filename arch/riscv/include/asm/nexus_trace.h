#ifndef __TRACE_NEXUS_H_INCLUDE__
#define __TRACE_NEXUS_H_INCLUDE__

#include <target/generic.h>
#include <target/arch.h>
#include <driver/coresight.h>

#define TRACE_BASE			__RMU_TRACE_SUB_PATH_1_BASE
#define TRACE_REG(base, offset)		(base + offset)

#define TRACE_FUNNEL(c)			TRACE_REG(TRACE_BASE, 0x10000 + c * 0x10000)
#define TRACE_TIMESTAMP(c)		TRACE_REG(TRACE_BASE, 0x10040 + c * 0x10000)
#define TRACE_ENCODER(c, n)		TRACE_REG(TRACE_BASE, 0x10000 + c * 0x10000 + 0x2000 + n * 0x2000)
#define TRACE_ATBBRIDGE(c, n)		TRACE_REG(TRACE_BASE, 0x10000 + c * 0x10000 + 0x3000 + n * 0x2000)

#define TYPE_FUNNEL			0
#define TYPE_TIMESTAMP			1
#define TYPE_ENCODER			2
#define TYPE_ATBBRIDGE			3

#define TRACE_CTL(base)			TRACE_REG(base, 0x000)
#define TRACE_IMPL(base)		TRACE_REG(base, 0x004)

/* TRACE_COMMON */
#define TCTL_ACTIVE			_BV(0)
#define TCTL_ENABLE			_BV(1)
#define TCTL_EMPTY			_BV(3)
#define TIMPL_VERMAJOR_OFFSET		0
#define TIMPL_VERMAJOR_MASK		REG_4BIT_MASK
#define TIMPL_VERMAJOR(value)		_GET_FV(TIMPL_VERMAJOR, value)
#define TIMPL_VERMINOR_OFFSET		4
#define TIMPL_VERMINOR_MASK		REG_4BIT_MASK
#define TIMPL_VERMINOR(value)		_GET_FV(TIMPL_VERMINOR, value)
#define TIMPL_COMPTYPE_OFFSET		8
#define TIMPL_COMPTYPE_MASK		REG_4BIT_MASK
#define TIMPL_COMPTYPE(value)		_GET_FV(TIMPL_COMPTYPE, value)

/* ======================================================================
 * Timestamp Registers
 * ====================================================================== */

/* TS_CTL */
#define TS_ACTIVE			_BV(0)
#define TS_COUNT			_BV(1)
#define TS_RESET			_BV(2)
#define TS_RUNINDEBUG			_BV(3)
#define TS_MODE_OFFSET			4
#define TS_MODE_MASK			REG_3BIT_MASK
#define TS_MODE(value)			_SET_FV(TS_MODE, value)
#define TS_MODE_NONE			0
#define TS_MODE_EXT			1
#define TS_MODE_INTSYS			2
#define TS_MODE_INTCORE			3
#define TS_MODE_SHARED			4
#define TS_PRESCALE_OFFSET		8
#define TS_PRESCALE_MASK		REG_2BIT_MASK
#define TS_PRESCALE(value)		_SET_FV(TS_PRESCALE, value)
#define TS_ENABLE			_BV(15)
#define TS_WIDTH_OFFSET			24
#define TS_WIDTH_MASK			REG_6BIT_MASK
#define TS_WIDTH(value)			_SET_FV(TS_WIDTH, value)

/* ======================================================================
 * Encoder Registers
 * ====================================================================== */

#define TENCODER_INSTFEAT(base)		TRACE_REG(base, 0x008)
#define TENCODER_INSTFILTER(base)	TRACE_REG(base, 0x00C)

/* TENCODER_INSTFEAT */
#define TEINST_NOADDRDIFF		_BV(0)
#define TEINST_NOTRAPADDR		_BV(1)
#define TEINST_ENSEQJMP			_BV(2)
#define TEINST_ENIMPRET			_BV(3)
#define TEINST_ENBRANCHPRE		_BV(4)
#define TEINST_ENJMPTGTCACHE		_BV(5)
#define TEINST_IMPRETMODE_OFFSET	6
#define TEINST_IMPRETMODE_MASK		REG_2BIT_MASK
#define TEINST_IMPRETMODE(value)	_SET_FV(TEINST_IMPRETMODE, value)
#define TEINST_ENRPTHISTORY		_BV(8)
#define TEINST_ENALLJMP			_BV(9)
#define TEINST_EXTADDRMSB		_BV(10)
#define TEINST_SRCID_OFFSET		16
#define TEINST_SRCID_MASK		REG_12BIT_MASK
#define TEINST_SRCID(value)		_SET_FV(TEINST_SRCID, value)
#define TEINST_SRCBIT_OFFSET		28
#define TEINST_SRCBIT_MASK		REG_4BIT_MASK
#define TEINST_SRCBIT(value)		_SET_FV(TEINST_SRCBIT, value)

/* ======================================================================
 * Funnel Registers
 * ====================================================================== */

extern uint32_t cluster_table[];
extern uint32_t cluster_to_id[];

#endif