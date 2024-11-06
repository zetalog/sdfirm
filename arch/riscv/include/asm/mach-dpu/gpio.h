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
 * @(#)gpio.h: DPU general purpose input/output (GPIO) definitions
 * $Id: gpio.h,v 1.1 2020-05-20 14:37:00 zhenglv Exp $
 */

#ifndef __GPIO_DPU_H_INCLUDE__
#define __GPIO_DPU_H_INCLUDE__

#include <target/arch.h>
#include <target/clk.h>

#ifdef CONFIG_DPU_GPIO
#ifdef CONFIG_MMU
#define __GPIO_BASE		dpu_gpio_reg_base
extern caddr_t dpu_gpio_reg_base;
#else
#define __GPIO_BASE		GPIO_BASE
#endif
#define DW_GPIO_REG(n, offset)	(__GPIO_BASE + (offset))
#ifndef ARCH_HAVE_GPIO
#include <driver/dw_gpio.h>
#define ARCH_HAVE_GPIO		1
#else
#error "Multiple GPIO controller defined"
#endif
#endif

/* GPIO abstraction */
#ifdef CONFIG_GPIO
#define GPIO_HW_MAX_PORTS	4
#define GPIO_HW_MAX_PINS	32

#define gpio_hw_ctrl_init()			clk_enable(srst_gpio)
#define gpio_hw_read_pin(port, pin)		\
	dw_gpio_read_pin(0, port, pin)
#define gpio_hw_write_pin(port, pin, val)	\
	dw_gpio_write_pin(0, port, pin, val)
/* No programmable PAD model and no multiplexing for now */
#define gpio_hw_config_pad(port, pin, pad, ma)				\
	do {								\
		if (pad & GPIO_PAD_GPIO)				\
			dw_gpio_config_pad(0, port, pin, pad, ma);	\
	} while (0)
#define gpio_hw_config_mux(port, pin, mux)	do { } while (0)
#ifndef CONFIG_SYS_NOIRQ
#define gpio_hw_config_irq(port, pin, mode)	\
	dw_gpio_config_irq(0, pin, mode)
#define gpio_hw_enable_irq(port, pin)		\
	dw_gpio_enable_irq(0, pin)
#define gpio_hw_disable_irq(port, pin)		\
	dw_gpio_disable_irq(0, pin)
#define gpio_hw_irq_status(port, pin)		\
	dw_gpio_irq_status(0, pin)
#define gpio_hw_clear_irq(port, pin)		\
	dw_gpio_clear_irq(0, pin)
#endif /* CONFIG_SYS_NOIRQ */
#endif /* CONFIG_GPIO */

#ifdef CONFIG_DPU_SIM_GPIO_IRQ
void dpu_gpio_irq_init(void);
#else
#define dpu_gpio_irq_init()			do { } while (0)
#endif
#ifdef CONFIG_MMU
#define gpio_hw_mmu_init()			dpu_mmu_map_gpio()
#endif

#endif /* __GPIO_DPU_H_INCLUDE__ */
