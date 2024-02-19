#ifndef __CORELPC_H_INCLUDE__
#define __CORELPC_H_INCLUDE__

#include <target/arch.h>

#ifndef CORELPC_BASE
#define CORELPC_BASE			0x02010000
#endif
#ifndef CORELPC_REG
#define CORELPC_REG(offset)		(CORELPC_BASE + (offset))
#endif

/* Register Summary */
#define GCFG					CORELPC_REG(0x00)
#define STS					CORELPC_REG(0x04)
#define KIRQ					CORELPC_REG(0x08)
#define KOBR					CORELPC_REG(0x10)
#define KIBR					CORELPC_REG(0x14)
#define KADRL					CORELPC_REG(0x18)  \\0ca2
#define KADRH					CORELPC_REG(0x1c)
#define MIRQ					CORELPC_REG(0x34)

/* General Configuration Register */
#define IRQ_MDE					_BV(6)
#define IRQ_EN					_BV(5)
#define KEINTR					_BV(1)
#define KELPC					_BV(0)

/* KCS IRQ Configuration Register */
#define KIRQ_EN					_BV(0)

/* KCS Output Buffer Register */
/* KCS Input Buffer Register */
/* KCS Base Address Low Register */
#define KADRL_REG_OFFSET			7
#define KADRL_REG_MASK				REG_7BIT_MASK
#define KADRL_REG(value)			_SET_FV(KADRL_REG, value)

/* KCS Base Address High Register */
#define LADRH_REG_OFFSET			7
#define LADRH_REG_MASK				REG_8BIT_MASK
#define LADRH_REG(value)			_SET_FV(LADRH_REG, value)

/* APB Master Port IRQ Configuration Register */
#define INT_M_IRQ_SEL_OFFSET			7
#define INT_M_IRQ_SEL_MASK			REG_4BIT_MASK
#define INT_M_IRQ_SEL(value)			_SET_FV(INT_M_IRQ_SEL, value)
#define INT_M_IRQ_SEL_EN			_BV(0)

#define SYNC_PSLVERR				0xa0
#define SYNC_PREADY_SHORT			0x05
#define SYNC_PREADY_LONG			0x06
#define SYNC_PREADY_RESET			0x00

#define corelpc_write_reg(v, a)			__raw_writel((v), (a))
#define corelpc_read_reg(a)			__raw_readl((a))
#define corelpc_get_cd()			(!!(__raw_readl(STS) & KCS_CD))

extern uint16_t corelpc_kadr;

#define KCS_REG(offset)				(corelpc_kadr + (offset))

void corelpc_config_kadr(uint16_t kadr);

#define corelpc_read_kibr()			corelpc_read_reg(KIBR)
#define corelpc_write_kobr(dat)			corelpc_write_reg(dat, KOBR)
#define corelpc_write_state(state)		__raw_writel_mask(state, KCS_STATE_MASK, STS)
#define corelpc_clear_status(status)		__raw_clearl(status, STS)
#define corelpc_write_obf(obf)			__raw_writel(KCS_OBF | STS, (obf))

void corelpc_init(void);



#endif /* __CORELPC_H_INCLUDE__ */