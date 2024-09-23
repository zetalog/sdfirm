#include <target/perf.h>
#include <target/cmdline.h>
#include <target/console.h>

struct hpm_cpu {
	DECLARE_BITMAP(configured, HPM_MAX_COUNTERS);
	uint64_t events[HPM_MAX_COUNTERS];
	perf_evt_t perf_eids[HPM_MAX_COUNTERS];
	perf_cnt_t perf_snaps[HPM_MAX_COUNTERS];
	perf_cnt_t perf_inits[HPM_MAX_COUNTERS];
	uint32_t overflows[HPM_MAX_COUNTERS];
};

#ifdef CONFIG_SMP
DEFINE_PERCPU(struct hpm_cpu, hpm_ctx);
#define get_hpm			this_cpu_ptr(&hpm_ctx)
#define get_hpm_cpu(cpu)	per_cpu_ptr(&hpm_ctx, cpu)
#else
struct hpm_cpu hpm_ctx;
#define get_hpm			(&hpm_ctx)
#define get_cpu_hpm(cpu)	(&hpm_ctx)
#endif
#define hpm_configured		get_hpm->configured
#define hpm_events		get_hpm->events
#define hpm_perf_eids		get_hpm->perf_eids
#define hpm_perf_snaps		get_hpm->perf_snaps
#define hpm_perf_inits		get_hpm->perf_inits

#define HPM_MAX_EVENTS		CONFIG_HPM_MAX_EVENTS

const char *hpmnames[HPM_MAX_EVENTS];
uint64_t hpmevents[HPM_MAX_EVENTS];
int hpm_max_events;

#ifdef CONFIG_RISCV_SSCOFPMF
#else
#endif

uint8_t hpm_event2count(perf_evt_t evt)
{
	uint8_t cnt;

	cnt = find_first_set_bit(hpm_configured, HPM_MAX_COUNTERS);
	while (cnt < HPM_MAX_COUNTERS) {
		if (evt == hpm_perf_eids[cnt])
			return cnt;
		cnt = find_next_set_bit(hpm_configured, HPM_MAX_COUNTERS, cnt+1);
	}
	return 0;
}

uint64_t hpm_event2event(perf_evt_t evt)
{
	uint8_t cnt;

	cnt = find_first_set_bit(hpm_configured, HPM_MAX_COUNTERS);
	while (cnt < HPM_MAX_COUNTERS) {
		if (evt == hpm_perf_eids[cnt])
			return hpm_events[cnt];
		cnt = find_next_set_bit(hpm_configured, HPM_MAX_COUNTERS, cnt+1);
	}
	return HPM_NO_EVENT;
}

void hpm_configure_event(perf_evt_t evt)
{
	uint8_t id = hpm_event2count(evt);
	uint64_t event = hpm_event2event(evt);

	if (event < 3)
		return;

	/* first 3 events are not programmable */
	event -= 3;
	/* 0 meas NO_EVENT */
	event += 1;

	switch (id) {
	case 3:
		csr_write(CSR_HPMEVENT(3), event);
		break;
	case 4:
		csr_write(CSR_HPMEVENT(4), event);
		break;
	case 5:
		csr_write(CSR_HPMEVENT(5), event);
		break;
	case 6:
		csr_write(CSR_HPMEVENT(6), event);
		break;
	case 7:
		csr_write(CSR_HPMEVENT(7), event);
		break;
	case 8:
		csr_write(CSR_HPMEVENT(8), event);
		break;
	case 9:
		csr_write(CSR_HPMEVENT(9), event);
		break;
	case 10:
		csr_write(CSR_HPMEVENT(10), event);
		break;
	case 11:
		csr_write(CSR_HPMEVENT(11), event);
		break;
	case 12:
		csr_write(CSR_HPMEVENT(12), event);
		break;
	case 13:
		csr_write(CSR_HPMEVENT(13), event);
		break;
	case 14:
		csr_write(CSR_HPMEVENT(14), event);
		break;
	case 15:
		csr_write(CSR_HPMEVENT(15), event);
		break;
	case 16:
		csr_write(CSR_HPMEVENT(16), event);
		break;
	case 17:
		csr_write(CSR_HPMEVENT(17), event);
		break;
	case 18:
		csr_write(CSR_HPMEVENT(18), event);
		break;
	case 19:
		csr_write(CSR_HPMEVENT(19), event);
		break;
	case 20:
		csr_write(CSR_HPMEVENT(20), event);
		break;
	case 21:
		csr_write(CSR_HPMEVENT(21), event);
		break;
	case 22:
		csr_write(CSR_HPMEVENT(22), event);
		break;
	case 23:
		csr_write(CSR_HPMEVENT(23), event);
		break;
	case 24:
		csr_write(CSR_HPMEVENT(24), event);
		break;
	case 25:
		csr_write(CSR_HPMEVENT(25), event);
		break;
	case 26:
		csr_write(CSR_HPMEVENT(26), event);
		break;
	case 27:
		csr_write(CSR_HPMEVENT(27), event);
		break;
	case 28:
		csr_write(CSR_HPMEVENT(28), event);
		break;
	case 29:
		csr_write(CSR_HPMEVENT(29), event);
		break;
	case 30:
		csr_write(CSR_HPMEVENT(30), event);
		break;
	case 31:
		csr_write(CSR_HPMEVENT(31), event);
		break;
	}
	hpm_perf_inits[id] = hpm_get_event_count(evt);
}

