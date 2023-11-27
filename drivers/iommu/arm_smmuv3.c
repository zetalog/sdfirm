#include <target/iommu.h>
#include <target/iommu-pgtable.h>
#include <target/panic.h>

#define CONFIG_SIMULATION_SMMU 1

bool smmu_disable_bypass = false;

static inline caddr_t arm_smmu_page1_fixup(caddr_t reg)
{
	if (((reg - SMMU_BASE(iommu_dev)) > SZ_64K) &&
	    (smmu_device_ctrl.options & ARM_SMMU_OPT_PAGE0_REGS_ONLY))
		reg -= SZ_64K;

	return reg;
}

static int arm_smmu_bitmap_alloc(bits_t *map, int span)
{
	int idx, size = 1 << span;

	do {
		idx = find_first_clear_bit(map, size);
		if (idx == size)
			return -ENOSPC;
	} while (test_and_set_bit(idx, map));

	return idx;
}

static void arm_smmu_bitmap_free(bits_t *map, int idx)
{
	clear_bit(idx, map);
}

/* Low-level queue manipulation functions */
static bool queue_has_space(struct arm_smmu_ll_queue *q, uint32_t n)
{
	uint32_t space, prod, cons;

	prod = Q_IDX(q, q->u.prod);
	cons = Q_IDX(q, q->u.cons);

	if (Q_WRP(q, q->u.prod) == Q_WRP(q, q->u.cons))
		space = (1 << q->max_n_shift) - (prod - cons);
	else
		space = cons - prod;

	return space >= n;
}

static bool queue_full(struct arm_smmu_ll_queue *q)
{
	return Q_IDX(q, q->u.prod) == Q_IDX(q, q->u.cons) &&
	       Q_WRP(q, q->u.prod) != Q_WRP(q, q->u.cons);
}

static bool queue_empty(struct arm_smmu_ll_queue *q)
{
	return Q_IDX(q, q->u.prod) == Q_IDX(q, q->u.cons) &&
	       Q_WRP(q, q->u.prod) == Q_WRP(q, q->u.cons);
}

static bool queue_consumed(struct arm_smmu_ll_queue *q, uint32_t prod)
{
	return ((Q_WRP(q, q->u.cons) == Q_WRP(q, prod)) &&
		(Q_IDX(q, q->u.cons) > Q_IDX(q, prod))) ||
	       ((Q_WRP(q, q->u.cons) != Q_WRP(q, prod)) &&
		(Q_IDX(q, q->u.cons) <= Q_IDX(q, prod)));
}

static void queue_sync_cons_out(struct arm_smmu_queue *q)
{
	/*
	 * Ensure that all CPU accesses (reads and writes) to the queue
	 * are complete before we update the cons pointer.
	 */
	mb();
	__raw_writel(q->llq.u.cons, q->cons_reg);
}

static void queue_inc_cons(struct arm_smmu_ll_queue *q)
{
	uint32_t cons = (Q_WRP(q, q->u.cons) | Q_IDX(q, q->u.cons)) + 1;
	q->u.cons = Q_OVF(q->u.cons) | Q_WRP(q, cons) | Q_IDX(q, cons);
}

static int queue_sync_prod_in(struct arm_smmu_queue *q)
{
	int ret = 0;
	uint32_t prod = __raw_readl(q->prod_reg);

	if (Q_OVF(prod) != Q_OVF(q->llq.u.prod))
		ret = -EOVERFLOW;

	q->llq.u.prod = prod;
	return ret;
}

static uint32_t queue_inc_prod_n(struct arm_smmu_ll_queue *q, int n)
{
	uint32_t prod = (Q_WRP(q, q->u.prod) | Q_IDX(q, q->u.prod)) + n;
	return Q_OVF(q->u.prod) | Q_WRP(q, prod) | Q_IDX(q, prod);
}

static void queue_write(uint64_t *dst, uint64_t *src, size_t n_dwords)
{
	int i;

	for (i = 0; i < n_dwords; ++i) {
#if 0
		con_log("smmuv3: CMDQ: %016llx=%016llx\n",
			(unsigned long long)dst, (unsigned long long)*src);
#endif
		*dst++ = cpu_to_le64(*src++);
	}
}

static void queue_read(uint64_t *dst, uint64_t *src, size_t n_dwords)
{
	int i;

	for (i = 0; i < n_dwords; ++i)
		*dst++ = le64_to_cpu(*src++);
}

static int queue_remove_raw(struct arm_smmu_queue *q, uint64_t *ent)
{
	if (queue_empty(&q->llq))
		return -EAGAIN;

	queue_read(ent, Q_ENT(q, q->llq.u.cons), q->ent_dwords);
	queue_inc_cons(&q->llq);
	queue_sync_cons_out(q);
	return 0;
}

/* High-level queue accessors */
static int arm_smmu_cmdq_build_cmd(uint64_t *cmd, struct arm_smmu_cmdq_ent *ent)
{
	memset(cmd, 0, 1 << CMDQ_ENT_SZ_SHIFT);
	cmd[0] |= FIELD_PREP(CMDQ_0_OP, ent->opcode);

	switch (ent->opcode) {
	case CMDQ_OP_TLBI_EL2_ALL:
	case CMDQ_OP_TLBI_NSNH_ALL:
		break;
	case CMDQ_OP_PREFETCH_CFG:
		cmd[0] |= FIELD_PREP(CMDQ_PREFETCH_0_SID, ent->prefetch.sid);
		cmd[1] |= FIELD_PREP(CMDQ_PREFETCH_1_SIZE, ent->prefetch.size);
		cmd[1] |= ent->prefetch.addr & CMDQ_PREFETCH_1_ADDR_MASK;
		break;
	case CMDQ_OP_CFGI_CD:
		cmd[0] |= FIELD_PREP(CMDQ_CFGI_0_SSID, ent->cfgi.ssid);
		/* Fallthrough */
	case CMDQ_OP_CFGI_STE:
		cmd[0] |= FIELD_PREP(CMDQ_CFGI_0_SID, ent->cfgi.sid);
		cmd[1] |= FIELD_PREP(CMDQ_CFGI_1_LEAF, ent->cfgi.leaf);
		break;
	case CMDQ_OP_CFGI_CD_ALL:
		cmd[0] |= FIELD_PREP(CMDQ_CFGI_0_SID, ent->cfgi.sid);
		break;
	case CMDQ_OP_CFGI_ALL:
		/* Cover the entire SID range */
		cmd[1] |= FIELD_PREP(CMDQ_CFGI_1_RANGE, 31);
		break;
	case CMDQ_OP_TLBI_NH_VA:
		cmd[0] |= FIELD_PREP(CMDQ_TLBI_0_VMID, ent->tlbi.vmid);
		cmd[0] |= FIELD_PREP(CMDQ_TLBI_0_ASID, ent->tlbi.asid);
		cmd[1] |= FIELD_PREP(CMDQ_TLBI_1_LEAF, ent->tlbi.leaf);
		cmd[1] |= ent->tlbi.addr & CMDQ_TLBI_1_VA_MASK;
		break;
	case CMDQ_OP_TLBI_S2_IPA:
		cmd[0] |= FIELD_PREP(CMDQ_TLBI_0_VMID, ent->tlbi.vmid);
		cmd[1] |= FIELD_PREP(CMDQ_TLBI_1_LEAF, ent->tlbi.leaf);
		cmd[1] |= ent->tlbi.addr & CMDQ_TLBI_1_IPA_MASK;
		break;
	case CMDQ_OP_TLBI_NH_ASID:
		cmd[0] |= FIELD_PREP(CMDQ_TLBI_0_ASID, ent->tlbi.asid);
		/* Fallthrough */
	case CMDQ_OP_TLBI_S12_VMALL:
		cmd[0] |= FIELD_PREP(CMDQ_TLBI_0_VMID, ent->tlbi.vmid);
		break;
	case CMDQ_OP_ATC_INV:
		cmd[0] |= FIELD_PREP(CMDQ_0_SSV, ent->substream_valid);
		cmd[0] |= FIELD_PREP(CMDQ_ATC_0_GLOBAL, ent->atc.global);
		cmd[0] |= FIELD_PREP(CMDQ_ATC_0_SSID, ent->atc.ssid);
		cmd[0] |= FIELD_PREP(CMDQ_ATC_0_SID, ent->atc.sid);
		cmd[1] |= FIELD_PREP(CMDQ_ATC_1_SIZE, ent->atc.size);
		cmd[1] |= ent->atc.addr & CMDQ_ATC_1_ADDR_MASK;
		break;
	case CMDQ_OP_PRI_RESP:
		cmd[0] |= FIELD_PREP(CMDQ_0_SSV, ent->substream_valid);
		cmd[0] |= FIELD_PREP(CMDQ_PRI_0_SSID, ent->pri.ssid);
		cmd[0] |= FIELD_PREP(CMDQ_PRI_0_SID, ent->pri.sid);
		cmd[1] |= FIELD_PREP(CMDQ_PRI_1_GRPID, ent->pri.grpid);
		switch (ent->pri.resp) {
		case PRI_RESP_DENY:
		case PRI_RESP_FAIL:
		case PRI_RESP_SUCC:
			break;
		default:
			return -EINVAL;
		}
		cmd[1] |= FIELD_PREP(CMDQ_PRI_1_RESP, ent->pri.resp);
		break;
	case CMDQ_OP_CMD_SYNC:
		if (ent->sync.msiaddr) {
			cmd[0] |= FIELD_PREP(CMDQ_SYNC_0_CS, CMDQ_SYNC_0_CS_IRQ);
			cmd[1] |= ent->sync.msiaddr & CMDQ_SYNC_1_MSIADDR_MASK;
		} else {
			cmd[0] |= FIELD_PREP(CMDQ_SYNC_0_CS, CMDQ_SYNC_0_CS_SEV);
		}
		cmd[0] |= FIELD_PREP(CMDQ_SYNC_0_MSH, ARM_SMMU_SH_ISH);
		cmd[0] |= FIELD_PREP(CMDQ_SYNC_0_MSIATTR, ARM_SMMU_MEMATTR_OIWB);
		break;
	default:
		return -ENOENT;
	}

	return 0;
}

static void arm_smmu_cmdq_build_sync_cmd(uint64_t *cmd, uint32_t prod)
{
	struct arm_smmu_queue *q = &smmu_device_ctrl.cmdq.q;
	struct arm_smmu_cmdq_ent ent = {
		.opcode = CMDQ_OP_CMD_SYNC,
	};

	/*
	 * Beware that Hi16xx adds an extra 32 bits of goodness to its MSI
	 * payload, so the write will zero the entire command on that platform.
	 */
	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_MSI &&
	    smmu_device_ctrl.features & ARM_SMMU_FEAT_COHERENCY) {
		ent.sync.msiaddr = q->base_dma + Q_IDX(&q->llq, prod) *
				   q->ent_dwords * 8;
	}

	arm_smmu_cmdq_build_cmd(cmd, &ent);
}

static void arm_smmu_cmdq_skip_err(void)
{
	static const char *cerror_str[] = {
		[CMDQ_ERR_CERROR_NONE_IDX]	= "No error",
		[CMDQ_ERR_CERROR_ILL_IDX]	= "Illegal command",
		[CMDQ_ERR_CERROR_ABT_IDX]	= "Abort on command fetch",
		[CMDQ_ERR_CERROR_ATC_INV_IDX]	= "ATC invalidate timeout",
	};

	int i;
	uint64_t cmd[CMDQ_ENT_DWORDS];
	struct arm_smmu_queue *q = &smmu_device_ctrl.cmdq.q;
	uint32_t cons = __raw_readl(q->cons_reg);
	uint32_t idx = FIELD_GET(CMDQ_CONS_ERR, cons);
	struct arm_smmu_cmdq_ent cmd_sync = {
		.opcode = CMDQ_OP_CMD_SYNC,
	};

	con_err("smmuv3: CMDQ error (cons 0x%08x): %s\n", cons,
		idx < ARRAY_SIZE(cerror_str) ?  cerror_str[idx] : "Unknown");

	switch (idx) {
	case CMDQ_ERR_CERROR_ABT_IDX:
		con_err("smmuv3: retrying command fetch\n");
	case CMDQ_ERR_CERROR_NONE_IDX:
		return;
	case CMDQ_ERR_CERROR_ATC_INV_IDX:
		/*
		 * ATC Invalidation Completion timeout. CONS is still pointing
		 * at the CMD_SYNC. Attempt to complete other pending commands
		 * by repeating the CMD_SYNC, though we might well end up back
		 * here since the ATC invalidation may still be pending.
		 */
		return;
	case CMDQ_ERR_CERROR_ILL_IDX:
		/* Fallthrough */
	default:
		break;
	}

	/*
	 * We may have concurrent producers, so we need to be careful
	 * not to touch any of the shadow cmdq state.
	 */
	queue_read(cmd, Q_ENT(q, cons), q->ent_dwords);
	con_err("smmuv3: skipping command in error state:\n");
	for (i = 0; i < ARRAY_SIZE(cmd); ++i)
		con_err("\t0x%016llx\n", (unsigned long long)cmd[i]);

	/* Convert the erroneous command into a CMD_SYNC */
	if (arm_smmu_cmdq_build_cmd(cmd, &cmd_sync)) {
		con_err("smmuv3: failed to convert to CMD_SYNC\n");
		return;
	}

	queue_write(Q_ENT(q, cons), cmd, q->ent_dwords);
}

/*
 * Command queue locking.
 * This is a form of bastardised rwlock with the following major changes:
 *
 * - The only LOCK routines are exclusive_trylock() and shared_lock().
 *   Neither have barrier semantics, and instead provide only a control
 *   dependency.
 *
 * - The UNLOCK routines are supplemented with shared_tryunlock(), which
 *   fails if the caller appears to be the last lock holder (yes, this is
 *   racy). All successful UNLOCK routines have RELEASE semantics.
 */
static void arm_smmu_cmdq_shared_lock(struct arm_smmu_cmdq *cmdq)
{
	int val;

	/*
	 * We can try to avoid the cmpxchg() loop by simply incrementing the
	 * lock counter. When held in exclusive state, the lock counter is set
	 * to INT_MIN so these increments won't hurt as the value will remain
	 * negative.
	 */
	if (atomic_fetch_inc(&cmdq->lock) >= 0)
		return;

	do {
		val = atomic_cond_read_relaxed(&cmdq->lock, VAL >= 0);
	} while (atomic_cmpxchg(&cmdq->lock, val, val + 1) != val);
}

