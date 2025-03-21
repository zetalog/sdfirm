// SPDX-License-Identifier: GPL-2.0-only
/*
 * IOMMU API for RISC-V architected Ziommu implementations.
 *
 * Copyright © 2022-2023 Rivos Inc.
 * Copyright © 2023 FORTH-ICS/CARV
 *
 * Authors
 *	Tomasz Jeznach <tjeznach@rivosinc.com>
 *	Nick Kossifidis <mick@ics.forth.gr>
 */
#include <target/iommu.h>
#include <target/iommu-pgtable.h>
#include <target/panic.h>
#include <target/paging.h>
#include <stdlib.h>

#include "iommu.h"
#include "iommu-bits.h"
// #include "task.h"

#define cmpxchg_relaxed(p, o, n)		\
	__cmpxchg(p, (unsigned long)o, (unsigned long)n, sizeof(*p))

#define RV_IOMMU_BASE(n)	ULL(0x04780000000)


#define DMA_BIT_MASK(n)	(((n) == 64) ? ~0ULL : ((1ULL<<(n))-1))

/* Generic fault types, can be expanded IRQ remapping fault */
enum iommu_fault_type {
	IOMMU_FAULT_DMA_UNRECOV = 1,	/* unrecoverable fault */
	IOMMU_FAULT_PAGE_REQ,		/* page request fault */
};

enum iommu_cap {
	IOMMU_CAP_CACHE_COHERENCY,	/* IOMMU_CACHE is supported */
	IOMMU_CAP_NOEXEC,		/* IOMMU_NOEXEC flag */
	IOMMU_CAP_PRE_BOOT_PROTECTION,	/* Firmware says it used the IOMMU for
					   DMA protection and we should too */
	/*
	 * Per-device flag indicating if enforce_cache_coherency() will work on
	 * this device.
	 */
	IOMMU_CAP_ENFORCE_CACHE_COHERENCY,
	/*
	 * IOMMU driver does not issue TLB maintenance during .unmap, so can
	 * usefully support the non-strict DMA flush queue.
	 */
	IOMMU_CAP_DEFERRED_FLUSH,
};

enum iommu_page_response_code {
	IOMMU_PAGE_RESP_SUCCESS = 0,
	IOMMU_PAGE_RESP_INVALID,
	IOMMU_PAGE_RESP_FAILURE,
};

/* These are the possible reserved region types */
enum iommu_resv_type {
	/* Memory regions which must be mapped 1:1 at all times */
	IOMMU_RESV_DIRECT,
	/*
	 * Memory regions which are advertised to be 1:1 but are
	 * commonly considered relaxable in some conditions,
	 * for instance in device assignment use case (USB, Graphics)
	 */
	IOMMU_RESV_DIRECT_RELAXABLE,
	/* Arbitrary "never map this or give it to a device" address ranges */
	IOMMU_RESV_RESERVED,
	/* Hardware MSI region (untranslated) */
	IOMMU_RESV_MSI,
	/* Software-managed MSI translation window */
	IOMMU_RESV_SW_MSI,
};

/* Domain feature flags */
#define __IOMMU_DOMAIN_PAGING	(1U << 0)  /* Support for iommu_map/unmap */
#define __IOMMU_DOMAIN_DMA_API	(1U << 1)  /* Domain for use in DMA-API
					      implementation              */
#define __IOMMU_DOMAIN_PT	(1U << 2)  /* Domain is identity mapped   */
#define __IOMMU_DOMAIN_DMA_FQ	(1U << 3)  /* DMA-API uses flush queue    */

#define __IOMMU_DOMAIN_SVA	(1U << 4)  /* Shared process address space */

#define IOMMU_DOMAIN_ALLOC_FLAGS ~__IOMMU_DOMAIN_DMA_FQ
/*
 * This are the possible domain-types
 *
 *	IOMMU_DOMAIN_BLOCKED	- All DMA is blocked, can be used to isolate
 *				  devices
 *	IOMMU_DOMAIN_IDENTITY	- DMA addresses are system physical addresses
 *	IOMMU_DOMAIN_UNMANAGED	- DMA mappings managed by IOMMU-API user, used
 *				  for VMs
 *	IOMMU_DOMAIN_DMA	- Internally used for DMA-API implementations.
 *				  This flag allows IOMMU drivers to implement
 *				  certain optimizations for these domains
 *	IOMMU_DOMAIN_DMA_FQ	- As above, but definitely using batched TLB
 *				  invalidation.
 *	IOMMU_DOMAIN_SVA	- DMA addresses are shared process addresses
 *				  represented by mm_struct's.
 */
// #define IOMMU_DOMAIN_BLOCKED	(0U)
// #define IOMMU_DOMAIN_IDENTITY	(__IOMMU_DOMAIN_PT)
// #define IOMMU_DOMAIN_UNMANAGED	(__IOMMU_DOMAIN_PAGING)
// #define IOMMU_DOMAIN_DMA	(__IOMMU_DOMAIN_PAGING |  __IOMMU_DOMAIN_DMA_API)
#define IOMMU_DOMAIN_DMA_FQ	(__IOMMU_DOMAIN_PAGING |	\
				 __IOMMU_DOMAIN_DMA_API |	\
				 __IOMMU_DOMAIN_DMA_FQ)
#define IOMMU_DOMAIN_SVA	(__IOMMU_DOMAIN_SVA)



/**
 * enum iommu_dev_features - Per device IOMMU features
 * @IOMMU_DEV_FEAT_SVA: Shared Virtual Addresses
 * @IOMMU_DEV_FEAT_IOPF: I/O Page Faults such as PRI or Stall. Generally
 *			 enabling %IOMMU_DEV_FEAT_SVA requires
 *			 %IOMMU_DEV_FEAT_IOPF, but some devices manage I/O Page
 *			 Faults themselves instead of relying on the IOMMU. When
 *			 supported, this feature must be enabled before and
 *			 disabled after %IOMMU_DEV_FEAT_SVA.
 *
 * Device drivers enable a feature using iommu_dev_enable_feature().
 */
enum iommu_dev_features {
	IOMMU_DEV_FEAT_SVA,
	IOMMU_DEV_FEAT_IOPF,
};

struct rb_root {
	struct rb_node *rb_node;
};

struct rb_node {
	unsigned long  __rb_parent_color;
	struct rb_node *rb_right;
	struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));
    /* The alignment might seem pointless, but allegedly CRIS needs it */

/**
 * struct iommu_fault_unrecoverable - Unrecoverable fault data
 * @reason: reason of the fault, from &enum iommu_fault_reason
 * @flags: parameters of this fault (IOMMU_FAULT_UNRECOV_* values)
 * @pasid: Process Address Space ID
 * @perm: requested permission access using by the incoming transaction
 *        (IOMMU_FAULT_PERM_* values)
 * @addr: offending page address
 * @fetch_addr: address that caused a fetch abort, if any
 */
struct iommu_fault_unrecoverable {
	uint32_t	reason;
#define IOMMU_FAULT_UNRECOV_PASID_VALID		(1 << 0)
#define IOMMU_FAULT_UNRECOV_ADDR_VALID		(1 << 1)
#define IOMMU_FAULT_UNRECOV_FETCH_ADDR_VALID	(1 << 2)
	uint32_t	flags;
	uint32_t	pasid;
	uint32_t	perm;
	uint64_t	addr;
	uint64_t	fetch_addr;
};

/**
 * struct iommu_fault_page_request - Page Request data
 * @flags: encodes whether the corresponding fields are valid and whether this
 *         is the last page in group (IOMMU_FAULT_PAGE_REQUEST_* values).
 *         When IOMMU_FAULT_PAGE_RESPONSE_NEEDS_PASID is set, the page response
 *         must have the same PASID value as the page request. When it is clear,
 *         the page response should not have a PASID.
 * @pasid: Process Address Space ID
 * @grpid: Page Request Group Index
 * @perm: requested page permissions (IOMMU_FAULT_PERM_* values)
 * @addr: page address
 * @private_data: device-specific private information
 */
struct iommu_fault_page_request {
#define IOMMU_FAULT_PAGE_REQUEST_PASID_VALID	(1 << 0)
#define IOMMU_FAULT_PAGE_REQUEST_LAST_PAGE	(1 << 1)
#define IOMMU_FAULT_PAGE_REQUEST_PRIV_DATA	(1 << 2)
#define IOMMU_FAULT_PAGE_RESPONSE_NEEDS_PASID	(1 << 3)
	uint32_t	flags;
	uint32_t	pasid;
	uint32_t	grpid;
	uint32_t	perm;
	uint64_t	addr;
	uint64_t	private_data[2];
};

/**
 * struct iommu_fault - Generic fault data
 * @type: fault type from &enum iommu_fault_type
 * @padding: reserved for future use (should be zero)
 * @event: fault event, when @type is %IOMMU_FAULT_DMA_UNRECOV
 * @prm: Page Request message, when @type is %IOMMU_FAULT_PAGE_REQ
 * @padding2: sets the fault size to allow for future extensions
 */
struct iommu_fault {
	uint32_t	type;
	uint32_t	padding;
	union {
		struct iommu_fault_unrecoverable event;
		struct iommu_fault_page_request prm;
		uint8_t padding2[56];
	};
};

/**
 * struct iommu_fault_event - Generic fault event
 *
 * Can represent recoverable faults such as a page requests or
 * unrecoverable faults such as DMA or IRQ remapping faults.
 *
 * @fault: fault descriptor
 * @list: pending fault event list, used for tracking responses
 */
struct iommu_fault_event {
	struct iommu_fault fault;
	struct list_head list;
};

#define IOMMU_FAULT_PERM_READ	(1 << 0) /* read */
#define IOMMU_FAULT_PERM_WRITE	(1 << 1) /* write */
#define IOMMU_FAULT_PERM_EXEC	(1 << 2) /* exec */
#define IOMMU_FAULT_PERM_PRIV	(1 << 3) /* privileged */

/**
 * struct iommu_page_response - Generic page response information
 * @argsz: User filled size of this data
 * @version: API version of this structure
 * @flags: encodes whether the corresponding fields are valid
 *         (IOMMU_FAULT_PAGE_RESPONSE_* values)
 * @pasid: Process Address Space ID
 * @grpid: Page Request Group Index
 * @code: response code from &enum iommu_page_response_code
 */
struct iommu_page_response {
	uint32_t	argsz;
#define IOMMU_PAGE_RESP_VERSION_1	1
	uint32_t	version;
#define IOMMU_PAGE_RESP_PASID_VALID	(1 << 0)
	uint32_t	flags;
	uint32_t	pasid;
	uint32_t	grpid;
	uint32_t	code;
};

struct riscv_iommu_queue {
	dma_addr_t base_dma;	/* ring buffer bus address */
	void *base;		/* ring buffer pointer */
	size_t len;		/* single item length */
	uint32_t cnt;		/* items count */
	uint32_t lui;		/* last used index, consumer/producer share */
	unsigned qbr;		/* queue base register offset */
	unsigned qcr;		/* queue control and status register offset */
	int irq;		/* registered interrupt number */
	bool in_iomem;		/* indicates queue data are in I/O memory  */
};

enum riscv_queue_ids {
	RISCV_IOMMU_COMMAND_QUEUE	= 0,
	RISCV_IOMMU_FAULT_QUEUE		= 1,
	RISCV_IOMMU_PAGE_REQUEST_QUEUE	= 2
};

struct iommu_device_t {
	struct list_head list;
	const struct iommu_ops *ops;
	struct fwnode_handle *fwnode;
	struct riscv_iommu_device *dev;
	struct iommu_fwspec *fwspec;
	uint32_t max_pasids;

	void *priv;
};

/* Private dev_iommu_priv object, device-domain relationship. */
struct riscv_iommu_endpoint {
	struct riscv_iommu_device *iommu;		/* iommu core interface */
	unsigned devid;      			/* PCI bus:device:function number */
	unsigned domid;    			/* PCI domain number, segment */
	struct rb_node node;    		/* device tracking node (lookup by devid) */
	struct riscv_iommu_dc *dc;		/* device context pointer */
	struct riscv_iommu_pc *pc;		/* process context root, valid if pasid_enabled is true */
	struct riscv_iommu_msi_pte *msi_root;	/* interrupt re-mapping */

	struct list_head domain;		/* endpoint attached managed domain */
	struct list_head regions;		/* reserved regions, interrupt remapping window */

	/* end point info bits */
	unsigned pasid_bits;
	unsigned pasid_feat;
	bool pasid_enabled;
	bool ir_enabled;
};

struct riscv_iommu_device {
	struct iommu_device_t iommu;	/* iommu core interface */
	struct iommu_device *dev;
	uint32_t features;

	/* hardware control register space */
	void *reg;
	phys_addr_t reg_phys;

	struct riscv_iommu_endpoint ep;
	/* IRQs for the various queues */
	int irq_cmdq;
	int irq_fltq;
	int irq_pm;
	int irq_priq;

	/* Queue lengths */
	int cmdq_len;
	int fltq_len;
	int priq_len;

