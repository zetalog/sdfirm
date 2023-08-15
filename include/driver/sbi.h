/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __SBI_PLATFORM_H__
#define __SBI_PLATFORM_H__

/** OpenSBI 32-bit platform version with:
 *  1. upper 16-bits as major number
 *  2. lower 16-bits as minor number
 */
#define SBI_PLATFORM_VERSION(Major, Minor) ((Major << 16) | Minor)

#ifndef __ASSEMBLY__
/** Possible feature flags of a platform */
enum sbi_platform_features {
	SBI_PLATFORM_HAS_TIMER_VALUE = (1 << 0),
	/** Platform has HART hotplug support */
	SBI_PLATFORM_HAS_HART_HOTPLUG = (1 << 1),
	/** Platform has PMP support */
	SBI_PLATFORM_HAS_PMP = (1 << 2),
	/** Platform has fault delegation support */
	SBI_PLATFORM_HAS_MFAULTS_DELEGATION = (1 << 5),
	/** Platform has custom secondary hart booting support */
	SBI_PLATFORM_HAS_HART_SECONDARY_BOOT = (1 << 6),
	/** Last index of Platform features*/
	SBI_PLATFORM_HAS_LAST_FEATURE = SBI_PLATFORM_HAS_HART_HOTPLUG,
};

/** Default feature set for a platform */
#define SBI_PLATFORM_DEFAULT_FEATURES                                \
	(SBI_PLATFORM_HAS_TIMER_VALUE | SBI_PLATFORM_HAS_MFAULTS_DELEGATION)

/** Platform functions */
struct sbi_platform_operations {
	/** Platform early initialization */
	int (*early_init)(bool cold_boot);
	/** Platform final initialization */
	int (*final_init)(bool cold_boot);

	/** Get number of PMP regions for given HART */
	uint32_t (*pmp_region_count)(uint32_t hartid);
	/**
	 * Get PMP regions details (namely: protection, base address,
	 * and size) for given HART
	 */
	int (*pmp_region_info)(uint32_t hartid, uint32_t index,
			       ulong *prot, ulong *addr, ulong *log2size);

	/** Write a character to the platform console output */
	void (*console_putc)(char ch);
	/** Read a character from the platform console input */
	int (*console_getc)(void);

	/** Initialize the platform interrupt controller for current HART */
	int (*irqchip_init)(bool cold_boot);

	/** Send IPI to a target SMP CPU */
	void (*ipi_send)(uint32_t target_cpu);
	/** Clear IPI for a target SMP CPU */
	void (*ipi_clear)(uint32_t target_cpu);
	/** Initialize IPI for current HART */
	int (*ipi_init)(bool cold_boot);

	/** Get platform timer value */
	uint64_t (*timer_value)(void);
	/** Start platform timer event for current HART */
	void (*timer_event_start)(uint64_t next_event);
	/** Stop platform timer event for current HART */
	void (*timer_event_stop)(void);
	/** Initialize platform timer for current HART */
	int (*timer_init)(bool cold_boot);

	/** Bringup the given hart from previous stage **/
	int (*hart_start)(uint32_t hartid, uint64_t saddr, uint64_t priv);
	/**
	 *  Stop the current hart from running. This call doesn't expect to
	 *  return if success.
	 */
	int (*hart_stop)(void);

	/** Reboot the platform */
	int (*system_reboot)(uint32_t type);
	/** Shutdown or poweroff the platform */
	int (*system_shutdown)(uint32_t type);

	/** Handle platform specific IRQ */
	int (*process_irq)(uint32_t cause);
	/** Mark a bench test result and finish */
	int (*system_finish)(uint32_t code);

	/** hartid overridden */
	bool (*hart_disabled)(uint32_t hartid);
} __packed;

/** Representation of a platform */
struct sbi_platform {
	/**
	 * OpenSBI version this sbi_platform is based on.
	 * It's a 32-bit value where upper 16-bits are major number
	 * and lower 16-bits are minor number
	 */
	uint32_t opensbi_version;
	/**
	 * OpenSBI platform version released by vendor.
	 * It's a 32-bit value where upper 16-bits are major number
	 * and lower 16-bits are minor number
	 */
	uint32_t platform_version;
	/** Name of the platform */
	char name[64];
	/** Supported features */
	uint64_t features;
	/** Total number of HARTs */
	uint32_t hart_count;
	/** Mask representing the set of disabled HARTs */
	uint64_t disabled_hart_mask;
	/** Pointer to sbi platform operations */
	unsigned long platform_ops_addr;
	/** Pointer to system firmware specific context */
	unsigned long firmware_context;
} __packed;