perf_cnt_t hpm_get_event_count(perf_evt_t evt)
{
	uint8_t id = hpm_event2count(evt);

	switch (id) {
	case HPM_CYCLE:
		return csr_read(CSR_HPMCOUNTER(0));
	case HPM_TIME:
		return 0;
	case HPM_INSTRET:
		return csr_read(CSR_HPMCOUNTER(2));
	case 3:
		return csr_read(CSR_HPMCOUNTER(3));
	case 4:
		return csr_read(CSR_HPMCOUNTER(4));
	case 5:
		return csr_read(CSR_HPMCOUNTER(5));
	case 6:
		return csr_read(CSR_HPMCOUNTER(6));
	case 7:
		return csr_read(CSR_HPMCOUNTER(7));
	case 8:
		return csr_read(CSR_HPMCOUNTER(8));
	case 9:
		return csr_read(CSR_HPMCOUNTER(9));
	case 10:
		return csr_read(CSR_HPMCOUNTER(10));
	case 11:
		return csr_read(CSR_HPMCOUNTER(11));
	case 12:
		return csr_read(CSR_HPMCOUNTER(12));
	case 13:
		return csr_read(CSR_HPMCOUNTER(13));
	case 14:
		return csr_read(CSR_HPMCOUNTER(14));
	case 15:
		return csr_read(CSR_HPMCOUNTER(15));
	case 16:
		return csr_read(CSR_HPMCOUNTER(16));
	case 17:
		return csr_read(CSR_HPMCOUNTER(17));
	case 18:
		return csr_read(CSR_HPMCOUNTER(18));
	case 19:
		return csr_read(CSR_HPMCOUNTER(19));
	case 20:
		return csr_read(CSR_HPMCOUNTER(20));
	case 21:
		return csr_read(CSR_HPMCOUNTER(21));
	case 22:
		return csr_read(CSR_HPMCOUNTER(22));
	case 23:
		return csr_read(CSR_HPMCOUNTER(23));
	case 24:
		return csr_read(CSR_HPMCOUNTER(24));
	case 25:
		return csr_read(CSR_HPMCOUNTER(25));
	case 26:
		return csr_read(CSR_HPMCOUNTER(26));
	case 27:
		return csr_read(CSR_HPMCOUNTER(27));
	case 28:
		return csr_read(CSR_HPMCOUNTER(28));
	case 29:
		return csr_read(CSR_HPMCOUNTER(29));
	case 30:
		return csr_read(CSR_HPMCOUNTER(30));
	case 31:
		return csr_read(CSR_HPMCOUNTER(31));
	}
	return 0;
}

