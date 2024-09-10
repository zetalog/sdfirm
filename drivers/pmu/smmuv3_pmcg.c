#include <target/perf.h>
#include <target/console.h>

struct smmu_pmu smmu_pmus[NR_IOMMU_DEVICES];
#if 0
#define SMMU_PMU_EVENT_ATTR_EXTRACTOR(_name, _config, _start, _end)        \
	static inline uint32_t get_##_name(struct perf_event *event)            \
	{                                                                  \
		return FIELD_GET(GENMASK_ULL(_end, _start),                \
				 event->attr._config);                     \
	}                                                                  \

SMMU_PMU_EVENT_ATTR_EXTRACTOR(event, config, 0, 15);
SMMU_PMU_EVENT_ATTR_EXTRACTOR(filter_stream_id, config1, 0, 31);
SMMU_PMU_EVENT_ATTR_EXTRACTOR(filter_span, config1, 32, 32);
SMMU_PMU_EVENT_ATTR_EXTRACTOR(filter_enable, config1, 33, 33);

static inline void smmu_pmu_enable(struct pmu *pmu)
{
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(pmu);

	writel(SMMU_PMCG_IRQ_CTRL_IRQEN,
	       smmu_pmu->reg_base + SMMU_PMCG_IRQ_CTRL);
	writel(SMMU_PMCG_CR_ENABLE, smmu_pmu->reg_base + SMMU_PMCG_CR);
}

static int smmu_pmu_apply_event_filter(struct smmu_pmu *smmu_pmu,
				       struct perf_event *event, int idx);

static inline void smmu_pmu_enable_quirk_hip08_09(struct pmu *pmu)
{
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(pmu);
	unsigned int idx;

	for_each_set_bit(idx, smmu_pmu->used_counters, smmu_pmu->num_counters)
		smmu_pmu_apply_event_filter(smmu_pmu, smmu_pmu->events[idx], idx);

	smmu_pmu_enable(pmu);
}

static inline void smmu_pmu_counter_set_value(struct smmu_pmu *smmu_pmu,
					      uint32_t idx, uint64_t value)
{
	if (smmu_pmu->counter_mask & BIT(32))
		writeq(value, smmu_pmu->reloc_base + SMMU_PMCG_EVCNTR(idx, 8));
	else
		writel(value, smmu_pmu->reloc_base + SMMU_PMCG_EVCNTR(idx, 4));
}

static inline uint64_t smmu_pmu_counter_get_value(struct smmu_pmu *smmu_pmu, uint32_t idx)
{
	uint64_t value;

	if (smmu_pmu->counter_mask & BIT(32))
		value = readq(smmu_pmu->reloc_base + SMMU_PMCG_EVCNTR(idx, 8));
	else
		value = readl(smmu_pmu->reloc_base + SMMU_PMCG_EVCNTR(idx, 4));

	return value;
}

static inline void smmu_pmu_counter_enable(struct smmu_pmu *smmu_pmu, uint32_t idx)
{
	writeq(BIT(idx), smmu_pmu->reg_base + SMMU_PMCG_CNTENSET0);
}

static inline void smmu_pmu_counter_disable(struct smmu_pmu *smmu_pmu, uint32_t idx)
{
	writeq(BIT(idx), smmu_pmu->reg_base + SMMU_PMCG_CNTENCLR0);
}

static inline void smmu_pmu_interrupt_enable(struct smmu_pmu *smmu_pmu, uint32_t idx)
{
	writeq(BIT(idx), smmu_pmu->reg_base + SMMU_PMCG_INTENSET0);
}

static inline void smmu_pmu_interrupt_disable(struct smmu_pmu *smmu_pmu,
					      uint32_t idx)
{
	writeq(BIT(idx), smmu_pmu->reg_base + SMMU_PMCG_INTENCLR0);
}

static inline void smmu_pmu_set_evtyper(struct smmu_pmu *smmu_pmu, uint32_t idx,
					uint32_t val)
{
	writel(val, smmu_pmu->reg_base + SMMU_PMCG_EVTYPER(idx));
}

static inline void smmu_pmu_set_smr(struct smmu_pmu *smmu_pmu, uint32_t idx, uint32_t val)
{
	writel(val, smmu_pmu->reg_base + SMMU_PMCG_SMR(idx));
}