/** Get pointer to sbi_platform for sbi_scratch pointer */
#define sbi_platform_ptr(__s) \
	((const struct sbi_platform *)((__s)->platform_addr))
/** Get pointer to sbi_platform for current HART */
#define sbi_platform_thishart_ptr() ((const struct sbi_platform *) \
	(sbi_scratch_thishart_ptr()->platform_addr))
/** Get pointer to platform_ops_addr from platform pointer **/
#define sbi_platform_ops(__p) \
	((const struct sbi_platform_operations *)(__p)->platform_ops_addr)

/** Check whether the platform supports timer value */
#define sbi_platform_has_timer_value(__p) \
	((__p)->features & SBI_PLATFORM_HAS_TIMER_VALUE)
/** Check whether the platform supports HART hotplug */
#define sbi_platform_has_hart_hotplug(__p) \
	((__p)->features & SBI_PLATFORM_HAS_HART_HOTPLUG)
/** Check whether the platform has PMP support */
#define sbi_platform_has_pmp(__p) \
	((__p)->features & SBI_PLATFORM_HAS_PMP)
/** Check whether the platform supports fault delegation */
#define sbi_platform_has_mfaults_delegation(__p) \
	((__p)->features & SBI_PLATFORM_HAS_MFAULTS_DELEGATION)
/** Check whether the platform supports custom secondary hart booting support */
#define sbi_platform_has_hart_secondary_boot(__p) \
	((__p)->features & SBI_PLATFORM_HAS_HART_SECONDARY_BOOT)

/**
 * Get name of the platform
 *
 * @param plat pointer to struct sbi_platform
 *
 * @return pointer to platform name on success and NULL on failure
 */
static inline const char *sbi_platform_name(const struct sbi_platform *plat)
{
	if (plat)
		return plat->name;
	return NULL;
}

/**
 * Check whether the given HART is disabled
 *
 * @param plat pointer to struct sbi_platform
 * @param hartid HART ID
 *
 * @return TRUE if HART is disabled and FALSE otherwise
 */
static inline bool sbi_platform_hart_disabled(const struct sbi_platform *plat,
					      uint32_t hartid)
{
	if (plat) {
		if (sbi_platform_ops(plat)->hart_disabled)
			return sbi_platform_ops(plat)->hart_disabled(hartid);
		else if (plat->disabled_hart_mask & (1 << hartid))
			return true;
	}
	return false;
}

/**
 * Get total number of HARTs supported by the platform
 *
 * @param plat pointer to struct sbi_platform
 *
 * @return total number of HARTs
 */
static inline uint32_t sbi_platform_hart_count(const struct sbi_platform *plat)
{
	if (plat)
		return plat->hart_count;
	return 0;
}

/**
 * Stop the current hart in OpenSBI.
 *
 * @param plat pointer to struct sbi_platform
 *
 * @return Negative error code on failure. It doesn't return on success.
 */
static inline int sbi_platform_hart_stop(const struct sbi_platform *plat)
{
	if (plat && sbi_platform_ops(plat)->hart_stop)
		return sbi_platform_ops(plat)->hart_stop();
	return SBI_ENOTSUPP;
}

/**
 * Bringup a given hart from previous stage. Platform should implement this
 * operation if they support a custom mechanism to start a hart. Otherwise,
 * a generic WFI based approach will be used to start/stop a hart in OpenSBI.
 *
 * @param plat pointer to struct sbi_platform
 * @param hartid Hart ID
 * @param saddr  Physical address in supervisor mode for hart to jump after
 *		 OpenSBI
 * @param priv	 A private context data from the caller
 *
 * @return 0 if sucessful and negative error code on failure
 */
static inline int sbi_platform_hart_start(const struct sbi_platform *plat,
					  uint32_t hartid, uint64_t saddr, uint64_t priv)
{
	if (plat && sbi_platform_ops(plat)->hart_start)
		return sbi_platform_ops(plat)->hart_start(hartid, saddr, priv);
	return SBI_ENOTSUPP;
}

