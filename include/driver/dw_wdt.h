#ifndef __DW_WDT_H_INCLUDE__
#define __DW_WDT_H_INCLUDE__

#ifndef DW_WDT_BASE
#define DW_WDT_BASE(n, offset)		(DW_WDT##n##_BASE + (offset))
#endif
#ifndef DW_WDT_REG
#define DW_WDT_REG(n, offset)	(DW_WDT##n##_BASE + (offset))
#endif

#define WDT_CR(n)			DW_WDT_REG(n, 0x00)
#define WDT_TORR(n)			DW_WDT_REG(n, 0x04)
#define WDT_CCVR(n)			DW_WDT_REG(n, 0x08)
#define WDT_CRR(n)			DW_WDT_REG(n, 0x0C)
#define WDT_STAT(n)			DW_WDT_REG(n, 0x10)
#define WDT_EOI(n)			DW_WDT_REG(n, 0x14)
#define WDT_PROT_LEVEL(n)		DW_WDT_REG(n, 0X1C)
#define WDT_COMP_PARAM_5(n)		DW_WDT_REG(n, 0XE4)
#define WDT_COMP_PARAM_4(n)		DW_WDT_REG(n, 0XE8)
#define WDT_COMP_PARAM_3(n)		DW_WDT_REG(n, 0XEC)
#define WDT_COMP_PARAM_2(n)		DW_WDT_REG(n, 0XF0)
#define WDT_COMP_PARAM_1(n)		DW_WDT_REG(n, 0XF4)
#define WDT_COMP_VERSION(n)		DW_WDT_REG(n, 0XF8)
#define WDT_COMP_TYPE(n)		DW_WDT_REG(n, 0XFC)

#define WDT_ENABLE			(1 << 0)
#define WDT_RESET_ENABLE		(1 << 1)
#define WDT_RESTART_KEY			0x76

#define dw_wdt_write(offset, value)	\
	__raw_writel(value, offset)
#define dw_wdt_read(offset)		\
	__raw_readl(offset)

void dw_wdt_ctrl_init(int id, uint32_t timeout);
void dw_wdt_feed(int id);
int dw_wdt_has_timed_out(int id);
void dw_wdt_clear_timeout(int id);
void dw_wdt_disable(int id);

#endif /* __DW_WDT_H_INCLUDE__ */
