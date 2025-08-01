/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)noc.h: K1MXLite specific network on chip driver
 * $Id: noc.h,v 1.1 2023-09-06 18:27:00 zhenglv Exp $
 */

#ifndef __NOC_K1MXLITE_H_INCLUDE__
#define __NOC_K1MXLITE_H_INCLUDE__

#include <target/arch.h>

#define N100_HW_BASE		N100_CFG_BASE
#define N100_HW_NODES		(16 * 3 + 3 + 8 + 1 + 10)
#define N100_HW_COLS		3
#define N100_HW_ROWS		2
#define N100_HW_CHIPS		2

#include <driver/n100.h>

#ifdef CONFIG_K1MXLITE_D2D
#define n100_hw_max_chips()			2
#define n100_hw_chip_id()			sysreg_die_id()
#define n100_hw_ha_nid(chip)			0x40
#define n100_hw_link_nid(link)			0x40
#define n100_hw_chip_base(base, chip)		DIE1_BASE
#define n100_hw_chip_link()			true
#else
#define n100_hw_chip_link()			false
#endif
#ifdef CONFIG_K1MXLITE_N100
#define k1mxlite_n100_init()			spacemit_n100_noc_init(false)
#define k1mxlite_n100_d2d_init()		spacemit_n100_d2d_init()
#else
#define k1mxlite_n100_init()			do { } while (0)
#define k1mxlite_n100_d2d_init()		do { } while (0)
#endif
#define n100_hw_ctrl_init()			do { } while (0)
#define n100_hw_d2d_init()			do { } while (0)

#endif /* __NOC_K1MXLITE_H_INCLUDE__ */
