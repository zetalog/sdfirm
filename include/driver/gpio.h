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
 * @(#)gpio.h: required GPIO driver specific abstraction
 * $Id: gpio.h,v 1.1 2019-08-16 09:57:00 zhenglv Exp $
 */

#ifndef __GPIO_DRIVER_H_INCLUDE__
#define __GPIO_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_GPIO
#include <asm/mach/gpio.h>
#endif

#ifndef ARCH_HAVE_GPIO
#define gpio_hw_ctrl_init()				do { } while (0)
#define gpio_hw_read_pin(port, pin)			0
#define gpio_hw_write_pin(port, pin, val)		do { } while (0)
#define gpio_hw_read_port(port, mask)			0
#define gpio_hw_write_port(port, val)			do { } while (0)
/* NOTE: No Pad/Mux Interfaces
 *
 * driver are allowed not to provide following functions for convinience.
 */
#define gpio_hw_config_pad(port, pin, pad, ma)		do { } while (0)
#define gpio_hw_config_mux(port, pin, mux)		do { } while (0)
/* IRQ */
#define gpio_hw_config_irq(port, pin, mode)		do { } while (0)
#define gpio_hw_enable_irq(port, pin)			do { } while (0)
#define gpio_hw_disable_irq(port, pin)			do { } while (0)
#define gpio_hw_irq_status(port, pin)			0
#define gpio_hw_clear_irq(port, pin)			do { } while (0)
#define gpio_hw_config_irq(port, pin, mode)		do { } while (0)
#define gpio_hw_mmu_init()				do { } while (0)
#else
#ifdef CONFIG_SYS_NOIRQ
#define gpio_hw_enable_irq(port, pin)			do { } while (0)
#define gpio_hw_disable_irq(port, pin)			do { } while (0)
#endif /* CONFIG_SYS_NOIRQ */
#ifndef CONFIG_MMU
#define gpio_hw_mmu_init()				do { } while (0)
#endif /* CONFIG_MMU */
#endif

#endif /* __GPIO_DRIVER_H_INCLUDE__ */
