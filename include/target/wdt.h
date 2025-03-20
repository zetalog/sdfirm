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
 * @(#)wdt.h: specific WDT interface
 * $Id: wdt.h,v 1.1 2025-01-15 14:56:00 zhenglv Exp $
 */

#ifndef __WDT_H_INCLUDE__
#define __WDT_H_INCLUDE__

#include <target/generic.h>
#include <driver/wdt.h>

#ifdef CONFIG_WDT
void wdt_init();
void wdt_timer_handler();
void wdt_bite(void);
void wdt_bark(void);
void wdt_int_handler();
void wdt_clear_timeout(int id);
int wdt_has_timed_out(int id);
void wdt_feed(int id);
void wdt_disable(int id);
void wdt_enable(int id);
#else
#define wdt_init()		do { } while (0)
#define wdt_timer_handler()	do { } while (0)
#define wdt_bite(void)		do { } while (0)
#define wdt_bark(void)		do { } while (0)
#define wdt_int_handler()		do { } while (0)
#define wdt_clear_timeout(int id)	do { } while (0)
#define wdt_has_timed_out(int id)	do { } while (0)
#define wdt_feed(int id)		do { } while (0)
#define wdt_disable(int id)	do { } while (0)
#define wdt_enable(int id)		do { } while (0)
#endif


#endif