static void smmu_pmu_event_update(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(event->pmu);
	uint64_t delta, prev, now;
	uint32_t idx = hwc->idx;

	do {
		prev = local64_read(&hwc->prev_count);
		now = smmu_pmu_counter_get_value(smmu_pmu, idx);
	} while (local64_cmpxchg(&hwc->prev_count, prev, now) != prev);

	/* handle overflow. */
	delta = now - prev;
	delta &= smmu_pmu->counter_mask;

	local64_add(delta, &event->count);
}

static void smmu_pmu_set_period(struct smmu_pmu *smmu_pmu,
				struct hw_perf_event *hwc)
{
	uint32_t idx = hwc->idx;
	uint64_t new;

	if (smmu_pmu->options & SMMU_PMCG_EVCNTR_RDONLY) {
		/*
		 * On platforms that require this quirk, if the counter starts
		 * at < half_counter value and wraps, the current logic of
		 * handling the overflow may not work. It is expected that,
		 * those platforms will have full 64 counter bits implemented
		 * so that such a possibility is remote(eg: HiSilicon HIP08).
		 */
		new = smmu_pmu_counter_get_value(smmu_pmu, idx);
	} else {
		/*
		 * We limit the max period to half the max counter value
		 * of the counter size, so that even in the case of extreme
		 * interrupt latency the counter will (hopefully) not wrap
		 * past its initial value.
		 */
		new = smmu_pmu->counter_mask >> 1;
		smmu_pmu_counter_set_value(smmu_pmu, idx, new);
	}

	local64_set(&hwc->prev_count, new);
}

static void smmu_pmu_set_event_filter(struct perf_event *event,
				      int idx, uint32_t span, uint32_t sid)
{
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(event->pmu);
	uint32_t evtyper;

	evtyper = get_event(event) | span << SMMU_PMCG_SID_SPAN_SHIFT;
	smmu_pmu_set_evtyper(smmu_pmu, idx, evtyper);
	smmu_pmu_set_smr(smmu_pmu, idx, sid);
}

static bool smmu_pmu_check_global_filter(struct perf_event *curr,
					 struct perf_event *new)
{
	if (get_filter_enable(new) != get_filter_enable(curr))
		return false;

	if (!get_filter_enable(new))
		return true;

	return get_filter_span(new) == get_filter_span(curr) &&
	       get_filter_stream_id(new) == get_filter_stream_id(curr);
}

static int smmu_pmu_apply_event_filter(struct smmu_pmu *smmu_pmu,
				       struct perf_event *event, int idx)
{
	uint32_t span, sid;
	unsigned int cur_idx, num_ctrs = smmu_pmu->num_counters;
	bool filter_en = !!get_filter_enable(event);

	span = filter_en ? get_filter_span(event) :
			   SMMU_PMCG_DEFAULT_FILTER_SPAN;
	sid = filter_en ? get_filter_stream_id(event) :
			   SMMU_PMCG_DEFAULT_FILTER_SID;

	cur_idx = find_first_bit(smmu_pmu->used_counters, num_ctrs);
	/*
	 * Per-counter filtering, or scheduling the first globally-filtered
	 * event into an empty PMU so idx == 0 and it works out equivalent.
	 */
	if (!smmu_pmu->global_filter || cur_idx == num_ctrs) {
		smmu_pmu_set_event_filter(event, idx, span, sid);
		return 0;
	}

	/* Otherwise, must match whatever's currently scheduled */
	if (smmu_pmu_check_global_filter(smmu_pmu->events[cur_idx], event)) {
		smmu_pmu_set_evtyper(smmu_pmu, idx, get_event(event));
		return 0;
	}

	return -EAGAIN;
}

static int smmu_pmu_get_event_idx(struct smmu_pmu *smmu_pmu,
				  struct perf_event *event)
{
	int idx, err;
	unsigned int num_ctrs = smmu_pmu->num_counters;

	idx = find_first_zero_bit(smmu_pmu->used_counters, num_ctrs);
	if (idx == num_ctrs)
		/* The counters are all in use. */
		return -EAGAIN;

	err = smmu_pmu_apply_event_filter(smmu_pmu, event, idx);
	if (err)
		return err;

	set_bit(idx, smmu_pmu->used_counters);

	return idx;
}

static bool smmu_pmu_events_compatible(struct perf_event *curr,
				       struct perf_event *new)
{
	if (new->pmu != curr->pmu)
		return false;

	if (to_smmu_pmu(new->pmu)->global_filter &&
	    !smmu_pmu_check_global_filter(curr, new))
		return false;

	return true;
}

/*
 * Implementation of abstract pmu functionality required by
 * the core perf events code.
 */

