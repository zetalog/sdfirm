#ifndef __SBI_H_INCLUDE__
#define __SBI_H_INCLUDE__

#include <target/arch.h>
#include <target/irq.h>
#include <target/jiffies.h>
#include <target/console.h>
#include <target/atomic.h>
#include <target/clk.h>
#include <target/list.h>

#define OPENSBI_VERSION_MAJOR 0
#define OPENSBI_VERSION_MINOR 4

#ifdef CONFIG_SBI_V10
#define SBI_ECALL_VERSION_MAJOR		1
#define SBI_ECALL_VERSION_MINOR		0
#define SBI_OPENSBI_IMPID		1
#define SBIECALL_INPUT(align)				\
	. = ALIGN((align));				\
	__sbi_ecall_start = .;				\
	KEEP(*(SORT(.sbi_ecall.rodata)))		\
	__sbi_ecall_end = .;

#define SBIECALL_SECTION(align)				\
	.sbi_ecall.rodata : {				\
		SBIECALL_INPUT(align)			\
	}
#else
#define SBI_ECALL_VERSION_MAJOR		0
#define SBI_ECALL_VERSION_MINOR		1
#define SBIECALL_INPUT(align)
#define SBIECALL_SECTION(align)
#endif

/**
 *  OpenSBI 32-bit version with:
 *  1. upper 16-bits as major number
 *  2. lower 16-bits as minor number
 */
#define OPENSBI_VERSION ((OPENSBI_VERSION_MAJOR << 16) | \
			 (OPENSBI_VERSION_MINOR))

#define ETRAP		-256

/* Maximum size of sbi_scratch and sbi_ipi_data */
#define SBI_SCRATCH_SIZE			(64 * __SIZEOF_POINTER__)

#define SBI_IPI_EVENT_SOFT			0x1
#define SBI_IPI_EVENT_RFENCE			0x2
#define SBI_IPI_EVENT_HALT			0x10

#ifdef CONFIG_SBI_V01
#define SBI_ECALL_SET_TIMER			0x0
#define SBI_ECALL_CONSOLE_PUTCHAR		0x1
#define SBI_ECALL_CONSOLE_GETCHAR		0x2
#define SBI_ECALL_CLEAR_IPI			0x3
#define SBI_ECALL_SEND_IPI			0x4
#define SBI_ECALL_REMOTE_FENCE_I		0x5
#define SBI_ECALL_REMOTE_SFENCE_VMA		0x6
#define SBI_ECALL_REMOTE_SFENCE_VMA_ASID	0x7
#define SBI_ECALL_SHUTDOWN			0x8

/* sdfirm specific SBI calls */
/* Testbench finish */
#define SBI_ECALL_FINISH			0x20
/* Enable/disable trap logs */
#define SBI_ECALL_ENABLE_LOG			0x30
#define SBI_ECALL_DISABLE_LOG			0x31
/* Clock framework */
#define SBI_ECALL_GET_CLK_FREQ			0x40
#define SBI_ECALL_SET_CLK_FREQ			0x41
#define SBI_ECALL_ENABLE_CLK			0x42
#define SBI_ECALL_DISABLE_CLK			0x43
#define SBI_ECALL_CONFIG_PIN_MUX		0x44
#define SBI_ECALL_CONFIG_PIN_PAD		0x45
#endif

#ifdef CONFIG_SBI_V10
#define SBI_EXT_0_1_SET_TIMER			SBI_ECALL_SET_TIMER
#define SBI_EXT_0_1_CONSOLE_PUTCHAR		SBI_ECALL_CONSOLE_PUTCHAR
#define SBI_EXT_0_1_CONSOLE_GETCHAR		SBI_ECALL_CONSOLE_GETCHAR
#define SBI_EXT_0_1_CLEAR_IPI			SBI_ECALL_CLEAR_IPI
#define SBI_EXT_0_1_SEND_IPI			SBI_ECALL_SEND_IPI
#define SBI_EXT_0_1_REMOTE_FENCE_I		SBI_ECALL_REMOTE_FENCE_I
#define SBI_EXT_0_1_REMOTE_SFENCE_VMA		SBI_ECALL_REMOTE_SFENCE_VMA
#define SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID	SBI_ECALL_REMOTE_SFENCE_VMA_ASID
#define SBI_EXT_0_1_SHUTDOWN			SBI_ECALL_SHUTDOWN
#define SBI_EXT_BASE				0x10
#define SBI_EXT_TIME				0x54494D45
#define SBI_EXT_IPI				0x735049
#define SBI_EXT_RFENCE				0x52464E43
#define SBI_EXT_HSM				0x48534D
#define SBI_EXT_SRST				0x53525354
#define SBI_EXT_PMU				0x504D55
#define SBI_EXT_DBCN				0x4442434E
#define SBI_EXT_SUSP				0x53555350
#define SBI_EXT_CPPC				0x43505043

