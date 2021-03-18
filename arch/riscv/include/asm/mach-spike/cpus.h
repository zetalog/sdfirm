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
 * @(#)cpus.h: SPIKE specific CPU definitions
 * $Id: cpus.h,v 1.1 2019-09-05 13:51:00 zhenglv Exp $
 */

#ifndef __CPUS_SPIKE_H_INCLUDE__
#define __CPUS_SPIKE_H_INCLUDE__

#ifdef CONFIG_SMP
#ifdef CONFIG_SPIKE_BOOT_CPU
#define CPUS_PER_CLUSTER	CONFIG_SPIKE_SMP_CPUS
#define CLUSTERS_PER_RAIL	1
#define MAX_CPU_RAILS		1
#define MAX_HARTS		(CONFIG_SPIKE_SMP_CPUS + 1)
#define HART_ALL		((CPU_TO_MASK(MAX_HARTS)-1)-1)
#else /* CONFIG_SPIKE_BOOT_CPU */
#define CPUS_PER_CLUSTER	CONFIG_SPIKE_SMP_CPUS
#define CLUSTERS_PER_RAIL	CONFIG_SPIKE_SMP_CLUSTERS
#define MAX_CPU_RAILS		CONFIG_SPIKE_SMP_RAILS
#endif /* CONFIG_SPIKE_BOOT_CPU */
#define MAX_CPU_CLUSTERS	(MAX_CPU_RAILS * CLUSTERS_PER_RAIL)
#define CPUS_PER_RAIL		(CPUS_PER_CLUSTER * CLUSTERS_PER_RAIL)
#define MAX_CPU_NUM		(MAX_CPU_RAILS * CPUS_PER_RAIL)
#else /* CONFIG_SMP */
#define MAX_CPU_NUM		1
#define MAX_CPU_CLUSTERS	1
#define MAX_CPU_RAILS		1
#define CPUS_PER_CLUSTER	1
#define CLUSTERS_PER_RAIL	1
#define CPUS_PER_RAIL		1
#endif /* CONFIG_SMP */

#endif /* __CPUS_SPIKE_H_INCLUDE__ */