/**
 * Early initialization for current HART
 *
 * @param plat pointer to struct sbi_platform
 * @param cold_boot whether cold boot (TRUE) or warm_boot (FALSE)
 *
 * @return 0 on success and negative error code on failure
 */
static inline int sbi_platform_early_init(const struct sbi_platform *plat,
					  bool cold_boot)
{
	if (plat && sbi_platform_ops(plat)->early_init)
		return sbi_platform_ops(plat)->early_init(cold_boot);
	return 0;
}

/**
 * Final initialization for current HART
 *
 * @param plat pointer to struct sbi_platform
 * @param cold_boot whether cold boot (TRUE) or warm_boot (FALSE)
 *
 * @return 0 on success and negative error code on failure
 */
static inline int sbi_platform_final_init(const struct sbi_platform *plat,
					  bool cold_boot)
{
	if (plat && sbi_platform_ops(plat)->final_init)
		return sbi_platform_ops(plat)->final_init(cold_boot);
	return 0;
}

/**
 * Get the number of PMP regions of a HART
 *
 * @param plat pointer to struct sbi_platform
 * @param hartid HART ID
 *
 * @return number of PMP regions
 */
static inline
uint32_t sbi_platform_pmp_region_count(const struct sbi_platform *plat,
				       uint32_t hartid)
{
	if (plat && sbi_platform_ops(plat)->pmp_region_count)
		return sbi_platform_ops(plat)->pmp_region_count(hartid);
	return 0;
}

/**
 * Get PMP regions details (namely: protection, base address,
 * and size) of a HART
 *
 * @param plat pointer to struct sbi_platform
 * @param hartid HART ID
 * @param index index of PMP region for which we want details
 * @param prot output pointer for PMP region protection
 * @param addr output pointer for PMP region base address
 * @param log2size output pointer for log-of-2 PMP region size
 *
 * @return 0 on success and negative error code on failure
 */
static inline int sbi_platform_pmp_region_info(const struct sbi_platform *plat,
						uint32_t hartid,
						uint32_t index,
						ulong *prot, ulong *addr,
						ulong *log2size)
{
	if (plat && sbi_platform_ops(plat)->pmp_region_info)
		return sbi_platform_ops(plat)->pmp_region_info(hartid,
							       index,
							       prot,
							       addr,
							       log2size);
	return 0;
}

/**
 * Write a character to the platform console output
 *
 * @param plat pointer to struct sbi_platform
 * @param ch character to write
 */
static inline void sbi_platform_console_putc(const struct sbi_platform *plat,
						char ch)
{
	if (plat && sbi_platform_ops(plat)->console_putc) {
		if (ch == '\n')
			sbi_platform_ops(plat)->console_putc('\r');
		sbi_platform_ops(plat)->console_putc(ch);
	}
}

/**
 * Read a character from the platform console input
 *
 * @param plat pointer to struct sbi_platform
 *
 * @return character read from console input
 */
static inline int sbi_platform_console_getc(const struct sbi_platform *plat)
{
	if (plat && sbi_platform_ops(plat)->console_getc)
		return sbi_platform_ops(plat)->console_getc();
	return -1;
}

/**
 * Initialize the platform interrupt controller for current HART
 *
 * @param plat pointer to struct sbi_platform
 * @param cold_boot whether cold boot (TRUE) or warm_boot (FALSE)
 *
 * @return 0 on success and negative error code on failure
 */
static inline int sbi_platform_irqchip_init(const struct sbi_platform *plat,
					    bool cold_boot)
{
	if (plat && sbi_platform_ops(plat)->irqchip_init)
		return sbi_platform_ops(plat)->irqchip_init(cold_boot);
	return 0;
}

/**
 * Send IPI to a target CPU
 *
 * @param plat pointer to struct sbi_platform
 * @param target_cpu SMP CPU ID of IPI target
 */
static inline void sbi_platform_ipi_send(const struct sbi_platform *plat,
					 uint32_t target_cpu)
{
	if (plat && sbi_platform_ops(plat)->ipi_send)
		sbi_platform_ops(plat)->ipi_send(target_cpu);
}

/**
 * Clear IPI for a target CPU
 *
 * @param plat pointer to struct sbi_platform
 * @param target_hart SMP CPU ID of IPI target
 */