static int smmu_pmu_event_init(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(event->pmu);
	struct device *dev = smmu_pmu->dev;
	struct perf_event *sibling;
	int group_num_events = 1;
	u16 event_id;

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	if (hwc->sample_period) {
		dev_dbg(dev, "Sampling not supported\n");
		return -EOPNOTSUPP;
	}

	if (event->cpu < 0) {
		dev_dbg(dev, "Per-task mode not supported\n");
		return -EOPNOTSUPP;
	}

	/* Verify specified event is supported on this PMU */
	event_id = get_event(event);
	if (event_id < SMMU_PMCG_ARCH_MAX_EVENTS &&
	    (!test_bit(event_id, smmu_pmu->supported_events))) {
		dev_dbg(dev, "Invalid event %d for this PMU\n", event_id);
		return -EINVAL;
	}

	/* Don't allow groups with mixed PMUs, except for s/w events */
	if (!is_software_event(event->group_leader)) {
		if (!smmu_pmu_events_compatible(event->group_leader, event))
			return -EINVAL;

		if (++group_num_events > smmu_pmu->num_counters)
			return -EINVAL;
	}

	for_each_sibling_event(sibling, event->group_leader) {
		if (is_software_event(sibling))
			continue;

		if (!smmu_pmu_events_compatible(sibling, event))
			return -EINVAL;

		if (++group_num_events > smmu_pmu->num_counters)
			return -EINVAL;
	}

	hwc->idx = -1;

	/*
	 * Ensure all events are on the same cpu so all events are in the
	 * same cpu context, to avoid races on pmu_enable etc.
	 */
	event->cpu = smmu_pmu->on_cpu;

	return 0;
}

static void smmu_pmu_event_start(struct perf_event *event, int flags)
{
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;

	hwc->state = 0;

	smmu_pmu_set_period(smmu_pmu, hwc);

	smmu_pmu_counter_enable(smmu_pmu, idx);
}

static void smmu_pmu_event_stop(struct perf_event *event, int flags)
{
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;

	if (hwc->state & PERF_HES_STOPPED)
		return;

	smmu_pmu_counter_disable(smmu_pmu, idx);
	/* As the counter gets updated on _start, ignore PERF_EF_UPDATE */
	smmu_pmu_event_update(event);
	hwc->state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
}

static int smmu_pmu_event_add(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;
	int idx;
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(event->pmu);

	idx = smmu_pmu_get_event_idx(smmu_pmu, event);
	if (idx < 0)
		return idx;

	hwc->idx = idx;
	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;
	smmu_pmu->events[idx] = event;
	local64_set(&hwc->prev_count, 0);

	smmu_pmu_interrupt_enable(smmu_pmu, idx);

	if (flags & PERF_EF_START)
		smmu_pmu_event_start(event, flags);

	/* Propagate changes to the userspace mapping. */
	perf_event_update_userpage(event);

	return 0;
}

static void smmu_pmu_event_del(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(event->pmu);
	int idx = hwc->idx;

	smmu_pmu_event_stop(event, flags | PERF_EF_UPDATE);
	smmu_pmu_interrupt_disable(smmu_pmu, idx);
	smmu_pmu->events[idx] = NULL;
	clear_bit(idx, smmu_pmu->used_counters);

	perf_event_update_userpage(event);
}

static void smmu_pmu_event_read(struct perf_event *event)
{
	smmu_pmu_event_update(event);
}

/* cpumask */

static ssize_t smmu_pmu_cpumask_show(struct device *dev,
				     struct device_attribute *attr,
				     char *buf)
{
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(dev_get_drvdata(dev));

	return cpumap_print_to_pagebuf(true, buf, cpumask_of(smmu_pmu->on_cpu));
}

static struct device_attribute smmu_pmu_cpumask_attr =
		__ATTR(cpumask, 0444, smmu_pmu_cpumask_show, NULL);

static struct attribute *smmu_pmu_cpumask_attrs[] = {
	&smmu_pmu_cpumask_attr.attr,
	NULL
};

static const struct attribute_group smmu_pmu_cpumask_group = {
	.attrs = smmu_pmu_cpumask_attrs,
};

/* Events */

static ssize_t smmu_pmu_event_show(struct device *dev,
				   struct device_attribute *attr, char *page)
{
	struct perf_pmu_events_attr *pmu_attr;

	pmu_attr = container_of(attr, struct perf_pmu_events_attr, attr);

	return sysfs_emit(page, "event=0x%02llx\n", pmu_attr->id);
}

#define SMMU_EVENT_ATTR(name, config)			\
	PMU_EVENT_ATTR_ID(name, smmu_pmu_event_show, config)