static void arm_smmu_cmdq_shared_unlock(struct arm_smmu_cmdq *cmdq)
{
	(void)atomic_dec_return(&cmdq->lock);
}

static bool arm_smmu_cmdq_shared_tryunlock(struct arm_smmu_cmdq *cmdq)
{
	if (atomic_read(&cmdq->lock) == 1)
		return false;

	arm_smmu_cmdq_shared_unlock(cmdq);
	return true;
}

#define arm_smmu_cmdq_exclusive_trylock_irqsave(cmdq, flags)		\
({									\
	bool __ret;							\
	irq_local_save(flags);						\
	__ret = !atomic_cmpxchg(&cmdq->lock, 0, INT_MIN);		\
	if (!__ret)							\
		irq_local_restore(flags);				\
	__ret;								\
})

#define arm_smmu_cmdq_exclusive_unlock_irqrestore(cmdq, flags)		\
({									\
	atomic_set(&cmdq->lock, 0);					\
	irq_local_restore(flags);					\
})

static void queue_poll_init(struct arm_smmu_queue_poll *qp)
{
	qp->delay = 1;
	qp->spin_cnt = 0;
	qp->wfe = !!(smmu_device_ctrl.features & ARM_SMMU_FEAT_SEV);
	qp->timeout = tick_get_counter() + ARM_SMMU_POLL_TIMEOUT_US;
}

static int queue_poll(struct arm_smmu_queue_poll *qp)
{
	if (time_after(tick_get_counter(), qp->timeout))
		return -ETIMEDOUT;

	if (qp->wfe) {
		/* wfe(); */
		wfi();
	} else if (++qp->spin_cnt < ARM_SMMU_POLL_SPIN_COUNT) {
		cpu_relax();
	} else {
		udelay(qp->delay);
		qp->delay *= 2;
		qp->spin_cnt = 0;
	}

	return 0;
}

/*
 * Command queue insertion.
 * This is made fiddly by our attempts to achieve some sort of scalability
 * since there is one queue shared amongst all of the CPUs in the system.  If
 * you like mixed-size concurrency, dependency ordering and relaxed atomics,
 * then you'll *love* this monstrosity.
 *
 * The basic idea is to split the queue up into ranges of commands that are
 * owned by a given CPU; the owner may not have written all of the commands
 * itself, but is responsible for advancing the hardware prod pointer when
 * the time comes. The algorithm is roughly:
 *
 * 	1. Allocate some space in the queue. At this point we also discover
 *	   whether the head of the queue is currently owned by another CPU,
 *	   or whether we are the owner.
 *
 *	2. Write our commands into our allocated slots in the queue.
 *
 *	3. Mark our slots as valid in arm_smmu_cmdq.valid_map.
 *
 *	4. If we are an owner:
 *		a. Wait for the previous owner to finish.
 *		b. Mark the queue head as unowned, which tells us the range
 *		   that we are responsible for publishing.
 *		c. Wait for all commands in our owned range to become valid.
 *		d. Advance the hardware prod pointer.
 *		e. Tell the next owner we've finished.
 *
 *	5. If we are inserting a CMD_SYNC (we may or may not have been an
 *	   owner), then we need to stick around until it has completed:
 *		a. If we have MSIs, the SMMU can write back into the CMD_SYNC
 *		   to clear the first 4 bytes.
 *		b. Otherwise, we spin waiting for the hardware cons pointer to
 *		   advance past our command.
 *
 * The devil is in the details, particularly the use of locking for handling
 * SYNC completion and freeing up space in the queue before we think that it is
 * full.
 */
static void __arm_smmu_cmdq_poll_set_valid_map(struct arm_smmu_cmdq *cmdq,
					       uint32_t sprod,
					       uint32_t eprod, bool set)
{
	uint32_t swidx, sbidx, ewidx, ebidx;
	struct arm_smmu_ll_queue llq = {
		.max_n_shift	= cmdq->q.llq.max_n_shift,
		.u		= {
			.prod	= sprod,
		},
	};

	ewidx = BIT_WORD(Q_IDX(&llq, eprod));
	ebidx = Q_IDX(&llq, eprod) % BITS_PER_LONG;

	while (llq.u.prod != eprod) {
		unsigned long mask;
		atomic_long_t *ptr;
		uint32_t limit = BITS_PER_LONG;

		swidx = BIT_WORD(Q_IDX(&llq, llq.u.prod));
		sbidx = Q_IDX(&llq, llq.u.prod) % BITS_PER_LONG;

		ptr = &cmdq->valid_map[swidx];

		if ((swidx == ewidx) && (sbidx < ebidx))
			limit = ebidx;

		mask = GENMASK(limit - 1, sbidx);

		/*
		 * The valid bit is the inverse of the wrap bit. This means
		 * that a zero-initialised queue is invalid and, after marking
		 * all entries as valid, they become invalid again when we
		 * wrap.
		 */
		if (set) {
			atomic_long_xor(mask, ptr);
		} else { /* Poll */
			unsigned long valid;

			valid = (ULONG_MAX + !!Q_WRP(&llq, llq.u.prod)) & mask;
			atomic_long_cond_read_relaxed(ptr, (VAL & mask) == valid);
		}

		llq.u.prod = queue_inc_prod_n(&llq, limit - sbidx);
	}
}

/* Mark all entries in the range [sprod, eprod) as valid */
static void arm_smmu_cmdq_set_valid_map(struct arm_smmu_cmdq *cmdq,
					uint32_t sprod, uint32_t eprod)
{
	__arm_smmu_cmdq_poll_set_valid_map(cmdq, sprod, eprod, true);
}

/* Wait for all entries in the range [sprod, eprod) to become valid */
static void arm_smmu_cmdq_poll_valid_map(struct arm_smmu_cmdq *cmdq,
					 uint32_t sprod, uint32_t eprod)
{
	__arm_smmu_cmdq_poll_set_valid_map(cmdq, sprod, eprod, false);
}

/* Wait for the command queue to become non-full */
static int arm_smmu_cmdq_poll_until_not_full(struct arm_smmu_ll_queue *llq)
{
	__unused irq_flags_t flags;
	struct arm_smmu_queue_poll qp;
	struct arm_smmu_cmdq *cmdq = &smmu_device_ctrl.cmdq;
	int ret = 0;

	/*
	 * Try to update our copy of cons by grabbing exclusive cmdq access. If
	 * that fails, spin until somebody else updates it for us.
	 */
	if (arm_smmu_cmdq_exclusive_trylock_irqsave(cmdq, flags)) {
		WRITE_ONCE(cmdq->q.llq.u.cons, __raw_readl(cmdq->q.cons_reg));
		arm_smmu_cmdq_exclusive_unlock_irqrestore(cmdq, flags);
		llq->u.val = READ_ONCE(cmdq->q.llq.u.val);
		return 0;
	}

	queue_poll_init(&qp);
	do {
		llq->u.val = READ_ONCE(smmu_device_ctrl.cmdq.q.llq.u.val);
		if (!queue_full(llq))
			break;

		ret = queue_poll(&qp);
	} while (!ret);

	return ret;
}

/*
 * Wait until the SMMU signals a CMD_SYNC completion MSI.
 * Must be called with the cmdq lock held in some capacity.
 */
static int __arm_smmu_cmdq_poll_until_msi(struct arm_smmu_ll_queue *llq)
{
	int ret = 0;
	struct arm_smmu_queue_poll qp;
	struct arm_smmu_cmdq *cmdq = &smmu_device_ctrl.cmdq;
	uint32_t *cmd = (uint32_t *)(Q_ENT(&cmdq->q, llq->u.prod));

	queue_poll_init(&qp);

	/*
	 * The MSI won't generate an event, since it's being written back
	 * into the command queue.
	 */
	qp.wfe = false;
	smp_cond_load_relaxed(cmd, !VAL || (ret = queue_poll(&qp)));
	llq->u.cons = ret ? llq->u.prod : queue_inc_prod_n(llq, 1);
	return ret;
}

/*
 * Wait until the SMMU cons index passes llq->prod.
 * Must be called with the cmdq lock held in some capacity.
 */
static int __arm_smmu_cmdq_poll_until_consumed(struct arm_smmu_ll_queue *llq)
{
	struct arm_smmu_queue_poll qp;
	struct arm_smmu_cmdq *cmdq = &smmu_device_ctrl.cmdq;
	uint32_t prod = llq->u.prod;
	int ret = 0;

	queue_poll_init(&qp);
	llq->u.val = READ_ONCE(smmu_device_ctrl.cmdq.q.llq.u.val);
	do {
		if (queue_consumed(llq, prod))
			break;

		ret = queue_poll(&qp);

		/*
		 * This needs to be a readl() so that our subsequent call
		 * to arm_smmu_cmdq_shared_tryunlock() can fail accurately.
		 *
		 * Specifically, we need to ensure that we observe all
		 * shared_lock()s by other CMD_SYNCs that share our owner,
		 * so that a failing call to tryunlock() means that we're
		 * the last one out and therefore we can safely advance
		 * cmdq->q.llq.cons. Roughly speaking:
		 *
		 * CPU 0		CPU1			CPU2 (us)
		 *
		 * if (sync)
		 * 	shared_lock();
		 *
		 * dma_wmb();
		 * set_valid_map();
		 *
		 * 			if (owner) {
		 *				poll_valid_map();
		 *				<control dependency>
		 *				writel(prod_reg);
		 *
		 *						readl(cons_reg);
		 *						tryunlock();
		 *
		 * Requires us to see CPU 0's shared_lock() acquisition.
		 */
		llq->u.cons = __raw_readl(cmdq->q.cons_reg);
	} while (!ret);

	return ret;
}

static int arm_smmu_cmdq_poll_until_sync(struct arm_smmu_ll_queue *llq)
{
	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_MSI &&
	    smmu_device_ctrl.features & ARM_SMMU_FEAT_COHERENCY)
		return __arm_smmu_cmdq_poll_until_msi(llq);

	return __arm_smmu_cmdq_poll_until_consumed(llq);
}

static void arm_smmu_cmdq_write_entries(struct arm_smmu_cmdq *cmdq,
					uint64_t *cmds, uint32_t prod, int n)
{
	int i;
	struct arm_smmu_ll_queue llq = {
		.max_n_shift	= cmdq->q.llq.max_n_shift,
		.u		= {
			.prod	= prod,
		},
	};

	for (i = 0; i < n; ++i) {
		uint64_t *cmd = &cmds[i * CMDQ_ENT_DWORDS];

		prod = queue_inc_prod_n(&llq, i);
		queue_write(Q_ENT(&cmdq->q, prod), cmd, CMDQ_ENT_DWORDS);
	}
}

/*
 * This is the actual insertion function, and provides the following
 * ordering guarantees to callers:
 *
 * - There is a dma_wmb() before publishing any commands to the queue.
 *   This can be relied upon to order prior writes to data structures
 *   in memory (such as a CD or an STE) before the command.
 *
 * - On completion of a CMD_SYNC, there is a control dependency.
 *   This can be relied upon to order subsequent writes to memory (e.g.
 *   freeing an IOVA) after completion of the CMD_SYNC.
 *
 * - Command insertion is totally ordered, so if two CPUs each race to
 *   insert their own list of commands then all of the commands from one
 *   CPU will appear before any of the commands from the other CPU.
 */
static int arm_smmu_cmdq_issue_cmdlist(uint64_t *cmds, int n, bool sync)
{
	uint64_t cmd_sync[CMDQ_ENT_DWORDS];
	uint32_t prod;
	__unused irq_flags_t flags;
	bool owner;
	struct arm_smmu_cmdq *cmdq = &smmu_device_ctrl.cmdq;
	struct arm_smmu_ll_queue llq = {
		.max_n_shift = cmdq->q.llq.max_n_shift,
	}, head = llq;
	int ret = 0;

	/* 1. Allocate some space in the queue */
	irq_local_save(flags);
	llq.u.val = READ_ONCE(cmdq->q.llq.u.val);
	do {
		uint64_t old;

		while (!queue_has_space(&llq, n + sync)) {
			irq_local_restore(flags);
			if (arm_smmu_cmdq_poll_until_not_full(&llq))
				con_log("smmuv3: CMDQ timeout\n");
			irq_local_save(flags);
		}

		head.u.cons = llq.u.cons;
		head.u.prod = queue_inc_prod_n(&llq, n + sync) |
			      CMDQ_PROD_OWNED_FLAG;

		old = cmpxchg_relaxed(&cmdq->q.llq.u.val,
				      llq.u.val, head.u.val);
		if (old == llq.u.val)
			break;

		llq.u.val = old;
	} while (1);
	owner = !(llq.u.prod & CMDQ_PROD_OWNED_FLAG);
	head.u.prod &= ~CMDQ_PROD_OWNED_FLAG;
	llq.u.prod &= ~CMDQ_PROD_OWNED_FLAG;

	/*
	 * 2. Write our commands into the queue
	 * Dependency ordering from the cmpxchg() loop above.
	 */
	arm_smmu_cmdq_write_entries(cmdq, cmds, llq.u.prod, n);
	if (sync) {
		prod = queue_inc_prod_n(&llq, n);
		arm_smmu_cmdq_build_sync_cmd(cmd_sync, prod);
		queue_write(Q_ENT(&cmdq->q, prod), cmd_sync, CMDQ_ENT_DWORDS);

		/*
		 * In order to determine completion of our CMD_SYNC, we must
		 * ensure that the queue can't wrap twice without us noticing.
		 * We achieve that by taking the cmdq lock as shared before
		 * marking our slot as valid.
		 */
		arm_smmu_cmdq_shared_lock(cmdq);
	}

	/* 3. Mark our slots as valid, ensuring commands are visible first */
	dma_wmb();
	arm_smmu_cmdq_set_valid_map(cmdq, llq.u.prod, head.u.prod);

	/* 4. If we are the owner, take control of the SMMU hardware */
	if (owner) {
		/* a. Wait for previous owner to finish */
		atomic_cond_read_relaxed(&cmdq->owner_prod, VAL == llq.u.prod);

		/* b. Stop gathering work by clearing the owned flag */
		prod = cmdq->q.llq.u.prod & (~CMDQ_PROD_OWNED_FLAG);
		cmdq->q.llq.u.prod = prod;

		/*
		 * c. Wait for any gathered work to be written to the queue.
		 * Note that we read our own entries so that we have the control
		 * dependency required by (d).
		 */
		arm_smmu_cmdq_poll_valid_map(cmdq, llq.u.prod, prod);

		/*
		 * d. Advance the hardware prod pointer
		 * Control dependency ordering from the entries becoming valid.
		 */
		__raw_writel(prod, cmdq->q.prod_reg);

		/*
		 * e. Tell the next owner we're done
		 * Make sure we've updated the hardware first, so that we don't
		 * race to update prod and potentially move it backwards.
		 */
		atomic_set(&cmdq->owner_prod, prod);
	}

	/* 5. If we are inserting a CMD_SYNC, we must wait for it to complete */
	if (sync) {
		llq.u.prod = queue_inc_prod_n(&llq, n);
		ret = arm_smmu_cmdq_poll_until_sync(&llq);
		if (ret) {
			con_log("smmuv3: CMD_SYNC timeout at 0x%08x [hwprod 0x%08x, hwcons 0x%08x]\n",
				llq.u.prod,
				__raw_readl(cmdq->q.prod_reg),
				__raw_readl(cmdq->q.cons_reg));
		}

		/*
		 * Try to unlock the cmq lock. This will fail if we're the last
		 * reader, in which case we can safely update cmdq->q.llq.cons
		 */
		if (!arm_smmu_cmdq_shared_tryunlock(cmdq)) {
			WRITE_ONCE(cmdq->q.llq.u.cons, llq.u.cons);
			arm_smmu_cmdq_shared_unlock(cmdq);
		}
	}

	irq_local_restore(flags);
	return ret;
}

