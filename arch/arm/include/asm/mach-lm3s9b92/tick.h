#ifndef __TICK_LM3S9B92_H_INCLUDE__
#define __TICK_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/mem.h>
#include <asm/mach/clk.h>
#include <asm/mach/vic.h>

#define STCTRL				CORTEXM3(0x010)
#define COUNT				16
#define CLK_SRC				12
#define INTEN				1
#define ENABLE				0

#define STRELOAD			CORTEXM3(0x014)
#define STCURRENT			CORTEXM3(0x018)

#define SYSTICK_HW_CONFIG		_BV(CLK_SRC)
#define SYSTICK_HW_MAX_COUNT		0x00FFFFFF
#define __SYSTICK_HW_FREQ		CLK_SYS

#define __systick_hw_disable_trap()	__raw_clearl_atomic(INTEN, STCTRL)
#define __systick_hw_enable_trap()	__raw_setl_atomic(INTEN, STCTRL)
#define __systick_hw_disable_ctrl()	\
	__raw_clearl((_BV(ENABLE) | SYSTICK_HW_CONFIG), STCTRL)
#define __systick_hw_enable_ctrl()	\
	__raw_setl((_BV(ENABLE) | SYSTICK_HW_CONFIG), STCTRL)
#define __systick_hw_write_reload(c)	\
	__raw_writel((c & SYSTICK_HW_MAX_COUNT), STRELOAD)
#define __systick_hw_write_current(c)	\
	__raw_writel((c & SYSTICK_HW_MAX_COUNT), STCURRENT)
#define __systick_hw_read_current()	\
	(__raw_readl(STCURRENT) & SYSTICK_HW_MAX_COUNT)

#endif /* __TICK_LM3S9B92_H_INCLUDE__ */