/* SBI function IDs for BASE extension*/
#define SBI_EXT_BASE_GET_SPEC_VERSION		0x0
#define SBI_EXT_BASE_GET_IMP_ID			0x1
#define SBI_EXT_BASE_GET_IMP_VERSION		0x2
#define SBI_EXT_BASE_PROBE_EXT			0x3
#define SBI_EXT_BASE_GET_MVENDORID		0x4
#define SBI_EXT_BASE_GET_MARCHID		0x5
#define SBI_EXT_BASE_GET_MIMPID			0x6

/* SBI function IDs for TIME extension*/
#define SBI_EXT_TIME_SET_TIMER			0x0

/* SBI function IDs for IPI extension*/
#define SBI_EXT_IPI_SEND_IPI			0x0

/* SBI function IDs for RFENCE extension*/
#define SBI_EXT_RFENCE_REMOTE_FENCE_I		0x0
#define SBI_EXT_RFENCE_REMOTE_SFENCE_VMA	0x1
#define SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID	0x2
#define SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID	0x3
#define SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA	0x4
#define SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID	0x5
#define SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA	0x6

/* SBI function IDs for HSM extension */
#define SBI_EXT_HSM_HART_START			0x0
#define SBI_EXT_HSM_HART_STOP			0x1
#define SBI_EXT_HSM_HART_GET_STATUS		0x2
#define SBI_EXT_HSM_HART_SUSPEND		0x3

#define SBI_HSM_STATE_STARTED			0x0
#define SBI_HSM_STATE_STOPPED			0x1
#define SBI_HSM_STATE_START_PENDING		0x2
#define SBI_HSM_STATE_STOP_PENDING		0x3
#define SBI_HSM_STATE_SUSPENDED			0x4
#define SBI_HSM_STATE_SUSPEND_PENDING		0x5
#define SBI_HSM_STATE_RESUME_PENDING		0x6

#define SBI_HSM_SUSP_BASE_MASK			0x7fffffff
#define SBI_HSM_SUSP_NON_RET_BIT		0x80000000
#define SBI_HSM_SUSP_PLAT_BASE			0x10000000

#define SBI_HSM_SUSPEND_RET_DEFAULT		0x00000000
#define SBI_HSM_SUSPEND_RET_PLATFORM		SBI_HSM_SUSP_PLAT_BASE
#define SBI_HSM_SUSPEND_RET_LAST		SBI_HSM_SUSP_BASE_MASK
#define SBI_HSM_SUSPEND_NON_RET_DEFAULT		SBI_HSM_SUSP_NON_RET_BIT
#define SBI_HSM_SUSPEND_NON_RET_PLATFORM	(SBI_HSM_SUSP_NON_RET_BIT | \
						 SBI_HSM_SUSP_PLAT_BASE)
#define SBI_HSM_SUSPEND_NON_RET_LAST		(SBI_HSM_SUSP_NON_RET_BIT | \
						 SBI_HSM_SUSP_BASE_MASK)

/* SBI function IDs for SRST extension */
#define SBI_EXT_SRST_RESET			0x0

#define SBI_SRST_RESET_TYPE_SHUTDOWN		0x0
#define SBI_SRST_RESET_TYPE_COLD_REBOOT	0x1
#define SBI_SRST_RESET_TYPE_WARM_REBOOT	0x2
#define SBI_SRST_RESET_TYPE_LAST	SBI_SRST_RESET_TYPE_WARM_REBOOT

#define SBI_SRST_RESET_REASON_NONE	0x0
#define SBI_SRST_RESET_REASON_SYSFAIL	0x1

/* SBI function IDs for PMU extension */
#define SBI_EXT_PMU_NUM_COUNTERS	0x0
#define SBI_EXT_PMU_COUNTER_GET_INFO	0x1
#define SBI_EXT_PMU_COUNTER_CFG_MATCH	0x2
#define SBI_EXT_PMU_COUNTER_START	0x3
#define SBI_EXT_PMU_COUNTER_STOP	0x4
#define SBI_EXT_PMU_COUNTER_FW_READ	0x5
#define SBI_EXT_PMU_COUNTER_FW_READ_HI	0x6

#ifndef __ASSEMBLY__
/** General pmu event codes specified in SBI PMU extension */
enum sbi_pmu_hw_generic_events_t {
	SBI_PMU_HW_NO_EVENT			= 0,
	SBI_PMU_HW_CPU_CYCLES			= 1,
	SBI_PMU_HW_INSTRUCTIONS			= 2,
	SBI_PMU_HW_CACHE_REFERENCES		= 3,
	SBI_PMU_HW_CACHE_MISSES			= 4,
	SBI_PMU_HW_BRANCH_INSTRUCTIONS		= 5,
	SBI_PMU_HW_BRANCH_MISSES		= 6,
	SBI_PMU_HW_BUS_CYCLES			= 7,
	SBI_PMU_HW_STALLED_CYCLES_FRONTEND	= 8,
	SBI_PMU_HW_STALLED_CYCLES_BACKEND	= 9,
	SBI_PMU_HW_REF_CPU_CYCLES		= 10,