static int arm_smmu_cmdq_issue_cmd(struct arm_smmu_cmdq_ent *ent)
{
	uint64_t cmd[CMDQ_ENT_DWORDS];

	if (arm_smmu_cmdq_build_cmd(cmd, ent)) {
		con_log("smmuv3: ignoring unknown CMDQ opcode 0x%x\n",
			ent->opcode);
		return -EINVAL;
	}

	return arm_smmu_cmdq_issue_cmdlist(cmd, 1, false);
}

static int arm_smmu_cmdq_issue_sync(void)
{
	return arm_smmu_cmdq_issue_cmdlist(NULL, 0, true);
}

static void arm_smmu_sync_ste_for_sid(uint32_t sid)
{
	struct arm_smmu_cmdq_ent cmd = {
		.opcode	= CMDQ_OP_CFGI_STE,
		.cfgi	= {
			.sid	= sid,
			.leaf	= true,
		},
	};

	arm_smmu_cmdq_issue_cmd(&cmd);
	arm_smmu_cmdq_issue_sync();
}

static void
arm_smmu_atc_inv_to_cmd(int ssid, unsigned long iova, size_t size,
			struct arm_smmu_cmdq_ent *cmd)
{
	size_t log2_span;
	size_t span_mask;
	/* ATC invalidates are always on 4096-bytes pages */
	size_t inval_grain_shift = 12;
	unsigned long page_start, page_end;

	*cmd = (struct arm_smmu_cmdq_ent) {
		.opcode			= CMDQ_OP_ATC_INV,
		.substream_valid	= !!ssid,
		.atc.ssid		= ssid,
	};

	if (!size) {
		cmd->atc.size = ATC_INV_SIZE_ALL;
		return;
	}

	page_start	= iova >> inval_grain_shift;
	page_end	= (iova + size - 1) >> inval_grain_shift;

	/*
	 * In an ATS Invalidate Request, the address must be aligned on the
	 * range size, which must be a power of two number of page sizes. We
	 * thus have to choose between grossly over-invalidating the region, or
	 * splitting the invalidation into multiple commands. For simplicity
	 * we'll go with the first solution, but should refine it in the future
	 * if multiple commands are shown to be more efficient.
	 *
	 * Find the smallest power of two that covers the range. The most
	 * significant differing bit between the start and end addresses,
	 * fls(start ^ end), indicates the required span. For example:
	 *
	 * We want to invalidate pages [8; 11]. This is already the ideal range:
	 *		x = 0b1000 ^ 0b1011 = 0b11
	 *		span = 1 << fls(x) = 4
	 *
	 * To invalidate pages [7; 10], we need to invalidate [0; 15]:
	 *		x = 0b0111 ^ 0b1010 = 0b1101
	 *		span = 1 << fls(x) = 16
	 */
	log2_span	= __fls64(page_start ^ page_end);
	span_mask	= (1ULL << log2_span) - 1;

	page_start	&= ~span_mask;

	cmd->atc.addr	= page_start << inval_grain_shift;
	cmd->atc.size	= log2_span;
}

static int arm_smmu_atc_inv_master(struct arm_smmu_cmdq_ent *cmd)
{
	int i;

	if (!smmu_group_ctrl.ats_enabled)
		return 0;

	for (i = 0; i < smmu_group_ctrl.num_sids; i++) {
		cmd->atc.sid = smmu_group_ctrl.sids[i];
		arm_smmu_cmdq_issue_cmd(cmd);
	}

	return arm_smmu_cmdq_issue_sync();
}

static int arm_smmu_atc_inv_domain(int ssid, unsigned long iova, size_t size)
{
	int ret = 0;
	__unused irq_flags_t flags;
	struct arm_smmu_cmdq_ent cmd;
	struct smmu_group *group;
	iommu_grp_t grp;

	if (!(smmu_device_ctrl.features & ARM_SMMU_FEAT_ATS))
		return 0;

	/*
	 * Ensure that we've completed prior invalidation of the main TLBs
	 * before we read 'nr_ats_masters' in case of a concurrent call to
	 * arm_smmu_enable_ats():
	 *
	 *	// unmap()			// arm_smmu_enable_ats()
	 *	TLBI+SYNC			atomic_inc(&nr_ats_masters);
	 *	smp_mb();			[...]
	 *	atomic_read(&nr_ats_masters);	pci_enable_ats() // writel()
	 *
	 * Ensures that we always see the incremented 'nr_ats_masters' count if
	 * ATS was enabled at the PCI device before completion of the TLBI.
	 */
	smp_mb();
	if (!atomic_read(&smmu_domain_ctrl.nr_ats_masters))
		return 0;

	arm_smmu_atc_inv_to_cmd(ssid, iova, size, &cmd);

	list_for_each_entry(struct smmu_group, group,
			    &smmu_domain_ctrl.devices, domain_head) {
		grp = iommu_group_save(group->grp);
		ret |= arm_smmu_atc_inv_master(&cmd);
		iommu_group_restore(grp);
	}

	return ret ? -ETIMEDOUT : 0;
}

/* IO_PGTABLE API */
void smmuv3_tlb_inv_context(void)
{
	struct arm_smmu_cmdq_ent cmd;

	if (smmu_domain_ctrl.stage == ARM_SMMU_DOMAIN_S1) {
		cmd.opcode	= CMDQ_OP_TLBI_NH_ASID;
		cmd.tlbi.asid	= smmu_domain_ctrl.s1_cfg.cd.asid;
		cmd.tlbi.vmid	= 0;
	} else {
		cmd.opcode	= CMDQ_OP_TLBI_S12_VMALL;
		cmd.tlbi.vmid	= smmu_domain_ctrl.s2_cfg.vmid;
	}

	/*
	 * NOTE: when io-pgtable is in non-strict mode, we may get here with
	 * PTEs previously cleared by unmaps on the current CPU not yet visible
	 * to the SMMU. We are relying on the dma_wmb() implicit during cmd
	 * insertion to guarantee those are observed before the TLBI. Do be
	 * careful, 007.
	 */
	arm_smmu_cmdq_issue_cmd(&cmd);
	arm_smmu_cmdq_issue_sync();
	arm_smmu_atc_inv_domain(0, 0, 0);
}

static void arm_smmu_tlb_inv_range(unsigned long iova, size_t size,
				   size_t granule, bool leaf)
{
	uint64_t cmds[CMDQ_BATCH_ENTRIES * CMDQ_ENT_DWORDS];
	unsigned long start = iova, end = iova + size;
	int i = 0;
	struct arm_smmu_cmdq_ent cmd = {
		.tlbi = {
			.leaf	= leaf,
		},
	};

	if (!size)
		return;

	if (smmu_domain_ctrl.stage == ARM_SMMU_DOMAIN_S1) {
		cmd.opcode	= CMDQ_OP_TLBI_NH_VA;
		cmd.tlbi.asid	= smmu_domain_ctrl.s1_cfg.cd.asid;
	} else {
		cmd.opcode	= CMDQ_OP_TLBI_S2_IPA;
		cmd.tlbi.vmid	= smmu_domain_ctrl.s2_cfg.vmid;
	}

	while (iova < end) {
		if (i == CMDQ_BATCH_ENTRIES) {
			arm_smmu_cmdq_issue_cmdlist(cmds, i, false);
			i = 0;
		}

		cmd.tlbi.addr = iova;
		arm_smmu_cmdq_build_cmd(&cmds[i * CMDQ_ENT_DWORDS], &cmd);
		iova += granule;
		i++;
	}

	arm_smmu_cmdq_issue_cmdlist(cmds, i, true);

	/*
	 * Unfortunately, this can't be leaf-only since we may have
	 * zapped an entire table.
	 */
	arm_smmu_atc_inv_domain(0, start, size);
}

void smmuv3_tlb_inv_page_nosync(struct iommu_iotlb_gather *gather,
				unsigned long iova, size_t granule)
{
	iommu_iotlb_gather_add_page(gather, iova, granule);
}

void smmuv3_tlb_inv_walk(unsigned long iova, size_t size, size_t granule)
{
	arm_smmu_tlb_inv_range(iova, size, granule, false);
}

void smmuv3_tlb_inv_leaf(unsigned long iova, size_t size, size_t granule)
{
	arm_smmu_tlb_inv_range(iova, size, granule, true);
}

void smmuv3_flush_iotlb_all(void)
{
	smmuv3_tlb_inv_context();
}

void smmuv3_iotlb_sync(struct iommu_iotlb_gather *gather)
{
	arm_smmu_tlb_inv_range(gather->start, gather->end - gather->start,
			       gather->pgsize, true);
}

/* Probing and initialisation functions */
static void arm_smmu_init_one_queue(struct arm_smmu_queue *q,
				    caddr_t prod_reg,
				    caddr_t cons_reg,
				    size_t dwords, const char *name)
{
	size_t qsz;

	do {
		qsz = ((1 << q->llq.max_n_shift) * dwords) << 3;
#ifdef CONFIG_SIMULATION_SMMU
		q->base = (void *)dma_alloc_coherent(iommu_device_ctrl.dma,
						     qsz << 1, &q->base_dma);
		q->base = (void *)ALIGN((caddr_t)q->base, qsz);
		q->base_dma = (dma_addr_t)ALIGN(q->base_dma, qsz);
#else
		q->base = (void *)dma_alloc_coherent(iommu_device_ctrl.dma,
						     qsz, &q->base_dma);
#endif
		if (q->base || qsz < PAGE_SIZE)
			break;

		q->llq.max_n_shift--;
	} while (1);

	if (!q->base) {
		con_err("smmuv3: failed to allocate queue (0x%zx bytes) for %s\n",
			qsz, name);
		BUG();
		return;
	}

	if (!(q->base_dma & (qsz - 1))) {
		con_log("smmuv3: allocated %u entries for %s\n",
			1 << q->llq.max_n_shift, name);
	}

	q->prod_reg	= arm_smmu_page1_fixup(prod_reg);
	q->cons_reg	= arm_smmu_page1_fixup(cons_reg);
	q->ent_dwords	= dwords;

#ifdef CONFIG_SIMULATION_SMMU
	q->q_base  = q->base_dma & Q_BASE_ADDR_MASK;
#else
	q->q_base  = Q_BASE_RWA;
	q->q_base |= q->base_dma & Q_BASE_ADDR_MASK;
#endif
	con_log("smmuv3: %s: BASE=%016llx, SIZE=%d(%d), RWA=%08lx, MASK=%08lx\n",
		name, (unsigned long long)q->q_base,
		(1 << q->llq.max_n_shift), (int)dwords,
		(unsigned long)Q_BASE_RWA,
		(unsigned long)Q_BASE_ADDR_MASK);
	q->q_base |= FIELD_PREP(Q_BASE_LOG2SIZE, q->llq.max_n_shift);

	q->llq.u.prod = q->llq.u.cons = 0;
}

static void arm_smmu_cmdq_init(void)
{
	struct arm_smmu_cmdq *cmdq = &smmu_device_ctrl.cmdq;
	unsigned int nents = 1 << cmdq->q.llq.max_n_shift;
	atomic_long_t *bitmap;

	atomic_set(&cmdq->owner_prod, 0);
	atomic_set(&cmdq->lock, 0);

	bitmap = (atomic_long_t *)heap_calloc(BITS_TO_UNITS(nents) * sizeof(bits_t));
	if (!bitmap) {
		con_err("smmuv3: failed to allocate cmdq bitmap\n");
		BUG();
		return;
	}
	cmdq->valid_map = bitmap;
}

static void arm_smmu_init_queues(void)
{
	/* cmdq */
	arm_smmu_init_one_queue(&smmu_device_ctrl.cmdq.q,
				SMMU_CMDQ_PROD(iommu_dev),
				SMMU_CMDQ_CONS(iommu_dev),
				CMDQ_ENT_DWORDS, "CMDQ");
	arm_smmu_cmdq_init();

	/* evtq */
	arm_smmu_init_one_queue(&smmu_device_ctrl.evtq.q,
				SMMU_EVTQ_PROD(iommu_dev),
				SMMU_EVTQ_CONS(iommu_dev),
				EVTQ_ENT_DWORDS, "EVTQ");

	/* priq */
	if (!(smmu_device_ctrl.features & ARM_SMMU_FEAT_PRI))
		return;
	arm_smmu_init_one_queue(&smmu_device_ctrl.priq.q,
				SMMU_PRIQ_PROD(iommu_dev),
				SMMU_PRIQ_CONS(iommu_dev),
				PRIQ_ENT_DWORDS, "PRIQ");
}

