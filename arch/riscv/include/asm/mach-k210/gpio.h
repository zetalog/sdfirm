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
 * @(#)gpio.h: FU540 (unleashed) specific GPIO pin defintions
 * $Id: gpio.h,v 1.1 2019-10-16 14:32:00 zhenglv Exp $
 */

#ifndef __GPIO_K210_H_INCLUDE__
#define __GPIO_K210_H_INCLUDE__

#include <target/arch.h>
#include <asm/mach/fpioa.h>

#ifdef CONFIG_GPIO
#ifndef ARCH_HAVE_GPIO
#define ARCH_HAVE_GPIO		1
#else
#error "Multiple GPIO controller defined"
#endif
#endif

/* GPIO abstraction */
#define GPIO_HW_MAX_PORTS	1
#define GPIO_HW_MAX_PINS	16

#define gpio_hw_read_pin(port, pin)		0
#define gpio_hw_write_pin(port, pin, val)	do { } while (0)
/* TODO: PIN Controller */
#define gpio_hw_config_mux(port, pin, mux)	do { } while (0)
#define gpio_hw_config_pad(port, pin, pad, drv)	do { } while (0)
/* TODO: GPIO IRQ Controller */
#define gpio_hw_ctrl_init()			do { } while (0)
#ifdef CONFIG_MMU
#define gpio_hw_mmu_init()			do { } while (0)
#endif

#endif /* __GPIO_K210_H_INCLUDE__ */