	SBI_PMU_HW_GENERAL_MAX,
};

/**
 * Generalized hardware cache events:
 *
 *       { L1-D, L1-I, LLC, ITLB, DTLB, BPU, NODE } x
 *       { read, write, prefetch } x
 *       { accesses, misses }
 */
enum sbi_pmu_hw_cache_id {
	SBI_PMU_HW_CACHE_L1D		= 0,
	SBI_PMU_HW_CACHE_L1I		= 1,
	SBI_PMU_HW_CACHE_LL		= 2,
	SBI_PMU_HW_CACHE_DTLB		= 3,
	SBI_PMU_HW_CACHE_ITLB		= 4,
	SBI_PMU_HW_CACHE_BPU		= 5,
	SBI_PMU_HW_CACHE_NODE		= 6,

	SBI_PMU_HW_CACHE_MAX,
};

enum sbi_pmu_hw_cache_op_id {
	SBI_PMU_HW_CACHE_OP_READ	= 0,
	SBI_PMU_HW_CACHE_OP_WRITE	= 1,
	SBI_PMU_HW_CACHE_OP_PREFETCH	= 2,

	SBI_PMU_HW_CACHE_OP_MAX,
};

enum sbi_pmu_hw_cache_op_result_id {
	SBI_PMU_HW_CACHE_RESULT_ACCESS	= 0,
	SBI_PMU_HW_CACHE_RESULT_MISS	= 1,

	SBI_PMU_HW_CACHE_RESULT_MAX,
};

/**
 * Special "firmware" events provided by the OpenSBI, even if the hardware
 * does not support performance events. These events are encoded as a raw
 * event type in Linux kernel perf framework.
 */
enum sbi_pmu_fw_event_code_id {
	SBI_PMU_FW_MISALIGNED_LOAD	= 0,
	SBI_PMU_FW_MISALIGNED_STORE	= 1,
	SBI_PMU_FW_ACCESS_LOAD		= 2,
	SBI_PMU_FW_ACCESS_STORE		= 3,
	SBI_PMU_FW_ILLEGAL_INSN		= 4,
	SBI_PMU_FW_SET_TIMER		= 5,
	SBI_PMU_FW_IPI_SENT		= 6,
	SBI_PMU_FW_IPI_RECVD		= 7,
	SBI_PMU_FW_FENCE_I_SENT		= 8,
	SBI_PMU_FW_FENCE_I_RECVD	= 9,
	SBI_PMU_FW_SFENCE_VMA_SENT	= 10,
	SBI_PMU_FW_SFENCE_VMA_RCVD	= 11,
	SBI_PMU_FW_SFENCE_VMA_ASID_SENT	= 12,
	SBI_PMU_FW_SFENCE_VMA_ASID_RCVD	= 13,

	SBI_PMU_FW_HFENCE_GVMA_SENT	= 14,
	SBI_PMU_FW_HFENCE_GVMA_RCVD	= 15,
	SBI_PMU_FW_HFENCE_GVMA_VMID_SENT = 16,
	SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD = 17,

	SBI_PMU_FW_HFENCE_VVMA_SENT	= 18,
	SBI_PMU_FW_HFENCE_VVMA_RCVD	= 19,
	SBI_PMU_FW_HFENCE_VVMA_ASID_SENT = 20,
	SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD = 21,
	SBI_PMU_FW_MAX,
	/*
	 * Event codes 22 to 255 are reserved for future use.
	 * Event codes 256 to 65534 are reserved for SBI implementation
	 * specific custom firmware events.
	 */
	SBI_PMU_FW_RESERVED_MAX = 0xFFFE,
	/*
	 * Event code 0xFFFF is used for platform specific firmware
	 * events where the event data contains any event specific information.
	 */
	SBI_PMU_FW_PLATFORM = 0xFFFF,
};

/** SBI PMU event idx type */
enum sbi_pmu_event_type_id {
	SBI_PMU_EVENT_TYPE_HW				= 0x0,
	SBI_PMU_EVENT_TYPE_HW_CACHE			= 0x1,
	SBI_PMU_EVENT_TYPE_HW_RAW			= 0x2,
	SBI_PMU_EVENT_TYPE_FW				= 0xf,
	SBI_PMU_EVENT_TYPE_MAX,
};

/** SBI PMU counter type */
enum sbi_pmu_ctr_type {
	SBI_PMU_CTR_TYPE_HW = 0,
	SBI_PMU_CTR_TYPE_FW,
};
#endif

/* Helper macros to decode event idx */
#define SBI_PMU_EVENT_IDX_MASK 0xFFFFF
#define SBI_PMU_EVENT_IDX_TYPE_OFFSET 16
#define SBI_PMU_EVENT_IDX_TYPE_MASK (0xF << SBI_PMU_EVENT_IDX_TYPE_OFFSET)
#define SBI_PMU_EVENT_IDX_CODE_MASK 0xFFFF
#define SBI_PMU_EVENT_RAW_IDX 0x20000

