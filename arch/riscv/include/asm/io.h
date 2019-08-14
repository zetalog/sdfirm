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
 * @(#)io.h: RISCV specific input/output interface
 * $Id: io.h,v 1.1 2019-08-14 11:06:00 zhenglv Exp $
 */

#ifndef __IO_RISCV_H_INCLUDE__
#define __IO_RISCV_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>
#include <asm/mach/io.h>

#ifndef __ASSEMBLY__
static inline void __raw_writeb(uint8_t v, caddr_t a)
{
	asm volatile("sb %0, 0(%1)" : : "r"(v), "r"(a));
}
static inline void __raw_writew(uint16_t v, caddr_t a)
{
	asm volatile("sh %0, 0(%1)" : : "r" (v), "r" (a));
}
static inline void __raw_writel(uint32_t v, caddr_t a)
{
	asm volatile("sw %0, 0(%1)" : : "r" (v), "r" (a));
}
#ifdef CONFIG_64BIT
static inline void __raw_writeq(uint64_t v, caddr_t a)
{
	asm volatile("sd %0, 0(%1)" : : "r" (v), "r" (a));
}
#else
#define __raw_writeq(v,a)	(*(volatile uint64_t *)(caddr_t)(a) = (v))
#endif

static inline uint8_t __raw_readb(const caddr_t a)
{
	uint8_t v;

	asm volatile("lb %0, 0(%1)" : "=r" (v) : "r" (a));
	return v;
}
static inline uint16_t __raw_readw(const caddr_t a)
{
	uint16_t v;

	asm volatile("lh %0, 0(%1)" : "=r" (v) : "r" (a));
	return v;
}
static inline uint32_t __raw_readl(const caddr_t a)
{
	uint32_t v;

	asm volatile("lw %0, 0(%1)" : "=r" (v) : "r" (a));
	return v;
}
#ifdef CONFIG_64BIT
static inline uint64_t __raw_readq(const caddr_t a)
{
	uint64_t v;

	asm volatile("ld %0, 0(%1)" : "=r" (v) : "r" (a));
	return v;
}
#else
#define __raw_readq(a)		(*(volatile uint64_t *)(caddr_t)(a))
#endif

/* XXX: Atomic Register Access
 *
 * Be aware that no atomic assurance is made for the following macros.
 * Please use __raw_xxxbwl_atomic versions for atomic register access.
 */
#define __raw_setb(v,a)					\
	do {						\
		uint8_t __v = __raw_readb(a);		\
		__v |= (v);				\
		__raw_writeb(__v, (a));			\
	} while (0)
#define __raw_setw(v, a)				\
	do {						\
		uint16_t __v = __raw_readw(a);		\
		__v |= (v);				\
		__raw_writew(__v, (a));			\
	} while (0)
#define __raw_setl(v,a)					\
	do {						\
		uint32_t __v = __raw_readl(a);		\
		__v |= (v);				\
		__raw_writel(__v, (a));			\
	} while (0)
#define __raw_clearb(v,a)				\
	do {						\
		uint8_t __v = __raw_readb(a);		\
		__v &= ~(v);				\
		__raw_writeb(__v, (a));			\
	} while(0)
#define __raw_clearw(v,a)				\
	do {						\
		uint16_t __v = __raw_readw(a);		\
		__v &= ~(v);				\
		__raw_writew(__v, (a));			\
	} while(0)
#define __raw_clearl(v,a)				\
	do {						\
		uint32_t __v = __raw_readl(a);		\
		__v &= ~(v);				\
		__raw_writel(__v, (a));			\
	} while (0)
#define __raw_writeb_mask(v,m,a)			\
	do {						\
		uint8_t __v = __raw_readb(a);		\
		__v &= ~(m);				\
		__v |= (v);				\
		__raw_writeb(__v, (a));			\
	} while (0)
#define __raw_writew_mask(v,m,a)			\
	do {						\
		uint16_t __v = __raw_readw(a);		\
		__v &= ~(m);				\
		__v |= (v);				\
		__raw_writew(__v, (a));			\
	} while (0)
#define __raw_writel_mask(v,m,a)			\
	do {						\
		uint32_t __v = __raw_readl(a);		\
		__v &= ~(m);				\
		__v |= (v);				\
		__raw_writel(__v, (a));			\
	} while (0)
#define __raw_testb(v, a)				\
	(__raw_readb(a) & (v))
#define __raw_testw(v, a)				\
	(__raw_readw(a) & (v))
#define __raw_testl(v, a)				\
	(__raw_readl(a) & (v))

#ifndef ARCH_HAVE_IO_ATOMIC
#define __raw_setb_atomic(b, a)				\
	do {						\
		uint8_t __v = __raw_readb(a);		\
		__v |= _BV(b);				\
		__raw_writeb(__v, (a));			\
	} while (0)
#define __raw_setw_atomic(b, a)				\
	do {						\
		uint16_t __v = __raw_readw(a);		\
		__v |= _BV(b);				\
		__raw_writew(__v, (a));			\
	} while (0)
#define __raw_setl_atomic(b, a)				\
	do {						\
		uint32_t __v = __raw_readl(a);		\
		__v |= _BV(b);				\
		__raw_writel(__v, (a));			\
	} while (0)
#define __raw_clearb_atomic(b, a)			\
	do {						\
		uint8_t __v = __raw_readb(a);		\
		__v &= ~_BV(b);				\
		__raw_writeb(__v, (a));			\
	} while(0)
#define __raw_clearw_atomic(b, a)			\
	do {						\
		uint16_t __v = __raw_readw(a);		\
		__v &= ~_BV(b);				\
		__raw_writew(__v, (a));			\
	} while(0)
#define __raw_clearl_atomic(b, a)			\
	do {						\
		uint32_t __v = __raw_readl(a);		\
		__v &= ~_BV(b);				\
		__raw_writel(__v, (a));			\
	} while(0)
#define __raw_testb_atomic(b, a)			\
	((__raw_readb(a) >> b) & 0x01)
#define __raw_testw_atomic(b, a)			\
	((__raw_readw(a) >> b) & 0x01)
#define __raw_testl_atomic(b, a)			\
	((__raw_readl(a) >> b) & 0x01)
#endif
#endif /* __ASSEMBLY__ */

#endif /* __IO_RISCV_H_INCLUDE__*/
