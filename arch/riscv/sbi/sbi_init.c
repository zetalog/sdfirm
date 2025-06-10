/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/smp.h>
#include <target/sbi.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_pmu.h>

#define BANNER                                              \
	"   ____                    _____ ____ _____\n"     \
	"  / __ \\                  / ____|  _ \\_   _|\n"  \
	" | |  | |_ __   ___ _ __ | (___ | |_) || |\n"      \
	" | |  | | '_ \\ / _ \\ '_ \\ \\___ \\|  _ < | |\n" \
	" | |__| | |_) |  __/ | | |____) | |_) || |_\n"     \
	"  \\____/| .__/ \\___|_| |_|_____/|____/_____|\n"  \
	"        | |\n"                                     \
	"        |_|\n\n"

struct sbi_scratch *sbi_scratches[MAX_HARTS];

#ifdef CONFIG_SBI_PAYLOAD_DUMP
static DEFINE_SPINLOCK(payload_dump_lock);
unsigned long _payload_start[0];
unsigned long _payload_end[0];

static void sbi_payload_dump(void)
{
	uint32_t hartid = sbi_current_hartid();
	struct sbi_scratch *scratch = sbi_scratches[hartid];
	size_t payload_size =
		(caddr_t)_payload_end - (caddr_t)_payload_start;

	spin_lock(&payload_dump_lock);
	sbi_printf("CPU %d loading 0x%llx to 0x%llx...\n",
		   sbi_processor_id(), payload_size, scratch->next_addr);
	hexdump(0, (void *)scratch->next_addr, 1, payload_size);
	spin_unlock(&payload_dump_lock);
}
#else
#define sbi_payload_dump()	do { } while (0)
#endif