#define SBI_PMU_EVENT_IDX_INVALID 0xFFFFFFFF

#define SBI_PMU_EVENT_HW_CACHE_OPS_RESULT	0x1
#define SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK	0x6
#define SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET	1
#define SBI_PMU_EVENT_HW_CACHE_ID_MASK		0xfff8
#define SBI_PMU_EVENT_HW_CACHE_ID_OFFSET	3

/* Flags defined for config matching function */
#define SBI_PMU_CFG_FLAG_SKIP_MATCH	(1 << 0)
#define SBI_PMU_CFG_FLAG_CLEAR_VALUE	(1 << 1)
#define SBI_PMU_CFG_FLAG_AUTO_START	(1 << 2)
#define SBI_PMU_CFG_FLAG_SET_VUINH	(1 << 3)
#define SBI_PMU_CFG_FLAG_SET_VSINH	(1 << 4)
#define SBI_PMU_CFG_FLAG_SET_UINH	(1 << 5)
#define SBI_PMU_CFG_FLAG_SET_SINH	(1 << 6)
#define SBI_PMU_CFG_FLAG_SET_MINH	(1 << 7)

/* Flags defined for counter start function */
#define SBI_PMU_START_FLAG_SET_INIT_VALUE (1 << 0)

/* Flags defined for counter stop function */
#define SBI_PMU_STOP_FLAG_RESET (1 << 0)

/* SBI function IDs for DBCN extension */
#define SBI_EXT_DBCN_CONSOLE_WRITE		0x0
#define SBI_EXT_DBCN_CONSOLE_READ		0x1
#define SBI_EXT_DBCN_CONSOLE_WRITE_BYTE		0x2

/* SBI function IDs for SUSP extension */
#define SBI_EXT_SUSP_SUSPEND			0x0

#define SBI_SUSP_SLEEP_TYPE_SUSPEND		0x0
#define SBI_SUSP_SLEEP_TYPE_LAST		SBI_SUSP_SLEEP_TYPE_SUSPEND
#define SBI_SUSP_PLATFORM_SLEEP_START		0x80000000

/* SBI function IDs for CPPC extension */
#define SBI_EXT_CPPC_PROBE			0x0
#define SBI_EXT_CPPC_READ			0x1
#define SBI_EXT_CPPC_READ_HI			0x2
#define SBI_EXT_CPPC_WRITE			0x3

#ifndef __ASSEMBLY__
enum sbi_cppc_reg_id {
	SBI_CPPC_HIGHEST_PERF		= 0x00000000,
	SBI_CPPC_NOMINAL_PERF		= 0x00000001,
	SBI_CPPC_LOW_NON_LINEAR_PERF	= 0x00000002,
	SBI_CPPC_LOWEST_PERF		= 0x00000003,
	SBI_CPPC_GUARANTEED_PERF	= 0x00000004,
	SBI_CPPC_DESIRED_PERF		= 0x00000005,
	SBI_CPPC_MIN_PERF		= 0x00000006,
	SBI_CPPC_MAX_PERF		= 0x00000007,
	SBI_CPPC_PERF_REDUC_TOLERANCE	= 0x00000008,
	SBI_CPPC_TIME_WINDOW		= 0x00000009,
	SBI_CPPC_CTR_WRAP_TIME		= 0x0000000A,
	SBI_CPPC_REFERENCE_CTR		= 0x0000000B,
	SBI_CPPC_DELIVERED_CTR		= 0x0000000C,
	SBI_CPPC_PERF_LIMITED		= 0x0000000D,
	SBI_CPPC_ENABLE			= 0x0000000E,
	SBI_CPPC_AUTO_SEL_ENABLE	= 0x0000000F,
	SBI_CPPC_AUTO_ACT_WINDOW	= 0x00000010,
	SBI_CPPC_ENERGY_PERF_PREFERENCE	= 0x00000011,
	SBI_CPPC_REFERENCE_PERF		= 0x00000012,
	SBI_CPPC_LOWEST_FREQ		= 0x00000013,
	SBI_CPPC_NOMINAL_FREQ		= 0x00000014,
	SBI_CPPC_ACPI_LAST		= SBI_CPPC_NOMINAL_FREQ,
	SBI_CPPC_TRANSITION_LATENCY	= 0x80000000,
	SBI_CPPC_NON_ACPI_LAST		= SBI_CPPC_TRANSITION_LATENCY,
};
#endif

/* SBI base specification related macros */
#define SBI_SPEC_VERSION_MAJOR_OFFSET		24
#define SBI_SPEC_VERSION_MAJOR_MASK		0x7f
#define SBI_SPEC_VERSION_MINOR_MASK		0xffffff
#define SBI_EXT_VENDOR_START			0x09000000
#define SBI_EXT_VENDOR_END			0x09FFFFFF
#define SBI_EXT_FIRMWARE_START			0x0A000000
#define SBI_EXT_FIRMWARE_END			0x0AFFFFFF
#endif /* CONFIG_SBI_V10 */

