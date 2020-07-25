#include <target/perf.h>

#ifdef CONFIG_HPM_EVENT
uint8_t hpm_event2count(uint8_t event)
{
	if (event < 3)
		return event;
	else
		return perf_event_id(event) + 3;
}

void pmu_hw_configure_event(uint8_t event)
{
	uint8_t id = hpm_event2count(event);

	switch (id) {
	case HPM_CYCLE:
		csr_write(CSR_MHPMEVENT(0), event);
		break;
	case HPM_TIME:
		csr_write(CSR_MHPMEVENT(1), event);
		break;
	case HPM_INSTRET:
		csr_write(CSR_MHPMEVENT(2), event);
		break;
	case 3:
		csr_write(CSR_MHPMEVENT(3), event);
		break;
	case 4:
		csr_write(CSR_MHPMEVENT(4), event);
		break;
	case 5:
		csr_write(CSR_MHPMEVENT(5), event);
		break;
	case 6:
		csr_write(CSR_MHPMEVENT(6), event);
		break;
	case 7:
		csr_write(CSR_MHPMEVENT(7), event);
		break;
	case 8:
		csr_write(CSR_MHPMEVENT(8), event);
		break;
	case 9:
		csr_write(CSR_MHPMEVENT(9), event);
		break;
	case 10:
		csr_write(CSR_MHPMEVENT(10), event);
		break;
	case 11:
		csr_write(CSR_MHPMEVENT(11), event);
		break;
	case 12:
		csr_write(CSR_MHPMEVENT(12), event);
		break;
	case 13:
		csr_write(CSR_MHPMEVENT(13), event);
		break;
	case 14:
		csr_write(CSR_MHPMEVENT(14), event);
		break;
	case 15:
		csr_write(CSR_MHPMEVENT(15), event);
		break;
	case 16:
		csr_write(CSR_MHPMEVENT(16), event);
		break;
	case 17:
		csr_write(CSR_MHPMEVENT(17), event);
		break;
	case 18:
		csr_write(CSR_MHPMEVENT(18), event);
		break;
	case 19:
		csr_write(CSR_MHPMEVENT(19), event);
		break;
	case 20:
		csr_write(CSR_MHPMEVENT(20), event);
		break;
	case 21:
		csr_write(CSR_MHPMEVENT(21), event);
		break;
	case 22:
		csr_write(CSR_MHPMEVENT(22), event);
		break;
	case 23:
		csr_write(CSR_MHPMEVENT(23), event);
		break;
	case 24:
		csr_write(CSR_MHPMEVENT(24), event);
		break;
	case 25:
		csr_write(CSR_MHPMEVENT(25), event);
		break;
	case 26:
		csr_write(CSR_MHPMEVENT(26), event);
		break;
	case 27:
		csr_write(CSR_MHPMEVENT(27), event);
		break;
	case 28:
		csr_write(CSR_MHPMEVENT(28), event);
		break;
	case 29:
		csr_write(CSR_MHPMEVENT(29), event);
		break;
	case 30:
		csr_write(CSR_MHPMEVENT(30), event);
		break;
	case 31:
		csr_write(CSR_MHPMEVENT(31), event);
		break;
	}
}
#else
#define hpm_event2count(event)		(event)
#endif