/* Stream table manipulation functions */
static void arm_smmu_write_strtab_l1_desc(bool l1std, uint32_t sid,
					  uint64_t *dst,
					  struct arm_smmu_strtab_l1_desc *desc)
{
	uint64_t val = 0;

	val |= FIELD_PREP(STRTAB_L1_DESC_SPAN, desc->span);
	val |= desc->l2ptr_dma & STRTAB_L1_DESC_L2PTR_MASK;

	*dst = cpu_to_le64(val);
	con_log("smmuv3: %s(%d): %016llx=%016llx\n",
		l1std ? "L1STD" : "STD", sid,
		(unsigned long long)dst,
		(unsigned long long)cpu_to_le64(val));
}

static void arm_smmu_write_strtab_ent(struct smmu_group *group,
				      uint32_t sid, uint64_t *dst)
{
	/*
	 * This is hideously complicated, but we only really care about
	 * three cases at the moment:
	 *
	 * 1. Invalid (all zero) -> bypass/fault (init)
	 * 2. Bypass/fault -> translation/bypass (attach)
	 * 3. Translation/bypass -> bypass/fault (detach)
	 *
	 * Given that we can't update the STE atomically and the SMMU
	 * doesn't read the thing in a defined order, that leaves us
	 * with the following maintenance requirements:
	 *
	 * 1. Update Config, return (init time STEs aren't live)
	 * 2. Write everything apart from dword 0, sync, write dword 0, sync
	 * 3. Update Config, sync
	 */
	uint64_t val = le64_to_cpu(dst[0]);
	bool ste_live = false;
	struct arm_smmu_s1_cfg *s1_cfg = NULL;
	struct arm_smmu_s2_cfg *s2_cfg = NULL;
	struct smmu_domain *smmu_domain = NULL;
	struct arm_smmu_cmdq_ent prefetch_cmd = {
		.opcode		= CMDQ_OP_PREFETCH_CFG,
		.prefetch	= {
			.sid	= sid,
		},
	};

	if (group) {
		BUG_ON(group != &smmu_group_ctrl);
		if (iommu_group_ctrl.dom != INVALID_IOMMU_DOM) {
			iommu_domain_select(iommu_group_ctrl.dom);
			smmu_domain = &smmu_domain_ctrl;
		}
	}
	if (smmu_domain) {
		switch (smmu_domain->stage) {
		case ARM_SMMU_DOMAIN_S1:
			s1_cfg = &smmu_domain->s1_cfg;
			break;
		case ARM_SMMU_DOMAIN_S2:
		case ARM_SMMU_DOMAIN_NESTED:
			s2_cfg = &smmu_domain->s2_cfg;
			break;
		default:
			break;
		}
	}

	if (val & STRTAB_STE_0_V) {
		switch (FIELD_GET(STRTAB_STE_0_CFG, val)) {
		case STRTAB_STE_0_CFG_BYPASS:
			break;
		case STRTAB_STE_0_CFG_S1_TRANS:
		case STRTAB_STE_0_CFG_S2_TRANS:
			ste_live = true;
			break;
		case STRTAB_STE_0_CFG_ABORT:
			BUG_ON(!smmu_disable_bypass);
			break;
		default:
			BUG(); /* STE corruption */
		}
	}

	/* Nuke the existing STE_0 value, as we're going to rewrite it */
	val = STRTAB_STE_0_V;

	/* Bypass/fault */
	if (!smmu_domain || !(s1_cfg || s2_cfg)) {
		if (!smmu_domain && smmu_disable_bypass)
			val |= FIELD_PREP(STRTAB_STE_0_CFG, STRTAB_STE_0_CFG_ABORT);
		else
			val |= FIELD_PREP(STRTAB_STE_0_CFG, STRTAB_STE_0_CFG_BYPASS);

		dst[0] = cpu_to_le64(val);
		dst[1] = cpu_to_le64(FIELD_PREP(STRTAB_STE_1_SHCFG,
						STRTAB_STE_1_SHCFG_INCOMING));
		dst[2] = 0; /* Nuke the VMID */
		con_log("smmuv3: STE(%08x): %016llx=%016llx\n"
			"                       %016llx=%016llx\n"
			"                       %016llx=%016llx\n"
			"                       %016llx=%016llx\n",
			(unsigned int)sid,
			(unsigned long long)&dst[0],
			(unsigned long long)dst[0],
			(unsigned long long)&dst[1],
			(unsigned long long)dst[1],
			(unsigned long long)&dst[2],
			(unsigned long long)dst[2],
			(unsigned long long)&dst[3],
			(unsigned long long)dst[3]);
		/*
		 * The SMMU can perform negative caching, so we must sync
		 * the STE regardless of whether the old value was live.
		 */
		arm_smmu_sync_ste_for_sid(sid);
		return;
	}

	if (s1_cfg) {
		BUG_ON(ste_live);
		dst[1] = cpu_to_le64(
			 FIELD_PREP(STRTAB_STE_1_S1DSS, STRTAB_STE_1_S1DSS_SSID0) |
			 FIELD_PREP(STRTAB_STE_1_S1CIR, STRTAB_STE_1_S1C_CACHE_WBRA) |
			 FIELD_PREP(STRTAB_STE_1_S1COR, STRTAB_STE_1_S1C_CACHE_WBRA) |
			 FIELD_PREP(STRTAB_STE_1_S1CSH, ARM_SMMU_SH_ISH) |
			 FIELD_PREP(STRTAB_STE_1_STRW, STRTAB_STE_1_STRW_NSEL1));

		if (smmu_device_ctrl.features & ARM_SMMU_FEAT_STALLS &&
		   !(smmu_device_ctrl.features & ARM_SMMU_FEAT_STALL_FORCE))
			dst[1] |= cpu_to_le64(STRTAB_STE_1_S1STALLD);

		val |= (s1_cfg->cdcfg.cdtab_dma & STRTAB_STE_0_S1CTXPTR_MASK) |
			FIELD_PREP(STRTAB_STE_0_CFG, STRTAB_STE_0_CFG_S1_TRANS) |
			FIELD_PREP(STRTAB_STE_0_S1CDMAX, s1_cfg->s1cdmax) |
			FIELD_PREP(STRTAB_STE_0_S1FMT, s1_cfg->s1fmt);
	}

	if (s2_cfg) {
		BUG_ON(ste_live);
		dst[2] = cpu_to_le64(
			 FIELD_PREP(STRTAB_STE_2_S2VMID, s2_cfg->vmid) |
			 FIELD_PREP(STRTAB_STE_2_VTCR, s2_cfg->vtcr) |
#ifdef __BIG_ENDIAN
			 STRTAB_STE_2_S2ENDI |
#endif
			 STRTAB_STE_2_S2PTW | STRTAB_STE_2_S2AA64 |
			 STRTAB_STE_2_S2R);

		dst[3] = cpu_to_le64(s2_cfg->vttbr & STRTAB_STE_3_S2TTB_MASK);

		val |= FIELD_PREP(STRTAB_STE_0_CFG, STRTAB_STE_0_CFG_S2_TRANS);
	}

	if (group && group->ats_enabled)
		dst[1] |= cpu_to_le64(FIELD_PREP(STRTAB_STE_1_EATS,
						 STRTAB_STE_1_EATS_TRANS));

	arm_smmu_sync_ste_for_sid(sid);
	/* See comment in arm_smmu_write_ctx_desc() */
	WRITE_ONCE(dst[0], cpu_to_le64(val));
	con_log("smmuv3: STE(%08x): %016llx=%016llx\n"
		"                       %016llx=%016llx\n"
		"                       %016llx=%016llx\n"
		"                       %016llx=%016llx\n",
		(unsigned int)sid,
		(unsigned long long)&dst[0],
		(unsigned long long)dst[0],
		(unsigned long long)&dst[1],
		(unsigned long long)dst[1],
		(unsigned long long)&dst[2],
		(unsigned long long)dst[2],
		(unsigned long long)&dst[3],
		(unsigned long long)dst[3]);
	arm_smmu_sync_ste_for_sid(sid);

	/* It's likely that we'll want to use the new STE soon */
	if (!(smmu_device_ctrl.options & ARM_SMMU_OPT_SKIP_PREFETCH))
		arm_smmu_cmdq_issue_cmd(&prefetch_cmd);
}

static void arm_smmu_init_bypass_stes(uint64_t *strtab, unsigned int nent)
{
	unsigned int i;

	for (i = 0; i < nent; ++i) {
		arm_smmu_write_strtab_ent(NULL, -1, strtab);
		strtab += STRTAB_STE_DWORDS;
	}
}

static void arm_smmu_init_l1_strtab(void)
{
	unsigned int i;
	struct arm_smmu_strtab_cfg *cfg = &smmu_device_ctrl.strtab_cfg;
	size_t size = sizeof(*cfg->l1_desc) * cfg->num_l1_ents;
	void *strtab = smmu_device_ctrl.strtab_cfg.strtab;

	cfg->l1_desc = (struct arm_smmu_strtab_l1_desc *)heap_calloc(size);
	if (!cfg->l1_desc) {
		con_err("smmuv3: failed to allocate l1 stream table desc\n");
		BUG();
		return;
	}

#if 0
	con_log("smmuv3: L1STD descriptors: BASE=%016llx, SIZE=%d/%d\n",
		(unsigned long long)cfg->l1_desc,
		(int)cfg->num_l1_ents, (int)sizeof(*cfg->l1_desc));
#endif
	for (i = 0; i < cfg->num_l1_ents; ++i) {
		arm_smmu_write_strtab_l1_desc(true, i << STRTAB_SPLIT,
					      strtab, &cfg->l1_desc[i]);
		strtab += STRTAB_L1_DESC_DWORDS << 3;
	}
}

static void arm_smmu_init_l2_strtab(uint32_t sid)
{
	size_t size;
	void *strtab;
	struct arm_smmu_strtab_cfg *cfg = &smmu_device_ctrl.strtab_cfg;
	struct arm_smmu_strtab_l1_desc *desc = &cfg->l1_desc[sid >> STRTAB_SPLIT];
	/* The level 2 array is aligned to its size by the SMMU */
	uint8_t align = 1 << 6;

	if (desc->l2ptr)
		return;

	size = 1 << (STRTAB_SPLIT + ilog2_const(STRTAB_STE_DWORDS) + 3);
	strtab = &cfg->strtab[(sid >> STRTAB_SPLIT) * STRTAB_L1_DESC_DWORDS];

	desc->span = STRTAB_SPLIT + 1;
	desc->l2ptr = (void *)dma_alloc_coherent(iommu_device_ctrl.dma,
						 size + align,
						 &desc->l2ptr_dma);
	if (!desc->l2ptr) {
		con_err("smmuv3: failed to allocate l2 stream table for SID %u\n",
			sid);
		BUG();
	}
	desc->l2ptr = (void *)ALIGN((caddr_t)desc->l2ptr, align);
	desc->l2ptr_dma = ALIGN(desc->l2ptr_dma, align);

	con_log("smmuv3: L2 Ptr (%d): BASE=%016llx SIZE=%d/%d\n",
		sid, (unsigned long long)desc->l2ptr_dma,
		1 << STRTAB_SPLIT, 1 << STRTAB_L1_DESC_DWORDS);
	arm_smmu_init_bypass_stes(desc->l2ptr, 1 << STRTAB_SPLIT);
	arm_smmu_write_strtab_l1_desc(false, sid, strtab, desc);
}

static void arm_smmu_init_strtab_2lvl(void)
{
	void *strtab;
	uint64_t reg;
	uint32_t size, l1size;
	struct arm_smmu_strtab_cfg *cfg = &smmu_device_ctrl.strtab_cfg;

	/* Calculate the L1 size, capped to the SIDSIZE. */
	size = STRTAB_L1_SZ_SHIFT - (ilog2_const(STRTAB_L1_DESC_DWORDS) + 3);
	size = min(size, smmu_device_ctrl.sid_bits - STRTAB_SPLIT);
	cfg->num_l1_ents = 1 << size;

	size += STRTAB_SPLIT;
	if (size < smmu_device_ctrl.sid_bits)
		con_log("smmuv3: 2-level strtab only covers %u/%u bits of SID\n",
			size, smmu_device_ctrl.sid_bits);

	l1size = cfg->num_l1_ents * (STRTAB_L1_DESC_DWORDS << 3);
	strtab = (void *)dma_alloc_coherent(iommu_device_ctrl.dma, l1size,
					    &cfg->strtab_dma);
	if (!strtab) {
		con_err("smmuv3: failed to allocate l1 stream table (%u bytes)\n", size);
		BUG();
		return;
	}
	cfg->strtab = strtab;
	con_log("smmuv3: 2-Level STD: BASE=%016llx, SIZE=%d/%d\n",
		(unsigned long long)cfg->strtab,
		(int)cfg->num_l1_ents, STRTAB_L1_DESC_DWORDS << 3);

	/* Configure strtab_base_cfg for 2 levels */
	reg  = FIELD_PREP(STRTAB_BASE_CFG_FMT, STRTAB_BASE_CFG_FMT_2LVL);
	reg |= FIELD_PREP(STRTAB_BASE_CFG_LOG2SIZE, size);
	reg |= FIELD_PREP(STRTAB_BASE_CFG_SPLIT, STRTAB_SPLIT);
	cfg->strtab_base_cfg = reg;

	arm_smmu_init_l1_strtab();
}

static void arm_smmu_init_strtab_linear(void)
{
	void *strtab;
	uint64_t reg;
	uint32_t size;
	struct arm_smmu_strtab_cfg *cfg = &smmu_device_ctrl.strtab_cfg;

	size = (1 << smmu_device_ctrl.sid_bits) * (STRTAB_STE_DWORDS << 3);
	strtab = (void *)dma_alloc_coherent(iommu_device_ctrl.dma, size,
					    &cfg->strtab_dma);
	if (!strtab) {
		con_err("smmuv3: failed to allocate linear stream table (%u bytes)\n", size);
		BUG();
		return;
	}
	cfg->strtab = strtab;
	cfg->num_l1_ents = 1 << smmu_device_ctrl.sid_bits;
	con_log("smmuv3: Linear STD: BASE=%016llx, SIZE=%d/%d\n",
		(unsigned long long)cfg->strtab, cfg->num_l1_ents,
		STRTAB_STE_DWORDS);

	/* Configure strtab_base_cfg for a linear table covering all SIDs */
	reg  = FIELD_PREP(STRTAB_BASE_CFG_FMT, STRTAB_BASE_CFG_FMT_LINEAR);
	reg |= FIELD_PREP(STRTAB_BASE_CFG_LOG2SIZE, smmu_device_ctrl.sid_bits);
	cfg->strtab_base_cfg = reg;

	arm_smmu_init_bypass_stes(strtab, cfg->num_l1_ents);
}

