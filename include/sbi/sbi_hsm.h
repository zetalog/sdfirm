/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ifndef __SBI_HSM_H__
#define __SBI_HSM_H__

#include <target/types.h>
#include <target/sbi.h>

/** Hart state values **/
#define SBI_HART_STOPPED	0
#define SBI_HART_STOPPING	1
#define SBI_HART_STARTING	2
#define SBI_HART_STARTED	3

/** Hart state managment device */
struct sbi_hsm_device {
	/** Name of the hart state managment device */
	char name[32];

	/** Start (or power-up) the given hart */
	int (*hart_start)(u32 hartid, ulong saddr);

	/**
	 * Stop (or power-down) the current hart from running.
	 *
	 * Return SBI_ENOTSUPP if the hart does not support platform-specific
	 * stop actions.
	 *
	 * For successful stop, the call won't return.
	 */
	int (*hart_stop)(void);

	/**
	 * Put the current hart in platform specific suspend (or low-power)
	 * state.
	 *
	 * For successful retentive suspend, the call will return 0 when
	 * the hart resumes normal execution.
	 *
	 * For successful non-retentive suspend, the hart will resume from
	 * the warm boot entry point.
	 *
	 * NOTE: mmode_resume_addr(resume address) is optional hence it
	 * may or may not be honored by the platform. If its not honored
	 * then platform must ensure to resume from the warmboot address.
	 */
	int (*hart_suspend)(u32 suspend_type, ulong mmode_resume_addr);

	/**
	 * Perform platform-specific actions to resume from a suspended state.
	 *
	 * This includes restoring any platform state that was lost during
	 * non-retentive suspend.
	 */
	void (*hart_resume)(void);
};

int sbi_hsm_init(struct sbi_scratch *scratch, uint32_t hartid, bool cold_boot);
void __noreturn sbi_hsm_exit(struct sbi_scratch *scratch);

int sbi_hsm_hart_start(struct sbi_scratch *scratch, uint32_t hartid,
			   uint64_t saddr, uint64_t priv);
int sbi_hsm_hart_stop(struct sbi_scratch *scratch, bool exitnow);
int sbi_hsm_hart_get_state(struct sbi_scratch *scratch, uint32_t hartid);
int sbi_hsm_hart_state_to_status(int state);
bool sbi_hsm_hart_started(struct sbi_scratch *scratch, uint32_t hartid);
void sbi_hsm_prepare_next_jump(struct sbi_scratch *scratch, uint32_t hartid);
void sbi_hsm_set_device(const struct sbi_hsm_device *dev);
#endif