/* SBI return error codes */
#define SBI_SUCCESS				0
#define SBI_ERR_FAILED				-1
#define SBI_ERR_NOT_SUPPORTED			-2
#define SBI_ERR_INVALID_PARAM			-3
#define SBI_ERR_DENIED				-4
#define SBI_ERR_INVALID_ADDRESS			-5
#define SBI_ERR_ALREADY_AVAILABLE		-6
#define SBI_ERR_ALREADY_STARTED			-7
#define SBI_ERR_ALREADY_STOPPED			-8

#define SBI_LAST_ERR				SBI_ERR_ALREADY_STOPPED

#define SBI_OK			0
#define SBI_EFAIL		SBI_ERR_FAILED
#define SBI_ENOTSUPP		SBI_ERR_NOT_SUPPORTED
#define SBI_EINVAL		SBI_ERR_INVALID_PARAM
#define SBI_EDENIED		SBI_ERR_DENIED
#define SBI_EINVALID_ADDR	SBI_ERR_INVALID_ADDRESS
#define SBI_EALREADY		SBI_ERR_ALREADY_AVAILABLE
#define SBI_EALREADY_STARTED	SBI_ERR_ALREADY_STARTED
#define SBI_EALREADY_STOPPED	SBI_ERR_ALREADY_STOPPED

#define SBI_ENODEV		-1000
#define SBI_ENOSYS		-1001
#define SBI_ETIMEDOUT		-1002
#define SBI_EIO			-1003
#define SBI_EILL		-1004
#define SBI_ENOSPC		-1005
#define SBI_ENOMEM		-1006
#define SBI_ETRAP		-1007
#define SBI_EUNKNOWN		-1008
#define SBI_ENOENT		-1009

#define SBI_TLB_FLUSH_ALL			((unsigned long)-1)
#define SBI_TLB_FLUSH_MAX_SIZE			(1UL << 30)

#define SBI_TLB_FIFO_NUM_ENTRIES		8

#define SBI_HARTMASK_MAX_BITS			128

#define EXTRACT_FIELD(val, which)		\
	(((val) & (which)) / ((which) & ~((which)-1)))
#define INSERT_FIELD(val, which, fieldval)	\
	(((val) & ~(which)) | ((fieldval) * ((which) & ~((which)-1))))

#ifndef __ASSEMBLY__
#include <target/atomic.h>
#include <target/spinlock.h>
#include <target/clk.h>

#if __riscv_xlen == 64
#define PRILX			"016lx"
#elif __riscv_xlen == 32
#define PRILX			"08lx"
#else
#error "Unexpected __riscv_xlen"
#endif

typedef unsigned long		ulong;

#include <sbi/riscv_unpriv.h>

/** Representation of per-HART scratch space */
struct sbi_scratch {
	/** Start (or base) address of firmware linked to OpenSBI library */
	unsigned long fw_start;
	/** Size (in bytes) of firmware linked to OpenSBI library */
	unsigned long fw_size;
	/** Arg1 (or 'a1' register) of next booting stage for this HART */
	unsigned long next_arg1;
	/** Address of next booting stage for this HART */
	unsigned long next_addr;
	/** Priviledge mode of next booting stage for this HART */
	unsigned long next_mode;
	/** Warm boot entry point address for this HART */
	unsigned long warmboot_addr;
	/** Address of sbi_platform */
	unsigned long platform_addr;
	/** Address of HART ID to sbi_scratch conversion function */
	unsigned long hartid_to_scratch;
	/** Temporary storage */
	unsigned long tmp0;
	/** Options for OpenSBI library */
	unsigned long options;
	/** Extra space */
	unsigned long extra_space[0];
} __packed;

/** Possible options for OpenSBI library */
enum sbi_scratch_options {
	/** Disable prints during boot */
	SBI_SCRATCH_NO_BOOT_PRINTS = (1 << 0),
};

enum sbi_tlb_info_types {
	SBI_TLB_FLUSH_VMA,
	SBI_TLB_FLUSH_VMA_ASID,
	SBI_TLB_FLUSH_VMA_VMID,
	SBI_TLB_FLUSH_VVMA,
	SBI_TLB_FLUSH_GVMA,
	SBI_TLB_FLUSH_VVMA_ASID,
	SBI_TLB_FLUSH_GVMA_VMID,
	SBI_ITLB_FLUSH
};

struct sbi_tlb_info {
	unsigned long start;
	unsigned long size;
	unsigned long asid;
	unsigned long type;
	unsigned long vmid;
	void (*local_fn)(struct sbi_tlb_info *tinfo);
	unsigned long shart_mask;
};

struct sbi_fifo {
	void *queue;
	spinlock_t qlock;
	uint16_t entry_size;
	uint16_t num_entries;
	uint16_t avail;
	uint16_t tail;
};