#ifdef CONFIG_SBI_BOOT_PRINTS
void sbi_boot_hart_prints(void)
{
	uint32_t hartid = sbi_current_hartid();
	struct sbi_scratch *scratch = sbi_scratches[hartid];
	caddr_t sp;

	if (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		return;

	sp = (caddr_t)scratch + SBI_SCRATCH_SIZE;
	sbi_printf("Current Hart           : %u\n", hartid);
	sbi_printf("Current CPU            : %u\n", sbi_processor_id());
	sbi_printf("Current Thread Pointer : 0x%016lx\n", scratch);
	sbi_printf("Current Thread Stack   : 0x%016lx - 0x%016lx\n",
		   sp - PERCPU_STACK_SIZE, sp);
}

void sbi_boot_prints(void)
{
	char str[64];
	cpu_t hartid = sbi_current_hartid();
	struct sbi_scratch *scratch = sbi_scratches[hartid];
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	if (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		return;

	misa_string(str, sizeof(str));
	sbi_printf("\nOpenSBI v%d.%d (%s %s)\n",
		   OPENSBI_VERSION_MAJOR, OPENSBI_VERSION_MINOR,
		   __DATE__, __TIME__);

	sbi_printf(BANNER);

	/* Platform details */
	sbi_printf("Platform Name          : %s\n",
		   sbi_platform_name(plat));
	sbi_printf("Platform HART Features : RV%d%s\n",
		   misa_xlen(), str);
	/* Firmware details */
	sbi_printf("Firmware Base          : 0x%lx\n",
		   scratch->fw_start);
	sbi_printf("Firmware Size          : %d KB\n",
		   (uint32_t)(scratch->fw_size / 1024));
	/* Generic details */
	sbi_printf("Runtime SBI Version    : %d.%d\n",
		   sbi_ecall_version_major(), sbi_ecall_version_minor());
	sbi_printf("Platform Max HARTs     : %d\n", MAX_HARTS);
	sbi_printf("Firmware Max CPUs      : %d\n", NR_CPUS);
	sbi_boot_hart_prints();

	pmp_dump(0, NULL);
}
#else
#define sbi_boot_hart_prints()		do { } while (0)
#define sbi_boot_prints()		do { } while (0)
#endif

static DEFINE_SPINLOCK(coldboot_lock);
static unsigned long coldboot_done = 0;
static DECLARE_BITMAP(coldboot_wait_bitmap, NR_CPUS);

static void wait_for_coldboot(struct sbi_scratch *scratch, uint32_t cpu)
{
	unsigned long saved_mie;
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	if (cpu >= NR_CPUS)
		bh_panic();

	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE bit to receive IPI */
	csr_set(CSR_MIE, IR_MSI);

	/* Acquire coldboot lock */
	spin_lock(&coldboot_lock);

	/* Mark current HART as waiting */
	set_bit(cpu, coldboot_wait_bitmap);

	/* Wait for coldboot to finish using WFI */
	while (!coldboot_done) {
		spin_unlock(&coldboot_lock);
		wfi();
		spin_lock(&coldboot_lock);
	};

	/* Unmark current HART as waiting */
	clear_bit(cpu, coldboot_wait_bitmap);

	/* Release coldboot lock */
	spin_unlock(&coldboot_lock);

	/* Restore MIE CSR */
	csr_write(CSR_MIE, saved_mie);

	/* Clear current HART IPI */
	sbi_platform_ipi_clear(plat, cpu);
}

static void wake_coldboot_harts(struct sbi_scratch *scratch, uint32_t cpu)
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	int i;

	/* Acquire coldboot lock */
	spin_lock(&coldboot_lock);

	/* Mark coldboot done */
	coldboot_done = 1;

	/* Send an IPI to all HARTs waiting for coldboot */
	for (i = 0; i < NR_CPUS; i++) {
		if ((i != cpu) && test_bit(i, coldboot_wait_bitmap))
			sbi_platform_ipi_send(plat, i);
	}

	/* Release coldboot lock */
	spin_unlock(&coldboot_lock);
}
static unsigned long init_count_offset;

static void __noreturn init_coldboot(void)
{
	int rc;
	unsigned long *init_count;
	cpu_t hartid = sbi_current_hartid();
	struct sbi_scratch *scratch = sbi_scratches[hartid];
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	init_count_offset = sbi_scratch_alloc_offset(__SIZEOF_POINTER__,
						     "INIT_COUNT");
	if (!init_count_offset)
		bh_panic();

	rc = sbi_heap_init(scratch);
	if (rc)
		bh_panic();

#ifdef CONFIG_SBI_ECALL_HSM
	rc = sbi_hsm_init(scratch, hartid, true);
	if (rc)
		bh_panic();
#endif

	rc = sbi_system_early_init(scratch, true);
	if (rc)
		bh_panic();

	rc = sbi_hart_init(scratch, hartid, true);
	if (rc)
		bh_panic();

	rc = sbi_console_init(scratch);
	if (rc)
		bh_panic();

	rc = sbi_pmu_init(scratch, true);
	if (rc) {
		sbi_printf("%s: pmu init failed (error %d)\n",
			__func__, rc);
		bh_panic();
	}

	rc = sbi_platform_irqchip_init(plat, true);
	if (rc)
		bh_panic();

	rc = sbi_ipi_init(scratch, true);
	if (rc)
		bh_panic();

	sbi_timer_init(scratch, true);

	rc = sbi_system_final_init(scratch, true);
	if (rc)
		bh_panic();

	/*
	 * Note: Ecall initialization should be after platform final
	 * initialization so that all available platform devices are
	 * already registered.
	 */
	rc = sbi_ecall_init();
	if (rc) {
		sbi_printf("%s: ecall init failed (error %d)\n", __func__, rc);
		bh_panic();
	}

	sbi_boot_prints();

	if (!sbi_platform_has_hart_hotplug(plat))
		wake_coldboot_harts(scratch, smp_hw_hart_cpu(hartid));
	sbi_payload_dump();
	sbi_hart_mark_available(hartid);

	init_count = sbi_scratch_offset_ptr(scratch, init_count_offset);
	(*init_count)++;
#ifdef CONFIG_SBI_ECALL_HSM
	sbi_hsm_prepare_next_jump(scratch, hartid);
#endif
	sbi_hart_switch_mode(hartid, scratch->next_arg1, scratch->next_addr,
			     scratch->next_mode);
}

static void __noreturn init_warmboot(void)
{
	int rc;
	unsigned long *init_count;
	cpu_t hartid = sbi_current_hartid();
	struct sbi_scratch *scratch = sbi_scratches[hartid];
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	if (!sbi_platform_has_hart_hotplug(plat))
		wait_for_coldboot(scratch, smp_hw_hart_cpu(hartid));

	if (!init_count_offset)
		bh_panic();

#ifdef CONFIG_SBI_ECALL_HSM
	rc = sbi_hsm_init(scratch, hartid, false);
	if (rc)
		bh_panic();
#endif

	if (sbi_platform_hart_disabled(plat, hartid))
		bh_panic();

	rc = sbi_system_early_init(scratch, false);
	if (rc)
		bh_panic();

	rc = sbi_hart_init(scratch, hartid, false);
	if (rc)
		bh_panic();

	rc = sbi_pmu_init(scratch, false);
	if (rc)
		bh_panic();

	rc = sbi_platform_irqchip_init(plat, false);
	if (rc)
		bh_panic();

	sbi_ipi_init(scratch, false);
	sbi_timer_init(scratch, false);

	rc = sbi_system_final_init(scratch, false);
	if (rc)
		bh_panic();

	sbi_hart_mark_available(hartid);

	init_count = sbi_scratch_offset_ptr(scratch, init_count_offset);
	(*init_count)++;

	if (sbi_platform_has_hart_hotplug(plat))
		/* TODO: To be implemented in-future. */
		bh_panic();
	else {
		sbi_boot_hart_prints();
		sbi_payload_dump();
#ifdef CONFIG_SBI_ECALL_HSM
		sbi_hsm_prepare_next_jump(scratch, hartid);
#endif
		sbi_hart_switch_mode(hartid, scratch->next_arg1,
				     scratch->next_addr, scratch->next_mode);
	}
}

static atomic_t coldboot_lottery = ATOMIC_INIT(0);

void __noreturn sbi_finish_hang(void)
{
	cpu_t hartid = sbi_current_hartid();
	struct sbi_scratch *scratch = sbi_scratches[hartid];
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	sbi_platform_system_finish(plat, 1);
	bh_panic();
}

/**
 * Initialize OpenSBI library for current HART and jump to next
 * booting stage.
 *
 * The function expects following:
 * 1. The 'mscratch' CSR is pointing to sbi_scratch of current HART
 * 2. Stack pointer (SP) is setup for current HART
 * 3. Interrupts are disabled in MSTATUS CSR
 * 4. All interrupts are disabled in MIE CSR
 *
 * @param scratch pointer to sbi_scratch of current HART
 */
void __noreturn sbi_init(void)
{
	cpu_t hartid = sbi_current_hartid();
	struct sbi_scratch *scratch =
		(struct sbi_scratch *)csr_read(CSR_MSCRATCH);
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	bool coldboot = false;

	if (sbi_platform_hart_disabled(plat, hartid))
		bh_panic();

	sbi_scratches[hartid] = scratch;

	if ((atomic_xchg(&coldboot_lottery, 1) == 0) && sbi_platform_cold_boot_allowed(plat, hartid))
		coldboot = true;

	if (coldboot)
		init_coldboot();
	else
		init_warmboot();
}

unsigned long sbi_init_count(uint32_t hartid)
{
	struct sbi_scratch *scratch;
	unsigned long *init_count;

	if (sbi_platform_hart_count(sbi_platform_thishart_ptr()) <= hartid ||
	    !init_count_offset)
		return 0;

	scratch = sbi_hart_id_to_scratch(sbi_scratch_thishart_ptr(), hartid);
	init_count = sbi_scratch_offset_ptr(scratch, init_count_offset);

	return *init_count;
}
