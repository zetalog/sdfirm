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
 * @(#)mmc.h: multimedia card (MMC) driver interface
 * $Id: mmc.h,v 1.1 2019-10-09 14:45:00 zhenglv Exp $
 */

#ifndef __MMC_DRIVER_H_INCLUDE__
#define __MMC_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_MMC
#include <asm/mach/mmc.h>
#endif
#ifdef CONFIG_ARCH_HAS_SD
#include <asm/mach/sd.h>
#endif

#if !defined(ARCH_HAVE_MMC) && !defined(ARCH_HAVE_SD)
#define mmc_hw_ctrl_init()		do { } while (0)
#define mmc_hw_slot_select(sid)		do { } while (0)
#define mmc_hw_send_command(cmd, arg)	do { } while (0)
#define mmc_hw_recv_response(resp, len)	do { } while (0)
#define mmc_hw_card_detect()		false
#define mmc_hw_set_clock(clock)		do { } while (0)
#define mmc_hw_set_width(width)		do { } while (0)
#define mmc_hw_card_busy()		false
#endif
#if !defined(CONFIG_MMC_SPI) && !defined(CONFIG_SD_SPI)
#define mmc_hw_spi_init()
#endif

#endif /* __MMC_DRIVER_H_INCLUDE__ */
