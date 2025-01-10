#ifndef __CMN600_PMU_H_INCLUDE__
#define __CMN600_PMU_H_INCLUDE__

#include <target/noc.h>

#define CMN_MAX_GLOBAL_COUNTERS		8
#define CMN_MAX_LOCAL_COUNTERS		4
#define CMN_MAX_COUNTERS	(CMN_MAX_DTCS * CMN_MAX_GLOBAL_COUNTERS)
#define CMN_MAX_DTMS		(CMN_MAX_MXP_COUNT + (CMN_MAX_DIMENS - 1) * CMN_MAX_PORTS)

#define CMN_EVENT_TYPE_OFFSET		0
#define CMN_EVENT_TYPE_MASK		REG_16BIT_MASK
#define CMN_EVENT_TYPE(value)		_SET_FV_ULL(CMN_EVENT_TYPE, value)
#define CMN_EVENT_EVENTID_OFFSET	16
#define CMN_EVENT_EVENTID_MASK		REG_11BIT_MASK
#define CMN_EVENT_EVENTID(value)	_SET_FV_ULL(CMN_EVENT_EVENTID, value)
#define CMN_EVENT_OCCUPLID_OFFSET	27
#define CMN_EVENT_OCCUPLID_MASK		REG_4BIT_MASK
#define CMN_EVENT_OCCUPLID(value)	_SET_FV_ULL(CMN_EVENT_OCCUPLID, value)
#define CMN_EVENT_BYNODEID		_BV_ULL(31)
#define CMN_EVENT_NODEID_OFFSET		32
#define CMN_EVENT_NODEID_MASK		REG_16BIT_MASK
#define CMN_EVENT_NODEID(value)		_SET_FV_ULL(CMN_EVENT_NODEID, value)
#define CMN_WP_UP			0
#define CMN_WP_DN			2

#define CMN_EVENT_COUNTER_OFFSET	48
#define CMN_EVENT_COUNTER_MASK		REG_16BIT_MASK
#define CMN_EVENT_COUNTER(value)	_SET_FV_ULL(CMN_EVENT_COUNTER, value)

#define PERF_EVENT_TO_CMN_EVENT(evt)	\
	((evt) & ~CMN_EVENT_COUNTER(CMN_EVENT_COUNTER_MASK))

#define CMN_EVENT_TYPE_DTC		0
#define CMN_EVENT_TYPE_HNF		1
#define CMN_EVENT_TYPE_HNI		2
#define CMN_EVENT_TYPE_RNI		3 /* Including RND */
#define CMN_EVENT_TYPE_SBSX		4
#define CMN_EVENT_TYPE_XP		5
#define CMN_EVENT_TYPE_DN		6
#define CMN_EVENT_TYPE_WP		7

#ifdef CONFIG_CMN600_PMU
extern irq_t cmn_dtc_irqs[];
void cmn600_pmu_init(void);
void cmn600_pmu_start(void);
void cmn600_pmu_stop(void);
void cmn600_pmu_configure_event(perf_evt_t evt);
void cmn600_pmu_enable_event(perf_evt_t evt);
void cmn600_pmu_disable_event(perf_evt_t evt);
perf_cnt_t cmn600_pmu_get_event_count(perf_evt_t evt);
void cmn600_pmu_set_event_count(perf_evt_t evt, perf_cnt_t cnt);
#else
#define cmn600_pmu_init()		do { } while (0)
#define cmn600_pmu_start()		do { } while (0)
#define cmn600_pmu_stop()		do { } while (0)
#define cmn600_pmu_configure_event(evt)		\
					do { } while (0)
#define cmn600_pmu_enable_event(evt)	do { } while (0)
#define cmn600_pmu_disable_event(evt)	do { } while (0)
#define cmn600_pmu_get_event_count(evt)	0
#define cmn600_pmu_set_event_count(evt, cnt)	\
					do { } while (0)
#endif

#endif /* __CMN600_PMU_H_INCLUDE__ */