enum sbi_fifo_inplace_update_types {
	SBI_FIFO_SKIP,
	SBI_FIFO_UPDATED,
	SBI_FIFO_UNCHANGED,
};

void sbi_tlb_info_fence_i(struct sbi_tlb_info *tinfo);
void sbi_tlb_info_sfence_vma(struct sbi_tlb_info *tinfo);
void sbi_tlb_info_sfence_vma_asid(struct sbi_tlb_info *tinfo);
void sbi_tlb_info_hfence_vvma(struct sbi_tlb_info *tinfo);
void sbi_tlb_info_hfence_vvma_asid(struct sbi_tlb_info *tinfo);
void sbi_tlb_info_hfence_gvma(struct sbi_tlb_info *tinfo);
void sbi_tlb_info_hfence_gvma_vmid(struct sbi_tlb_info *tinfo);

#define SBI_TLB_INFO_TYPE(__p, __start, __size, __asid, __vmid, __type, __src) \
do { \
	(__p)->start = (__start); \
	(__p)->size = (__size); \
	(__p)->asid = (__asid); \
	(__p)->vmid = (__vmid); \
	(__p)->type = (__type); \
	(__p)->shart_mask = 1UL << (__src); \
} while (0)
#define SBI_TLB_INFO_FUNC(__p, __start, __size, __asid, __vmid, __lfn, __src) \
do { \
	(__p)->start = (__start); \
	(__p)->size = (__size); \
	(__p)->asid = (__asid); \
	(__p)->vmid = (__vmid); \
	(__p)->local_fn = (__lfn); \
	(__p)->shart_mask = 1UL << (__src); \
} while (0)

#define SBI_TLB_INFO_SIZE		sizeof(struct sbi_tlb_info)

/** Possible privileged specification versions of a hart */
enum sbi_hart_priv_versions {
	/** Unknown privileged specification */
	SBI_HART_PRIV_VER_UNKNOWN = 0,
	/** Privileged specification v1.10 */
	SBI_HART_PRIV_VER_1_10 = 1,
	/** Privileged specification v1.11 */
	SBI_HART_PRIV_VER_1_11 = 2,
	/** Privileged specification v1.12 */
	SBI_HART_PRIV_VER_1_12 = 3,
};

/** Possible ISA extensions of a hart */
enum sbi_hart_extensions {
	/** HART has AIA M-mode CSRs */
	SBI_HART_EXT_SMAIA = 0,
	/** HART has Smepmp */
	SBI_HART_EXT_SMEPMP,
	/** HART has Smstateen CSR **/
	SBI_HART_EXT_SMSTATEEN,
	/** Hart has Sscofpmt extension */
	SBI_HART_EXT_SSCOFPMF,
	/** HART has Sstc extension */
	SBI_HART_EXT_SSTC,
	/** HART has Zicntr extension (i.e. HW cycle, time & instret CSRs) */
	SBI_HART_EXT_ZICNTR,
	/** HART has Zihpm extension */
	SBI_HART_EXT_ZIHPM,

	/** Maximum index of Hart extension */
	SBI_HART_EXT_MAX,
};

struct sbi_hart_features {
	bool detected;
	int priv_version;
	unsigned long extensions;
	unsigned int pmp_count;
	unsigned int pmp_addr_bits;
	unsigned long pmp_gran;
	unsigned int mhpm_count;
	unsigned int mhpm_bits;
};

/** Get pointer to sbi_scratch for current HART */
#define sbi_scratch_thishart_ptr() \
	((struct sbi_scratch *)csr_read(CSR_MSCRATCH))
/** Get Arg1 of next booting stage for current HART */
#define sbi_scratch_thishart_arg1_ptr() \
	((void *)(sbi_scratch_thishart_ptr()->next_arg1))
/** Allocate from extra space in sbi_scratch
 *
 * @return zero on failure and non-zero (>= SBI_SCRATCH_EXTRA_SPACE_OFFSET)
 * on success
 */
unsigned long sbi_scratch_alloc_offset(unsigned long size, const char *owner);
/** Free-up extra space in sbi_scratch */
void sbi_scratch_free_offset(unsigned long offset);
/** Get pointer from offset in sbi_scratch */
#define sbi_scratch_offset_ptr(scratch, offset)	((void *)scratch + (offset))
/** Get pointer from offset in sbi_scratch for current HART */
#define sbi_scratch_thishart_offset_ptr(offset)	\
	((void *)sbi_scratch_thishart_ptr() + (offset))

#ifdef CONFIG_ARCH_HAS_SBI_IPI
int sbi_tlb_request(unsigned long hmask, unsigned long hbase,
		     struct sbi_tlb_info *tinfo);