	/* supported and enabled hardware capabilities */
	uint64_t cap;


	/* device directory table root pointer and mode */
	unsigned long ddtp;
	unsigned ddt_mode;
	bool ddtp_in_iomem;

	/* hardware queues */
	struct riscv_iommu_queue cmdq;
	struct riscv_iommu_queue fltq;
	struct riscv_iommu_queue priq;

	uint32_t max_pasids;
};

struct iommu_resv_region {
	struct list_head	list;
	phys_addr_t		start;
	size_t			length;
	int			prot;
	enum iommu_resv_type	type;
	void (*free)(struct riscv_iommu_device *dev, struct iommu_resv_region *region);
};

struct riscv_iommu_domain riscv_iommu_domains[NR_IOMMU_DOMAINS];
#define riscv_iommu_domain_ctrl			riscv_iommu_domains[iommu_dom]

static struct riscv_iommu_device rv_iommu_devices[NR_IOMMU_DEVICES];
static uint32_t riscv_iommu_device_cnt = 0;

/* Global IOMMU params. */
static int ddt_mode = RISCV_IOMMU_DDTP_MODE_3LVL;
static int cmdq_length = 128;
static int fltq_length = 128;
static int priq_length = 128;
/* IOMMU PSCID allocation namespace. */
#define RISCV_IOMMU_MAX_PSCID	(1U << 20)
// static DEFINE_IDA(riscv_iommu_pscids);

/* TODO: Enable MSI remapping */
#define RISCV_IMSIC_BASE	0x28000000

/* 1 second */
#define RISCV_IOMMU_TIMEOUT	(1000000)

/* RISC-V IOMMU PPN <> PHYS address conversions, PHYS <=> PPN[53:10] */
#define phys_to_ppn(va)  (((va) >> 2) & (((1ULL << 44) - 1) << 10))
#define ppn_to_phys(pn)	 (((pn) << 2) & (((1ULL << 44) - 1) << 12))

static struct riscv_iommu_device *get_iommu_device(void)
{
	if (!iommu_device_ctrl.iommu) {
		BUG_ON(riscv_iommu_device_cnt >= NR_IOMMU_DEVICES);
		iommu_device_ctrl.iommu = (void *)&rv_iommu_devices[riscv_iommu_device_cnt];
		rv_iommu_devices[riscv_iommu_device_cnt].dev = &iommu_device_ctrl;
		riscv_iommu_device_cnt ++;
	}

	return iommu_device_ctrl.iommu;
}

static uint64_t g_ddt_buf[PAGE_SIZE];
static uint32_t g_ddt_cnt;
static int riscv_iommu_save_ddt(uint64_t ddt)
{
	g_ddt_buf[g_ddt_cnt++] = ddt;
	return 0;
}

static int riscv_iommu_ddt_show(void)
{
	int i;
	for (i = 0; i< g_ddt_cnt; i++) {
		con_log("riscv iommu ddt[%d]:%lx\n", i, g_ddt_buf[i]);
	}
	return 0;
}

bool riscv_iommu_is_ddt(uint64_t ddt)
{
	int i;
	for (i = 0; i< g_ddt_cnt; i++) {
		if (ddt >= g_ddt_buf[i] && ddt < (g_ddt_buf[i]+PAGE_SIZE)) {
			// con_log("riscv iommu is ddt:%lx\n", ddt);
			return true;
		}
	}

	return false;
}

static uint64_t g_pdt_buf[PAGE_SIZE];
static uint32_t g_pdt_cnt;
static int riscv_iommu_save_pdt(uint64_t pdt)
{
	g_pdt_buf[g_pdt_cnt++] = pdt;
	return 0;
}

static int riscv_iommu_pdt_show(void)
{
	int i;
	for (i = 0; i< g_pdt_cnt; i++) {
		con_log("riscv iommu pdt[%d]:%lx\n", i, g_pdt_buf[i]);
	}
	return 0;
}

bool riscv_iommu_is_pdt(uint64_t pdt)
{
	int i;
	for (i = 0; i< g_pdt_cnt; i++) {
		if (pdt >= g_pdt_buf[i] && pdt < (g_pdt_buf[i]+PAGE_SIZE)) {
			// con_log("riscv iommu is pdt:%lx\n", pdt);
			return true;
		}
	}

	return false;
}

static uint64_t g_pgtbl_buf[PAGE_SIZE];
static uint32_t g_pgtbl_cnt;
int riscv_iommu_save_pgtbl(uint64_t pgtbl)
{
	g_pgtbl_buf[g_pgtbl_cnt++] = pgtbl;
	return 0;
}

static int riscv_iommu_pgtbl_show(void)
{
	int i;
	for (i = 0; i< g_pgtbl_cnt; i++) {
		con_log("riscv iommu pgtbl[%d]:%lx\n", i, g_pgtbl_buf[i]);
	}
	return 0;
}

bool riscv_iommu_is_pgtbl(uint64_t pgtbl)
{
	int i;
	for (i = 0; i< g_pgtbl_cnt; i++) {
		if (pgtbl >= g_pgtbl_buf[i] && pgtbl < (g_pgtbl_buf[i]+PAGE_SIZE)) {
			// con_log("riscv iommu is pgtbl:%lx\n", pgtbl);
			return true;
		}
	}

	return false;
}

static uint64_t g_queue_buf[PAGE_SIZE];
static uint64_t g_queue_size[PAGE_SIZE];
static uint32_t g_queue_cnt;
static int riscv_iommu_save_queue(uint64_t addr, size_t queue_size)
{
	g_queue_buf[g_queue_cnt] = addr;
	g_queue_size[g_queue_cnt] = queue_size;

	g_queue_cnt ++;
	return 0;
}

static int riscv_iommu_queue_show(void)
{
	int i;
	for (i = 0; i< g_queue_cnt; i++) {
		con_log("riscv iommu queue[%d]:%lx\n", i, g_queue_buf[i]);
	}
	return 0;
}

bool riscv_iommu_is_queue(uint64_t addr)
{
	int i;
	for (i = 0; i< g_queue_cnt; i++) {
		if (addr >= g_queue_buf[i] && addr < (g_queue_buf[i]+g_queue_size[i])) {
			// con_log("riscv iommu is queue:%lx\n", addr);
			return true;
		}
	}

	return false;
}

int riscv_iommu_show_table(void)
{
	riscv_iommu_ddt_show();
	riscv_iommu_pdt_show();
	riscv_iommu_pgtbl_show();
	riscv_iommu_queue_show();
	return 0;
}

static struct riscv_iommu_dc *riscv_iommu_get_dc(struct riscv_iommu_device *iommu, uint32_t devid);
int riscv_iommu_pgtbl_ptw(uint64_t pgd_root, uint64_t iova, int mode);

int riscv_iommu_debug(int ddi, int pdi, uint64_t iova)
{
	struct riscv_iommu_device *iommu = get_iommu_device();
	struct riscv_iommu_dc *dc = riscv_iommu_get_dc(iommu, ddi);
	uint64_t dc_ppn = FIELD_GET(RISCV_IOMMU_DC_FSC_PPN, dc->fsc);
	con_log("dc(%p)->fsc.ppn:%lx ", dc, dc_ppn);

	struct riscv_iommu_pc *pdtp = (struct riscv_iommu_pc *)(dc_ppn << 12);
	con_log("pc(%p)->fsc:%lx\n", pdtp + pdi, pdtp[pdi].fsc);
	uint64_t pc_ppn =  FIELD_GET(RISCV_IOMMU_PC_FSC_PPN, pdtp[pdi].fsc);

	riscv_iommu_pgtbl_ptw(pc_ppn << 12, iova, 8); //8:sv39

	return 0;	
}

/* Helper functions and macros */

static inline uint32_t riscv_iommu_readl(struct riscv_iommu_device *iommu, unsigned offset)
{
	return __raw_readl((caddr_t)iommu->reg + offset);
}

static inline void riscv_iommu_writel(struct riscv_iommu_device *iommu, unsigned offset, uint32_t val)
{
	__raw_writel(val, (caddr_t)iommu->reg + offset);
}

static inline uint64_t riscv_iommu_readq(struct riscv_iommu_device *iommu, unsigned offset)
{
	return __raw_readq((caddr_t)iommu->reg + offset);
}

static inline void riscv_iommu_writeq(struct riscv_iommu_device *iommu, unsigned offset, uint64_t val)
{
	__raw_writeq(val, (caddr_t)iommu->reg + offset);
}

/*
 * Common queue management routines
 */

/* Note: offsets are the same for all queues */
#define Q_HEAD(q) ((q)->qbr + (RISCV_IOMMU_REG_CQH - RISCV_IOMMU_REG_CQB))
#define Q_TAIL(q) ((q)->qbr + (RISCV_IOMMU_REG_CQT - RISCV_IOMMU_REG_CQB))

static unsigned riscv_iommu_queue_consume(struct riscv_iommu_device *iommu, struct riscv_iommu_queue *q, unsigned *ready)
{
	uint32_t tail = riscv_iommu_readl(iommu, Q_TAIL(q));
	*ready = q->lui;

	BUG_ON(q->cnt <= tail);
	if (q->lui <= tail)
		return tail - q->lui;
	return q->cnt - q->lui;
}

static void riscv_iommu_queue_release(struct riscv_iommu_device *iommu, struct riscv_iommu_queue *q, unsigned count)
{
	q->lui = (q->lui + count) & (q->cnt - 1);
	riscv_iommu_writel(iommu, Q_HEAD(q), q->lui);
}

static uint32_t riscv_iommu_queue_ctrl(struct riscv_iommu_device *iommu, struct riscv_iommu_queue *q, uint32_t val)
{
	tick_t end_cycles = RISCV_IOMMU_TIMEOUT + tick_get_counter();

	riscv_iommu_writel(iommu, q->qcr, val);
	do {
		val = riscv_iommu_readl(iommu, q->qcr);
		if (!(val & RISCV_IOMMU_QUEUE_BUSY))
			break;
		cpu_relax();
	} while (tick_get_counter() < end_cycles);

	return val;
}

static void riscv_iommu_queue_free(struct riscv_iommu_device *iommu, struct riscv_iommu_queue *q)
{
	size_t size = q->len * q->cnt;

	riscv_iommu_queue_ctrl(iommu, q, 0);

	if (q->base) {
		// if (q->in_iomem)
		// 	iounmap(q->base);
		// else
			dma_free_coherent((dma_t)iommu->dev->dma, size, (caddr_t)q->base, q->base_dma);
	}
	// if (q->irq)
	// 	free_irq(q->irq, q);
}

static int riscv_iommu_cmdq_irq_check(int irq, void *data);
static int riscv_iommu_cmdq_process(int irq, void *data);
#if 0
static int riscv_iommu_fltq_irq_check(int irq, void *data);
static int riscv_iommu_fltq_process(int irq, void *data);
static int riscv_iommu_priq_irq_check(int irq, void *data);
static int riscv_iommu_priq_process(int irq, void *data);
#endif
static void riscv_iommu_cmdq_irq_hdl(irq_t irq)
{
	__unused void *data = NULL;
	riscv_iommu_cmdq_irq_check(irq, data);
	riscv_iommu_cmdq_process(irq, data);
}

#if 0
static void riscv_iommu_fltq_irq_hdl(irq_t irq)
{
	riscv_iommu_fltq_irq_check(irq);
	riscv_iommu_fltq_process(irq);
}

static void riscv_iommu_priq_irq_hdl(irq_t irq)
{
	riscv_iommu_priq_irq_check(irq);
	riscv_iommu_priq_process(irq);
}
#endif

