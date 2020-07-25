#ifndef __PMU_DPU_H_INCLUDE__
#define __PMU_DPU_H_INCLUDE__

#ifdef CONFIG_DPU_CV32E40P
#include <asm/ri5cy_pc.h>

#define PMU_HW_MAX_COUNTERS		RI5CY_MAX_COUNTERS
#define PMU_HW_DEFAULT_EVENT		PC_CYCLES

#define pmu_hw_ctrl_init()		do { } while (0)
#define pmu_hw_reset_events()		\
	do {				\
		ri5cy_disable_event();	\
		ri5cy_enable_event();	\
	} while (0)
#define pmu_hw_get_counters()		ri5cy_get_counters()
#define pmu_hw_enable_event(event)	ri5cy_enable_count(event)
#define pmu_hw_disable_event(event)	ri5cy_disable_count(event)
#define pmu_hw_configure_event(event)	ri5cy_set_count(event, 0)
#define pmu_hw_get_event_count(event)	ri5cy_get_count(event)
#endif

#ifdef CONFIG_DPU_CVA6
#define PMU_HW_MAX_COUNTERS		17

#include <asm/hpm.h>

#define HPM_L1_ICACHE_MISS		3  /* L1 instruction cache miss */
#define HPM_L1_DCACHE_MISS		4  /* L1 data cache miss */
#define HPM_ITLB_MISS			5  /* ITLB miss */
#define HPM_DTLB_MISS			6  /* DTLB miss */
#define HPM_LOAD			7  /* Loads */
#define HPM_STORE			8  /* Stores */
#define HPM_EXCEPTION			9  /* Taken exception */
#define HPM_EXCEPTION_RET		10 /* Exception return */
#define HPM_BRANCH_JUMP			11 /* Software change of PC */
#define HPM_CALL			12 /* Procedure call */
#define HPM_RET				13 /* Procedure return */
#define HPM_MIS_PREDICT			14 /* Branch mis-predicted */
#define HPM_SB_FULL			15 /* Scoreboard full */
#define HPM_IF_EMPTY			16 /* Instruction fetch queue empty */
#endif

#endif /* __PMU_DPU_H_INCLUDE__ */