int sbi_ipi_send_many(ulong hmask, ulong hbase, uint32_t event, void *data);
void sbi_ipi_clear_smode(struct sbi_scratch *scratch);
void sbi_ipi_process(struct sbi_scratch *scratch);
int sbi_ipi_init(struct sbi_scratch *scratch, bool cold_boot);
#else
static inline int sbi_tlb_request(unsigned long hmask, unsigned long hbase,
				  struct sbi_tlb_info *tinfo)
{
	return 0;
}
static inline int sbi_ipi_send_many(struct sbi_scratch *scratch,
				    struct unpriv_trap *uptrap,
				    ulong *pmask, uint32_t event, void *data)
{
	return 0;
}
#define sbi_ipi_clear_smode(scratch)		do { } while (0)
#define sbi_ipi_process(scratch)		do { } while (0)
static inline int sbi_ipi_init(struct sbi_scratch *scratch, bool cold_boot)
{
	return 0;
}
#endif

#define DEFINE_SBI_ECALL(name, extid_start, extid_end, reg, probe, handler)	\
	static struct sbi_ecall_extension name					\
	__attribute__((used,__section__(".sbi_ecall.rodata")))			\
	= { LIST_HEAD_INIT(name.head), extid_start, extid_end, reg, probe, handler }

struct sbi_ecall_extension {
	/* head is used by the extension list */
	struct list_head head;
	/*
	 * extid_start and extid_end specify the range for this extension. As
	 * the initial range may be wider than the valid runtime range, the
	 * register_extensions callback is responsible for narrowing the range
	 * before other callbacks may be invoked.
	 */
	unsigned long extid_start;
	unsigned long extid_end;
	/*
	 * register_extensions
	 *
	 * Calls sbi_ecall_register_extension() one or more times to register
	 * extension ID range(s) which should be handled by this extension.
	 * More than one sbi_ecall_extension struct and
	 * sbi_ecall_register_extension() call is necessary when the supported
	 * extension ID ranges have gaps. Additionally, extension availability
	 * must be checked before registering, which means, when this callback
	 * returns, only valid extension IDs from the initial range, which are
	 * also available, have been registered.
	 */
	int (* register_extensions)(void);
	/*
	 * probe
	 *
	 * Implements the Base extension's probe function for the extension. As
	 * the register_extensions callback ensures that no other extension
	 * callbacks will be invoked when the extension is not available, then
	 * probe can never fail. However, an extension may choose to set
	 * out_val to a nonzero value other than one. In those cases, it should
	 * implement this callback.
	 */
	int (* probe)(unsigned long extid, unsigned long *out_val);
	/*
	 * handle
	 *
	 * This is the extension handler. register_extensions ensures it is
	 * never invoked with an invalid or unavailable extension ID.
	 */
	int (* handle)(unsigned long extid, unsigned long funcid,
		       struct pt_regs *regs,
		       unsigned long *out_val,
		       struct csr_trap_info *out_trap);
};

extern struct sbi_ecall_extension __sbi_ecall_start[0];
extern struct sbi_ecall_extension __sbi_ecall_end[0];

uint16_t sbi_ecall_version_major(void);
uint16_t sbi_ecall_version_minor(void);
int sbi_ecall_handler(struct pt_regs *regs);
struct sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid);
int sbi_ecall_register_extension(struct sbi_ecall_extension *ext);
void sbi_ecall_unregister_extension(struct sbi_ecall_extension *ext);
unsigned long sbi_ecall_get_impid(void);
void sbi_ecall_set_impid(unsigned long impid);

int sbi_trap_redirect(struct pt_regs *regs, struct sbi_scratch *scratch,
		      ulong epc, ulong cause, ulong tval);
void sbi_trap_handler(struct pt_regs *regs, struct sbi_scratch *scratch);
bool sbi_trap_log_enabled(void);
void sbi_trap_log(const char *fmt, ...);

int sbi_misaligned_load_handler(uint32_t hartid, ulong mcause,
				struct pt_regs *regs,
				struct sbi_scratch *scratch);
int sbi_misaligned_store_handler(uint32_t hartid, ulong mcause,
				 struct pt_regs *regs,
				 struct sbi_scratch *scratch);
int sbi_illegal_insn_handler(uint32_t hartid, ulong mcause,
			     struct pt_regs *regs,
			     struct sbi_scratch *scratch);

int sbi_emulate_csr_read(int csr_num, struct pt_regs *regs,
			 struct sbi_scratch *scratch, ulong *csr_val);
int sbi_emulate_csr_write(int csr_num, struct pt_regs *regs,
			  struct sbi_scratch *scratch, ulong csr_val);

int sbi_fifo_dequeue(struct sbi_fifo *fifo, void *data);
int sbi_fifo_enqueue(struct sbi_fifo *fifo, void *data);
void sbi_fifo_init(struct sbi_fifo *fifo, void *queue_mem,
		   uint16_t entries, uint16_t entry_size);
bool sbi_fifo_is_empty(struct sbi_fifo *fifo);
bool sbi_fifo_is_full(struct sbi_fifo *fifo);
int sbi_fifo_inplace_update(struct sbi_fifo *fifo, void *in,
			    int (*fptr)(void *in, void *data));