uint32_t pmu_hw_get_event_count(uint8_t event)
{
	uint8_t id = hpm_event2count(event);

	switch (id) {
	case HPM_CYCLE:
		return csr_read(CSR_MHPMCOUNTER(0));
	case HPM_TIME:
		return csr_read(CSR_MHPMCOUNTER(1));
	case HPM_INSTRET:
		return csr_read(CSR_MHPMCOUNTER(2));
	case 3:
		return csr_read(CSR_MHPMCOUNTER(3));
	case 4:
		return csr_read(CSR_MHPMCOUNTER(4));
	case 5:
		return csr_read(CSR_MHPMCOUNTER(5));
	case 6:
		return csr_read(CSR_MHPMCOUNTER(6));
	case 7:
		return csr_read(CSR_MHPMCOUNTER(7));
	case 8:
		return csr_read(CSR_MHPMCOUNTER(8));
	case 9:
		return csr_read(CSR_MHPMCOUNTER(9));
	case 10:
		return csr_read(CSR_MHPMCOUNTER(10));
	case 11:
		return csr_read(CSR_MHPMCOUNTER(11));
	case 12:
		return csr_read(CSR_MHPMCOUNTER(12));
	case 13:
		return csr_read(CSR_MHPMCOUNTER(13));
	case 14:
		return csr_read(CSR_MHPMCOUNTER(14));
	case 15:
		return csr_read(CSR_MHPMCOUNTER(15));
	case 16:
		return csr_read(CSR_MHPMCOUNTER(16));
	case 17:
		return csr_read(CSR_MHPMCOUNTER(17));
	case 18:
		return csr_read(CSR_MHPMCOUNTER(18));
	case 19:
		return csr_read(CSR_MHPMCOUNTER(19));
	case 20:
		return csr_read(CSR_MHPMCOUNTER(20));
	case 21:
		return csr_read(CSR_MHPMCOUNTER(21));
	case 22:
		return csr_read(CSR_MHPMCOUNTER(22));
	case 23:
		return csr_read(CSR_MHPMCOUNTER(23));
	case 24:
		return csr_read(CSR_MHPMCOUNTER(24));
	case 25:
		return csr_read(CSR_MHPMCOUNTER(25));
	case 26:
		return csr_read(CSR_MHPMCOUNTER(26));
	case 27:
		return csr_read(CSR_MHPMCOUNTER(27));
	case 28:
		return csr_read(CSR_MHPMCOUNTER(28));
	case 29:
		return csr_read(CSR_MHPMCOUNTER(29));
	case 30:
		return csr_read(CSR_MHPMCOUNTER(30));
	case 31:
		return csr_read(CSR_MHPMCOUNTER(31));
	}
	return 0;
}

void pmu_hw_set_event_count(uint8_t event, uint32_t count)
{
	uint8_t id = hpm_event2count(event);

	switch (id) {
	case HPM_CYCLE:
		csr_write(CSR_MHPMCOUNTER(0), count);
		break;
	case HPM_TIME:
		csr_write(CSR_MHPMCOUNTER(1), count);
		break;
	case HPM_INSTRET:
		csr_write(CSR_MHPMCOUNTER(2), count);
		break;
	case 3:
		csr_write(CSR_MHPMCOUNTER(3), count);
		break;
	case 4:
		csr_write(CSR_MHPMCOUNTER(4), count);
		break;
	case 5:
		csr_write(CSR_MHPMCOUNTER(5), count);
		break;
	case 6:
		csr_write(CSR_MHPMCOUNTER(6), count);
		break;
	case 7:
		csr_write(CSR_MHPMCOUNTER(7), count);
		break;
	case 8:
		csr_write(CSR_MHPMCOUNTER(8), count);
		break;
	case 9:
		csr_write(CSR_MHPMCOUNTER(9), count);
		break;
	case 10:
		csr_write(CSR_MHPMCOUNTER(10), count);
		break;
	case 11:
		csr_write(CSR_MHPMCOUNTER(11), count);
		break;
	case 12:
		csr_write(CSR_MHPMCOUNTER(12), count);
		break;
	case 13:
		csr_write(CSR_MHPMCOUNTER(13), count);
		break;
	case 14:
		csr_write(CSR_MHPMCOUNTER(14), count);
		break;
	case 15:
		csr_write(CSR_MHPMCOUNTER(15), count);
		break;
	case 16:
		csr_write(CSR_MHPMCOUNTER(16), count);
		break;
	case 17:
		csr_write(CSR_MHPMCOUNTER(17), count);
		break;
	case 18:
		csr_write(CSR_MHPMCOUNTER(18), count);
		break;
	case 19:
		csr_write(CSR_MHPMCOUNTER(19), count);
		break;
	case 20:
		csr_write(CSR_MHPMCOUNTER(20), count);
		break;
	case 21:
		csr_write(CSR_MHPMCOUNTER(21), count);
		break;
	case 22:
		csr_write(CSR_MHPMCOUNTER(22), count);
		break;
	case 23:
		csr_write(CSR_MHPMCOUNTER(23), count);
		break;
	case 24:
		csr_write(CSR_MHPMCOUNTER(24), count);
		break;
	case 25:
		csr_write(CSR_MHPMCOUNTER(25), count);
		break;
	case 26:
		csr_write(CSR_MHPMCOUNTER(26), count);
		break;
	case 27:
		csr_write(CSR_MHPMCOUNTER(27), count);
		break;
	case 28:
		csr_write(CSR_MHPMCOUNTER(28), count);
		break;
	case 29:
		csr_write(CSR_MHPMCOUNTER(29), count);
		break;
	case 30:
		csr_write(CSR_MHPMCOUNTER(30), count);
		break;
	case 31:
		csr_write(CSR_MHPMCOUNTER(31), count);
		break;
	}
}
