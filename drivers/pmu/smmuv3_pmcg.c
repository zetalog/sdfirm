#include <target/perf.h>
#include <target/console.h>
#include <target/cmdline.h>

cpu_t pmcg_cpu;

struct smmu_pmu smmu_pmus[NR_IOMMU_DEVICES];

#define for_each_sibling_event(sibling, event)			\
	if ((event)->group_leader == (event))			\
		list_for_each_entry(struct smmu_perf_event, (sibling), &(event)->sibling_list, sibling_list)

static int smmu_pmcg_apply_event_filter(struct smmu_perf_event *event, int idx);

const char *smmu_pmcg_event_names[] = {
	"cycles",
	"transaction",
	"tlb_miss",
	"config_cache_miss",
	"trans_table_walk_access",
	"config_struct_access",
	"pcie_ats_trans_rq",
	"pcie_ats_trans_passed",
};

const char *smmu_pmcg_event_name(uint32_t val)
{
	if (val >= ARRAY_SIZE(smmu_pmcg_event_names))
		return "UNKNOWN";
	return smmu_pmcg_event_names[val];
}

static void smmu_pmcg_event_update(struct smmu_perf_event *event)
{
	uint64_t delta, prev, now;
	uint32_t idx = event->idx;

	do {
		prev = READ_ONCE(event->prev_count);
		now = smmu_pmcg_counter_get_value(idx);
	} while (cmpxchg(&event->prev_count, prev, now) != prev);

	/* handle overflow. */
	delta = now - prev;
	delta &= smmu_pmu_ctrl.counter_mask;

	event->count += delta;
}

static void smmu_pmcg_set_period(struct smmu_perf_event *event)
{
	uint32_t idx = event->idx;
	uint64_t new;

	/* We limit the max period to half the max counter value
	 * of the counter size, so that even in the case of extreme
	 * interrupt latency the counter will (hopefully) not wrap
	 * past its initial value.
	 */
	new = smmu_pmu_ctrl.counter_mask >> 1;
	smmu_pmcg_counter_set_value(idx, new);

	event->prev_count += new;
}

static void smmu_pmcg_set_event_filter(struct smmu_perf_event *event,
				       int idx, uint32_t span, uint32_t sid)
{
	uint32_t evtyper;

	evtyper = event->event | span << SMMU_PMCG_SID_SPAN_SHIFT;
	smmu_pmcg_set_evtyper(idx, evtyper);
	smmu_pmcg_set_smr(idx, sid);
}

static bool smmu_pmcg_check_global_filter(struct smmu_perf_event *curr,
					  struct smmu_perf_event *new)
{
	if (new->filter != curr->filter)
		return false;

	if (!new->filter)
		return true;

	return new->filter_sid_span == curr->filter_sid_span &&
	       new->streamid == curr->streamid;
}

static int smmu_pmcg_apply_event_filter(struct smmu_perf_event *event, int idx)
{
	uint32_t span, sid;
	unsigned int cur_idx, num_ctrs = smmu_pmu_ctrl.num_counters;
	bool filter_en = !!event->filter;

	span = filter_en ? event->filter_sid_span : SMMU_PMCG_DEFAULT_FILTER_SPAN;
	sid = filter_en ? event->streamid : SMMU_PMCG_DEFAULT_FILTER_SID;

	cur_idx = find_first_set_bit(smmu_pmu_ctrl.used_counters, num_ctrs);
	/*
	 * Per-counter filtering, or scheduling the first globally-filtered
	 * event into an empty PMU so idx == 0 and it works out equivalent.
	 */
	if (!smmu_pmu_ctrl.global_filter || cur_idx == num_ctrs) {
		smmu_pmcg_set_event_filter(event, idx, span, sid);
		return 0;
	}

	/* Otherwise, must match whatever's currently scheduled */
	if (smmu_pmcg_check_global_filter(&smmu_pmu_ctrl.events[cur_idx], event)) {
		smmu_pmcg_set_evtyper(idx, event->event);
		return 0;
	}

	return -EAGAIN;
}