uint16_t sbi_fifo_avail(struct sbi_fifo *fifo);

int sbi_tlb_fifo_update(struct sbi_scratch *scratch,
			uint32_t hartid, void *data);
void sbi_tlb_fifo_process(struct sbi_scratch *scratch);
int sbi_tlb_fifo_init(struct sbi_scratch *scratch, bool cold_boot);
void sbi_tlb_fifo_sync(struct sbi_scratch *scratch);

uint64_t sbi_timer_value(struct sbi_scratch *scratch);
#ifdef CONFIG_ARCH_HAS_SBI_TIMER
void sbi_timer_event_stop(struct sbi_scratch *scratch);
void sbi_timer_event_start(struct sbi_scratch *scratch, uint64_t next_event);
void sbi_timer_process(struct sbi_scratch *scratch);
int sbi_timer_init(struct sbi_scratch *scratch, bool cold_boot);
#else
#define sbi_timer_event_stop(scratch)			do { } while (0)
#define sbi_timer_event_start(scratch, next_event)	do { } while (0)
#define sbi_timer_process(scratch)			do { } while (0)
static inline int sbi_timer_init(struct sbi_scratch *scratch, bool cold_boot)
{
	return 0;
}
#endif

int sbi_hart_init(struct sbi_scratch *scratch,
		  uint32_t hartid, bool cold_boot);
void *sbi_hart_get_trap_info(struct sbi_scratch *scratch);
void sbi_hart_set_trap_info(struct sbi_scratch *scratch, void *data);
void sbi_hart_pmp_dump(struct sbi_scratch *scratch);
__noreturn void sbi_hart_switch_mode(unsigned long arg0, unsigned long arg1,
				     unsigned long next_addr,
				     unsigned long next_mode);
void sbi_hart_mark_available(uint32_t hartid);
ulong sbi_hart_available_mask(void);
void sbi_hart_unmark_available(uint32_t hartid);
struct sbi_scratch *sbi_hart_id_to_scratch(struct sbi_scratch *scratch,
					   uint32_t hartid);
uint32_t sbi_current_hartid(void);
int sbi_hart_priv_version(struct sbi_scratch *scratch);
void sbi_hart_get_priv_version_str(struct sbi_scratch *scratch,
				   char *version_str, int nvstr);
bool sbi_hart_has_extension(struct sbi_scratch *scratch,
			    enum sbi_hart_extensions ext);

#ifdef CONFIG_CONSOLE
int sbi_console_init(struct sbi_scratch *scratch);
#else
static inline int sbi_console_init(struct sbi_scratch *scratch)
{
	return 0;
}
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
int sbi_vprintf(const char *fmt, va_list arg);
int sbi_printf(const char *fmt, ...);
void sbi_putc(char ch);
#else
#define sbi_vprintf(fmt, arg)		do { } while (0)
#define sbi_printf(fmt, ...)		do { } while (0)
#define sbi_putc(ch)			do { } while (0)
#endif
#ifdef CONFIG_CONSOLE_INPUT
int sbi_getc(void);
#else
static inline int sbi_getc(void)
{
	return -1;
}
#endif

#ifdef CONFIG_ARCH_HAS_SBI_CLOCK
unsigned long sbi_clock_get_freq(unsigned long clkid);
void sbi_clock_set_freq(unsigned long clkid, unsigned long freq);
void sbi_clock_enable(unsigned long clkid);
void sbi_clock_disable(unsigned long clkid);
#else
static inline unsigned long sbi_clock_get_freq(unsigned long clkid)
{
	return INVALID_FREQ;
}
#define sbi_clock_set_freq(clkid, freq)		do { } while (0)
#define sbi_clock_enable(clkid)			do { } while (0)
#define sbi_clock_disable(clkid)		do { } while (0)
#endif
#ifdef CONFIG_ARCH_HAS_SBI_PINCTRL
void sbi_pin_config_mux(unsigned long pinid, unsigned long mux);
void sbi_pin_config_pad(unsigned long pinid, unsigned long cfg);
#else
#define sbi_pin_config_mux(pinid, mux)		do { } while (0)
#define sbi_pin_config_pad(pinid, cfg)		do { } while (0)
#endif

int sbi_system_early_init(struct sbi_scratch *scratch, bool cold_boot);
int sbi_system_final_init(struct sbi_scratch *scratch, bool cold_boot);
__noreturn void sbi_system_reboot(struct sbi_scratch *scratch, uint32_t type);
__noreturn void sbi_system_shutdown(struct sbi_scratch *scratch, uint32_t type);
void __noreturn sbi_system_finish(struct sbi_scratch *scratch, uint32_t code);
void __noreturn sbi_finish_hang(void);

__noreturn void sbi_init(void);
#endif /* __ASSEMBLY__ */

#include <target/sbi_fw.h>
#include <driver/sbi.h>
#include <asm/sbi.h>

#endif /* __SBI_H_INCLUDE__ */
