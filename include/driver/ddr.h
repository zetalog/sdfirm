/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)ddr.h: required DDR driver specific abstraction
 * $Id: ddr.h,v 1.1 2020-03-26 08:51:00 zhenglv Exp $
 */

#ifndef __DDR_DRIVER_H_INCLUDE__
#define __DDR_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_DDR
#include <asm/mach/ddr.h>
#endif

#ifndef ARCH_HAVE_DDR
#define ddr_hw_ctrl_init()		do { } while (0)
#define ddr_hw_ctrl_start()		do { } while (0)
#define ddr_hw_chan_select(s)		do { } while (0)
#define ddr_hw_slot_select(s)		do { } while (0)
#define ddr_hw_chan_reset()		do { } while (0)
#define ddr_hw_slot_reset()		do { } while (0)
#define ddr_hw_config_speed(speed)	do { } while (0)
#define ddr_hw_enable_speed(speed)	do { } while (0)
#define ddr_hw_wait_dfi(cycles)		do { } while (0)

#define ddr_hw_init_MR(n, v, r)		do { } while (0)
#ifndef CONFIG_DDR4
#define ddr4_hw_config_refresh(n, mode, tREFI, tRFCmin)	\
	do { } while (0)
#endif
#ifdef CONFIG_DDR_RAS
void ddr_ras_init(void);
#else
#define ddr_ras_init()	do { } while (0)
#endif
#endif

#endif /* __DDR_DRIVER_H_INCLUDE__ */