static int riscv_iommu_queue_init(struct riscv_iommu_device *iommu, int queue_id)
{
	struct riscv_iommu_queue *q = NULL;
	size_t queue_size = 0;
	// irq_handler_t irq_check;
	// irq_handler_t irq_process;
	const char *name;
	int count = 0;
	int irq = 0;
	unsigned order = 0;
	uint64_t qbr_val = 0;
	uint64_t qbr_readback = 0;
	// uint64_t qbr_paddr = 0;
	int ret = 0;

	switch (queue_id) {
	case RISCV_IOMMU_COMMAND_QUEUE:
		q = &iommu->cmdq;
		q->len = sizeof(struct riscv_iommu_command);
		count = iommu->cmdq_len;
		irq = iommu->irq_cmdq;
		q->qbr = RISCV_IOMMU_REG_CQB;
		q->qcr = RISCV_IOMMU_REG_CQCSR;
		name = "cmdq";
		break;
	case RISCV_IOMMU_FAULT_QUEUE:
		q = &iommu->fltq;
		q->len = sizeof(struct riscv_iommu_fq_record);
		count = iommu->fltq_len;
		irq = iommu->irq_fltq;
		q->qbr = RISCV_IOMMU_REG_FQB;
		q->qcr = RISCV_IOMMU_REG_FQCSR;
		name = "fltq";
		break;
#if 0
	case RISCV_IOMMU_PAGE_REQUEST_QUEUE:
		q = &iommu->priq;
		q->len = sizeof(struct riscv_iommu_pq_record);
		count = iommu->priq_len;
		irq = iommu->irq_priq;
		irq_check = riscv_iommu_priq_irq_check;
		irq_process = riscv_iommu_priq_process;
		q->qbr = RISCV_IOMMU_REG_PQB;
		q->qcr = RISCV_IOMMU_REG_PQCSR;
		name = "priq";
		break;
#endif
	default:
		con_err("invalid queue interrupt index in queue_init!\n");
		return -EINVAL;
	}

	/* Polling not implemented */
	if (!irq)
		return -ENODEV;

	/* Allocate queue in memory and set the base register */
	// order = ilog2_const((iommu->cmdq_len));
	order = 7;
	// order = ilog2_const(128);
	// order = count;
	do {
		queue_size = q->len * (1ULL << order);
		q->base = (void *)dma_alloc_coherent(iommu->dev->dma, queue_size, &q->base_dma);
		if (q->base || queue_size < PAGE_SIZE)
			break;

		order--;
	} while (1);

	if (!q->base) {
		con_err("failed to allocate %s queue (cnt: %u)\n", name, count);
		return -ENOMEM;
	}

	q->cnt = 1ULL << order;

	qbr_val = phys_to_ppn(q->base_dma) |
	    FIELD_PREP(RISCV_IOMMU_QUEUE_LOGSZ_FIELD, order - 1);

	riscv_iommu_save_queue((uint64_t)q->base_dma, queue_size);
	riscv_iommu_writeq(iommu, q->qbr, qbr_val);

	/*
	 * Queue base registers are WARL, so it's possible that whatever we wrote
	 * there was illegal/not supported by the hw in which case we need to make
	 * sure we set a supported PPN and/or queue size.
	 */
	qbr_readback = riscv_iommu_readq(iommu, q->qbr);
	if (qbr_readback == qbr_val)
		goto irq;

	dma_free_coherent(iommu->dev->dma, queue_size, (caddr_t)q->base, q->base_dma);

	/* Get supported queue size */
	order = FIELD_GET(RISCV_IOMMU_QUEUE_LOGSZ_FIELD, qbr_readback) + 1;
	q->cnt = 1ULL << order;
	queue_size = q->len * q->cnt;

	/*
	 * In case we also failed to set PPN, it means the field is hardcoded and the
	 * queue resides in I/O memory instead, so get its physical address and
	 * ioremap it.
	 */
	// qbr_paddr = ppn_to_phys(qbr_readback);
	// if (qbr_paddr != q->base_dma) {
	// 	con_log("hardcoded ppn in %s base register, using io memory for the queue\n", name);
	// 	con_log("queue length for %s set to %i\n", name, q->cnt);
	// 	q->in_iomem = true;
	// 	q->base = ioremap(qbr_paddr, queue_size);
	// 	if (!q->base) {
	// 		con_err("failed to map %s queue (cnt: %u)\n", name, q->cnt);
	// 		return -ENOMEM;
	// 	}
	// 	q->base_dma = qbr_paddr;
	// } else {
		/*
		 * We only failed to set the queue size, re-try to allocate memory with
		 * the queue size supported by the hw.
		 */
		con_log("hardcoded queue size in %s base register\n", name);
		con_log("retrying with queue length: %i\n", q->cnt);
		q->base = (void *)dma_alloc_coherent(iommu->dev->dma, queue_size, &q->base_dma);
		if (!q->base) {
			con_err("failed to allocate %s queue (cnt: %u)\n",
				name, q->cnt);
			return -ENOMEM;
		}
	// }

	qbr_val = phys_to_ppn(q->base_dma) |
	    FIELD_PREP(RISCV_IOMMU_QUEUE_LOGSZ_FIELD, order - 1);
	riscv_iommu_writeq(iommu, q->qbr, qbr_val);

	/* Final check to make sure hw accepted our write */
	qbr_readback = riscv_iommu_readq(iommu, q->qbr);
	if (qbr_readback != qbr_val) {
		con_err("failed to set base register for %s\n", name);
		goto fail;
	}

 irq:
 	// irq_register_vector(irq, );
	// if (request_threaded_irq(irq, irq_check, irq_process, IRQF_ONESHOT | IRQF_SHARED,
	// 			 dev_name(dev), q)) {
	// 	con_err("fail to request irq %d for %s\n", irq, name);
	// 	goto fail;
	// }

	q->irq = irq;

	/* Note: All RIO_xQ_EN/IE fields are in the same offsets */
	ret = riscv_iommu_queue_ctrl(iommu, q,
				   RISCV_IOMMU_QUEUE_ENABLE |
				   RISCV_IOMMU_QUEUE_INTR_ENABLE);
	if (ret & RISCV_IOMMU_QUEUE_BUSY) {
		con_err("%s init timeout\n", name);
		ret = -EBUSY;
		goto fail;
	}

	return 0;
 fail:
	riscv_iommu_queue_free(iommu, q);
	return 0;
}

/*
 * I/O MMU Command queue chapter 3.1
 */

static inline void riscv_iommu_cmd_inval_vma(struct riscv_iommu_command *cmd)
{
	cmd->dword0 =
	    FIELD_PREP(RISCV_IOMMU_CMD_OPCODE,
		       RISCV_IOMMU_CMD_IOTINVAL_OPCODE) | FIELD_PREP(RISCV_IOMMU_CMD_FUNC,
								     RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA);
	cmd->dword1 = 0;
}

static inline void riscv_iommu_cmd_inval_set_addr(struct riscv_iommu_command *cmd,
						  uint64_t addr)
{
	cmd->dword0 |= RISCV_IOMMU_CMD_IOTINVAL_AV;
	cmd->dword1 = addr;
}

static inline void riscv_iommu_cmd_inval_set_pscid(struct riscv_iommu_command *cmd,
						   unsigned pscid)
{
	cmd->dword0 |= FIELD_PREP(RISCV_IOMMU_CMD_IOTINVAL_PSCID, pscid) |
	    RISCV_IOMMU_CMD_IOTINVAL_PSCV;
}

static inline void riscv_iommu_cmd_inval_set_gscid(struct riscv_iommu_command *cmd,
						   unsigned gscid)
{
	cmd->dword0 |= FIELD_PREP(RISCV_IOMMU_CMD_IOTINVAL_GSCID, gscid) |
	    RISCV_IOMMU_CMD_IOTINVAL_GV;
}

static inline void riscv_iommu_cmd_iofence(struct riscv_iommu_command *cmd)
{
	cmd->dword0 = FIELD_PREP(RISCV_IOMMU_CMD_OPCODE, RISCV_IOMMU_CMD_IOFENCE_OPCODE) |
	    FIELD_PREP(RISCV_IOMMU_CMD_FUNC, RISCV_IOMMU_CMD_IOFENCE_FUNC_C);
	cmd->dword1 = 0;
}

static inline void riscv_iommu_cmd_iofence_set_av(struct riscv_iommu_command *cmd,
						  uint64_t addr, uint32_t data)
{
	cmd->dword0 = FIELD_PREP(RISCV_IOMMU_CMD_OPCODE, RISCV_IOMMU_CMD_IOFENCE_OPCODE) |
	    FIELD_PREP(RISCV_IOMMU_CMD_FUNC, RISCV_IOMMU_CMD_IOFENCE_FUNC_C) |
	    FIELD_PREP(RISCV_IOMMU_CMD_IOFENCE_DATA, data) | RISCV_IOMMU_CMD_IOFENCE_AV;
	cmd->dword1 = (addr >> 2);
}

static inline void riscv_iommu_cmd_iodir_inval_ddt(struct riscv_iommu_command *cmd)
{
	cmd->dword0 = FIELD_PREP(RISCV_IOMMU_CMD_OPCODE, RISCV_IOMMU_CMD_IODIR_OPCODE) |
	    FIELD_PREP(RISCV_IOMMU_CMD_FUNC, RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT);
	cmd->dword1 = 0;
}

static inline void riscv_iommu_cmd_iodir_inval_pdt(struct riscv_iommu_command *cmd)
{
	cmd->dword0 = FIELD_PREP(RISCV_IOMMU_CMD_OPCODE, RISCV_IOMMU_CMD_IODIR_OPCODE) |
	    FIELD_PREP(RISCV_IOMMU_CMD_FUNC, RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT);
	cmd->dword1 = 0;
}

static inline void riscv_iommu_cmd_iodir_set_did(struct riscv_iommu_command *cmd,
						 unsigned devid)
{
	cmd->dword0 |=
	    FIELD_PREP(RISCV_IOMMU_CMD_IODIR_DID, devid) | RISCV_IOMMU_CMD_IODIR_DV;
}

static inline void riscv_iommu_cmd_iodir_set_pid(struct riscv_iommu_command *cmd,
						 unsigned pasid)
{
	cmd->dword0 |= FIELD_PREP(RISCV_IOMMU_CMD_IODIR_PID, pasid);
}

#if 0
static void riscv_iommu_cmd_ats_inval(struct riscv_iommu_command *cmd)
{
	cmd->dword0 = FIELD_PREP(RISCV_IOMMU_CMD_OPCODE, RISCV_IOMMU_CMD_ATS_OPCODE) |
	    FIELD_PREP(RISCV_IOMMU_CMD_FUNC, RISCV_IOMMU_CMD_ATS_FUNC_INVAL);
	cmd->dword1 = 0;
}

static inline void riscv_iommu_cmd_ats_prgr(struct riscv_iommu_command *cmd)
{
	cmd->dword0 = FIELD_PREP(RISCV_IOMMU_CMD_OPCODE, RISCV_IOMMU_CMD_ATS_OPCODE) |
	    FIELD_PREP(RISCV_IOMMU_CMD_FUNC, RISCV_IOMMU_CMD_ATS_FUNC_PRGR);
	cmd->dword1 = 0;
}

static void riscv_iommu_cmd_ats_set_rid(struct riscv_iommu_command *cmd, uint32_t rid)
{
	cmd->dword0 |= FIELD_PREP(RISCV_IOMMU_CMD_ATS_RID, rid);
}

static void riscv_iommu_cmd_ats_set_pid(struct riscv_iommu_command *cmd, uint32_t pid)
{
	cmd->dword0 |= FIELD_PREP(RISCV_IOMMU_CMD_ATS_PID, pid) | RISCV_IOMMU_CMD_ATS_PV;
}

static void riscv_iommu_cmd_ats_set_dseg(struct riscv_iommu_command *cmd, uint8_t seg)
{
	cmd->dword0 |= FIELD_PREP(RISCV_IOMMU_CMD_ATS_DSEG, seg) | RISCV_IOMMU_CMD_ATS_DSV;
}

static void riscv_iommu_cmd_ats_set_payload(struct riscv_iommu_command *cmd, uint64_t payload)
{
	cmd->dword1 = payload;
}

/* Prepare the ATS invalidation payload */
static unsigned long riscv_iommu_ats_inval_payload(unsigned long start,
						   unsigned long end, bool global_inv)
{
	size_t len = end - start + 1;
	unsigned long payload = 0;

	/*
	 * PCI Express specification
	 * Section 10.2.3.2 Translation Range Size (S) Field
	 */
	if (len < PAGE_SIZE)
		len = PAGE_SIZE;
	else
		len = __roundup_pow_of_two(len);

	payload = (start & ~(len - 1)) | (((len - 1) >> 12) << 11);

	if (global_inv)
		payload |= RISCV_IOMMU_CMD_ATS_INVAL_G;

	return payload;
}

/* Prepare the ATS invalidation payload for all translations to be invalidated. */
static unsigned long riscv_iommu_ats_inval_all_payload(bool global_inv)
{
	unsigned long payload = GENMASK_ULL(62, 11);

	if (global_inv)
		payload |= RISCV_IOMMU_CMD_ATS_INVAL_G;

	return payload;
}

/* Prepare the ATS "Page Request Group Response" payload */
static unsigned long riscv_iommu_ats_prgr_payload(uint16_t dest_id, uint8_t resp_code, uint16_t grp_idx)
{
	return FIELD_PREP(RISCV_IOMMU_CMD_ATS_PRGR_DST_ID, dest_id) |
	    FIELD_PREP(RISCV_IOMMU_CMD_ATS_PRGR_RESP_CODE, resp_code) |
	    FIELD_PREP(RISCV_IOMMU_CMD_ATS_PRGR_PRG_INDEX, grp_idx);
}
#endif

