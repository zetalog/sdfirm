#ifndef __IO_AVR_H_INCLUDE__
#define __IO_AVR_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

static inline void writeb(uint8_t v, volatile void __iomem *addr)
{
	*(volatile uint8_t __force *)addr = v;
}
static inline void writew(uint16_t v, volatile void __iomem *addr)
{
	*(volatile uint16_t __force *)addr = v;
}
static inline void writel(uint32_t v, volatile void __iomem *addr)
{
	*(volatile uint32_t __force *)addr = v;
}

static inline uint8_t readb(const volatile void __iomem *addr)
{
	return *(const volatile uint8_t __force *)addr;
}
static inline uint16_t readw(const volatile void __iomem *addr)
{
	return *(const volatile uint16_t __force *)addr;
}
static inline uint32_t readl(const volatile void __iomem *addr)
{
	return *(const volatile uint32_t __force *)addr;
}

#endif /* __IO_AVR_H_INCLUDE__ */
