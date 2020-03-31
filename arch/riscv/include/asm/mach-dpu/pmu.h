#ifndef __PMU_DPU_H_INCLUDE__
#define __PMU_DPU_H_INCLUDE__

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

#endif /* __PMU_DPU_H_INCLUDE__ */