static void arm_smmu_init_strtab(void)
{
	uint64_t reg;

	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_2_LVL_STRTAB)
		arm_smmu_init_strtab_2lvl();
	else
		arm_smmu_init_strtab_linear();

	/* Set the strtab base address */
	reg  = smmu_device_ctrl.strtab_cfg.strtab_dma & STRTAB_BASE_ADDR_MASK;
	reg |= STRTAB_BASE_RA;
	smmu_device_ctrl.strtab_cfg.strtab_base = reg;

	/* Allocate the first VMID for stage-2 bypass STEs */
	set_bit(0, smmu_device_ctrl.vmid_map);
}

static uint64_t *arm_smmu_get_step_for_sid(uint32_t sid)
{
	uint64_t *step;
	struct arm_smmu_strtab_cfg *cfg = &smmu_device_ctrl.strtab_cfg;

	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_2_LVL_STRTAB) {
		struct arm_smmu_strtab_l1_desc *l1_desc;
		int idx;

		/* Two-level walk */
		idx = (sid >> STRTAB_SPLIT) * STRTAB_L1_DESC_DWORDS;
		l1_desc = &cfg->l1_desc[idx];
		idx = (sid & ((1 << STRTAB_SPLIT) - 1)) * STRTAB_STE_DWORDS;
		step = &l1_desc->l2ptr[idx];
	} else {
		/* Simple linear lookup */
		step = &cfg->strtab[sid * STRTAB_STE_DWORDS];
	}

	return step;
}

static void arm_smmu_install_ste_for_dev(void)
{
	int i, j;

	for (i = 0; i < smmu_group_ctrl.num_sids; ++i) {
		uint32_t sid = smmu_group_ctrl.sids[i];
		uint64_t *step = arm_smmu_get_step_for_sid(sid);

		/* Bridged PCI devices may end up with duplicated IDs */
		for (j = 0; j < i; j++)
			if (smmu_group_ctrl.sids[j] == sid)
				break;
		if (j < i)
			continue;

		arm_smmu_write_strtab_ent(&smmu_group_ctrl, sid, step);
	}
}

/* Context descriptor manipulation functions */
static void arm_smmu_sync_cd(int ssid, bool leaf)
{
	size_t i;
	struct arm_smmu_cmdq_ent cmd = {
		.opcode	= CMDQ_OP_CFGI_CD,
		.cfgi	= {
			.ssid	= ssid,
			.leaf	= leaf,
		},
	};
	struct smmu_group *group;
	iommu_grp_t grp;

	list_for_each_entry(struct smmu_group, group,
			    &smmu_domain_ctrl.devices, domain_head) {
		grp = iommu_group_save(group->grp);
		for (i = 0; i < smmu_group_ctrl.num_sids; i++) {
			cmd.cfgi.sid = group->sids[i];
			arm_smmu_cmdq_issue_cmd(&cmd);
		}
		iommu_group_restore(grp);
	}
	arm_smmu_cmdq_issue_sync();
}

static void arm_smmu_alloc_cd_leaf_table(struct arm_smmu_l1_ctx_desc *l1_desc)
{
	size_t size = CTXDESC_L2_ENTRIES * (CTXDESC_CD_DWORDS << 3);

	l1_desc->l2ptr = (void *)dma_alloc_coherent(iommu_device_ctrl.dma, size,
						    &l1_desc->l2ptr_dma);
	if (!l1_desc->l2ptr) {
		con_log("smmuv3: failed to allocate context descriptor table\n");
		BUG();
	}
}

static void arm_smmu_write_cd_l1_desc(uint64_t *dst,
				      struct arm_smmu_l1_ctx_desc *l1_desc)
{
	uint64_t val = (l1_desc->l2ptr_dma & CTXDESC_L1_DESC_L2PTR_MASK) |
		  CTXDESC_L1_DESC_V;

	WRITE_ONCE(*dst, cpu_to_le64(val));
	con_log("smmuv3: L1CD: (%016llx)=%016llx\n",
		(unsigned long long)dst,
		(unsigned long long)cpu_to_le64(val));
}

static uint64_t *arm_smmu_get_cd_ptr(uint32_t ssid)
{
	uint64_t *l1ptr;
	unsigned int idx;
	struct arm_smmu_l1_ctx_desc *l1_desc;
	struct arm_smmu_ctx_desc_cfg *cdcfg = &smmu_domain_ctrl.s1_cfg.cdcfg;

	if (smmu_domain_ctrl.s1_cfg.s1fmt == STRTAB_STE_0_S1FMT_LINEAR)
		return cdcfg->cdtab + ssid * CTXDESC_CD_DWORDS;

	idx = ssid >> CTXDESC_SPLIT;
	l1_desc = &cdcfg->l1_desc[idx];
	if (!l1_desc->l2ptr) {
		arm_smmu_alloc_cd_leaf_table(l1_desc);

		l1ptr = cdcfg->cdtab + idx * CTXDESC_L1_DESC_DWORDS;
		arm_smmu_write_cd_l1_desc(l1ptr, l1_desc);
		/* An invalid L1CD can be cached */
		arm_smmu_sync_cd(ssid, false);
	}
	idx = ssid & (CTXDESC_L2_ENTRIES - 1);
	return l1_desc->l2ptr + idx * CTXDESC_CD_DWORDS;
}

static void arm_smmu_write_ctx_desc(int ssid, struct arm_smmu_ctx_desc *cd)
{
	/*
	 * This function handles the following cases:
	 *
	 * (1) Install primary CD, for normal DMA traffic (SSID = 0).
	 * (2) Install a secondary CD, for SID+SSID traffic.
	 * (3) Update ASID of a CD. Atomically write the first 64 bits of the
	 *     CD, then invalidate the old entry and mappings.
	 * (4) Remove a secondary CD.
	 */
	uint64_t val;
	bool cd_live;
	uint64_t *cdptr;

	BUG_ON(ssid >= (1 << smmu_domain_ctrl.s1_cfg.s1cdmax));

	cdptr = arm_smmu_get_cd_ptr(ssid);
	BUG_ON(!cdptr);

	val = le64_to_cpu(cdptr[0]);
	cd_live = !!(val & CTXDESC_CD_0_V);

	if (!cd) { /* (4) */
		val = 0;
	} else if (cd_live) { /* (3) */
		val &= ~CTXDESC_CD_0_ASID;
		val |= FIELD_PREP(CTXDESC_CD_0_ASID, cd->asid);
		/*
		 * Until CD+TLB invalidation, both ASIDs may be used for tagging
		 * this substream's traffic
		 */
	} else { /* (1) and (2) */
		cdptr[1] = cpu_to_le64(cd->ttbr & CTXDESC_CD_1_TTB0_MASK);
		cdptr[2] = 0;
		cdptr[3] = cpu_to_le64(cd->mair);

		/*
		 * STE is live, and the SMMU might read dwords of this CD in any
		 * order. Ensure that it observes valid values before reading
		 * V=1.
		 */
		arm_smmu_sync_cd(ssid, true);

		val = cd->tcr |
#ifdef __BIG_ENDIAN
			CTXDESC_CD_0_ENDI |
#endif
			CTXDESC_CD_0_R | CTXDESC_CD_0_A | CTXDESC_CD_0_ASET |
			CTXDESC_CD_0_AA64 |
			FIELD_PREP(CTXDESC_CD_0_ASID, cd->asid) |
			CTXDESC_CD_0_V;

		/* STALL_MODEL==0b10 && CD.S==0 is ILLEGAL */
		if (smmu_device_ctrl.features & ARM_SMMU_FEAT_STALL_FORCE)
			val |= CTXDESC_CD_0_S;
	}

	/*
	 * The SMMU accesses 64-bit values atomically. See IHI0070Ca 3.21.3
	 * "Configuration structures and configuration invalidation completion"
	 *
	 *   The size of single-copy atomic reads made by the SMMU is
	 *   IMPLEMENTATION DEFINED but must be at least 64 bits. Any single
	 *   field within an aligned 64-bit span of a structure can be altered
	 *   without first making the structure invalid.
	 */
	WRITE_ONCE(cdptr[0], cpu_to_le64(val));
	con_log("smmuv3: CD(%06x): %016llx=%016llx\n"
		"                    %016llx=%016llx\n"
		"                    %016llx=%016llx\n"
		"                    %016llx=%016llx\n",
		(unsigned int)ssid,
		(unsigned long long)&cdptr[0], (unsigned long long)cdptr[0],
		(unsigned long long)&cdptr[1], (unsigned long long)cdptr[1],
		(unsigned long long)&cdptr[2], (unsigned long long)cdptr[2],
		(unsigned long long)&cdptr[3], (unsigned long long)cdptr[3]);
	arm_smmu_sync_cd(ssid, true);
}

static int arm_smmu_alloc_cd_tables(void)
{
	int ret;
	size_t l1size;
	size_t max_contexts;
	struct arm_smmu_s1_cfg *cfg = &smmu_domain_ctrl.s1_cfg;
	struct arm_smmu_ctx_desc_cfg *cdcfg = &cfg->cdcfg;
	int ent_dwords;
	bool cd2lvl;

	max_contexts = 1 << cfg->s1cdmax;

	if (!(smmu_device_ctrl.features & ARM_SMMU_FEAT_2_LVL_CDTAB) ||
	    max_contexts <= CTXDESC_L2_ENTRIES) {
		cfg->s1fmt = STRTAB_STE_0_S1FMT_LINEAR;
		cdcfg->num_l1_ents = max_contexts;

		ent_dwords = CTXDESC_CD_DWORDS;
		cd2lvl = false;
		l1size = max_contexts * (CTXDESC_CD_DWORDS << 3);
	} else {
		cfg->s1fmt = STRTAB_STE_0_S1FMT_64K_L2;
		cdcfg->num_l1_ents = DIV_ROUND_UP(max_contexts,
						  CTXDESC_L2_ENTRIES);

		cdcfg->l1_desc = (void *)heap_calloc(cdcfg->num_l1_ents *
						     sizeof(*cdcfg->l1_desc));
		if (!cdcfg->l1_desc)
			return -ENOMEM;
		con_log("smmuv3: L1CD: BASE=%016llx, SIZE=%d/%d\n",
			(unsigned long long)cdcfg->l1_desc,
			cdcfg->num_l1_ents, (int)sizeof(*cdcfg->l1_desc));

		ent_dwords = CTXDESC_L1_DESC_DWORDS;
		cd2lvl = true;
		l1size = cdcfg->num_l1_ents * (CTXDESC_L1_DESC_DWORDS << 3);
	}

	cdcfg->cdtab = (void *)dma_alloc_coherent(iommu_device_ctrl.dma,
						  l1size, &cdcfg->cdtab_dma);
	if (!cdcfg->cdtab) {
		con_log("smmuv3: failed to allocate context descriptor\n");
		ret = -ENOMEM;
		goto err_free_l1;
	}
	con_log("smmuv3: %s: BASE=%016llx, SIZE=%d/%d\n",
		cd2lvl ? "2-Level CD" : "Linear CD",
		(unsigned long long)cdcfg->cdtab,
		cdcfg->num_l1_ents, ent_dwords);

	return 0;

err_free_l1:
	if (cdcfg->l1_desc) {
		heap_free((caddr_t)cdcfg->l1_desc);
		cdcfg->l1_desc = NULL;
	}
	return ret;
}

static void arm_smmu_free_cd_tables(void)
{
	int i;
	size_t size, l1size;
	struct arm_smmu_ctx_desc_cfg *cdcfg = &smmu_domain_ctrl.s1_cfg.cdcfg;

	if (cdcfg->l1_desc) {
		size = CTXDESC_L2_ENTRIES * (CTXDESC_CD_DWORDS << 3);

		for (i = 0; i < cdcfg->num_l1_ents; i++) {
			if (!cdcfg->l1_desc[i].l2ptr)
				continue;

			dma_free_coherent(iommu_device_ctrl.dma, size,
					  (caddr_t)cdcfg->l1_desc[i].l2ptr,
					  cdcfg->l1_desc[i].l2ptr_dma);
		}
		heap_free((caddr_t)(cdcfg->l1_desc));
		cdcfg->l1_desc = NULL;

		l1size = cdcfg->num_l1_ents * (CTXDESC_L1_DESC_DWORDS << 3);
	} else {
		l1size = cdcfg->num_l1_ents * (CTXDESC_CD_DWORDS << 3);
	}

	dma_free_coherent(iommu_device_ctrl.dma, l1size,
			  (caddr_t)(cdcfg->cdtab), cdcfg->cdtab_dma);
	cdcfg->cdtab_dma = 0;
	cdcfg->cdtab = NULL;
}

static int arm_smmu_domain_finalise_s1(struct io_pgtable_cfg *pgtbl_cfg)
{
	int ret;
	int asid;
	struct arm_smmu_s1_cfg *cfg = &smmu_domain_ctrl.s1_cfg;
	typeof(&pgtbl_cfg->arm_lpae_s1_cfg.tcr) tcr =
		&pgtbl_cfg->arm_lpae_s1_cfg.tcr;

	asid = arm_smmu_bitmap_alloc(smmu_device_ctrl.asid_map,
				     smmu_device_ctrl.asid_bits);
	if (asid < 0)
		return asid;

	cfg->s1cdmax = smmu_group_ctrl.ssid_bits;

	ret = arm_smmu_alloc_cd_tables();
	if (ret)
		goto out_free_asid;

	cfg->cd.asid	= (uint16_t)asid;
	cfg->cd.ttbr	= pgtbl_cfg->arm_lpae_s1_cfg.ttbr;
	cfg->cd.tcr	= FIELD_PREP(CTXDESC_CD_0_TCR_T0SZ, tcr->tsz) |
			  FIELD_PREP(CTXDESC_CD_0_TCR_TG0, tcr->tg) |
			  FIELD_PREP(CTXDESC_CD_0_TCR_IRGN0, tcr->irgn) |
			  FIELD_PREP(CTXDESC_CD_0_TCR_ORGN0, tcr->orgn) |
			  FIELD_PREP(CTXDESC_CD_0_TCR_SH0, tcr->sh) |
			  FIELD_PREP(CTXDESC_CD_0_TCR_IPS, tcr->ips) |
			  CTXDESC_CD_0_TCR_EPD1 | CTXDESC_CD_0_AA64;
	cfg->cd.mair	= pgtbl_cfg->arm_lpae_s1_cfg.mair;

	/*
	 * Note that this will end up calling arm_smmu_sync_cd() before
	 * the master has been added to the devices list for this domain.
	 * This isn't an issue because the STE hasn't been installed yet.
	 */
	arm_smmu_write_ctx_desc(0, &cfg->cd);
	return 0;

out_free_asid:
	arm_smmu_bitmap_free(smmu_device_ctrl.asid_map, asid);
	return ret;
}