static int smmu_pmcg_get_event_idx(uint16_t evtype, bool filter, uint32_t streamid, bool span)
{
	int idx, err;
	unsigned int num_ctrs = smmu_pmu_ctrl.num_counters;
	struct smmu_perf_event *event;

	idx = find_first_clear_bit(smmu_pmu_ctrl.used_counters, num_ctrs);
	if (idx == num_ctrs)
		/* The counters are all in use. */
		return -EAGAIN;

	event = &smmu_pmu_ctrl.events[idx];
	event->event = evtype;
	event->filter = filter;
	event->filter_sid_span = span;
	if (streamid) {
		event->filter_sid_span = true;
		event->streamid = streamid;
	}
	err = smmu_pmcg_apply_event_filter(event, idx);
	if (err)
		return err;

	set_bit(idx, smmu_pmu_ctrl.used_counters);
	return idx;
}

static bool smmu_pmcg_events_compatible(struct smmu_perf_event *curr,
				       struct smmu_perf_event *new)
{
	if (smmu_pmu_ctrl.global_filter &&
	    !smmu_pmcg_check_global_filter(curr, new))
		return false;

	return true;
}

/*
 * Implementation of abstract pmu functionality required by
 * the core perf events code.
 */

static int smmu_pmcg_event_init(struct smmu_perf_event *event)
{
	struct smmu_perf_event *sibling;
	int group_num_events = 1;
	uint16_t event_id;

	/* Verify specified event is supported on this PMU */
	event_id = event->event;
	if (event_id < SMMU_PMCG_ARCH_MAX_EVENTS &&
	    (!test_bit(event_id, smmu_pmu_ctrl.supported_events))) {
		con_dbg("smcg: Invalid event %d for this PMU\n", event_id);
		return -EINVAL;
	}

	/* Don't allow groups with mixed PMUs, except for s/w events */
	if (!smmu_pmcg_events_compatible(event->group_leader, event))
		return -EINVAL;
	if (++group_num_events > smmu_pmu_ctrl.num_counters)
		return -EINVAL;

	for_each_sibling_event(sibling, event->group_leader) {
		if (!smmu_pmcg_events_compatible(sibling, event))
			return -EINVAL;
		if (++group_num_events > smmu_pmu_ctrl.num_counters)
			return -EINVAL;
	}

	event->idx = -1;
	return 0;
}

static void smmu_pmcg_event_start(struct smmu_perf_event *event)
{
	int idx = event->idx;

	event->state = 0;

	smmu_pmcg_set_period(event);
	smmu_pmcg_counter_enable(idx);
}

static void smmu_pmcg_event_stop(struct smmu_perf_event *event)
{
	int idx = event->idx;

	if (event->state & PMCG_STOPPED)
		return;

	smmu_pmcg_counter_disable(idx);
	/* As the counter gets updated on _start, ignore PERF_EF_UPDATE */
	smmu_pmcg_event_update(event);
	event->state |= PMCG_STOPPED | PMCG_UPTODATE;
}

int smmu_pmcg_event_add(uint16_t evtype, bool filter, uint32_t streamid, bool span)
{
	int idx;
	struct smmu_perf_event *event;

	idx = smmu_pmcg_get_event_idx(evtype, filter, streamid, span);
	if (idx < 0)
		return idx;

	event = &smmu_pmu_ctrl.events[idx];
	event->idx = idx;
	event->state = PMCG_STOPPED | PMCG_UPTODATE;
	WRITE_ONCE(event->prev_count, 0);

	smmu_pmcg_interrupt_enable(idx);
	return 0;
}

void smmu_pmcg_event_del(uint32_t idx)
{
	struct smmu_perf_event *event;

	if (idx >= smmu_pmu_ctrl.num_counters)
		return;
	if (!test_bit(idx, smmu_pmu_ctrl.used_counters))
		return;

	event = &smmu_pmu_ctrl.events[idx];
	smmu_pmcg_event_stop(event);
	smmu_pmcg_interrupt_disable(idx);
	event->idx = -1;
	event->event = 0;
	event->filter = false;
	event->filter_sid_span = false;
	event->streamid = 0;
	clear_bit(idx, smmu_pmu_ctrl.used_counters);
}

static void smmu_pmcg_event_read(struct smmu_perf_event *event)
{
	smmu_pmcg_event_update(event);
}

void smmu_pmcg_event_dump(void)
{
	uint32_t idx;

	idx = find_first_set_bit(smmu_pmu_ctrl.used_counters, smmu_pmu_ctrl.num_counters);
	while (idx < smmu_pmu_ctrl.num_counters) {
		struct smmu_perf_event *event = &smmu_pmu_ctrl.events[idx];

		if (!event)
			continue;

		smmu_pmcg_event_read(event);
		printf("%d: %s=%016llx\n",
		       idx, smmu_pmcg_event_name(event->event),
		       event->count);
		idx = find_next_set_bit(smmu_pmu_ctrl.used_counters,
					smmu_pmu_ctrl.num_counters, idx+1);
	}
}