void hpm_set_event_count(perf_evt_t evt, perf_cnt_t count)
{
	uint8_t id = hpm_event2count(evt);

	switch (id) {
	case HPM_CYCLE:
		csr_write(CSR_HPMCOUNTER(0), count);
		break;
	case HPM_TIME:
		csr_write(CSR_HPMCOUNTER(1), count);
		break;
	case HPM_INSTRET:
		csr_write(CSR_HPMCOUNTER(2), count);
		break;
	case 3:
		csr_write(CSR_HPMCOUNTER(3), count);
		break;
	case 4:
		csr_write(CSR_HPMCOUNTER(4), count);
		break;
	case 5:
		csr_write(CSR_HPMCOUNTER(5), count);
		break;
	case 6:
		csr_write(CSR_HPMCOUNTER(6), count);
		break;
	case 7:
		csr_write(CSR_HPMCOUNTER(7), count);
		break;
	case 8:
		csr_write(CSR_HPMCOUNTER(8), count);
		break;
	case 9:
		csr_write(CSR_HPMCOUNTER(9), count);
		break;
	case 10:
		csr_write(CSR_HPMCOUNTER(10), count);
		break;
	case 11:
		csr_write(CSR_HPMCOUNTER(11), count);
		break;
	case 12:
		csr_write(CSR_HPMCOUNTER(12), count);
		break;
	case 13:
		csr_write(CSR_HPMCOUNTER(13), count);
		break;
	case 14:
		csr_write(CSR_HPMCOUNTER(14), count);
		break;
	case 15:
		csr_write(CSR_HPMCOUNTER(15), count);
		break;
	case 16:
		csr_write(CSR_HPMCOUNTER(16), count);
		break;
	case 17:
		csr_write(CSR_HPMCOUNTER(17), count);
		break;
	case 18:
		csr_write(CSR_HPMCOUNTER(18), count);
		break;
	case 19:
		csr_write(CSR_HPMCOUNTER(19), count);
		break;
	case 20:
		csr_write(CSR_HPMCOUNTER(20), count);
		break;
	case 21:
		csr_write(CSR_HPMCOUNTER(21), count);
		break;
	case 22:
		csr_write(CSR_HPMCOUNTER(22), count);
		break;
	case 23:
		csr_write(CSR_HPMCOUNTER(23), count);
		break;
	case 24:
		csr_write(CSR_HPMCOUNTER(24), count);
		break;
	case 25:
		csr_write(CSR_HPMCOUNTER(25), count);
		break;
	case 26:
		csr_write(CSR_HPMCOUNTER(26), count);
		break;
	case 27:
		csr_write(CSR_HPMCOUNTER(27), count);
		break;
	case 28:
		csr_write(CSR_HPMCOUNTER(28), count);
		break;
	case 29:
		csr_write(CSR_HPMCOUNTER(29), count);
		break;
	case 30:
		csr_write(CSR_HPMCOUNTER(30), count);
		break;
	case 31:
		csr_write(CSR_HPMCOUNTER(31), count);
		break;
	}
}

const char *hpm_event_name(uint64_t event)
{
	int i;

	for (i = 0; i < hpm_max_events; i++) {
		if (hpmevents[i] == event)
			return hpmnames[i];
	}
	return NULL;
}

void hpm_start(void)
{
	uint8_t cnt;

	cnt = find_first_set_bit(hpm_configured, HPM_MAX_COUNTERS);
	while (cnt < HPM_MAX_COUNTERS) {
		hpm_perf_eids[cnt] = perf_add_event(hpm_event_name(hpm_events[cnt]));
		perf_configure_event(hpm_perf_eids[cnt]);
		perf_enable_event(hpm_perf_eids[cnt]);
		cnt = find_next_set_bit(hpm_configured, HPM_MAX_COUNTERS, cnt+1);
	}
}

void hpm_stop(void)
{
	uint8_t cnt;

	cnt = find_first_set_bit(hpm_configured, HPM_MAX_COUNTERS);
	while (cnt < HPM_MAX_COUNTERS) {
		hpm_perf_snaps[cnt] = hpm_get_event_count(cnt);
		perf_disable_event(hpm_perf_eids[cnt]);
		cnt = find_next_set_bit(hpm_configured, HPM_MAX_COUNTERS, cnt+1);
	}
}

void hpm_register_event(uint64_t event, const char *name)
{
	int i;

	if (hpm_max_events < HPM_MAX_EVENTS) {
		for (i = 0; i < hpm_max_events; i++) {
			if (hpmevents[i] == event) {
				con_err("hpm: already registered %016llx - %s!=%s\n",
					event, name, hpmnames[i]);
				return;
			}
		}
		hpmevents[hpm_max_events] = event;
		hpmnames[hpm_max_events] = name;
		hpm_max_events++;
	}
}

void hpm_add_event(cpu_t cpu, uint64_t event)
{
	uint8_t cnt;

	cnt = find_first_clear_bit(get_cpu_hpm(cpu)->configured,
				   HPM_MAX_COUNTERS);
	if (cnt >= HPM_MAX_COUNTERS)
		con_err("hpm: failed to register %016llx on cpu%d\n",
			event, cpu);
	else {
		set_bit(cnt, get_cpu_hpm(cpu)->configured);
		get_cpu_hpm(cpu)->events[cnt] = event;
		get_cpu_hpm(cpu)->perf_eids[cnt] = INVALID_PERF_EVT;
	}
}