static int arm_smmu_domain_finalise_s2(struct io_pgtable_cfg *pgtbl_cfg)
{
	int vmid;
	struct arm_smmu_s2_cfg *cfg = &smmu_domain_ctrl.s2_cfg;
	typeof(&pgtbl_cfg->arm_lpae_s2_cfg.vtcr) vtcr;

	vmid = arm_smmu_bitmap_alloc(smmu_device_ctrl.vmid_map,
				     smmu_device_ctrl.vmid_bits);
	if (vmid < 0)
		return vmid;

	vtcr = &pgtbl_cfg->arm_lpae_s2_cfg.vtcr;
	cfg->vmid	= (uint16_t)vmid;
	cfg->vttbr	= pgtbl_cfg->arm_lpae_s2_cfg.vttbr;
	cfg->vtcr	= FIELD_PREP(STRTAB_STE_2_VTCR_S2T0SZ, vtcr->tsz) |
			  FIELD_PREP(STRTAB_STE_2_VTCR_S2SL0, vtcr->sl) |
			  FIELD_PREP(STRTAB_STE_2_VTCR_S2IR0, vtcr->irgn) |
			  FIELD_PREP(STRTAB_STE_2_VTCR_S2OR0, vtcr->orgn) |
			  FIELD_PREP(STRTAB_STE_2_VTCR_S2SH0, vtcr->sh) |
			  FIELD_PREP(STRTAB_STE_2_VTCR_S2TG, vtcr->tg) |
			  FIELD_PREP(STRTAB_STE_2_VTCR_S2PS, vtcr->ps);
	return 0;
}

static void arm_smmu_domain_finalise(void)
{
	unsigned long ias, oas;
	struct io_pgtable_cfg pgtbl_cfg;
	int (*finalise_stage_fn)(struct io_pgtable_cfg *);

	if (iommu_domain_ctrl.type == IOMMU_DOMAIN_IDENTITY) {
		smmu_domain_ctrl.stage = ARM_SMMU_DOMAIN_BYPASS;
		return;
	}

	/* Restrict the stage to what we can actually support */
	if (!(smmu_device_ctrl.features & ARM_SMMU_FEAT_TRANS_S1))
		smmu_domain_ctrl.stage = ARM_SMMU_DOMAIN_S2;
	if (!(smmu_device_ctrl.features & ARM_SMMU_FEAT_TRANS_S2))
		smmu_domain_ctrl.stage = ARM_SMMU_DOMAIN_S1;

	switch (smmu_domain_ctrl.stage) {
	case ARM_SMMU_DOMAIN_S1:
		ias = (smmu_device_ctrl.features & ARM_SMMU_FEAT_VAX) ? 52 : 48;
		ias = min(ias, VA_BITS);
		oas = smmu_device_ctrl.ias;
		iommu_domain_ctrl.cfg.fmt = ARM_64_LPAE_S1;
		finalise_stage_fn = arm_smmu_domain_finalise_s1;
		break;
	case ARM_SMMU_DOMAIN_NESTED:
	case ARM_SMMU_DOMAIN_S2:
		ias = smmu_device_ctrl.ias;
		oas = smmu_device_ctrl.oas;
		iommu_domain_ctrl.cfg.fmt = ARM_64_LPAE_S2;
		finalise_stage_fn = arm_smmu_domain_finalise_s2;
		break;
	default:
		return;
	}

	pgtbl_cfg = (struct io_pgtable_cfg) {
		.pgsize_bitmap	= smmu_device_ctrl.pgsize_bitmap,
		.ias		= ias,
		.oas		= oas,
		.coherent_walk	= smmu_device_ctrl.features & ARM_SMMU_FEAT_COHERENCY,
#if 0
		.tlb		= &arm_smmu_flush_ops,
		.iommu_dev	= smmu->dev,
#endif
	};

#if 0
	if (smmu_domain_ctrl.non_strict)
		pgtbl_cfg.quirks |= IO_PGTABLE_QUIRK_NON_STRICT;
#endif

	BUG_ON(!iommu_pgtable_alloc(&pgtbl_cfg));

	iommu_domain_ctrl.geometry.aperture_end = (1UL << pgtbl_cfg.ias) - 1;
	iommu_domain_ctrl.geometry.force_aperture = true;

	finalise_stage_fn(&pgtbl_cfg);
#if 0
	smmu_domain->pgtbl_ops = pgtbl_ops;
#endif
}

static void arm_smmu_init_structures(void)
{
	arm_smmu_init_queues();
	arm_smmu_init_strtab();
}

static void arm_smmu_device_hw_probe(void)
{
	uint32_t reg;
	bool coherent = smmu_device_ctrl.features & ARM_SMMU_FEAT_COHERENCY;

	/* IDR0 */
	reg = __raw_readl(SMMU_IDR0(iommu_dev));

	/* 2-level structures */
	if (FIELD_GET(IDR0_ST_LVL, reg) == IDR0_ST_LVL_2LVL)
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_2_LVL_STRTAB;

	if (reg & IDR0_CD2L)
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_2_LVL_CDTAB;

	/*
	 * Translation table endianness.
	 * We currently require the same endianness as the CPU, but this
	 * could be changed later by adding a new IO_PGTABLE_QUIRK.
	 */
	switch (FIELD_GET(IDR0_TTENDIAN, reg)) {
	case IDR0_TTENDIAN_MIXED:
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_TT_LE | ARM_SMMU_FEAT_TT_BE;
		break;
#ifdef __BIG_ENDIAN
	case IDR0_TTENDIAN_BE:
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_TT_BE;
		break;
#else
	case IDR0_TTENDIAN_LE:
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_TT_LE;
		break;
#endif
	default:
		con_err("smmuv3: unknown/unsupported TT endianness!\n");
		BUG();
		return;
	}

	/* Boolean feature flags */
#ifdef CONFIG_PCI_PRI
	if (reg & IDR0_PRI)
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_PRI;
#endif
#ifdef CONFIG_PCI_ATS
	if (reg & IDR0_ATS)
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_ATS;
#endif
	if (reg & IDR0_SEV)
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_SEV;
	if (reg & IDR0_MSI)
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_MSI;
	if (reg & IDR0_HYP)
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_HYP;

	/*
	 * The coherency feature as set by FW is used in preference to the ID
	 * register, but warn on mismatch.
	 */
	if (!!(reg & IDR0_COHACC) != coherent)
		con_err("smmuv3: IDR0.COHACC overridden by FW configuration (%s)\n",
			coherent ? "true" : "false");

	switch (FIELD_GET(IDR0_STALL_MODEL, reg)) {
	case IDR0_STALL_MODEL_FORCE:
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_STALL_FORCE;
		/* Fallthrough */
	case IDR0_STALL_MODEL_STALL:
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_STALLS;
	}

	if (reg & IDR0_S1P)
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_TRANS_S1;
	if (reg & IDR0_S2P)
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_TRANS_S2;

	if (!(reg & (IDR0_S1P | IDR0_S2P))) {
		con_err("smmuv3: no translation support!\n");
		BUG();
		return;
	}

	/* We only support the AArch64 table format at present */
	switch (FIELD_GET(IDR0_TTF, reg)) {
	case IDR0_TTF_AARCH32_64:
		smmu_device_ctrl.ias = 40;
		/* Fallthrough */
	case IDR0_TTF_AARCH64:
		break;
	default:
		con_err("smmuv3: AArch64 table format not supported!\n");
		BUG();
		return;
	}

	/* ASID/VMID sizes */
	smmu_device_ctrl.asid_bits = reg & IDR0_ASID16 ? 16 : 8;
	smmu_device_ctrl.vmid_bits = reg & IDR0_VMID16 ? 16 : 8;

	/* IDR1 */
	reg = __raw_readl(SMMU_IDR1(iommu_dev));
	if (reg & (IDR1_TABLES_PRESET | IDR1_QUEUES_PRESET | IDR1_REL)) {
		con_err("smmuv3: embedded implementation not supported\n");
		BUG();
		return;
	}

	/* Queue sizes, capped to ensure natural alignment */
	smmu_device_ctrl.cmdq.q.llq.max_n_shift =
		min(CMDQ_MAX_SZ_SHIFT, FIELD_GET(IDR1_CMDQS, reg));
	if (smmu_device_ctrl.cmdq.q.llq.max_n_shift <=
	    ilog2_const(CMDQ_BATCH_ENTRIES)) {
		/*
		 * We don't support splitting up batches, so one batch of
		 * commands plus an extra sync needs to fit inside the command
		 * queue. There's also no way we can handle the weird alignment
		 * restrictions on the base pointer for a unit-length queue.
		 */
		con_err("smmuv3: command queue size <= %d entries not supported\n",
			CMDQ_BATCH_ENTRIES);
		BUG();
		return;
	}

	smmu_device_ctrl.evtq.q.llq.max_n_shift =
		min(EVTQ_MAX_SZ_SHIFT, FIELD_GET(IDR1_EVTQS, reg));
	smmu_device_ctrl.priq.q.llq.max_n_shift =
		min(PRIQ_MAX_SZ_SHIFT, FIELD_GET(IDR1_PRIQS, reg));

	/* SID/SSID sizes */
	smmu_device_ctrl.ssid_bits = FIELD_GET(IDR1_SSIDSIZE, reg);
	smmu_device_ctrl.sid_bits = FIELD_GET(IDR1_SIDSIZE, reg);

	/*
	 * If the SMMU supports fewer bits than would fill a single L2 stream
	 * table, use a linear table instead.
	 */
	if (smmu_device_ctrl.sid_bits <= STRTAB_SPLIT)
		smmu_device_ctrl.features &= ~ARM_SMMU_FEAT_2_LVL_STRTAB;

#if 0
	/* IDR3 */
	reg = __raw_readl(SMMU_IDR3(iommu_dev));
	if (FIELD_GET(IDR3_RIL, reg))
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_RANGE_INV;
#endif

	/* IDR5 */
	reg = __raw_readl(SMMU_IDR5(iommu_dev));

	/* Maximum number of outstanding stalls */
	smmu_device_ctrl.evtq.max_stalls = FIELD_GET(IDR5_STALL_MAX, reg);

	/* Page sizes */
	if (reg & IDR5_GRAN64K)
		smmu_device_ctrl.pgsize_bitmap |= SZ_64K | SZ_512M;
	if (reg & IDR5_GRAN16K)
		smmu_device_ctrl.pgsize_bitmap |= SZ_16K | SZ_32M;
	if (reg & IDR5_GRAN4K)
		smmu_device_ctrl.pgsize_bitmap |= SZ_4K | SZ_2M | SZ_1G;

	/* Input address size */
	if (FIELD_GET(IDR5_VAX, reg) == IDR5_VAX_52_BIT)
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_VAX;

	/* Output address size */
	switch (FIELD_GET(IDR5_OAS, reg)) {
	case IDR5_OAS_32_BIT:
		smmu_device_ctrl.oas = 32;
		break;
	case IDR5_OAS_36_BIT:
		smmu_device_ctrl.oas = 36;
		break;
	case IDR5_OAS_40_BIT:
		smmu_device_ctrl.oas = 40;
		break;
	case IDR5_OAS_42_BIT:
		smmu_device_ctrl.oas = 42;
		break;
	case IDR5_OAS_44_BIT:
		smmu_device_ctrl.oas = 44;
		break;
	case IDR5_OAS_52_BIT:
		smmu_device_ctrl.oas = 52;
		smmu_device_ctrl.pgsize_bitmap |= 1ULL << 42; /* 4TB */
		break;
	default:
		con_log("smmuv3: unknown output address size. Truncating to 48-bit\n");
		/* Fallthrough */
	case IDR5_OAS_48_BIT:
		smmu_device_ctrl.oas = 48;
	}

	if (smmu_hw_pgsize_bitmap == -1UL)
		smmu_hw_pgsize_bitmap = smmu_device_ctrl.pgsize_bitmap;
	else
		smmu_hw_pgsize_bitmap |= smmu_device_ctrl.pgsize_bitmap;

#if 0
	/* Set the DMA mask for our table walker */
	if (dma_set_mask_and_coherent(smmu_device_ctrl.dev,
				      DMA_BIT_MASK(smmu_device_ctrl.oas)))
		con_warn("failed to set DMA mask for table walker\n");
#endif

	smmu_device_ctrl.ias = max(smmu_device_ctrl.ias, smmu_device_ctrl.oas);

#if 0
	if ((smmu_device_ctrl.features & ARM_SMMU_FEAT_TRANS_S1) &&
	    (smmu_device_ctrl.features & ARM_SMMU_FEAT_TRANS_S2))
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_NESTING;

	arm_smmu_device_iidr_probe(smmu);

	if (arm_smmu_sva_supported(smmu))
		smmu_device_ctrl.features |= ARM_SMMU_FEAT_SVA;
#endif

	con_log("smmuv3: ias %lu-bit, oas %lu-bit (features 0x%08x)\n",
		smmu_device_ctrl.ias, smmu_device_ctrl.oas,
		smmu_device_ctrl.features);
	return;
}

static int arm_smmu_write_reg_sync(uint32_t val,
				   caddr_t reg_addr, caddr_t ack_addr)
{
	uint32_t reg;
	bool ret;

	__raw_writel(val, reg_addr);
	ret = __raw_read_poll(l, ack_addr, reg, reg == val,
			      1, ARM_SMMU_POLL_TIMEOUT_US);
	return ret ? 0 : -ETIMEDOUT;
}

