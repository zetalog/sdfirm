/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2022
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
 * @(#)cpus.h: K1MAX specific CPU definitions
 * $Id: cpus.h,v 1.1 2022-10-15 12:11:00 zhenglv Exp $
 */

#ifndef __CPUS_K1MAX_H_INCLUDE__
#define __CPUS_K1MAX_H_INCLUDE__

#ifdef CONFIG_K1M_PARTIAL_GOOD
#define BOOT_HART		CONFIG_K1M_BOOT_HART_ID
#define BOOT_MASK		CONFIG_K1M_PARTIAL_GOOD_MASK
#if !((1 << BOOT_HART) & BOOT_MASK)
#error "Bad CPU partial good configuration!"
#endif
#else /* CONFIG_K1M_PARTIAL_GOOD */
#define BOOT_HART		0
#define BOOT_MASK		CPU_ALL
#endif /* CONFIG_K1M_PARTIAL_GOOD */

#ifdef CONFIG_SMP_BOOT
#define MAX_CPU_NUM		CONFIG_K1M_SMP_CPUS
#else
#define MAX_CPU_NUM		1
#endif

#define CPUS_PER_CLUSTER	4
#define MAX_CPU_CLUSTERS	(MAX_CPU_NUM / CPUS_PER_CLUSTER)

#ifdef CONFIG_K1M_SMP_SPARSE_HART_ID
#define HART_CPU_ID_OFFSET		0
#define HART_CPU_ID_MASK		REG_2BIT_MASK
#define HART_CPU_ID(hart)		_GET_FV(HART_CPU_ID, hart)
#define HART_CLUSTER_ID_OFFSET		4
#define HART_CLUSTER_ID_MASK		REG_2BIT_MASK
#define HART_CLUSTER_ID(hart)		_GET_FV(HART_CLUSTER_ID, hart)
#define hart_cluster_id(cluster)	_SET_FV(HART_CLUSTER_ID, cluster)

#define CPU_TO_CLUSTER_CPU(cpu)		((cpu) % CPUS_PER_CLUSTER)

#define HART_TO_CPU(hart)	\
	(HART_CPU_ID(hart) + (HART_CLUSTER_ID(hart) * CPUS_PER_CLUSTER))
#define CPU_TO_HART(cpu)	\
	(hart_cluster_id(CPU_TO_CLUSTER(cpu)) | CPU_TO_CLUSTER_CPU(cpu))
#endif

#endif /* __CPUS_K1MAX_H_INCLUDE__ */