/* TODO: Convert into lock-less MPSC implementation. */
static bool riscv_iommu_post_sync(struct riscv_iommu_device *iommu, struct riscv_iommu_command *cmd, bool sync)
{
	uint32_t head, tail, next, last;
	// unsigned long flags;

	head = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_CQH) & (iommu->cmdq.cnt - 1);
	tail = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_CQT) & (iommu->cmdq.cnt - 1);
	last = iommu->cmdq.lui;
	if (tail != last) {
		/*
		 * FIXME: This is a workaround for dropped MMIO writes/reads on QEMU platform.
		 *        While debugging of the problem is still ongoing, this provides
		 *        a simple impolementation of try-again policy.
		 *        Will be changed to lock-less algorithm in the feature.
		 */
		con_log("IOMMU CQT: %x != %x (1st)\n", last, tail);
		tail =
		    riscv_iommu_readl(iommu, RISCV_IOMMU_REG_CQT) & (iommu->cmdq.cnt - 1);
		last = iommu->cmdq.lui;
		if (tail != last) {
			con_log("IOMMU CQT: %x != %x (2nd)\n", last, tail);
		}
	}

	next = (last + 1) & (iommu->cmdq.cnt - 1);
	if (next != head) {
		struct riscv_iommu_command *ptr = iommu->cmdq.base;
		ptr[last] = *cmd;
		wmb();
		riscv_iommu_writel(iommu, RISCV_IOMMU_REG_CQT, next);
		iommu->cmdq.lui = next;
	}

	if (sync && head != next) {
		tick_t start_time = tick_get_counter();
		while (1) {
			last = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_CQH) &
			    (iommu->cmdq.cnt - 1);
			if (head < next && last >= next)
				break;
			if (head > next && last < head && last >= next)
				break;
			if (RISCV_IOMMU_TIMEOUT < (tick_get_counter() - start_time)) {
				con_err("IOFENCE TIMEOUT\n");
				return false;
			}
			cpu_relax();
		}
	}

	return next != head;
}

static bool riscv_iommu_post(struct riscv_iommu_device *iommu, struct riscv_iommu_command *cmd)
{
	return riscv_iommu_post_sync(iommu, cmd, false);
}

static bool riscv_iommu_iodir_inv_devid(struct riscv_iommu_device *iommu, unsigned int devid)
{
	struct riscv_iommu_command cmd;
	riscv_iommu_cmd_iodir_inval_ddt(&cmd);
	riscv_iommu_cmd_iodir_set_did(&cmd, devid);
	return riscv_iommu_post(iommu, &cmd);
}

static bool riscv_iommu_iodir_inv_pasid(struct riscv_iommu_device *iommu, unsigned devid, unsigned pasid)
{
	struct riscv_iommu_command cmd;
	riscv_iommu_cmd_iodir_inval_pdt(&cmd);
	riscv_iommu_cmd_iodir_set_did(&cmd, devid);
	riscv_iommu_cmd_iodir_set_pid(&cmd, pasid);
	return riscv_iommu_post(iommu, &cmd);
}

static bool riscv_iommu_iofence_sync(struct riscv_iommu_device *iommu)
{
	struct riscv_iommu_command cmd;
	riscv_iommu_cmd_iofence(&cmd);
	return riscv_iommu_post_sync(iommu, &cmd, true);
}

#if 0
static void riscv_iommu_mm_invalidate(struct mmu_notifier *mn,
				      struct mm_struct *mm, unsigned long start,
				      unsigned long end)
{
	struct riscv_iommu_command cmd;
	struct riscv_iommu_endpoint *endpoint;
	struct riscv_iommu_domain *domain = iommu->iommu.
	    container_of(mn, struct riscv_iommu_domain, mn);
	unsigned long iova;
	/*
	 * The mm_types defines vm_end as the first byte after the end address,
	 * different from IOMMU subsystem using the last address of an address
	 * range. So do a simple translation here by updating what end means.
	 */
	unsigned long payload = riscv_iommu_ats_inval_payload(start, end - 1, true);

	riscv_iommu_cmd_inval_vma(&cmd);
	riscv_iommu_cmd_inval_set_gscid(&cmd, 0);
	riscv_iommu_cmd_inval_set_pscid(&cmd, domain->pscid);
	if (end > start) {
		/* Cover only the range that is needed */
		for (iova = start; iova < end; iova += PAGE_SIZE) {
			riscv_iommu_cmd_inval_set_addr(&cmd, iova);
			riscv_iommu_post(&cmd);
		}
	} else {
		riscv_iommu_post(&cmd);
	}

	riscv_iommu_iofence_sync();

	/* ATS invalidation for every device and for specific translation range. */
	list_for_each_entry(endpoint, &domain->endpoints, domain) {
		if (!endpoint->pasid_enabled)
			continue;

		riscv_iommu_cmd_ats_inval(&cmd);
		riscv_iommu_cmd_ats_set_dseg(&cmd, endpoint->domid);
		riscv_iommu_cmd_ats_set_rid(&cmd, endpoint->devid);
		riscv_iommu_cmd_ats_set_pid(&cmd, domain->pasid);
		riscv_iommu_cmd_ats_set_payload(&cmd, payload);
		riscv_iommu_post(&cmd);
	}
	riscv_iommu_iofence_sync();
}

static void riscv_iommu_mm_release(struct mmu_notifier *mn, struct mm_struct *mm)
{
	/* TODO: removed from notifier, cleanup PSCID mapping, flush IOTLB */
}

static const struct mmu_notifier_ops riscv_iommu_mmuops = {
	.release = riscv_iommu_mm_release,
	.invalidate_range = riscv_iommu_mm_invalidate,
};
#endif
/* Command queue primary interrupt handler */
static int riscv_iommu_cmdq_irq_check(int irq, void *data)
{
	struct riscv_iommu_device *iommu = (struct riscv_iommu_device *)data;

	uint32_t ipsr = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_IPSR);
	if (ipsr & RISCV_IOMMU_IPSR_CIP)
		return 0;
	return -1;
}

/* Command queue interrupt hanlder thread function */
static int riscv_iommu_cmdq_process(int irq, void *data)
{
	unsigned ctrl;
	struct riscv_iommu_device *iommu = (struct riscv_iommu_device *)data;

	/* Error reporting, clear error reports if any. */
	ctrl = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_CQCSR);
	if (ctrl & (RISCV_IOMMU_CQCSR_CQMF |
		    RISCV_IOMMU_CQCSR_CMD_TO | RISCV_IOMMU_CQCSR_CMD_ILL)) {
		riscv_iommu_queue_ctrl(iommu, &iommu->cmdq, ctrl);
		con_err("Command queue error: fault: %d tout: %d err: %d\n",
				     !!(ctrl & RISCV_IOMMU_CQCSR_CQMF),
				     !!(ctrl & RISCV_IOMMU_CQCSR_CMD_TO),
				     !!(ctrl & RISCV_IOMMU_CQCSR_CMD_ILL));
	}

	/* Clear fault interrupt pending. */
	riscv_iommu_writel(iommu, RISCV_IOMMU_REG_IPSR, RISCV_IOMMU_IPSR_CIP);

	return 0;
}

/*
 * Fault/event queue, chapter 3.2
 */

static void riscv_iommu_fault_report(struct riscv_iommu_fq_record *event)
{
	unsigned err, devid;

	err = FIELD_GET(RISCV_IOMMU_FQ_HDR_CAUSE, event->hdr);
	devid = FIELD_GET(RISCV_IOMMU_FQ_HDR_DID, event->hdr);

	con_err("Fault %d devid: %d" " iotval: %lx iotval2: %lx\n", err, devid, event->iotval, event->iotval2);
}

#if 0
/* Fault/event queue primary interrupt handler */
static int riscv_iommu_fltq_irq_check(int irq, void *data)
{
	uint32_t ipsr = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_IPSR);
	if (ipsr & RISCV_IOMMU_IPSR_FIP)
		return 0;
	return -1;
}

/* Fault queue interrupt hanlder thread function */
static int riscv_iommu_fltq_process(int irq, void *data)
{
	struct riscv_iommu_queue *q = (struct riscv_iommu_queue *)data;
	struct riscv_iommu_device *iommu;
	struct riscv_iommu_fq_record *events;
	unsigned cnt, len, idx, ctrl;

	iommu = container_of(q, struct riscv_iommu_device, fltq);
	events = (struct riscv_iommu_fq_record *)q->base;

	/* Error reporting, clear error reports if any. */
	ctrl = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_FQCSR);
	if (ctrl & (RISCV_IOMMU_FQCSR_FQMF | RISCV_IOMMU_FQCSR_FQOF)) {
		riscv_iommu_queue_ctrl(&iommu->fltq, ctrl);
		con_err(
				     "Fault queue error: fault: %d full: %d\n",
				     !!(ctrl & RISCV_IOMMU_FQCSR_FQMF),
				     !!(ctrl & RISCV_IOMMU_FQCSR_FQOF));
	}

	/* Clear fault interrupt pending. */
	riscv_iommu_writel(iommu, RISCV_IOMMU_REG_IPSR, RISCV_IOMMU_IPSR_FIP);

	/* Report fault events. */
	do {
		cnt = riscv_iommu_queue_consume(q, &idx);
		if (!cnt)
			break;
		for (len = 0; len < cnt; idx++, len++)
			riscv_iommu_fault_report(&events[idx]);
		riscv_iommu_queue_release(q, cnt);
	} while (1);

	return 0;
}
#endif
/*
 * Page request queue, chapter 3.3
 */

/*
 * Register device for IOMMU tracking.
 */
#if 0
static void riscv_iommu_add_device(struct riscv_iommu_device *struct riscv_iommu_device *dev)
{
	struct riscv_iommu_endpoint *ep, *rb_ep;
	struct rb_node **new_node, *parent_node = NULL;

	ep = dev_iommu_priv_get(dev);

	new_node = &(iommu->eps.rb_node);
	while (*new_node) {
		rb_ep = rb_entry(*new_node, struct riscv_iommu_endpoint, node);
		parent_node = *new_node;
		if (rb_ep->devid > ep->devid) {
			new_node = &((*new_node)->rb_left);
		} else if (rb_ep->devid < ep->devid) {
			new_node = &((*new_node)->rb_right);
		} else {
			con_log("device %u already in the tree\n", ep->devid);
			break;
		}
	}

	rb_link_node(&ep->node, parent_node, new_node);
	rb_insert_color(&ep->node, &iommu->eps);
}

/*
 * Get device reference based on device identifier (requester id).
 * Decrement reference count with put_device() call.
 */
static struct riscv_iommu_device *riscv_iommu_get_device(unsigned devid)
{
	struct rb_node *node;
	struct riscv_iommu_endpoint *ep;
	struct riscv_iommu_device *dev = NULL;

	node = iommu->eps.rb_node;
	while (node && !dev) {
		ep = rb_entry(node, struct riscv_iommu_endpoint, node);
		if (ep->devid < devid)
			node = node->rb_right;
		else if (ep->devid > devid)
			node = node->rb_left;
		else
			dev = get_device(ep->dev);
	}

	return dev;
}
static int riscv_iommu_ats_prgr(struct riscv_iommu_device *dev, struct iommu_page_response *msg)
{
	struct riscv_iommu_endpoint *ep = dev_iommu_priv_get(dev);
	struct riscv_iommu_command cmd;
	uint8_t resp_code;
	unsigned long payload;

	switch (msg->code) {
	case IOMMU_PAGE_RESP_SUCCESS:
		resp_code = 0b0000;
		break;
	case IOMMU_PAGE_RESP_INVALID:
		resp_code = 0b0001;
		break;
	case IOMMU_PAGE_RESP_FAILURE:
		resp_code = 0b1111;
		break;
	}
	payload = riscv_iommu_ats_prgr_payload(ep->devid, resp_code, msg->grpid);

	/* ATS Page Request Group Response */
	riscv_iommu_cmd_ats_prgr(&cmd);
	riscv_iommu_cmd_ats_set_dseg(&cmd, ep->domid);
	riscv_iommu_cmd_ats_set_rid(&cmd, ep->devid);
	if (msg->flags & IOMMU_PAGE_RESP_PASID_VALID)
		riscv_iommu_cmd_ats_set_pid(&cmd, msg->pasid);
	riscv_iommu_cmd_ats_set_payload(&cmd, payload);
	riscv_iommu_post(&cmd);

	return 0;
}

static void riscv_iommu_page_request(
				     struct riscv_iommu_pq_record *req)
{
	struct iommu_fault_event event = { 0 };
	struct iommu_fault_page_request *prm = &event.fault.prm;
	int ret;
	struct riscv_iommu_device *dev;
	unsigned devid = FIELD_GET(RISCV_IOMMU_PREQ_HDR_DID, req->hdr);

	/* Ignore PGR Stop marker. */
	if ((req->payload & RISCV_IOMMU_PREQ_PAYLOAD_M) == RISCV_IOMMU_PREQ_PAYLOAD_L)
		return;

	dev = riscv_iommu_get_device(devid);
	if (!dev) {
		/* TODO: Handle invalid page request */
		return;
	}

	event.fault.type = IOMMU_FAULT_PAGE_REQ;

	if (req->payload & RISCV_IOMMU_PREQ_PAYLOAD_L)
		prm->flags |= IOMMU_FAULT_PAGE_REQUEST_LAST_PAGE;
	if (req->payload & RISCV_IOMMU_PREQ_PAYLOAD_W)
		prm->perm |= IOMMU_FAULT_PERM_WRITE;
	if (req->payload & RISCV_IOMMU_PREQ_PAYLOAD_R)
		prm->perm |= IOMMU_FAULT_PERM_READ;

