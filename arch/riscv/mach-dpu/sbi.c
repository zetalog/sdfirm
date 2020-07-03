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
 * @(#)sbi.c: DPU specific SBI implementation
 * $Id: sbi.c,v 1.1 2020-03-14 22:31:00 zhenglv Exp $
 */

#include <target/sbi.h>
#include <target/fdt.h>
#include <target/uart.h>
#include <target/irq.h>
#include <target/delay.h>

static void dpu_modify_dt(void *fdt)
{
}

static int dpu_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = sbi_scratch_thishart_arg1_ptr();
	dpu_modify_dt(fdt);
	return 0;
}

static int dpu_early_init(bool cold_boot)
{
	if (!cold_boot)
		return 0;

	board_init_timestamp();
	return 0;
}

static u32 dpu_pmp_region_count(u32 hartid)
{
	return 1;
}

static int dpu_pmp_region_info(u32 hartid, u32 index, ulong *prot,
				 ulong *addr, ulong *log2size)
{
	int ret = 0;

	switch (index) {
	case 0:
		*prot	  = PMP_R | PMP_W | PMP_X;
		*addr	  = 0;
		*log2size = __riscv_xlen;
		break;
	default:
		ret = -1;
		break;
	};
	return ret;
}

#define __DPU_UART_REG(n, offset)		(__DPU_UART_BASE + (offset))
#define __UART_RBR(n)				__DPU_UART_REG(n, 0x00)
#define __UART_THR(n)				__DPU_UART_REG(n, 0x00)
#define __UART_LSR(n)				__DPU_UART_REG(n, 0x14)
#define __dpu_uart_write_poll(n)		(!!(__raw_readl(__UART_LSR(n)) & LSR_TEMT))
#define __dpu_uart_read_poll(n)		(!!(__raw_readl(__UART_LSR(n)) & LSR_DR))
#define __dpu_uart_read_byte(n)		__raw_readl(__UART_RBR(n))
#define __dpu_uart_write_byte(n, byte)	__raw_writel((byte), __UART_THR(n))

static void dpu_console_putc(char ch)
{
	while (!__dpu_uart_write_poll(UART_CON_ID));
	__dpu_uart_write_byte(UART_CON_ID, ch);
}

static int dpu_console_getc(void)
{
	if (!__dpu_uart_read_poll(UART_CON_ID))
		return -1;
	return __dpu_uart_read_byte(UART_CON_ID);
}

static int dpu_irqchip_init(bool cold_boot)
{
	cpu_t cpu = sbi_processor_id();

	if (cold_boot)
		plic_sbi_init_cold();
	else
		plic_sbi_init_warm(cpu);
	return 0;
}

void dpu_ipi_send(u32 target_cpu)
{
}

void dpu_ipi_sync(u32 target_cpu)
{
}

void dpu_ipi_clear(u32 target_cpu)
{
}

static int dpu_ipi_init(bool cold_boot)
{
	cpu_t cpu = sbi_processor_id();

	if (!cold_boot)
		dpu_ipi_clear(cpu);
	return 0;
}

u64 dpu_timer_value(void)
{
	return tmr_read_counter();
}

void dpu_timer_event_stop(void)
{
	__unused cpu_t cpu = smp_processor_id();

	tmr_disable_cmp(cpu);
}

void dpu_timer_event_start(u64 next_event)
{
	__unused cpu_t cpu = smp_processor_id();

	tmr_write_compare(cpu, next_event);
}

static int dpu_timer_init(bool cold_boot)
{
	if (!cold_boot)
		dpu_timer_event_stop();
	return 0;
}

static int dpu_system_down(u32 type)
{
	imc_sim_finish();
	return 0;
}

const struct sbi_platform_operations platform_ops = {
	.pmp_region_count	= dpu_pmp_region_count,
	.pmp_region_info	= dpu_pmp_region_info,
	.early_init		= dpu_early_init,
	.final_init		= dpu_final_init,
	.console_putc		= dpu_console_putc,
	.console_getc		= dpu_console_getc,
	.irqchip_init		= dpu_irqchip_init,
	.ipi_send		= dpu_ipi_send,
	.ipi_sync		= dpu_ipi_sync,
	.ipi_clear		= dpu_ipi_clear,
	.ipi_init		= dpu_ipi_init,
	.timer_value		= dpu_timer_value,
	.timer_event_stop	= dpu_timer_event_stop,
	.timer_event_start	= dpu_timer_event_start,
	.timer_init		= dpu_timer_init,
	.system_reboot		= dpu_system_down,
	.system_shutdown	= dpu_system_down
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "SmarCo DPU",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.disabled_hart_mask	= 0,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