static inline void sbi_platform_ipi_clear(const struct sbi_platform *plat,
					  uint32_t target_cpu)
{
	if (plat && sbi_platform_ops(plat)->ipi_clear)
		sbi_platform_ops(plat)->ipi_clear(target_cpu);
}

/**
 * Initialize the platform IPI support for current HART
 *
 * @param plat pointer to struct sbi_platform
 * @param cold_boot whether cold boot (TRUE) or warm_boot (FALSE)
 *
 * @return 0 on success and negative error code on failure
 */
static inline int sbi_platform_ipi_init(const struct sbi_platform *plat,
					bool cold_boot)
{
	if (plat && sbi_platform_ops(plat)->ipi_init)
		return sbi_platform_ops(plat)->ipi_init(cold_boot);
	return 0;
}

/**
 * Get platform timer value
 *
 * @param plat pointer to struct sbi_platform
 *
 * @return 64bit timer value
 */
static inline uint64_t sbi_platform_timer_value(const struct sbi_platform *plat)
{
	if (plat && sbi_platform_ops(plat)->timer_value)
		return sbi_platform_ops(plat)->timer_value();
	return 0;
}

/**
 * Start platform timer event for current HART
 *
 * @param plat pointer to struct struct sbi_platform
 * @param next_event timer value when timer event will happen
 */
static inline void
sbi_platform_timer_event_start(const struct sbi_platform *plat,
			       uint64_t next_event)
{
	if (plat && sbi_platform_ops(plat)->timer_event_start)
		sbi_platform_ops(plat)->timer_event_start(next_event);
}

/**
 * Stop platform timer event for current HART
 *
 * @param plat pointer to struct sbi_platform
 */
static inline void
sbi_platform_timer_event_stop(const struct sbi_platform *plat)
{
	if (plat && sbi_platform_ops(plat)->timer_event_stop)
		sbi_platform_ops(plat)->timer_event_stop();
}

/**
 * Initialize the platform timer for current HART
 *
 * @param plat pointer to struct sbi_platform
 * @param cold_boot whether cold boot (TRUE) or warm_boot (FALSE)
 *
 * @return 0 on success and negative error code on failure
 */
static inline int sbi_platform_timer_init(const struct sbi_platform *plat,
					  bool cold_boot)
{
	if (plat && sbi_platform_ops(plat)->timer_init)
		return sbi_platform_ops(plat)->timer_init(cold_boot);
	return 0;
}

/**
 * Reboot the platform
 *
 * @param plat pointer to struct sbi_platform
 * @param type type of reboot
 *
 * @return 0 on success and negative error code on failure
 */
static inline int sbi_platform_system_reboot(const struct sbi_platform *plat,
					     uint32_t type)
{
	if (plat && sbi_platform_ops(plat)->system_reboot)
		return sbi_platform_ops(plat)->system_reboot(type);
	return 0;
}

/**
 * Shutdown or poweroff the platform
 *
 * @param plat pointer to struct sbi_platform
 * @param type type of shutdown or poweroff
 *
 * @return 0 on success and negative error code on failure
 */
static inline int sbi_platform_system_shutdown(const struct sbi_platform *plat,
					       uint32_t type)
{
	if (plat && sbi_platform_ops(plat)->system_shutdown)
		return sbi_platform_ops(plat)->system_shutdown(type);
	return 0;
}

/**
 * Finish the testbench with test result
 *
 * @param plat pointer to struct sbi_platform
 * @param type type of the test result
 *
 * @return 0 on success and negative error code on failure
 */
static inline int sbi_platform_system_finish(const struct sbi_platform *plat,
					     uint32_t code)
{
	if (plat && sbi_platform_ops(plat)->system_finish)
		return sbi_platform_ops(plat)->system_finish(code);
	return 0;
}

/**
 * Handle platform specific IRQ
 *
 * @param plat pointer to struct sbi_platform
 * @param cause IRQ cause
 *
 * @return 0 on success and negative error code on failure
 */
static inline int sbi_platform_process_irq(const struct sbi_platform *plat,
					   uint32_t irq)
{
	if (plat && sbi_platform_ops(plat)->process_irq)
		return sbi_platform_ops(plat)->process_irq(irq);
	return 0;
}
#endif

#endif