iommu_dev_t smmu_pmu_irq2iommu(irq_t irq)
{
	iommu_dev_t dev;
	__unused iommu_dev_t sdev;
	irq_t pmu_irq;

	for (dev = 0; dev < NR_IOMMU_DEVICES; dev++) {
		sdev = iommu_device_save(dev);
		pmu_irq = SMMU_HW_IRQ_PMU(dev);
		iommu_device_restore(sdev);
		if (pmu_irq == irq)
			return dev;
	}
	return INVALID_IOMMU_DEV;
}

static void smmu_pmcg_handle_irq(irq_t irq)
{
	uint64_t ovsr;
	DECLARE_BITMAP(ovs, 64);
	unsigned int idx;
	__unused iommu_dev_t dev = smmu_pmu_irq2iommu(irq);
	__unused iommu_dev_t sdev;

	sdev = iommu_device_save(dev);
	ovsr = __raw_readq(TCU_PMCG_OVSSET0(iommu_dev));
	if (!ovsr)
		goto err_exit;

	__raw_writeq(ovsr, TCU_PMCG_OVSCLR0(iommu_dev));

	idx = find_first_set_bit(ovs, smmu_pmu_ctrl.num_counters);
	while (idx < smmu_pmu_ctrl.num_counters) {
		struct smmu_perf_event *event = &smmu_pmu_ctrl.events[idx];

		if (!event)
			continue;

		smmu_pmcg_event_update(event);
		smmu_pmcg_set_period(event);
		idx = find_next_set_bit(ovs, smmu_pmu_ctrl.num_counters, idx+1);
	}
err_exit:
	iommu_device_restore(sdev);
}

#ifdef CONFIG_SMMU_PMCG_MSI
static void smmu_pmcg_free_msis(void)
{
#if 0
	msi_free_irqs_all(iommu_dev);
#endif
}

static void smmu_pmcg_write_msi_msg(struct msi_desc *desc, struct msi_msg *msg)
{
	phys_addr_t doorbell;

	doorbell = (((uint64_t)msg->address_hi) << 32) | msg->address_lo;
	doorbell &= MSI_CFG0_ADDR_MASK;

	writeq_relaxed(doorbell, SMMU_PMCG_IRQ_CFG0(iommu_dev));
	writel_relaxed(msg->data, SMMU_PMCG_IRQ_CFG1(iommu_dev));
	writel_relaxed(MSI_CFG2_MEMATTR_DEVICE_nGnRE, SMMU_PMCG_IRQ_CFG2(iommu_dev));
}

static void smmu_pmcg_setup_msi(void)
{
	int ret;

	/* Clear MSI address reg */
	__raw_writeq(0, TCU_PMCG_IRQ_CFG0(iommu_dev));

	/* MSI supported or not */
	if (!(__raw_readl(TCU_PMCG_CFGR(iommu_dev)) & SMMU_PMCG_CFGR_MSI))
		return;

#if 0
	msi_init_and_alloc_irqs(iommu_dev, smmu_pmcg_alloc_msis);
	pmu->irq = msi_get_virq(dev, 0);
	smmu_pmcg_free_msis();
#endif
}
#else
#define smmu_pmcg_setup_msi()		do { } while (0)
#endif

static void smmu_pmcg_setup_irq(void)
{
	smmu_pmcg_setup_msi();
	irq_register_vector(SMMU_HW_IRQ_PMU(iommu_dev), smmu_pmcg_handle_irq);
}

static void smmu_pmcg_reset(void)
{
	uint64_t counter_present_mask = GENMASK_ULL(smmu_pmu_ctrl.num_counters - 1, 0);

	smmu_pmcg_disable();

	/* Disable counter and interrupt */
	__raw_writeq(counter_present_mask, TCU_PMCG_CNTENCLR0(iommu_dev));
	__raw_writeq(counter_present_mask, TCU_PMCG_INTENCLR0(iommu_dev));
	__raw_writeq(counter_present_mask, TCU_PMCG_OVSCLR0(iommu_dev));
}