	prm->grpid = FIELD_GET(RISCV_IOMMU_PREQ_PRG_INDEX, req->payload);
	prm->addr = FIELD_GET(RISCV_IOMMU_PREQ_UADDR, req->payload) << PAGE_SHIFT;

	if (req->hdr & RISCV_IOMMU_PREQ_HDR_PV) {
		prm->flags |= IOMMU_FAULT_PAGE_REQUEST_PASID_VALID;
		/* TODO: where to find this bit */
		prm->flags |= IOMMU_FAULT_PAGE_RESPONSE_NEEDS_PASID;
		prm->pasid = FIELD_GET(RISCV_IOMMU_PREQ_HDR_PID, req->hdr);
	}

	ret = iommu_report_device_fault(dev, &event);
	if (ret) {
		struct iommu_page_response resp = {
			.grpid = prm->grpid,
			.code = IOMMU_PAGE_RESP_FAILURE,
		};
		if (prm->flags & IOMMU_FAULT_PAGE_RESPONSE_NEEDS_PASID) {
			resp.flags |= IOMMU_PAGE_RESP_PASID_VALID;
			resp.pasid = prm->pasid;
		}
		riscv_iommu_ats_prgr(dev, &resp);
	}

	put_device(dev);
}

static int riscv_iommu_page_response(struct riscv_iommu_device *dev,
				     struct iommu_fault_event *evt,
				     struct iommu_page_response *msg)
{
	return riscv_iommu_ats_prgr(dev, msg);
}
#endif

#if 0
/* Page request interface queue primary interrupt handler */
static int riscv_iommu_priq_irq_check(int irq, void *data)
{
	struct riscv_iommu_queue *q = (struct riscv_iommu_queue *)data;
	uint32_t ipsr = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_IPSR);
	if (ipsr & RISCV_IOMMU_IPSR_PIP)
		return IRQ_WAKE_THREAD;
	return IRQ_NONE;
}

/* Page request interface queue interrupt hanlder thread function */
static int riscv_iommu_priq_process(int irq, void *data)
{
	struct riscv_iommu_queue *q = (struct riscv_iommu_queue *)data;
	struct riscv_iommu_pq_record *requests;
	unsigned cnt, len, idx, ctrl;

	requests = (struct riscv_iommu_pq_record *)q->base;

	/* Error reporting, clear error reports if any. */
	ctrl = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_PQCSR);
	if (ctrl & (RISCV_IOMMU_PQCSR_PQMF | RISCV_IOMMU_PQCSR_PQOF)) {
		riscv_iommu_queue_ctrl(&iommu->priq, ctrl);
		con_err(
				     "Page request queue error: fault: %d full: %d\n",
				     !!(ctrl & RISCV_IOMMU_PQCSR_PQMF),
				     !!(ctrl & RISCV_IOMMU_PQCSR_PQOF));
	}

	/* Clear page request interrupt pending. */
	riscv_iommu_writel(iommu, RISCV_IOMMU_REG_IPSR, RISCV_IOMMU_IPSR_PIP);

	/* Process page requests. */
	do {
		cnt = riscv_iommu_queue_consume(q, &idx);
		if (!cnt)
			break;
		for (len = 0; len < cnt; idx++, len++)
			riscv_iommu_page_request(&requests[idx]);
		riscv_iommu_queue_release(q, cnt);
	} while (1);

	return IRQ_HANDLED;
}
#endif
/*
 * Endpoint management
 */

static int riscv_iommu_enable_ir(struct riscv_iommu_endpoint *ep)
{
	struct iommu_resv_region *entry;
	struct riscv_iommu_device *iommu = ep->iommu;
	// struct irq_domain *msi_domain;
	uint64_t val;
	int i;

	/* Initialize MSI remapping */
	if (!ep->dc || !(iommu->cap & RISCV_IOMMU_CAP_MSI_FLAT))
		return 0;

	ep->msi_root = (struct riscv_iommu_msi_pte *)page_alloc_zeroed();
	if (!ep->msi_root)
		return -ENOMEM;

	for (i = 0; i < 256; i++) {
		ep->msi_root[i].pte = RISCV_IOMMU_MSI_PTE_V |
		    FIELD_PREP(RISCV_IOMMU_MSI_PTE_M, 3) |
		    phys_to_ppn(RISCV_IMSIC_BASE + i * PAGE_SIZE);
	}

	entry = (struct iommu_resv_region *)page_alloc_pages(PAGE_SIZE * 256);
	if (entry)
		list_add_tail(&entry->list, &ep->regions);

	val = virt_to_pfn(ep->msi_root) |
	    FIELD_PREP(RISCV_IOMMU_DC_MSIPTP_MODE, RISCV_IOMMU_DC_MSIPTP_MODE_FLAT);
	ep->dc->msiptp = cpu_to_le64(val);

	/* Single page of MSIPTP, 256 IMSIC files */
	ep->dc->msi_addr_mask = cpu_to_le64(255);
	ep->dc->msi_addr_pattern = cpu_to_le64(RISCV_IMSIC_BASE >> 12);
	wmb();

	/* set msi domain for the device as isolated. hack. */
#if 0
	msi_domain = dev_get_msi_domain(ep->dev);
	if (msi_domain) {
		msi_domain->flags |= IRQ_DOMAIN_FLAG_ISOLATED_MSI;
	}
#endif
	con_err("ep:RV-IR enabled\n");

	ep->ir_enabled = true;

	return 0;
}

static void riscv_iommu_disable_ir(struct riscv_iommu_endpoint *ep)
{
	if (!ep->ir_enabled)
		return;

	ep->dc->msi_addr_pattern = 0ULL;
	ep->dc->msi_addr_mask = 0ULL;
	ep->dc->msiptp = 0ULL;
	wmb();

	con_err("ep:RV-IR disabled\n");

	page_free_pages((struct page *)ep->msi_root, 1);
	ep->msi_root = NULL;
	ep->ir_enabled = false;
}

#if 0
/* Endpoint features/capabilities */
static void riscv_iommu_disable_ep(struct riscv_iommu_endpoint *ep)
{
	struct pci_dev *pdev;

	if (!dev_is_pci(ep->dev))
		return;

	pdev = to_pci_dev(ep->dev);

	if (ep->pasid_enabled) {
		pci_disable_ats(pdev);
		pci_disable_pri(pdev);
		pci_disable_pasid(pdev);
		ep->pasid_enabled = false;
	}
}
#endif
static void riscv_iommu_enable_ep(struct riscv_iommu_endpoint *ep)
{
#if 1
	ep->pc = (struct riscv_iommu_pc *)page_alloc_zeroed();
	if (!ep->pc) {
		return;
	}
	riscv_iommu_save_pdt((uint64_t)ep->pc);
	ep->pasid_enabled = true;
#else
	int rc, feat, num;
	struct pci_dev *pdev;
	struct riscv_iommu_device *dev = ep->dev;

	if (!dev_is_pci(dev))
		return;

	if (!iommu->max_pasids)
		return;

	pdev = to_pci_dev(dev);

	if (!pci_ats_supported(pdev))
		return;

	if (!pci_pri_supported(pdev))
		return;

	feat = pci_pasid_features(pdev);
	if (feat < 0)
		return;

	num = pci_max_pasids(pdev);
	if (!num) {
		con_log("Can't enable PASID (num: %d)\n", num);
		return;
	}

	if (num > iommu->max_pasids)
		num = iommu->max_pasids;

	rc = pci_enable_pasid(pdev, feat);
	if (rc) {
		con_log("Can't enable PASID (rc: %d)\n", rc);
		return;
	}

	rc = pci_reset_pri(pdev);
	if (rc) {
		con_log("Can't reset PRI (rc: %d)\n", rc);
		pci_disable_pasid(pdev);
		return;
	}

	/* TODO: Get supported PRI queue length, hard-code to 32 entries */
	rc = pci_enable_pri(pdev, 32);
	if (rc) {
		con_log("Can't enable PRI (rc: %d)\n", rc);
		pci_disable_pasid(pdev);
		return;
	}

	rc = pci_enable_ats(pdev, PAGE_SHIFT);
	if (rc) {
		con_log("Can't enable ATS (rc: %d)\n", rc);
		pci_disable_pri(pdev);
		pci_disable_pasid(pdev);
		return;
	}

	ep->pc = (struct riscv_iommu_pc *)page_alloc_zeroed();
	if (!ep->pc) {
		pci_disable_ats(pdev);
		pci_disable_pri(pdev);
		pci_disable_pasid(pdev);
		return;
	}

	ep->pasid_enabled = true;
	ep->pasid_feat = feat;
	ep->pasid_bits = ilog2(num);

	con_err("ep:PASID/ATS support enabled, %d bits\n", ep->pasid_bits);
#endif
}
#if 0
static int riscv_iommu_enable_sva(struct riscv_iommu_device *dev)
{
	int ret;
	struct riscv_iommu_endpoint *ep = dev_iommu_priv_get(dev);

	if (!ep || !ep->iommu || !ep->iommu->pq_work)
		return -EINVAL;

	if (!ep->pasid_enabled)
		return -ENODEV;

	ret = iopf_queue_add_device(ep->iommu->pq_work, dev);
	if (ret)
		return ret;

	return iommu_register_device_fault_handler(dev, iommu_queue_iopf, dev);
}

static int riscv_iommu_disable_sva(struct riscv_iommu_device *dev)
{
	int ret;
	struct riscv_iommu_endpoint *ep = dev_iommu_priv_get(dev);

	ret = iommu_unregister_device_fault_handler(dev);
	if (!ret)
		ret = iopf_queue_remove_device(ep->iommu->pq_work, dev);

	return ret;
}

static int riscv_iommu_enable_iopf(struct riscv_iommu_device *dev)
{
	struct riscv_iommu_endpoint *ep = dev_iommu_priv_get(dev);

	if (ep && ep->pasid_enabled)
		return 0;

	return -EINVAL;
}

static int riscv_iommu_dev_enable_feat(struct riscv_iommu_device *dev, enum iommu_dev_features feat)
{
	switch (feat) {
	case IOMMU_DEV_FEAT_IOPF:
		return riscv_iommu_enable_iopf(dev);

	case IOMMU_DEV_FEAT_SVA:
		return riscv_iommu_enable_sva(dev);

	default:
		return -ENODEV;
	}
}

static int riscv_iommu_dev_disable_feat(struct riscv_iommu_device *dev, enum iommu_dev_features feat)
{
	switch (feat) {
	case IOMMU_DEV_FEAT_IOPF:
		return 0;

	case IOMMU_DEV_FEAT_SVA:
		return riscv_iommu_disable_sva(dev);

	default:
		return -ENODEV;
	}
}

static int riscv_iommu_of_xlate(struct riscv_iommu_device *dev, struct of_phandle_args *args)
{
	return iommu_fwspec_add_ids(dev, args->args, 1);
}
#endif

static bool riscv_iommu_capable(struct riscv_iommu_device *dev, enum iommu_cap cap)
{
	switch (cap) {
	case IOMMU_CAP_CACHE_COHERENCY:
	case IOMMU_CAP_PRE_BOOT_PROTECTION:
		return true;

	default:
		break;
	}

	return false;
}

static struct riscv_iommu_pc *riscv_iommu_get_pc(struct riscv_iommu_pc *pc, uint32_t pdi)
{
        int depth = RISCV_IOMMU_DC_FSC_PDTP_MODE_PD17 - 1;
        uint8_t pdi_bits[3] = { 0 };
        uint64_t *pdtp = NULL, pdt;

        pdi_bits[0] = 8;
        pdi_bits[1] = 8 + 9;
        pdi_bits[2] = 8 + 9+ 3;

        pdtp = (uint64_t *)pc;
        for (;depth-- > 0;) {
                const int split = pdi_bits[depth];
                pdtp += (pdi >> split) & 0x1FF;
 retry:
                pdt = READ_ONCE(*pdtp);
                if (pdt & RISCV_IOMMU_PDTE_VALID) {
                        pdtp = (uint64_t *)__va(ppn_to_phys(pdt));
                } else {
                        uint64_t old, new;
                        // uint64_t val;

                        new  = (uint64_t)page_alloc_zeroed();
                        if (!new)
                                return NULL;
                        // val = phys_to_ppn(ptr_to_phys(new)) | RISCV_IOMMU_PDTE_VALID;
                        // old = cmpxchg_relaxed(pdtp, pdt, new);
			old = cmpxchg_relaxed(pdtp, pdt,
						phys_to_ppn(__pa(new)) |
						RISCV_IOMMU_PDTE_VALID);

                        if (old != pdt) {
                                page_free(new);
                                goto retry;
                        }
                        con_log("iommu: NL%dPDT(%d): %016llx=%016llx\n",
                                2-depth, pdi, (unsigned long long)pdtp,
                                (unsigned long long)phys_to_ppn(__pa(new)));
                        pdtp = (uint64_t *)new;
                }
        }

