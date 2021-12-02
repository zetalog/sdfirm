/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)sbi.c: DPU-LP specific SBI implementation
 * $Id: sbi.c,v 1.1 2021-12-02 15:32:00 zhenglv Exp $
 */

#include <target/sbi.h>
#include <target/fdt.h>
#include <target/uart.h>
#include <target/irq.h>
#include <target/delay.h>

static void dpulp_modify_dt(void *fdt)
{
	fdt_cpu_fixup(fdt);
	fdt_irq_fixup(fdt, "riscv,clint0");
	fdt_irqs_fixup(fdt, "riscv,plic0", PLIC_MAX_CHIPS);
	fdt_fixups(fdt);
}

static int dpulp_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = sbi_scratch_thishart_arg1_ptr();
	dpulp_modify_dt(fdt);
	return 0;
}

static int dpulp_early_init(bool cold_boot)
{
	if (!cold_boot)
		return 0;

	board_init_timestamp();
	return 0;
}

static uint32_t dpulp_pmp_region_count(uint32_t hartid)
{
	return 1;
}

static int dpulp_pmp_region_info(uint32_t hartid, uint32_t index,
			       ulong *prot, ulong *addr, ulong *log2size)
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

#define __SBI_UART_REG(n, offset)		(__SBI_UART_BASE + (offset))
#define __UART_RBR(n)				__SBI_UART_REG(n, 0x00)
#define __UART_THR(n)				__SBI_UART_REG(n, 0x00)
#define __UART_LSR(n)				__SBI_UART_REG(n, 0x14)
#define __dpulp_uart_write_poll(n)		(!!(__raw_readl(__UART_LSR(n)) & LSR_TEMT))
#define __dpulp_uart_read_poll(n)		(!!(__raw_readl(__UART_LSR(n)) & LSR_DR))
#define __dpulp_uart_read_byte(n)		__raw_readl(__UART_RBR(n))
#define __dpulp_uart_write_byte(n, byte)	__raw_writel((byte), __UART_THR(n))

static void dpulp_console_putc(char ch)
{
	while (!__dpulp_uart_write_poll(UART_CON_ID));
	__dpulp_uart_write_byte(UART_CON_ID, ch);
}

static int dpulp_console_getc(void)
{
	if (!__dpulp_uart_read_poll(UART_CON_ID))
		return -1;
	return __dpulp_uart_read_byte(UART_CON_ID);
}

static int dpulp_irqchip_init(bool cold_boot)
{
	if (cold_boot)
		plic_sbi_init_cold();
	plic_sbi_init_warm(sbi_processor_id());
	return 0;
}

void dpulp_ipi_send(uint32_t target_cpu)
{
	clint_set_ipi(target_cpu);
}

void dpulp_ipi_sync(uint32_t target_cpu)
{
	clint_sync_ipi(target_cpu);
}

void dpulp_ipi_clear(uint32_t target_cpu)
{
	clint_clear_ipi(target_cpu);
}

static int dpulp_ipi_init(bool cold_boot)
{
	cpu_t cpu = sbi_processor_id();

	if (!cold_boot)
		dpulp_ipi_clear(cpu);
	return 0;
}

uint64_t dpulp_timer_value(void)
{
	return clint_read_mtime();
}

void dpulp_timer_event_stop(void)
{
	cpu_t cpu = sbi_processor_id();

	clint_unset_mtimecmp(cpu);
}

void dpulp_timer_event_start(uint64_t next_event)
{
	cpu_t cpu = sbi_processor_id();

	clint_set_mtimecmp(cpu, next_event);
}

static int dpulp_timer_init(bool cold_boot)
{
	if (!cold_boot)
		dpulp_timer_event_stop();
	return 0;
}

static int dpulp_system_down(uint32_t type)
{
	tcsr_sim_finish(true);
	return 0;
}

const struct sbi_platform_operations platform_ops = {
	.pmp_region_count	= dpulp_pmp_region_count,
	.pmp_region_info	= dpulp_pmp_region_info,
	.early_init		= dpulp_early_init,
	.final_init		= dpulp_final_init,
	.console_putc		= dpulp_console_putc,
	.console_getc		= dpulp_console_getc,
	.irqchip_init		= dpulp_irqchip_init,
	.ipi_send		= dpulp_ipi_send,
	.ipi_sync		= dpulp_ipi_sync,
	.ipi_clear		= dpulp_ipi_clear,
	.ipi_init		= dpulp_ipi_init,
	.timer_value		= dpulp_timer_value,
	.timer_event_stop	= dpulp_timer_event_stop,
	.timer_event_start	= dpulp_timer_event_start,
	.timer_init		= dpulp_timer_init,
	.system_reboot		= dpulp_system_down,
	.system_shutdown	= dpulp_system_down
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "SmarCo DPU-LP",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.disabled_hart_mask	= 0,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
