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
 * @(#)barrier.h: memory barrier interface
 * $Id: barrier.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __BARRIER_H_INCLUDE__
#define __BARRIER_H_INCLUDE__

#include <target/compiler.h>

#define barrier()	__barrier()

#include <asm/barrier.h>

#ifndef mb
#define mb()		__mb()
#endif

#ifndef rmb
#define rmb()		__rmb()
#endif

#ifndef wmb
#define wmb()		__wmb()
#endif

#ifndef __smp_mb
#define __smp_mb()	mb()
#endif

#ifndef __smp_rmb
#define __smp_rmb()	rmb()
#endif

#ifndef __smp_wmb
#define __smp_wmb()	wmb()
#endif

#ifndef __dma_mb
#define __dma_mb	mb()
#endif

#ifndef __dma_rmb
#define __dma_rmb()	rmb()
#endif

#ifndef __dma_wmb
#define __dma_wmb()	wmb()
#endif

#ifndef dma_mb
#define dma_mb()	__dma_mb()
#endif

#ifndef dma_rmb
#define dma_rmb()	__dma_rmb()
#endif

#ifndef dma_wmb
#define dma_wmb()	__dma_wmb()
#endif

#ifdef CONFIG_SMP

#ifndef smp_mb
#define smp_mb()	__smp_mb()
#endif

#ifndef smp_rmb
#define smp_rmb()	__smp_rmb()
#endif

#ifndef smp_wmb
#define smp_wmb()	__smp_wmb()
#endif

#else	/* !CONFIG_SMP */

#ifndef smp_mb
#define smp_mb()	barrier()
#endif

#ifndef smp_rmb
#define smp_rmb()	barrier()
#endif

#ifndef smp_wmb
#define smp_wmb()	barrier()
#endif

#endif	/* CONFIG_SMP */

#endif /* __BARRIER_H_INCLUDE__ */