/* GBPA is "special" */
static int arm_smmu_update_gbpa(uint32_t set, uint32_t clr)
{
	int ret;
	uint32_t reg;
	caddr_t gbpa = SMMU_GBPA(iommu_dev);

	ret = __raw_read_poll(l, gbpa, reg, !(reg & GBPA_UPDATE),
			      1, ARM_SMMU_POLL_TIMEOUT_US);
	if (ret)
		return ret;

	reg &= ~clr;
	reg |= set;
	__raw_writel(reg | GBPA_UPDATE, gbpa);
	ret = __raw_read_poll(l, gbpa, reg, !(reg & GBPA_UPDATE),
			      1, ARM_SMMU_POLL_TIMEOUT_US);

	if (ret)
		con_err("smmuv3: GBPA not responding to update\n");
	return ret;
}

static int arm_smmu_device_disable(void)
{
	int ret;

	ret = arm_smmu_write_reg_sync(0,
				      SMMU_CR0(iommu_dev),
				      SMMU_CR0ACK(iommu_dev));
	if (ret)
		con_err("smmuv3: failed to clear CR0\n");
	return ret;
}

/* IRQ and event handlers */
static void arm_smmu_evtq_thread(irq_t irq)
{
	int i;
	struct arm_smmu_queue *q = &smmu_device_ctrl.evtq.q;
	struct arm_smmu_ll_queue *llq = &q->llq;
	uint64_t evt[EVTQ_ENT_DWORDS];

	do {
		while (!queue_remove_raw(q, evt)) {
			uint8_t id = FIELD_GET(EVTQ_0_ID, evt[0]);

			con_log("smmuv3: event 0x%02x received:\n", id);
			for (i = 0; i < ARRAY_SIZE(evt); ++i)
				con_log("\t0x%016llx\n",
					(unsigned long long)evt[i]);

		}

		/*
		 * Not much we can do on overflow, so scream and pretend we're
		 * trying harder.
		 */
		if (queue_sync_prod_in(q) == -EOVERFLOW)
			con_err("smmuv3: EVTQ overflow detected -- events lost\n");
	} while (!queue_empty(llq));

	/* Sync our overflow flag, as we believe we're up to speed */
	llq->u.cons = Q_OVF(llq->u.prod) | Q_WRP(llq, llq->u.cons) |
		      Q_IDX(llq, llq->u.cons);
}

static void arm_smmu_handle_ppr(uint64_t *evt)
{
	uint32_t sid, ssid;
	uint16_t grpid;
	bool ssv, last;

	sid = FIELD_GET(PRIQ_0_SID, evt[0]);
	ssv = FIELD_GET(PRIQ_0_SSID_V, evt[0]);
	ssid = ssv ? FIELD_GET(PRIQ_0_SSID, evt[0]) : 0;
	last = FIELD_GET(PRIQ_0_PRG_LAST, evt[0]);
	grpid = FIELD_GET(PRIQ_1_PRG_IDX, evt[1]);

	con_log("smmuv3: unexpected PRI request received:\n");
	con_log("\tsid 0x%08x.0x%05x: [%u%s] %sprivileged %s%s%s access at iova 0x%016llx\n",
		sid, ssid, grpid, last ? "L" : "",
		evt[0] & PRIQ_0_PERM_PRIV ? "" : "un",
		evt[0] & PRIQ_0_PERM_READ ? "R" : "",
		evt[0] & PRIQ_0_PERM_WRITE ? "W" : "",
		evt[0] & PRIQ_0_PERM_EXEC ? "X" : "",
		evt[1] & PRIQ_1_ADDR_MASK);

	if (last) {
		struct arm_smmu_cmdq_ent cmd = {
			.opcode			= CMDQ_OP_PRI_RESP,
			.substream_valid	= ssv,
			.pri			= {
				.sid	= sid,
				.ssid	= ssid,
				.grpid	= grpid,
				.resp	= PRI_RESP_DENY,
			},
		};

		arm_smmu_cmdq_issue_cmd(&cmd);
	}
}

static void arm_smmu_priq_thread(irq_t irq)
{
	struct arm_smmu_queue *q;
	struct arm_smmu_ll_queue *llq;
	uint64_t evt[PRIQ_ENT_DWORDS];

	q = &smmu_device_ctrl.priq.q;
	llq = &q->llq;
	do {
		while (!queue_remove_raw(q, evt))
			arm_smmu_handle_ppr(evt);

		if (queue_sync_prod_in(q) == -EOVERFLOW)
			con_err("smmuv3: PRIQ overflow detected -- requests lost\n");
	} while (!queue_empty(llq));

	/* Sync our overflow flag, as we believe we're up to speed */
	llq->u.cons = Q_OVF(llq->u.prod) | Q_WRP(llq, llq->u.cons) |
		      Q_IDX(llq, llq->u.cons);
	queue_sync_cons_out(q);
}

static void arm_smmu_gerror_handler(irq_t irq)
{
	uint32_t gerror, gerrorn, active;

	gerror = __raw_readl(SMMU_GERROR(iommu_dev));
	gerrorn = __raw_readl(SMMU_GERRORN(iommu_dev));

	active = gerror ^ gerrorn;
	if (!(active & GERROR_ERR_MASK))
		return; /* No errors pending */

	con_log("smmuv3: unexpected global error reported (0x%08x), this could be serious\n",
		active);

	if (active & GERROR_SFM_ERR) {
		con_err("smmuv3: device has entered Service Failure Mode!\n");
		arm_smmu_device_disable();
	}

	if (active & GERROR_MSI_GERROR_ABT_ERR)
		con_log("smmuv3: GERROR MSI write aborted\n");

	if (active & GERROR_MSI_PRIQ_ABT_ERR)
		con_log("smmuv3: PRIQ MSI write aborted\n");

	if (active & GERROR_MSI_EVTQ_ABT_ERR)
		con_log("smmuv3: EVTQ MSI write aborted\n");

	if (active & GERROR_MSI_CMDQ_ABT_ERR)
		con_log("smmuv3: CMDQ MSI write aborted\n");

	if (active & GERROR_PRIQ_ABT_ERR)
		con_err("smmuv3: PRIQ write aborted -- events may have been lost\n");

	if (active & GERROR_EVTQ_ABT_ERR)
		con_err("smmuv3: EVTQ write aborted -- events may have been lost\n");

	if (active & GERROR_CMDQ_ERR)
		arm_smmu_cmdq_skip_err();

	__raw_writel(gerror, SMMU_GERRORN(iommu_dev));
}

#ifdef SYS_REALTIME
void smmu_poll_irqs(void)
{
	irq_t irq;

	irq = smmu_device_ctrl.evtq.q.irq;
	arm_smmu_evtq_thread(irq);
	irq = smmu_device_ctrl.gerr_irq;
	arm_smmu_gerror_handler(irq);
	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_PRI) {
		irq = smmu_device_ctrl.priq.q.irq;
		arm_smmu_priq_thread(irq);
	}
}
#define arm_smmu_setup_unique_irqs()	do { } while (0)
#else /* SYS_REALTIME */
#if NR_IOMMU_DEVICES > 1
iommu_dev_t arm_smmu_save_irq(irq_t irq)
{
	__unused iommu_dev_t dev, sdev;

	sdev = iommu_device_save(dev);
	for (dev = 0; dev < NR_IOMMU_DEVICES; dev++) {
		iommu_device_restore(sdev);
		sdev = iommu_device_save(dev);
		if (iommu_device_ctrl.valid) {
			if (irq == smmu_device_ctrl.evtq.q.irq)
				goto found;
			if (irq == smmu_device_ctrl.gerr_irq)
				goto found;
			if (smmu_device_ctrl.features & ARM_SMMU_FEAT_PRI)
				if (irq == smmu_device_ctrl.priq.q.irq)
					goto found;
		}
	}
	dev = INVALID_IOMMU_DEV;
found:
	iommu_device_restore(sdev);
	return dev;
}
#else
#define arm_smmu_save_irq()		iommu_dev
#endif

static void arm_smmu_handle_evtq(irq_t irq)
{
	__unused iommu_dev_t dev;

	dev = arm_smmu_save_irq(irq);
	arm_smmu_evtq_thread(irq);
	iommu_device_restore(dev);
}

static void arm_smmu_handle_gerr(irq_t irq)
{
	__unused iommu_dev_t dev;

	dev = arm_smmu_save_irq(irq);
	arm_smmu_handle_gerror(irq);
	iommu_device_restore(dev);
}

static void arm_smmu_handle_priq(irq_t irq)
{
	__unused iommu_dev_t dev;

	dev = arm_smmu_save_irq(irq);
	arm_smmu_priq_thread(irq);
	iommu_device_restore(dev);
}

static void arm_smmu_setup_unique_irqs(void)
{
	irq_t irq;

#ifdef CONFIG_ARM_SMMU_MSI
	arm_smmu_setup_msis();
#endif

	/* Request interrupt lines */
	irq = smmu_device_ctrl.evtq.q.irq;
	irqc_configure_irq(irq, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(irq, arm_smmu_handle_evtq);
	irqc_enable_irq(irq);

	irq = smmu_device_ctrl.gerr_irq;
	irqc_configure_irq(irq, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(irq, arm_smmu_handle_gerr);
	irqc_enable_irq(irq);

	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_PRI) {
		irq = smmu_device_ctrl.priq.q.irq;
		irqc_configure_irq(irq, 0, IRQ_LEVEL_TRIGGERED);
		irq_register_vector(irq, arm_smmu_handle_priq);
		irqc_enable_irq(irq);
	}
}
#endif

static void arm_smmu_setup_irqs(void)
{
	uint32_t irqen_flags = IRQ_CTRL_EVTQ_IRQEN | IRQ_CTRL_GERROR_IRQEN;

	/* Disable IRQs first */
	arm_smmu_write_reg_sync(0, SMMU_IRQ_CTRL(iommu_dev),
				SMMU_IRQ_CTRLACK(iommu_dev));
	arm_smmu_setup_unique_irqs();

	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_PRI)
		irqen_flags |= IRQ_CTRL_PRIQ_IRQEN;

	/* Enable interrupt generation on the SMMU */
	arm_smmu_write_reg_sync(irqen_flags, SMMU_IRQ_CTRL(iommu_dev),
				SMMU_IRQ_CTRLACK(iommu_dev));
}

static int arm_smmu_device_reset(bool bypass)
{
	int ret;
	uint32_t reg, enables;
	struct arm_smmu_cmdq_ent cmd;

	/* Clear CR0 and sync (disables SMMU and queue processing) */
	reg = __raw_readl(SMMU_CR0(iommu_dev));
	if (reg & CR0_SMMUEN) {
		con_log("smmuv3: SMMU currently enabled! Resetting...\n");
		/* WARN_ON(is_kdump_kernel() && !disable_bypass); */
		arm_smmu_update_gbpa(GBPA_ABORT, 0);
	}

	ret = arm_smmu_device_disable();
	if (ret)
		return ret;

	/* CR1 (table and queue memory attributes) */
	reg = FIELD_PREP(CR1_TABLE_SH, ARM_SMMU_SH_ISH) |
	      FIELD_PREP(CR1_TABLE_OC, CR1_CACHE_WB) |
	      FIELD_PREP(CR1_TABLE_IC, CR1_CACHE_WB) |
	      FIELD_PREP(CR1_QUEUE_SH, ARM_SMMU_SH_ISH) |
	      FIELD_PREP(CR1_QUEUE_OC, CR1_CACHE_WB) |
	      FIELD_PREP(CR1_QUEUE_IC, CR1_CACHE_WB);
	__raw_writel(reg, SMMU_CR1(iommu_dev));

	/* CR2 (random crap) */
	reg = CR2_PTM | CR2_RECINVSID | CR2_E2H;
	__raw_writel(reg, SMMU_CR2(iommu_dev));

	/* Stream table */
	__raw_writeq(smmu_device_ctrl.strtab_cfg.strtab_base,
		     SMMU_STRTAB_BASE(iommu_dev));
	__raw_writel(smmu_device_ctrl.strtab_cfg.strtab_base_cfg,
		     SMMU_STRTAB_BASE_CFG(iommu_dev));

	/* Command queue */
	__raw_writeq(smmu_device_ctrl.cmdq.q.q_base,
		     SMMU_CMDQ_BASE(iommu_dev));
	__raw_writel(smmu_device_ctrl.cmdq.q.llq.u.prod,
		     SMMU_CMDQ_PROD(iommu_dev));
	__raw_writel(smmu_device_ctrl.cmdq.q.llq.u.cons,
		     SMMU_CMDQ_CONS(iommu_dev));

	enables = CR0_CMDQEN;
	ret = arm_smmu_write_reg_sync(enables,
				      SMMU_CR0(iommu_dev),
				      SMMU_CR0ACK(iommu_dev));
	if (ret) {
		con_err("smmuv3: failed to enable command queue\n");
		return ret;
	}

	/* Invalidate any cached configuration */
	cmd.opcode = CMDQ_OP_CFGI_ALL;
	arm_smmu_cmdq_issue_cmd(&cmd);
	arm_smmu_cmdq_issue_sync();

	/* Invalidate any stale TLB entries */
	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_HYP) {
		cmd.opcode = CMDQ_OP_TLBI_EL2_ALL;
		arm_smmu_cmdq_issue_cmd(&cmd);
	}

	cmd.opcode = CMDQ_OP_TLBI_NSNH_ALL;
	arm_smmu_cmdq_issue_cmd(&cmd);
	arm_smmu_cmdq_issue_sync();

	/* Event queue */
	__raw_writeq(smmu_device_ctrl.evtq.q.q_base,
		     SMMU_EVTQ_BASE(iommu_dev));
	__raw_writel(smmu_device_ctrl.evtq.q.llq.u.prod,
		     arm_smmu_page1_fixup(SMMU_EVTQ_PROD(iommu_dev)));
	__raw_writel(smmu_device_ctrl.evtq.q.llq.u.cons,
		     arm_smmu_page1_fixup(SMMU_EVTQ_CONS(iommu_dev)));

	enables |= CR0_EVTQEN;
	ret = arm_smmu_write_reg_sync(enables,
				      SMMU_CR0(iommu_dev),
				      SMMU_CR0ACK(iommu_dev));
	if (ret) {
		con_err("smmuv3: failed to enable event queue\n");
		return ret;
	}

	/* PRI queue */
	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_PRI) {
		__raw_writeq(smmu_device_ctrl.priq.q.q_base,
			     SMMU_PRIQ_BASE(iommu_dev));
		__raw_writel(smmu_device_ctrl.priq.q.llq.u.prod,
			     arm_smmu_page1_fixup(SMMU_PRIQ_PROD(iommu_dev)));
		__raw_writel(smmu_device_ctrl.priq.q.llq.u.cons,
			     arm_smmu_page1_fixup(SMMU_PRIQ_CONS(iommu_dev)));

		enables |= CR0_PRIQEN;
		ret = arm_smmu_write_reg_sync(enables,
					      SMMU_CR0(iommu_dev),
					      SMMU_CR0ACK(iommu_dev));
		if (ret) {
			con_err("smmuv3: failed to enable PRI queue\n");
			return ret;
		}
	}

	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_ATS) {
		enables |= CR0_ATSCHK;
		ret = arm_smmu_write_reg_sync(enables,
					      SMMU_CR0(iommu_dev),
					      SMMU_CR0ACK(iommu_dev));
		if (ret) {
			con_err("smmuv3: failed to enable ATS check\n");
			return ret;
		}
	}

	arm_smmu_setup_irqs();

	/* Enable the SMMU interface, or ensure bypass */
	if (!bypass || smmu_disable_bypass) {
		enables |= CR0_SMMUEN;
	} else {
		ret = arm_smmu_update_gbpa(0, GBPA_ABORT);
		if (ret)
			return ret;
	}
	ret = arm_smmu_write_reg_sync(enables,
				      SMMU_CR0(iommu_dev),
				      SMMU_CR0ACK(iommu_dev));
	if (ret) {
		con_err("smmuv3: failed to enable SMMU interface\n");
		return ret;
	}

	return 0;
}