        pdtp += (pdi & 0xff) * (sizeof(struct riscv_iommu_pc));

        return (struct riscv_iommu_pc *)pdtp;
}

/* TODO: implement proper device context management, e.g. teardown flow */

/* Lookup or initialize device directory info structure. */
static struct riscv_iommu_dc *riscv_iommu_get_dc(struct riscv_iommu_device *iommu, uint32_t devid)
{
	const bool base_format = !(iommu->cap & RISCV_IOMMU_CAP_MSI_FLAT);
	uint32_t depth = iommu->ddt_mode - RISCV_IOMMU_DDTP_MODE_1LVL;
	uint8_t ddi_bits[3] = { 0 };
	uint64_t *ddtp = NULL, ddt;

	if (iommu->ddt_mode == RISCV_IOMMU_DDTP_MODE_OFF ||
	    iommu->ddt_mode == RISCV_IOMMU_DDTP_MODE_BARE)
		return NULL;

	/* Make sure the mode is valid */
	if (iommu->ddt_mode > RISCV_IOMMU_DDTP_MODE_MAX)
		return NULL;

	/*
	 * Device id partitioning for base format:
	 * DDI[0]: bits 0 - 6   (1st level) (7 bits)
	 * DDI[1]: bits 7 - 15  (2nd level) (9 bits)
	 * DDI[2]: bits 16 - 23 (3rd level) (8 bits)
	 *
	 * For extended format:
	 * DDI[0]: bits 0 - 5   (1st level) (6 bits)
	 * DDI[1]: bits 6 - 14  (2nd level) (9 bits)
	 * DDI[2]: bits 15 - 23 (3rd level) (9 bits)
	 */
	if (base_format) {
		ddi_bits[0] = 7;
		ddi_bits[1] = 7 + 9;
		ddi_bits[2] = 7 + 9 + 8;
	} else {
		ddi_bits[0] = 6;
		ddi_bits[1] = 6 + 9;
		ddi_bits[2] = 6 + 9 + 9;
	}

	/* Make sure device id is within range */
	if (devid >= (1 << ddi_bits[depth]))
		return NULL;

	/* Get to the level of the non-leaf node that holds the device context */
	for (ddtp = (uint64_t *) iommu->ddtp; depth-- > 0;) {
		const int split = ddi_bits[depth];
		/*
		 * Each non-leaf node is 64bits wide and on each level
		 * nodes are indexed by DDI[depth].
		 */
		ddtp += (devid >> split) & 0x1FF;

 retry:
		/*
		 * Check if this node has been populated and if not
		 * allocate a new level and populate it.
		 */
		ddt = READ_ONCE(*ddtp);
		if (ddt & RISCV_IOMMU_DDTE_VALID) {
			ddtp = (uint64_t *)(__va(ppn_to_phys(ddt)));
		} else {
			uint64_t old, new = page_alloc_zeroed();
			if (!new)
				return NULL;
			riscv_iommu_save_ddt(new);
			old = cmpxchg_relaxed(ddtp, ddt,
						phys_to_ppn(__pa(new)) |
						RISCV_IOMMU_DDTE_VALID);

			if (old != ddt) {
				page_free(new);
				goto retry;
			}

			con_log("iommu: NL%dDDT(%d): %016llx=%016llx\n",
				2-depth, devid, (unsigned long long)ddtp,
				(unsigned long long)phys_to_ppn(__pa(new)));

			ddtp = (uint64_t *) new;
		}
	}

	/*
	 * Grab the node that matches DDI[depth], note that when using base
	 * format the device context is 4 * 64bits, and the extended format
	 * is 8 * 64bits, hence the (3 - base_format) below.
	 */
	ddtp += (devid & ((64 << base_format) - 1)) << (3 - base_format);
	return (struct riscv_iommu_dc *)ddtp;
}
struct riscv_master {
	iommu_dev_t dev;
	iommu_grp_t grp;
	bool valid;
	int count;
	struct riscv_iommu_endpoint ep;
	struct list_head domain_head;
	uint32_t devid[MAX_IOMMU_RIDS];
	uint32_t num_devids;
};

struct riscv_master riscv_masters[4];
#define riscv_master_ctrl		riscv_masters[iommu_mst]
//2. probe device
void riscv_iommu_probe_device(void)
{
	uint32_t devid = iommu_master_ctrl.map;
	struct riscv_iommu_device *iommu = get_iommu_device();
	struct riscv_iommu_endpoint *ep = &riscv_master_ctrl.ep;

	INIT_LIST_HEAD(&riscv_master_ctrl.domain_head);

/* Check the SIDs are in range of the SMMU and our stream table */
	riscv_master_ctrl.ep.dc = riscv_iommu_get_dc(iommu, devid);
	riscv_master_ctrl.ep.iommu = iommu;
	riscv_master_ctrl.ep.devid = devid;

	riscv_iommu_enable_ir(&riscv_master_ctrl.ep);
	riscv_iommu_enable_ep(&riscv_master_ctrl.ep);
	// con_log("iommu: devid[%d] mst[%d], DDT base=0x%p PDT base=0x%p\n", \
		devid, iommu_mst, riscv_master_ctrl.ep.dc, riscv_master_ctrl.ep.pc);

	riscv_master_ctrl.devid[riscv_master_ctrl.num_devids] = devid;
	riscv_master_ctrl.num_devids++;

	INIT_LIST_HEAD(&ep->domain);
	INIT_LIST_HEAD(&ep->regions);
}

static void riscv_iommu_probe_finalize(struct riscv_iommu_device *dev)
{
	// set_dma_ops(dev, NULL);
	// iommu_setup_dma_ops(dev, 0, uint64_t_MAX);
}

static void riscv_iommu_release_device(struct riscv_iommu_device *dev)
{
#if 0
	struct riscv_iommu_endpoint *ep = dev_iommu_priv_get(dev);
	struct riscv_iommu_device *iommu = ep->iommu;

	con_log("device with devid %i released\n", ep->devid);

	list_del(&ep->domain);

	if (ep->dc) {
		// this should be already done by domain detach.
		ep->dc->tc = 0ULL;
		wmb();
		ep->dc->fsc = 0ULL;
		ep->dc->iohgatp = 0ULL;
		wmb();
		riscv_iommu_iodir_inv_devid(ep->devid);
	}

	riscv_iommu_disable_ir(ep);
	riscv_iommu_disable_ep(ep);

	/* Remove endpoint from IOMMU tracking structures */
	rb_erase(&ep->node, &iommu->eps);

	set_dma_ops(dev, NULL);
	dev_iommu_priv_set(dev, NULL);

	kfree(ep);
#endif
}

#if 0
static struct iommu_group *riscv_iommu_device_group(struct riscv_iommu_device *dev)
{
	// if (dev_is_pci(dev))
	// 	return pci_device_group(dev);
	// return generic_device_group(dev);
	return (struct iommu_group *)iommu_alloc_group();
}

static void riscv_iommu_get_resv_regions(struct riscv_iommu_device *dev, struct list_head *head)
{
	struct iommu_resv_region *entry, *new_entry;
	struct riscv_iommu_endpoint *ep = dev_iommu_priv_get(dev);

	list_for_each_entry(entry, &ep->regions, list) {
		new_entry = kmemdup(entry, sizeof(*entry), GFP_KERNEL);
		if (new_entry)
			list_add_tail(&new_entry->list, head);
	}

	iommu_dma_get_resv_regions(dev, head);
}
#endif
/*
 * Domain management
 */

static struct iommu_domain *riscv_iommu_domain_alloc(unsigned type)
{
	struct riscv_iommu_domain *domain;

	if (type != IOMMU_DOMAIN_DMA &&
	    type != IOMMU_DOMAIN_DMA_FQ &&
	    type != IOMMU_DOMAIN_UNMANAGED &&
	    type != IOMMU_DOMAIN_IDENTITY &&
	    type != IOMMU_DOMAIN_BLOCKED &&
	    type != IOMMU_DOMAIN_SVA)
		return NULL;

	domain = (struct riscv_iommu_domain *)heap_alloc(sizeof(*domain));
	if (!domain)
		return NULL;

	INIT_LIST_HEAD(&domain->endpoints);

	// domain->domain.ops = &riscv_iommu_domain_ops;
	domain->mode = RISCV_IOMMU_DC_FSC_MODE_BARE;
	// domain->pscid = ida_alloc_range(&riscv_iommu_pscids, 1,
	// 				RISCV_IOMMU_MAX_PSCID, GFP_KERNEL);

	// printf("domain alloc %u\n", domain->pscid);

	return &domain->domain;
}

/* mark domain as second-stage translation */
#if 0
static int riscv_iommu_enable_nesting(struct iommu_domain *iommu_domain)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;

	if (list_empty(&domain->endpoints))
		domain->g_stage = true;

	return domain->g_stage ? 0 : -EBUSY;
}

static void riscv_iommu_domain_free(struct iommu_domain *iommu_domain)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;

	if (!list_empty(&domain->endpoints)) {
		con_log("IOMMU domain is not empty!\n");
	}

	// if (domain->mn.ops && iommu_domain->mm)
	// 	mmu_notifier_unregister(&domain->mn, iommu_domain->mm);

	// if (domain->pgtbl.cookie)
	// 	free_io_pgtable_ops(&domain->pgtbl.ops);

	if (domain->pgd_root)
		page_free_pages((struct page *)domain->pgd_root, domain->g_stage ? 2 : 0);

	// if ((int)domain->pscid > 0)
	// 	ida_free(&riscv_iommu_pscids, domain->pscid);

	printf("domain free %u\n", domain->pscid);

	heap_free((caddr_t)domain);
}
#endif

static int riscv_iommu_domain_finalize(struct riscv_iommu_domain *domain, struct riscv_iommu_device *iommu)
{
	/* Domain assigned to another iommu */
	if (domain->iommu && domain->iommu != iommu)
		return -EINVAL;
	/* Domain already initialized */
	else if (domain->iommu)
		return 0;

	domain->iommu = iommu;

	if (domain->domain.type == IOMMU_DOMAIN_IDENTITY)
		return 0;

	/* TODO: Fix this for RV32 */
#define SATP_MODE_39	_AC(0x8000000000000000, UL)
	domain->mode = SATP_MODE_39 >> 60;
	domain->pgd_root = (uint64_t *)page_alloc_pages(domain->g_stage ? 4 : 1);

	if (!domain->pgd_root)
		return -ENOMEM;
	riscv_iommu_save_pgtbl((uint64_t)domain->pgd_root);
	memory_set(domain->pgd_root, 0, PAGE_SIZE*(domain->g_stage ? 4 : 1));
	// if (!alloc_io_pgtable_ops(RISCV_IOMMU, &domain->pgtbl.cfg, domain))
	// 	return -ENOMEM;
	// riscv_iommu_alloc_pgtable(&domain->pgtbl.cfg)
	iommu_pgtable_alloc(&domain->pgtbl.cfg);

	con_log("iommu: pgtable base=0x%p\n", domain->pgd_root);

	return 0;
}

static uint64_t riscv_iommu_domain_atp(struct riscv_iommu_domain *domain)
{
	uint64_t atp = FIELD_PREP(RISCV_IOMMU_DC_FSC_MODE, domain->mode);
	if (domain->mode != RISCV_IOMMU_DC_FSC_MODE_BARE)
		atp |= FIELD_PREP(RISCV_IOMMU_DC_FSC_PPN, virt_to_pfn(domain->pgd_root));
	if (domain->g_stage)
		atp |= FIELD_PREP(RISCV_IOMMU_DC_IOHGATP_GSCID, domain->pscid);
	return atp;
}

int riscv_iommu_inv_devid(int ddi)
{
	struct riscv_iommu_device *iommu = get_iommu_device();

	riscv_iommu_iodir_inv_devid(iommu, ddi);
	riscv_iommu_iofence_sync(iommu);

	return 0;
}

int riscv_iommu_inv_pdt(int pid)
{
	struct riscv_iommu_endpoint *ep = &riscv_master_ctrl.ep;
	struct riscv_iommu_device *iommu = get_iommu_device();

	riscv_iommu_iodir_inv_pasid(iommu, ep->devid, pid);
	riscv_iommu_iofence_sync(iommu);

	return 0;
}

int riscv_iommu_inv_vma(void)
{
	struct riscv_iommu_command cmd;
	struct riscv_iommu_device *iommu = get_iommu_device();
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;

	riscv_iommu_cmd_inval_vma(&cmd);
	riscv_iommu_cmd_inval_set_gscid(&cmd, 0);
	riscv_iommu_cmd_inval_set_pscid(&cmd, domain->pscid);
	riscv_iommu_post(iommu, &cmd);
	riscv_iommu_iofence_sync(iommu);

	return 0;
}

