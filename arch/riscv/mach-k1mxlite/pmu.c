#include <target/perf.h>

void pmu_hw_enable_event(perf_evt_t evt)
{
	if (PMU_IS_HPM_EVENT(evt))
		hpm_enable_event(evt);
#if 0
	if (PMU_IS_PMCG_EVENT(evt))
#endif
}

void pmu_hw_disable_event(perf_evt_t evt)
{
	if (PMU_IS_HPM_EVENT(evt))
		hpm_enable_event(evt);
}

void pmu_hw_configure_event(perf_evt_t evt)
{
	if (PMU_IS_HPM_EVENT(evt))
		hpm_configure_event(evt);
}

void pmu_hw_ctrl_init(void)
{
	hpm_init();
	smmu_pmcg_init();
}

void pmu_hw_task_start(void)
{
	hpm_start();
	smmu_pmcg_start();
}

void pmu_hw_task_stop(void)
{
}