static bool arm_smmu_sid_in_range(uint32_t sid)
{
	unsigned long limit = smmu_device_ctrl.strtab_cfg.num_l1_ents;

	if (smmu_device_ctrl.features & ARM_SMMU_FEAT_2_LVL_STRTAB)
		limit *= 1UL << STRTAB_SPLIT;

	return sid < limit;
}

void smmu_master_init(void)
{
	int i;

	INIT_LIST_HEAD(&smmu_group_ctrl.domain_head);

	/* Check the SIDs are in range of the SMMU and our stream table */
	for (i = 0; i < iommu_group_ctrl.nr_iommus; i++) {
		iommu_map_t sid = iommu_group_ctrl.iommus[i];

		BUG_ON(!arm_smmu_sid_in_range(sid));
		/* Ensure l2 strtab is initialised */
		if (smmu_device_ctrl.features & ARM_SMMU_FEAT_2_LVL_STRTAB)
			arm_smmu_init_l2_strtab(sid);

		smmu_group_ctrl.sids[smmu_group_ctrl.num_sids] = sid;
		smmu_group_ctrl.num_sids++;
	}

	smmu_group_ctrl.ssid_bits = min(smmu_device_ctrl.ssid_bits,
					smmu_hw_num_pasid_bits);

#if 0
	/*
	 * Note that PASID must be enabled before, and disabled after ATS:
	 * PCI Express Base 4.0r1.0 - 10.5.1.3 ATS Control Register
	 *
	 *   Behavior is undefined if this bit is Set and the value of the PASID
	 *   Enable, Execute Requested Enable, or Privileged Mode Requested bits
	 *   are changed.
	 */
	arm_smmu_enable_pasid();
#endif

	if (!(smmu_device_ctrl.features & ARM_SMMU_FEAT_2_LVL_CDTAB))
		smmu_group_ctrl.ssid_bits =
			min(smmu_group_ctrl.ssid_bits, CTXDESC_LINEAR_CDMAX);
}

#ifdef CONFIG_PCI_ATS
static bool arm_smmu_ats_supported(void)
{
	if (!(smmu_device_ctrl.features & ARM_SMMU_FEAT_ATS) ||
	    !iommu_group_ctrl.is_pci || !iommu_group_ctrl.pci_rc_ats ||
	    pci_ats_disabled())
		return false;

	return !iommu_group_ctrl.untrusted && iommu_group_ctrl.ats_cap;
}
#else
static bool arm_smmu_ats_supported(void)
{
	return false;
}
#endif

static void arm_smmu_enable_ats(void)
{
	__unused size_t stu;

	/* Don't enable ATS at the endpoint if it's not enabled in the STE */
	if (!smmu_group_ctrl.ats_enabled)
		return;

	/* Smallest Translation Unit: log2 of the smallest supported granule */
	stu = __ffs64(smmu_device_ctrl.pgsize_bitmap);

	atomic_inc(&(smmu_domain_ctrl.nr_ats_masters));
	arm_smmu_atc_inv_domain(0, 0, 0);
#if 0
	if (pci_enable_ats(iommu_group_ctrl.dma, stu))
		con_err("Failed to enable ATS (STU %zu)\n", stu);
#endif
}

static void arm_smmu_disable_ats(void)
{
	struct arm_smmu_cmdq_ent cmd;

	if (!smmu_group_ctrl.ats_enabled)
		return;

#if 0
	pci_disable_ats(iommu_group_ctrl.dma);
#endif
	/*
	 * Ensure ATS is disabled at the endpoint before we issue the
	 * ATC invalidation via the SMMU.
	 */
	wmb();
	arm_smmu_atc_inv_to_cmd(0, 0, 0, &cmd);
	arm_smmu_atc_inv_master(&cmd);
	atomic_dec(&(smmu_domain_ctrl.nr_ats_masters));
}

static void smmu_master_detach(void)
{
	arm_smmu_disable_ats();
	list_del(&smmu_group_ctrl.domain_head);
	smmu_group_ctrl.ats_enabled = false;
	arm_smmu_install_ste_for_dev();
}

void smmu_domain_init(void)
{
	unsigned int type = iommu_domain_ctrl.type;

	if (type != IOMMU_DOMAIN_UNMANAGED &&
	    type != IOMMU_DOMAIN_DMA &&
	    type != IOMMU_DOMAIN_IDENTITY)
		return;

#if 0
	if (type == IOMMU_DOMAIN_DMA &&
	    iommu_get_dma_cookie(&smmu_domain->domain)) {
		return;
	}
#endif

	INIT_LIST_HEAD(&smmu_domain_ctrl.devices);
}

void smmu_master_attach(void)
{
	smmu_master_detach();
	if (iommu_domain_ctrl.dev == INVALID_IOMMU_DEV) {
		iommu_domain_ctrl.dev = iommu_dev;
		arm_smmu_domain_finalise();
	} else if (iommu_domain_ctrl.dev != iommu_dev) {
		con_err("smmuv3: cannot attach to SMMU %d (upstream of %d)\n",
			iommu_domain_ctrl.dev,
			iommu_dev);
		BUG();
	} else if (smmu_domain_ctrl.stage == ARM_SMMU_DOMAIN_S1 &&
		   smmu_group_ctrl.ssid_bits !=
		   smmu_domain_ctrl.s1_cfg.s1cdmax) {
		con_err("smmuv3: cannot attach to incompatible domain (%u SSID bits != %u)\n",
			smmu_domain_ctrl.s1_cfg.s1cdmax,
			smmu_group_ctrl.ssid_bits);
		BUG();
	}

	iommu_group_ctrl.dom = iommu_dom;

	if (smmu_domain_ctrl.stage != ARM_SMMU_DOMAIN_BYPASS)
		smmu_group_ctrl.ats_enabled = arm_smmu_ats_supported();

	arm_smmu_install_ste_for_dev();

	list_add(&smmu_group_ctrl.domain_head, &smmu_domain_ctrl.devices);

	arm_smmu_enable_ats();
}

void smmu_device_init(void)
{
	bool bypass = false;

#if 0
	smmu_device_ctrl.features |= ARM_SMMU_FEAT_COHERENCY;
#endif
	arm_smmu_device_hw_probe();
	arm_smmu_init_structures();
	arm_smmu_device_reset(bypass);
}

void smmu_device_exit(void)
{
}

#if 0
static void arm_smmu_domain_free(iommu_dom_t dom)
{
	struct arm_smmu_domain *smmu_domain = to_smmu_domain(domain);
	struct arm_smmu_device *smmu = smmu_domain->smmu;

	iommu_put_dma_cookie(domain);
	free_io_pgtable_ops(smmu_domain->pgtbl_ops);

	/* Free the CD and ASID, if we allocated them */
	if (smmu_domain->stage == ARM_SMMU_DOMAIN_S1) {
		struct arm_smmu_s1_cfg *cfg = &smmu_domain->s1_cfg;

		if (cfg->cdcfg.cdtab) {
			arm_smmu_free_cd_tables(smmu_domain);
			arm_smmu_bitmap_free(smmu->asid_map, cfg->cd.asid);
		}
	} else {
		struct arm_smmu_s2_cfg *cfg = &smmu_domain->s2_cfg;
		if (cfg->vmid)
			arm_smmu_bitmap_free(smmu->vmid_map, cfg->vmid);
	}

	kfree(smmu_domain);
}

static phys_addr_t
arm_smmu_iova_to_phys(struct iommu_domain *domain, dma_addr_t iova)
{
	struct io_pgtable_ops *ops = to_smmu_domain(domain)->pgtbl_ops;

	if (domain->type == IOMMU_DOMAIN_IDENTITY)
		return iova;

	if (!ops)
		return 0;

	return ops->iova_to_phys(ops, iova);
}

static int arm_smmu_add_device(struct device *dev)
{
	group = iommu_group_get_for_dev(dev);
}

static void arm_smmu_get_resv_regions(struct device *dev,
				      struct list_head *head)
{
	struct iommu_resv_region *region;
	int prot = IOMMU_WRITE | IOMMU_NOEXEC | IOMMU_MMIO;

	region = iommu_alloc_resv_region(MSI_IOVA_BASE, MSI_IOVA_LENGTH,
					 prot, IOMMU_RESV_SW_MSI);
	if (!region)
		return;

	list_add_tail(&region->list, head);
	iommu_dma_get_resv_regions(dev, head);
}

static struct iommu_ops arm_smmu_ops = {
	.domain_alloc		= arm_smmu_domain_alloc,
	.domain_free		= arm_smmu_domain_free,
	.map			= arm_smmu_map,
	.unmap			= arm_smmu_unmap,
	.iova_to_phys		= arm_smmu_iova_to_phys,
	.get_resv_regions	= arm_smmu_get_resv_regions,
	.put_resv_regions	= generic_iommu_put_resv_regions,
	.pgsize_bitmap		= -1UL, /* Restricted during device attach */
};

static phys_addr_t arm_smmu_msi_cfg[ARM_SMMU_MAX_MSIS][3] = {
	[EVTQ_MSI_INDEX] = {
		SMMU_EVTQ_IRQ_CFG0,
		SMMU_EVTQ_IRQ_CFG1,
		SMMU_EVTQ_IRQ_CFG2,
	},
	[GERROR_MSI_INDEX] = {
		SMMU_GERROR_IRQ_CFG0,
		SMMU_GERROR_IRQ_CFG1,
		SMMU_GERROR_IRQ_CFG2,
	},
	[PRIQ_MSI_INDEX] = {
		SMMU_PRIQ_IRQ_CFG0,
		SMMU_PRIQ_IRQ_CFG1,
		SMMU_PRIQ_IRQ_CFG2,
	},
};

static void arm_smmu_free_msis(void *data)
{
	struct device *dev = data;
	platform_msi_domain_free_irqs(dev);
}

static void arm_smmu_write_msi_msg(struct msi_desc *desc, struct msi_msg *msg)
{
	phys_addr_t doorbell;
	struct device *dev = msi_desc_to_dev(desc);
	struct arm_smmu_device *smmu = dev_get_drvdata(dev);
	phys_addr_t *cfg = arm_smmu_msi_cfg[desc->platform.msi_index];

	doorbell = (((u64)msg->address_hi) << 32) | msg->address_lo;
	doorbell &= MSI_CFG0_ADDR_MASK;

	writeq_relaxed(doorbell, smmu->base + cfg[0]);
	writel_relaxed(msg->data, smmu->base + cfg[1]);
	writel_relaxed(ARM_SMMU_MEMATTR_DEVICE_nGnRE, smmu->base + cfg[2]);
}

static void arm_smmu_setup_msis(struct arm_smmu_device *smmu)
{
	struct msi_desc *desc;
	int ret, nvec = ARM_SMMU_MAX_MSIS;
	struct device *dev = smmu->dev;

	/* Clear the MSI address regs */
	writeq_relaxed(0, smmu->base + ARM_SMMU_GERROR_IRQ_CFG0);
	writeq_relaxed(0, smmu->base + ARM_SMMU_EVTQ_IRQ_CFG0);

	if (smmu->features & ARM_SMMU_FEAT_PRI)
		writeq_relaxed(0, smmu->base + ARM_SMMU_PRIQ_IRQ_CFG0);
	else
		nvec--;

	if (!(smmu->features & ARM_SMMU_FEAT_MSI))
		return;

	if (!dev->msi_domain) {
		dev_info(smmu->dev, "msi_domain absent - falling back to wired irqs\n");
		return;
	}

	/* Allocate MSIs for evtq, gerror and priq. Ignore cmdq */
	ret = platform_msi_domain_alloc_irqs(dev, nvec, arm_smmu_write_msi_msg);
	if (ret) {
		dev_warn(dev, "failed to allocate MSIs - falling back to wired irqs\n");
		return;
	}

	for_each_msi_entry(desc, dev) {
		switch (desc->platform.msi_index) {
		case EVTQ_MSI_INDEX:
			smmu->evtq.q.irq = desc->irq;
			break;
		case GERROR_MSI_INDEX:
			smmu->gerr_irq = desc->irq;
			break;
		case PRIQ_MSI_INDEX:
			smmu->priq.q.irq = desc->irq;
			break;
		default:	/* Unknown */
			continue;
		}
	}

	/* Add callback to free MSIs on teardown */
	devm_add_action(dev, arm_smmu_free_msis, dev);
}
#endif