static void smmu_pmu_get_iidr(void)
{
	uint32_t pidr0 = __raw_readl(TCU_PMCG_PIDR0(iommu_dev));
	uint32_t pidr1 = __raw_readl(TCU_PMCG_PIDR1(iommu_dev));
	uint32_t pidr2 = __raw_readl(TCU_PMCG_PIDR2(iommu_dev));
	uint32_t pidr3 = __raw_readl(TCU_PMCG_PIDR3(iommu_dev));
	uint32_t pidr4 = __raw_readl(TCU_PMCG_PIDR4(iommu_dev));

	uint32_t productid = FIELD_GET(SMMU_PMCG_PIDR0_PART_0, pidr0) |
			    (FIELD_GET(SMMU_PMCG_PIDR1_PART_1, pidr1) << 8);
	uint32_t variant = FIELD_GET(SMMU_PMCG_PIDR2_REVISION, pidr2);
	uint32_t revision = FIELD_GET(SMMU_PMCG_PIDR3_REVAND, pidr3);
	uint32_t implementer = FIELD_GET(SMMU_PMCG_PIDR1_DES_0, pidr1) |
			      (FIELD_GET(SMMU_PMCG_PIDR2_DES_1, pidr2) << 4) |
			      (FIELD_GET(SMMU_PMCG_PIDR4_DES_2, pidr4) << 8);

	smmu_pmu_ctrl.iidr = FIELD_PREP(SMMU_PMCG_IIDR_PRODUCTID, productid) |
			     FIELD_PREP(SMMU_PMCG_IIDR_VARIANT, variant) |
			     FIELD_PREP(SMMU_PMCG_IIDR_REVISION, revision) |
			     FIELD_PREP(SMMU_PMCG_IIDR_IMPLEMENTER, implementer);
}

void __smmu_pmcg_exit(void)
{
	smmu_pmcg_disable();
}

void __smmu_pmcg_init(void)
{
	uint32_t cfgr, reg_size;
	uint8_t i;

	cfgr = __raw_readl(TCU_PMCG_CFGR(iommu_dev));

	/* Determine if page 1 is present */
	if (cfgr & SMMU_PMCG_CFGR_RELOC_CTRS) {
		smmu_pmu_ctrl.reloc_base = (SMMU_PAGESIZE << 1);
	} else {
		smmu_pmu_ctrl.reloc_base = 0;
	}

	for (i = 0; i < SMMU_PMCG_ARCH_MAX_EVENTS; i++) {
		if (__raw_readq(TCU_PMCG_CEID(iommu_dev, i)) & PMCG_CEID(i))
			set_bit(i, smmu_pmu_ctrl. supported_events);
	}

	/* Preferred CPU to handle PMU events */
	pmcg_cpu = smp_processor_id();

	smmu_pmu_ctrl.num_counters = FIELD_GET(SMMU_PMCG_CFGR_NCTR, cfgr) + 1;
	smmu_pmu_ctrl.global_filter = !!(cfgr & SMMU_PMCG_CFGR_SID_FILTER_TYPE);

	reg_size = FIELD_GET(SMMU_PMCG_CFGR_SIZE, cfgr);
	smmu_pmu_ctrl.counter_mask = GENMASK_ULL(reg_size, 0);

	smmu_pmcg_reset();
	smmu_pmcg_setup_irq();
	smmu_pmu_get_iidr();

	con_log("pmcg: counters=%d filters=%s\n",
		smmu_pmu_ctrl.num_counters,
		smmu_pmu_ctrl.global_filter ? "Global(Counter0)" : "Individual");
}

static void __smmu_pmcg_start(void)
{
	uint32_t idx;

	idx = find_first_set_bit(smmu_pmu_ctrl.used_counters, smmu_pmu_ctrl.num_counters);
	while (idx < smmu_pmu_ctrl.num_counters) {
		struct smmu_perf_event *event = &smmu_pmu_ctrl.events[idx];
		if (!event)
			continue;
		smmu_pmcg_event_start(event);
		idx = find_next_set_bit(smmu_pmu_ctrl.used_counters,
					smmu_pmu_ctrl.num_counters, idx+1);
	}
}

static void __smmu_pmcg_stop(void)
{
	uint32_t idx;

	idx = find_first_set_bit(smmu_pmu_ctrl.used_counters, smmu_pmu_ctrl.num_counters);
	while (idx < smmu_pmu_ctrl.num_counters) {
		struct smmu_perf_event *event = &smmu_pmu_ctrl.events[idx];
		if (!event)
			continue;
		smmu_pmcg_event_stop(event);
		idx = find_next_set_bit(smmu_pmu_ctrl.used_counters,
					smmu_pmu_ctrl.num_counters, idx+1);
	}
}