int riscv_iommu_attach_dev(void)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;
	struct riscv_iommu_endpoint *ep = &riscv_master_ctrl.ep;
	struct riscv_iommu_dc *dc = ep->dc;
	struct riscv_iommu_device *iommu = get_iommu_device();
	int ret;
	uint64_t val;

	if (!list_empty(&ep->domain)) {
		con_log("endpoint already attached to a domain. dropping\n");
		list_del_init(&ep->domain);
	}

	domain->domain.type = IOMMU_DOMAIN_DMA;
	domain->pasid = 0;
	domain->pscid = ep->devid;
	/* allocate root pages, initialize io-pgtable ops, etc. */
	ret = riscv_iommu_domain_finalize(domain, iommu);
	if (ret < 0) {
		con_err("can not finalize domain: %d\n", ret);
		return ret;
	}

	if (iommu->ddt_mode == RISCV_IOMMU_DDTP_MODE_BARE &&
	    domain->domain.type == IOMMU_DOMAIN_IDENTITY) {
		con_log("domain type %d attached w/ PSCID %u\n",
		    domain->domain.type, domain->pscid);
		return 0;
	}

	if (domain->g_stage) {
		/*
		 * Enable G-Stage translation with initial pass-through mode
		 * for S-Stage. VMM is responsible for more restrictive
		 * guest VA translation scheme configuration.
		 */
		dc->ta = 0;
		dc->fsc = 0ULL; /* RISCV_IOMMU_DC_FSC_MODE_BARE */ ;
		dc->iohgatp = cpu_to_le64(riscv_iommu_domain_atp(domain));
	} else {
		/* S-Stage translation table. G-Stage remains unmodified. */
		if (ep->pasid_enabled) {
			val = FIELD_PREP(RISCV_IOMMU_DC_TA_PSCID, domain->pscid);
			int pid = 0;
			ep->pc[pid].ta = cpu_to_le64(val | RISCV_IOMMU_PC_TA_V);
			ep->pc[pid].fsc = cpu_to_le64(riscv_iommu_domain_atp(domain));
			dc->ta = 0;
			val = FIELD_PREP(RISCV_IOMMU_DC_FSC_MODE, RISCV_IOMMU_DC_FSC_PDTP_MODE_PD8);
			dc->fsc = cpu_to_le64(val | virt_to_pfn(ep->pc));
#if 0
			con_log("iommu: PC(%08x-%016llx):\n"
				"       ta=0x%lx\n"
				"       fsc=0x%lx\n",
				pid,
				(unsigned long long)(ep->pc+pid),
				ep->pc[pid].ta,
				ep->pc[pid].fsc);
#endif
		} else {
			val = FIELD_PREP(RISCV_IOMMU_DC_TA_PSCID, domain->pscid);
			dc->ta = cpu_to_le64(val);
			dc->fsc = cpu_to_le64(riscv_iommu_domain_atp(domain));
		}
	}

	wmb();

	/* Mark device context as valid, synchronise device context cache. */
	val = RISCV_IOMMU_DC_TC_V;

	if (ep->pasid_enabled) {
		val |= RISCV_IOMMU_DC_TC_EN_ATS |
		       RISCV_IOMMU_DC_TC_EN_PRI |
		       RISCV_IOMMU_DC_TC_DPE |
		       RISCV_IOMMU_DC_TC_PDTV;
	}

	if (iommu->cap & RISCV_IOMMU_CAP_AMO) {
		val |= RISCV_IOMMU_DC_TC_GADE |
		       RISCV_IOMMU_DC_TC_SADE;
	}

	dc->tc = cpu_to_le64(val);
	wmb();

	riscv_iommu_iodir_inv_devid(iommu, ep->devid);

	// con_log("domain type %d attached w/ PSCID %u\n",
	//     domain->domain.type, domain->pscid);
#if 0
	con_log("iommu: DC(%08x-%016llx):\n"
		"       tc=0x%lx\n"
		"       iohgatp=0x%lx\n"
		"       ta=0x%lx\n"
		"       fsc=0x%lx\n"
		"       msiptp=0x%lx\n"
		"       msi_addr_mask=0x%lx\n"
		"       msi_addr_pattern=0x%lx\n",
		(unsigned int)ep->devid,
		(unsigned long long)dc,
		dc->tc,
		dc->iohgatp,
		dc->ta,
		dc->fsc,
		dc->msiptp,
		dc->msi_addr_mask,
		dc->msi_addr_pattern);
#endif	
	return 0;
}

int riscv_iommu_set_dev_pasid(uint32_t pasid)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;
	struct riscv_iommu_endpoint *ep = &riscv_master_ctrl.ep;
	struct riscv_iommu_device *iommu = get_iommu_device();
	uint64_t ta, fsc;

	// if (!iommu_domain || !iommu_domain->mm)
	// 	return -EINVAL;
	if (domain->g_stage)
		return -EINVAL;

	/* Driver uses TC.DPE mode, PASID #0 is incorrect. */
	if (pasid == 0)
		return -EINVAL;

	/* Incorrect domain identifier */
	if ((int)domain->pscid < 0)
		return -ENOMEM;

	/* Process Context table should be set for pasid enabled endpoints. */
	if (!ep || !ep->pasid_enabled || !ep->dc || !ep->pc)
		return -ENODEV;

	domain->pasid = pasid;
	// domain->iommu = ep->iommu;
	// domain->mn.ops = &riscv_iommu_mmuops;

	/* TODO: get SXL value for the process, use 32 bit or SATP mode */
	// fsc = virt_to_pfn(iommu_domain->mm->pgd) | satp_mode;
	fsc = cpu_to_le64(riscv_iommu_domain_atp(domain));
	ta = RISCV_IOMMU_PC_TA_V | FIELD_PREP(RISCV_IOMMU_PC_TA_PSCID, domain->pscid);

	fsc = le64_to_cpu(cmpxchg_relaxed(&(ep->pc[pasid].fsc), ep->pc[pasid].fsc, cpu_to_le64(fsc)));
	ta = le64_to_cpu(cmpxchg_relaxed(&(ep->pc[pasid].ta), ep->pc[pasid].ta, cpu_to_le64(ta)));
#if 0
	con_log("iommu: set pasid PC(%08x-%016llx):\n"
		"       ta=0x%lx\n"
		"       fsc=0x%lx\n",
		pasid,
		(unsigned long long)(ep->pc+pasid),
		ep->pc[pasid].ta,
		ep->pc[pasid].fsc);
#endif
	wmb();

	if (ta & RISCV_IOMMU_PC_TA_V) {
		riscv_iommu_iodir_inv_pasid(iommu, ep->devid, pasid);
		riscv_iommu_iofence_sync(iommu);
	}

	return 0;
}
#if 0
static void riscv_iommu_remove_dev_pasid(struct riscv_iommu_device *dev, uint32_t pasid)
{
	struct riscv_iommu_endpoint *ep = dev_iommu_priv_get(dev);
	struct riscv_iommu_command cmd;
	unsigned long payload = riscv_iommu_ats_inval_all_payload(false);
	uint64_t ta;

	/* invalidate TA.V */
	ta = le64_to_cpu(xchg_relaxed(&(ep->pc[pasid].ta), 0));

	wmb();

	con_log("domain removed w/ PSCID %u PASID %u\n",
	    (unsigned)FIELD_GET(RISCV_IOMMU_PC_TA_PSCID, ta), pasid);

	/* 1. invalidate PDT entry */
	riscv_iommu_iodir_inv_pasid(ep->ep->devid, pasid);

	/* 2. invalidate all matching IOATC entries (if PASID was valid) */
	if (ta & RISCV_IOMMU_PC_TA_V) {
		riscv_iommu_cmd_inval_vma(&cmd);
		riscv_iommu_cmd_inval_set_gscid(&cmd, 0);
		riscv_iommu_cmd_inval_set_pscid(&cmd,
		    FIELD_GET(RISCV_IOMMU_PC_TA_PSCID, ta));
		riscv_iommu_post(ep->&cmd);
	}

	/* 3. Wait IOATC flush to happen */
	riscv_iommu_iofence_sync(ep->iommu);

	/* 4. ATS invalidation */
	riscv_iommu_cmd_ats_inval(&cmd);
	riscv_iommu_cmd_ats_set_dseg(&cmd, ep->domid);
	riscv_iommu_cmd_ats_set_rid(&cmd, ep->devid);
	riscv_iommu_cmd_ats_set_pid(&cmd, pasid);
	riscv_iommu_cmd_ats_set_payload(&cmd, payload);
	riscv_iommu_post(ep->&cmd);

	/* 5. Wait DevATC flush to happen */
	riscv_iommu_iofence_sync(ep->iommu);
}

static void riscv_iommu_flush_iotlb_range(struct iommu_domain *iommu_domain,
					  unsigned long *start, unsigned long *end,
					  size_t *pgsize)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;
	struct riscv_iommu_command cmd;
	struct riscv_iommu_endpoint *endpoint;
	unsigned long iova;
	unsigned long payload;

	if (domain->mode == RISCV_IOMMU_DC_FSC_MODE_BARE)
		return;

	/* Domain not attached to an IOMMU! */
	BUG_ON(!domain->iommu);

	if (start && end) {
		payload = riscv_iommu_ats_inval_payload(*start, *end, true);
	} else {
		payload = riscv_iommu_ats_inval_all_payload(true);
	}

	riscv_iommu_cmd_inval_vma(&cmd);
	riscv_iommu_cmd_inval_set_pscid(&cmd, domain->pscid);

	if (start && end && pgsize) {
		/* Cover only the range that is needed */
		for (iova = *start; iova <= *end; iova += *pgsize) {
			riscv_iommu_cmd_inval_set_addr(&cmd, iova);
			riscv_iommu_post(domain->&cmd);
		}
	} else {
		riscv_iommu_post(domain->&cmd);
	}
	riscv_iommu_iofence_sync();

	/* ATS invalidation for every device and for every translation */
	list_for_each_entry(endpoint, &domain->endpoints, domain) {
		if (!endpoint->pasid_enabled)
			continue;

		riscv_iommu_cmd_ats_inval(&cmd);
		riscv_iommu_cmd_ats_set_dseg(&cmd, endpoint->domid);
		riscv_iommu_cmd_ats_set_rid(&cmd, endpoint->devid);
		riscv_iommu_cmd_ats_set_pid(&cmd, domain->pasid);
		riscv_iommu_cmd_ats_set_payload(&cmd, payload);
		riscv_iommu_post(domain->&cmd);
	}
	riscv_iommu_iofence_sync();
}

static void riscv_iommu_flush_iotlb_all(struct iommu_domain *iommu_domain)
{
	riscv_iommu_flush_iotlb_range(iommu_domain, NULL, NULL, NULL);
}

static void riscv_iommu_iotlb_sync(struct iommu_domain *iommu_domain,
				   struct iommu_iotlb_gather *gather)
{
	riscv_iommu_flush_iotlb_range(iommu_domain, &gather->start, &gather->end,
				      &gather->pgsize);
}

static void riscv_iommu_iotlb_sync_map(struct iommu_domain *iommu_domain,
				       unsigned long iova, size_t size)
{
	unsigned long end = iova + size - 1;
	/*
	 * Given we don't know the page size used by this range, we assume the
	 * smallest page size to ensure all possible entries are flushed from
	 * the IOATC.
	 */
	size_t pgsize = PAGE_SIZE;
	riscv_iommu_flush_iotlb_range(iommu_domain, &iova, &end, &pgsize);
}

static int riscv_iommu_map_pages(struct iommu_domain *iommu_domain,
				 unsigned long iova, phys_addr_t phys,
				 size_t pgsize, size_t pgcount, int prot,
				 gfp_t gfp, size_t *mapped)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;

	if (!domain->pgtbl.ops.map_pages)
		return -ENODEV;

	return domain->pgtbl.ops.map_pages(&domain->pgtbl.ops, iova, phys,
					   pgsize, pgcount, prot, gfp, mapped);
}

static size_t riscv_iommu_unmap_pages(struct iommu_domain *iommu_domain,
				      unsigned long iova, size_t pgsize,
				      size_t pgcount, struct iommu_iotlb_gather *gather)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;

	if (!domain->pgtbl.ops.unmap_pages)
		return 0;

	return domain->pgtbl.ops.unmap_pages(&domain->pgtbl.ops, iova, pgsize,
					     pgcount, gather);
}

static phys_addr_t riscv_iommu_iova_to_phys(struct iommu_domain *iommu_domain,
					    dma_addr_t iova)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;

	if (!domain->pgtbl.ops.iova_to_phys)
		return 0;

	return domain->pgtbl.ops.iova_to_phys(&domain->pgtbl.ops, iova);
}

#endif
/*
 * Translation mode setup
 */

static uint64_t riscv_iommu_get_ddtp(struct riscv_iommu_device *iommu)
{
	uint64_t ddtp;
	tick_t end_cycles = RISCV_IOMMU_TIMEOUT + tick_get_counter();

	/* Wait for DDTP.BUSY to be cleared and return latest value */
	do {
		ddtp = riscv_iommu_readq(iommu, RISCV_IOMMU_REG_DDTP);
		if (!(ddtp & RISCV_IOMMU_DDTP_BUSY))
			break;
		cpu_relax();
	} while (tick_get_counter() < end_cycles);

	return ddtp;
}