static struct attribute *smmu_pmu_events[] = {
	SMMU_EVENT_ATTR(cycles, 0),
	SMMU_EVENT_ATTR(transaction, 1),
	SMMU_EVENT_ATTR(tlb_miss, 2),
	SMMU_EVENT_ATTR(config_cache_miss, 3),
	SMMU_EVENT_ATTR(trans_table_walk_access, 4),
	SMMU_EVENT_ATTR(config_struct_access, 5),
	SMMU_EVENT_ATTR(pcie_ats_trans_rq, 6),
	SMMU_EVENT_ATTR(pcie_ats_trans_passed, 7),
	NULL
};

static umode_t smmu_pmu_event_is_visible(struct kobject *kobj,
					 struct attribute *attr, int unused)
{
	struct device *dev = kobj_to_dev(kobj);
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(dev_get_drvdata(dev));
	struct perf_pmu_events_attr *pmu_attr;

	pmu_attr = container_of(attr, struct perf_pmu_events_attr, attr.attr);

	if (test_bit(pmu_attr->id, smmu_pmu->supported_events))
		return attr->mode;

	return 0;
}

static const struct attribute_group smmu_pmu_events_group = {
	.name = "events",
	.attrs = smmu_pmu_events,
	.is_visible = smmu_pmu_event_is_visible,
};

static ssize_t smmu_pmu_identifier_attr_show(struct device *dev,
					struct device_attribute *attr,
					char *page)
{
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(dev_get_drvdata(dev));

	return sysfs_emit(page, "0x%08x\n", smmu_pmu->iidr);
}

static umode_t smmu_pmu_identifier_attr_visible(struct kobject *kobj,
						struct attribute *attr,
						int n)
{
	struct device *dev = kobj_to_dev(kobj);
	struct smmu_pmu *smmu_pmu = to_smmu_pmu(dev_get_drvdata(dev));

	if (!smmu_pmu->iidr)
		return 0;
	return attr->mode;
}

static struct device_attribute smmu_pmu_identifier_attr =
	__ATTR(identifier, 0444, smmu_pmu_identifier_attr_show, NULL);

static struct attribute *smmu_pmu_identifier_attrs[] = {
	&smmu_pmu_identifier_attr.attr,
	NULL
};

static const struct attribute_group smmu_pmu_identifier_group = {
	.attrs = smmu_pmu_identifier_attrs,
	.is_visible = smmu_pmu_identifier_attr_visible,
};

/* Formats */
PMU_FORMAT_ATTR(event,		   "config:0-15");
PMU_FORMAT_ATTR(filter_stream_id,  "config1:0-31");
PMU_FORMAT_ATTR(filter_span,	   "config1:32");
PMU_FORMAT_ATTR(filter_enable,	   "config1:33");

static struct attribute *smmu_pmu_formats[] = {
	&format_attr_event.attr,
	&format_attr_filter_stream_id.attr,
	&format_attr_filter_span.attr,
	&format_attr_filter_enable.attr,
	NULL
};

static const struct attribute_group smmu_pmu_format_group = {
	.name = "format",
	.attrs = smmu_pmu_formats,
};

static const struct attribute_group *smmu_pmu_attr_grps[] = {
	&smmu_pmu_cpumask_group,
	&smmu_pmu_events_group,
	&smmu_pmu_format_group,
	&smmu_pmu_identifier_group,
	NULL
};

/*
 * Generic device handlers
 */

static int smmu_pmu_offline_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct smmu_pmu *smmu_pmu;
	unsigned int target;

	smmu_pmu = hlist_entry_safe(node, struct smmu_pmu, node);
	if (cpu != smmu_pmu->on_cpu)
		return 0;

	target = cpumask_any_but(cpu_online_mask, cpu);
	if (target >= nr_cpu_ids)
		return 0;

	perf_pmu_migrate_context(&smmu_pmu->pmu, cpu, target);
	smmu_pmu->on_cpu = target;
	WARN_ON(irq_set_affinity(smmu_pmu->irq, cpumask_of(target)));

	return 0;
}

static void smmu_pmu_free_msis(void *data)
{
	struct device *dev = data;

	platform_device_msi_free_irqs_all(dev);
}