void hpm_remove_event(cpu_t cpu, uint64_t event)
{
	uint8_t cnt;

	cnt = find_first_set_bit(get_cpu_hpm(cpu)->configured,
				 HPM_MAX_COUNTERS);
	while (cnt < HPM_MAX_COUNTERS) {
		if (get_cpu_hpm(cpu)->events[cnt] == event)
			clear_bit(cnt, get_cpu_hpm(cpu)->configured);
		cnt = find_next_set_bit(get_cpu_hpm(cpu)->configured,
					HPM_MAX_COUNTERS, cnt+1);
	}
}

void hpm_enable_event(perf_evt_t evt)
{
	uint8_t id = hpm_event2count(evt);

	csr_clear(CSR_COUNTINHIBIT, _BV(id));
}

void hpm_disable_event(perf_evt_t evt)
{
	uint8_t id = hpm_event2count(evt);

	csr_set(CSR_COUNTINHIBIT, _BV(id));
}

void hpm_init(void)
{
	cpu_t cpu;

	hpm_register_event(0, "cpu:CY");
	hpm_register_event(1, "cpu:TM");
	hpm_register_event(2, "cpu:IR");
	for_each_cpu(cpu, smp_online_cpus) {
		hpm_add_event(cpu, HPM_CYCLE);
		hpm_add_event(cpu, HPM_TIME);
		hpm_add_event(cpu, HPM_INSTRET);
	}
}

void hpm_dump_events(cpu_t cpu)
{
	uint8_t cnt;

	cnt = find_first_set_bit(get_cpu_hpm(cpu)->configured,
				 HPM_MAX_COUNTERS);
	while (cnt < HPM_MAX_COUNTERS) {
		printf("%016llx: %016llx - %d -> %016llx\n",
		       get_cpu_hpm(cpu)->events[cnt],
		       get_cpu_hpm(cpu)->perf_inits[cnt],
		       get_cpu_hpm(cpu)->overflows[cnt],
		       get_cpu_hpm(cpu)->perf_snaps[cnt]);
		cnt = find_next_set_bit(get_cpu_hpm(cpu)->configured,
					HPM_MAX_COUNTERS, cnt+1);
	}
}

static int do_hpm_dump(int argc, char *argv[])
{
	cpu_t cpu;

	if (argc > 2) {
		cpu = (cpu_t)strtoull(argv[2], 0, 0);
		hpm_dump_events(cpu);
	} else {
		for_each_cpu(cpu, smp_online_cpus) {
			hpm_dump_events(cpu);
		}
	}
	return 0;
}

static int do_hpm_remove(int argc, char *argv[])
{
	cpu_t cpu;
	uint64_t event;

	if (argc < 3)
		return -EINVAL;
	event = (uint64_t)strtoull(argv[2], 0, 0);
	if (event < 3) {
		printf("CY/TM/IR are not configurable\n");
		return -EINVAL;
	}
	if (argc > 3) {
		cpu = (cpu_t)strtoull(argv[3], 0, 0);
		hpm_remove_event(cpu, event);
	} else {
		for_each_cpu(cpu, smp_online_cpus)
			hpm_remove_event(cpu, event);
	}
	return 0;
}

static int do_hpm_add(int argc, char *argv[])
{
	cpu_t cpu;
	uint64_t event;

	if (argc < 3)
		return -EINVAL;
	event = (uint64_t)strtoull(argv[2], 0, 0);
	if (event < 3) {
		printf("CY/TM/IR are not configurable\n");
		return -EINVAL;
	}
	if (argc > 3) {
		cpu = (cpu_t)strtoull(argv[3], 0, 0);
		hpm_add_event(cpu, event);
	} else {
		for_each_cpu(cpu, smp_online_cpus)
			hpm_add_event(cpu, event);
	}
	return 0;
}

static int do_hpm(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "dump") == 0)
		return do_hpm_dump(argc, argv);
	if (strcmp(argv[1], "add") == 0)
		return do_hpm_add(argc, argv);
	if (strcmp(argv[1], "remove") == 0)
		return do_hpm_remove(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(hpm, do_hpm, "Hardware performance monitor (HPM) commands",
	"hpm dump [cpu]\n"
	"    -dump all HPM events\n"
	"hpm add <event> [cpu]\n"
	"    -register HPM event\n"
	"hpm remove <event> [cpu]\n"
	"    -deregister HPM event\n"
);