static void riscv_iommu_ddt_cleanup(struct riscv_iommu_device *iommu)
{
	/* TODO: teardown whole device directory tree. */
	if (iommu->ddtp) {
		// if (iommu->ddtp_in_iomem) {
		// 	iounmap((void *)iommu->ddtp);
		// } else
			page_free_pages((struct page *)iommu->ddtp, 1);
		iommu->ddtp = 0;
	}
}

static int riscv_iommu_enable(struct riscv_iommu_device *iommu, unsigned requested_mode)
{
	uint64_t ddtp = 0;
	// uint64_t ddtp_paddr = 0;
	unsigned mode = requested_mode;
	unsigned mode_readback = 0;

	ddtp = riscv_iommu_get_ddtp(iommu);
	if (ddtp & RISCV_IOMMU_DDTP_BUSY)
		return -EBUSY;

	/* Disallow state transtion from xLVL to xLVL. */
	switch (FIELD_GET(RISCV_IOMMU_DDTP_MODE, ddtp)) {
	case RISCV_IOMMU_DDTP_MODE_BARE:
	case RISCV_IOMMU_DDTP_MODE_OFF:
		break;
	default:
		if ((mode != RISCV_IOMMU_DDTP_MODE_BARE)
		    && (mode != RISCV_IOMMU_DDTP_MODE_OFF))
			return -EINVAL;
		break;
	}

 retry:
	switch (mode) {
	case RISCV_IOMMU_DDTP_MODE_BARE:
	case RISCV_IOMMU_DDTP_MODE_OFF:
		riscv_iommu_ddt_cleanup(iommu);
		ddtp = FIELD_PREP(RISCV_IOMMU_DDTP_MODE, mode);
		break;
	case RISCV_IOMMU_DDTP_MODE_1LVL:
	case RISCV_IOMMU_DDTP_MODE_2LVL:
	case RISCV_IOMMU_DDTP_MODE_3LVL:
		if (!iommu->ddtp) {
			/*
			 * We haven't initialized ddtp yet, since it's WARL make
			 * sure that we don't have a hardwired PPN field there
			 * that points to i/o memory instead.
			 */
			riscv_iommu_writeq(iommu, RISCV_IOMMU_REG_DDTP, 0);
			ddtp = riscv_iommu_get_ddtp(iommu);
			// ddtp_paddr = ppn_to_phys(ddtp);
			// if (ddtp_paddr) {
			// 	con_log("ddtp at 0x%llx\n", ddtp_paddr);
			// 	iommu->ddtp =
			// 	    (unsigned long)ioremap(ddtp_paddr, PAGE_SIZE);
			// 	iommu->ddtp_in_iomem = true;
			// } else {
				iommu->ddtp = page_alloc_zeroed();
				riscv_iommu_save_ddt(iommu->ddtp);
			// }
		}
		if (!iommu->ddtp)
			return -ENOMEM;

		ddtp = FIELD_PREP(RISCV_IOMMU_DDTP_MODE, mode) |
		    phys_to_ppn(__pa(iommu->ddtp));

		break;
	default:
		return -EINVAL;
	}

	riscv_iommu_writeq(iommu, RISCV_IOMMU_REG_DDTP, ddtp);
	ddtp = riscv_iommu_get_ddtp(iommu);
	if (ddtp & RISCV_IOMMU_DDTP_BUSY) {
		con_log("timeout when setting ddtp (ddt mode: %i)\n", mode);
		return -EBUSY;
	}

	mode_readback = FIELD_GET(RISCV_IOMMU_DDTP_MODE, ddtp);
	con_log("mode_readback: %i, mode: %i\n", mode_readback, mode);
	if (mode_readback != mode) {
		/*
		 * Mode field is WARL, an I/O MMU may support a subset of
		 * directory table levels in which case if we tried to set
		 * an unsupported number of levels we'll readback either
		 * a valid xLVL or off/bare. If we got off/bare, try again
		 * with a smaller xLVL.
		 */
		if (mode_readback < RISCV_IOMMU_DDTP_MODE_1LVL &&
		    mode > RISCV_IOMMU_DDTP_MODE_1LVL) {
			mode--;
			goto retry;
		}

		/*
		 * We tried all supported xLVL modes and still got off/bare instead,
		 * an I/O MMU must support at least one supported xLVL mode so something
		 * went very wrong.
		 */
		if (mode_readback < RISCV_IOMMU_DDTP_MODE_1LVL &&
		    mode == RISCV_IOMMU_DDTP_MODE_1LVL)
			goto fail;

		/*
		 * We tried setting off or bare and got something else back, something
		 * went very wrong since off/bare is always legal.
		 */
		if (mode < RISCV_IOMMU_DDTP_MODE_1LVL)
			goto fail;

		/*
		 * We tried setting an xLVL mode but got another xLVL mode that
		 * we don't support (e.g. a custom one).
		 */
		if (mode_readback > RISCV_IOMMU_DDTP_MODE_MAX)
			goto fail;

		/* We tried setting an xLVL mode but got another supported xLVL mode */
		mode = mode_readback;
	}

	if (mode != requested_mode)
		con_log("unsupported DDT mode requested (%i), using %i instead\n",
			 requested_mode, mode);

	iommu->ddt_mode = mode;
	// iommu->ddt_mode = RISCV_IOMMU_DDTP_MODE_3LVL;
static char *ddtp_modes[] = {
	"DDTP_MODE_OFF",
	"DDTP_MODE_BARE",
	"DDTP_MODE_1LVL",
	"DDTP_MODE_2LVL",
	"DDTP_MODE_3LVL",
};
	con_log("ddt_mode: %s ddtp:%lx iommu->ddtp:0x%lx\n", ddtp_modes[iommu->ddt_mode], riscv_iommu_get_ddtp(iommu), iommu->ddtp);
	return 0;

 fail:
	con_err("failed to set DDT mode, tried: %i and got %i\n", mode,
		mode_readback);
	riscv_iommu_ddt_cleanup(iommu);
	return -EINVAL;
}

/*
 * Common I/O MMU driver probe/teardown
 */
#if 0
static const struct iommu_domain_ops riscv_iommu_domain_ops = {
	.free = riscv_iommu_domain_free,
	.attach_dev = riscv_iommu_attach_dev,
	.set_dev_pasid = riscv_iommu_set_dev_pasid,
	.map_pages = riscv_iommu_map_pages,
	.unmap_pages = riscv_iommu_unmap_pages,
	.iova_to_phys = riscv_iommu_iova_to_phys,
	.iotlb_sync = riscv_iommu_iotlb_sync,
	.iotlb_sync_map = riscv_iommu_iotlb_sync_map,
	.flush_iotlb_all = riscv_iommu_flush_iotlb_all,
	.enable_nesting = riscv_iommu_enable_nesting,
};

static const struct iommu_ops riscv_iommu_ops = {
	.owner = THIS_MODULE,
	.pgsize_bitmap = SZ_4K | SZ_2M | SZ_512M,
	.capable = riscv_iommu_capable,
	.domain_alloc = riscv_iommu_domain_alloc,
	.probe_device = riscv_iommu_probe_device,
	.probe_finalize = riscv_iommu_probe_finalize,
	.release_device = riscv_iommu_release_device,
	.remove_dev_pasid = riscv_iommu_remove_dev_pasid,
	.device_group = riscv_iommu_device_group,
	.get_resv_regions = riscv_iommu_get_resv_regions,
	.of_xlate = riscv_iommu_of_xlate,
	.dev_enable_feat = riscv_iommu_dev_enable_feat,
	.dev_disable_feat = riscv_iommu_dev_disable_feat,
	.page_response = riscv_iommu_page_response,
	.default_domain_ops = &riscv_iommu_domain_ops,
};

#endif

void riscv_iommu_remove(void)
{

}

static int riscv_iommu_init(struct riscv_iommu_device *iommu)
{
	__unused uint32_t fctl = 0;
	int ret;

#if 0
	iommu->eps = RB_ROOT;
#endif

	fctl = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_FCTL);
#ifdef CONFIG_CPU_BIG_ENDIAN
	if (!(cap & RISCV_IOMMU_CAP_END)) {
		con_err("IOMMU doesn't support Big Endian\n");
		return -EIO;
	} else if (!(fctl & RISCV_IOMMU_FCTL_BE)) {
		fctl |= FIELD_PREP(RISCV_IOMMU_FCTL_BE, 1);
		riscv_iommu_writel(iommu, RISCV_IOMMU_REG_FCTL, fctl);
	}
#endif

	if (iommu->cap & RISCV_IOMMU_CAP_PD20)
		iommu->max_pasids = 1u << 20;
	else if (iommu->cap & RISCV_IOMMU_CAP_PD17)
		iommu->max_pasids = 1u << 17;
	else if (iommu->cap & RISCV_IOMMU_CAP_PD8)
		iommu->max_pasids = 1u << 8;
	/*
	 * Assign queue lengths from module parameters if not already
	 * set on the device tree.
	 */
	if (!iommu->cmdq_len)
		iommu->cmdq_len = cmdq_length;
	if (!iommu->fltq_len)
		iommu->fltq_len = fltq_length;
	if (!iommu->priq_len)
		iommu->priq_len = priq_length;
	/* Clear any pending interrupt flag. */
	riscv_iommu_writel(iommu, RISCV_IOMMU_REG_IPSR,
			   RISCV_IOMMU_IPSR_CIP |
			   RISCV_IOMMU_IPSR_FIP |
			   RISCV_IOMMU_IPSR_PMIP | RISCV_IOMMU_IPSR_PIP);

	ret = riscv_iommu_queue_init(iommu, RISCV_IOMMU_COMMAND_QUEUE);
	if (ret)
		goto fail;
	ret = riscv_iommu_queue_init(iommu, RISCV_IOMMU_FAULT_QUEUE);
	if (ret)
		goto fail;
	if (!(iommu->cap & RISCV_IOMMU_CAP_ATS))
		goto no_ats;
	/* PRI functionally depends on ATS’s capabilities. */
#if 0
	iommu->pq_work = iopf_queue_alloc(dev_name(dev));
	if (!iommu->pq_work) {
		con_err("failed to allocate iopf queue\n");
		ret = -ENOMEM;
		goto fail;
	}
#endif
	// ret = riscv_iommu_queue_init(RISCV_IOMMU_PAGE_REQUEST_QUEUE);
	// if (ret)
	// 	goto fail;

 no_ats:
	// if (iommu_default_passthrough()) {
	// 	con_log("iommu set to passthrough mode\n");
	// 	ret = riscv_iommu_enable(RISCV_IOMMU_DDTP_MODE_BARE);
	// } else {
		ret = riscv_iommu_enable(iommu, ddt_mode);
	// }

	if (ret) {
		con_err("cannot enable iommu device (%d)\n", ret);
		goto fail;
	}

#if 0
	ret = iommu_device_register(&iommu->&riscv_iommu_ops, dev);
	if (ret) {
		con_err("cannot register iommu interface (%d)\n", ret);
		iommu_device_sysfs_remove(&iommu->iommu);
		goto fail;
	}
#endif
	return 0;
 fail:
	riscv_iommu_enable(iommu, RISCV_IOMMU_DDTP_MODE_OFF);
	// riscv_iommu_queue_free(&iommu->priq);
	// riscv_iommu_queue_free(&iommu->fltq);
	riscv_iommu_queue_free(iommu, &iommu->cmdq);
	// iopf_queue_free(iommu->pq_work);
	return ret;
}

static int riscv_iommu_platform_probe(struct riscv_iommu_device *iommu)
{
	uint32_t fctl = 0;
	int irq = 0;

	iommu->reg = (void *)RV_IOMMU_BASE(0);
	iommu->cap = riscv_iommu_readq(iommu, RISCV_IOMMU_REG_CAP);

	/* The PCI driver only uses MSIs, make sure the IOMMU supports this */
	switch (FIELD_GET(RISCV_IOMMU_CAP_IGS, iommu->cap)) {
	case RISCV_IOMMU_CAP_IGS_MSI:
	case RISCV_IOMMU_CAP_IGS_BOTH:
		con_log("using msi interrupts\n");
		break;
	default:
		con_log("using wire-signaled interrupts\n");
		break;
	}

	/* Parse IRQ assignment */
	iommu->irq_cmdq = 60;
	iommu->irq_fltq = 60;
	iommu->irq_pm = 60;

	if (iommu->cap & RISCV_IOMMU_CAP_ATS) {
		iommu->irq_priq = irq;
	}

	/* Make sure fctl.WSI is set */
	fctl = riscv_iommu_readl(iommu, RISCV_IOMMU_REG_FCTL);
	fctl |= RISCV_IOMMU_FCTL_WSI;
	riscv_iommu_writel(iommu, RISCV_IOMMU_REG_FCTL, fctl);

	// dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));

	riscv_iommu_init(iommu);
	return 0;
}

void riscv_iommu_probe(void)
{
	struct riscv_iommu_device *iommu = get_iommu_device();
	riscv_iommu_platform_probe(iommu);
}