static void smmu_pmu_write_msi_msg(struct msi_desc *desc, struct msi_msg *msg)
{
	phys_addr_t doorbell;
	struct device *dev = msi_desc_to_dev(desc);
	struct smmu_pmu *pmu = dev_get_drvdata(dev);

	doorbell = (((uint64_t)msg->address_hi) << 32) | msg->address_lo;
	doorbell &= MSI_CFG0_ADDR_MASK;

	writeq_relaxed(doorbell, pmu->reg_base + SMMU_PMCG_IRQ_CFG0);
	writel_relaxed(msg->data, pmu->reg_base + SMMU_PMCG_IRQ_CFG1);
	writel_relaxed(MSI_CFG2_MEMATTR_DEVICE_nGnRE,
		       pmu->reg_base + SMMU_PMCG_IRQ_CFG2);
}

static void smmu_pmu_shutdown(void)
{
	struct smmu_pmu *smmu_pmu = platform_get_drvdata(pdev);

	smmu_pmu_disable(&smmu_pmu->pmu);
}
#endif

static void smmu_pmu_handle_irq(irq_t irq)
{
	uint64_t ovsr;
	DECLARE_BITMAP(ovs, 64);
	unsigned int idx;

	ovsr = __raw_readq(TCU_PMCG_OVSSET0(iommu_dev));
	if (!ovsr)
		return;

	__raw_writeq(ovsr, TCU_PMCG_OVSCLR0(iommu_dev));

#if 0
	for_each_set_bit(idx, ovs, smmu_pmu->num_counters) {
		struct perf_event *event = smmu_pmu->events[idx];
		struct hw_perf_event *hwc;

		if (WARN_ON_ONCE(!event))
			continue;

		smmu_pmu_event_update(event);
		hwc = &event->hw;

		smmu_pmu_set_period(smmu_pmu, hwc);
	}
#endif
}

static inline void smmu_pmu_disable(void)
{
	__raw_writel(0, TCU_PMCG_CR(iommu_dev));
	__raw_writel(0, TCU_PMCG_IRQ_CTRL(iommu_dev));
}

static void smmu_pmu_reset(void)
{
	uint64_t counter_present_mask = GENMASK_ULL(smmu_pmu_ctrl.num_counters - 1, 0);

	smmu_pmu_disable();

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

#ifdef CONFIG_SMMU_PMCG_MSI
static void smmu_pmu_setup_msi(void)
{
	int ret;

	/* Clear MSI address reg */
	__raw_writeq(0, TCU_PMCG_IRQ_CFG0(iommu_dev));

	/* MSI supported or not */
	if (!(__raw_readl(TCU_PMCG_CFGR(iommu_dev)) & SMMU_PMCG_CFGR_MSI))
		return;

#if 0
	smmu_pmu_write_msi_msg();
	pmu->irq = msi_get_virq(dev, 0);
	smmu_pmu_free_msis();
#endif
}
#else
#define smmu_pmu_setup_msi()		do { } while (0)
#endif

static void smmu_pmu_setup_irq(void)
{
	smmu_pmu_setup_msi();
	irq_register_vector(SMMU_HW_IRQ_PMU(iommu_dev), smmu_pmu_handle_irq);
}

void smmu_pmcg_init(void)
{
	uint32_t cfgr, reg_size;
	uint8_t i;

	cfgr = __raw_readl(TCU_PMCG_CFGR(iommu_dev));

	/* Determine if page 1 is present */
	if (cfgr & SMMU_PMCG_CFGR_RELOC_CTRS) {
		smmu_pmu_ctrl.reloc_base = SMMU_PAGESIZE;
	} else {
		smmu_pmu_ctrl.reloc_base = 0;
	}

	for (i = 0; i < SMMU_PMCG_ARCH_MAX_EVENTS; i++) {
		if (__raw_readq(TCU_PMCG_CEID(iommu_dev, i) & PMCG_CEID(i)))
			set_bit(i, smmu_pmu_ctrl. supported_events);
	}

	smmu_pmu_ctrl.num_counters = FIELD_GET(SMMU_PMCG_CFGR_NCTR, cfgr) + 1;
	smmu_pmu_ctrl.global_filter = !!(cfgr & SMMU_PMCG_CFGR_SID_FILTER_TYPE);

	reg_size = FIELD_GET(SMMU_PMCG_CFGR_SIZE, cfgr);
	smmu_pmu_ctrl.counter_mask = GENMASK_ULL(reg_size, 0);

	smmu_pmu_reset();
	smmu_pmu_setup_irq();
	smmu_pmu_get_iidr();
	con_log("pmcg: counters=%d filters=%s\n",
		smmu_pmu_ctrl.num_counters,
		smmu_pmu_ctrl.global_filter ? "Global(Counter0)" : "Individual");
}