void smmu_pmcg_start(void)
{
	__unused iommu_dev_t dev, sdev;

	for (dev = 0; dev < NR_IOMMU_DEVICES; dev++) {
		sdev = iommu_device_save(dev);
		if (iommu_device_ctrl.valid)
			__smmu_pmcg_start();
		iommu_device_restore(sdev);
	}
}

void smmu_pmcg_stop(void)
{
	__unused iommu_dev_t dev, sdev;

	for (dev = 0; dev < NR_IOMMU_DEVICES; dev++) {
		sdev = iommu_device_save(dev);
		if (iommu_device_ctrl.valid)
			__smmu_pmcg_stop();
		iommu_device_restore(sdev);
	}
}

void smmu_pmcg_init(void)
{
	__unused iommu_dev_t dev, sdev;

	for (dev = 0; dev < NR_IOMMU_DEVICES; dev++) {
		sdev = iommu_device_save(dev);
		if (iommu_device_ctrl.valid)
			__smmu_pmcg_init();
		iommu_device_restore(sdev);
	}
}

static int do_pmcg_remove(int argc, char *argv[])
{
	__unused iommu_dev_t dev;
	__unused iommu_dev_t sdev;
	uint32_t idx;

	if (argc < 4)
		return -EINVAL;
	dev = (iommu_dev_t)strtoull(argv[2], 0, 0);
	idx = (uint32_t)strtoull(argv[3], 0, 0);
	sdev = iommu_device_save(dev);
	smmu_pmcg_event_del(idx);
	iommu_device_restore(sdev);
	return 0;
}

static int do_pmcg_add(int argc, char *argv[])
{
	__unused iommu_dev_t dev;
	__unused iommu_dev_t sdev;
	uint16_t event;
	bool filter = false;
	uint32_t streamid = 0;
	bool span = false;

	if (argc < 4)
		return -EINVAL;
	dev = (iommu_dev_t)strtoull(argv[2], 0, 0);
	event = (uint16_t)strtoull(argv[3], 0, 0);
	if (argc > 4) {
		filter = true;
		streamid = (uint32_t)strtoull(argv[4], 0, 0);
		if (argc > 5) {
			if (strcmp(argv[5], "span") == 0)
				span = true;
		}
	}
	dev = (iommu_dev_t)strtoull(argv[2], 0, 0);
	sdev = iommu_device_save(dev);
	smmu_pmcg_event_add(event, filter, streamid, span);
	iommu_device_restore(sdev);
	return 0;
}

static int do_pmcg_dump(int argc, char *argv[])
{
	__unused iommu_dev_t dev;
	__unused iommu_dev_t sdev;

	if (argc < 3)
		return -EINVAL;
	dev = (iommu_dev_t)strtoull(argv[2], 0, 0);
	sdev = iommu_device_save(dev);
	smmu_pmcg_event_dump();
	iommu_device_restore(sdev);
	return 0;
}

static int do_pmcg_id(int argc, char *argv[])
{
	__unused iommu_dev_t dev;
	__unused iommu_dev_t sdev;
	uint16_t event = 0;

	if (argc < 3)
		return -EINVAL;
	dev = (iommu_dev_t)strtoull(argv[2], 0, 0);
	sdev = iommu_device_save(dev);
	printf("iidr: 0x%08x\n", smmu_pmu_ctrl.iidr);
	printf("supported events:\n");
	while (event < ARRAY_SIZE(smmu_pmcg_event_names)) {
		if (test_bit(event, smmu_pmu_ctrl.supported_events))
			printf("%d - %s\n", event, smmu_pmcg_event_name(event));
		event++;
	}
	iommu_device_restore(sdev);
	return 0;
}

static int do_pmcg(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "id") == 0)
		return do_pmcg_id(argc, argv);
	if (strcmp(argv[1], "dump") == 0)
		return do_pmcg_dump(argc, argv);
	if (strcmp(argv[1], "add") == 0)
		return do_pmcg_add(argc, argv);
	if (strcmp(argv[1], "remove") == 0)
		return do_pmcg_remove(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(pmcg, do_pmcg, "SMMUv3 performance monitor counter group commands",
	"pmcg id <iommu>\n"
	"    -display IIDR and supported events\n"
	"pmcg dump <iommu>\n"
	"    -display event counter relationship\n"
	"pmcg add <iommu> <event> [streamid] [span]\n"
	"    -add PMCG event with/without filter enabled\n"
	"pmcg remove <iommu> <counter>\n"
	"    -remove PMCG event using counter ID\n"
);
