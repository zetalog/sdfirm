#ifndef __SBI_H_INCLUDE__
#define __SBI_H_INCLUDE__

#include <target/arch.h>
#include <target/irq.h>
#include <target/jiffies.h>

#define OPENSBI_VERSION_MAJOR 0
#define OPENSBI_VERSION_MINOR 4

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
#define SBI_IPI_EVENT_FENCE_I			0x2
#define SBI_IPI_EVENT_SFENCE_VMA		0x4
#define SBI_IPI_EVENT_SFENCE_VMA_ASID		0x8
#define SBI_IPI_EVENT_HALT			0x10

#define SBI_ECALL_SET_TIMER			0
#define SBI_ECALL_CONSOLE_PUTCHAR		1
#define SBI_ECALL_CONSOLE_GETCHAR		2
#define SBI_ECALL_CLEAR_IPI			3
#define SBI_ECALL_SEND_IPI			4
#define SBI_ECALL_REMOTE_FENCE_I		5
#define SBI_ECALL_REMOTE_SFENCE_VMA		6
#define SBI_ECALL_REMOTE_SFENCE_VMA_ASID	7
#define SBI_ECALL_SHUTDOWN			8

#define SBI_TLB_FLUSH_ALL			((unsigned long)-1)
#define SBI_TLB_FLUSH_MAX_SIZE			(1UL << 30)

#define SBI_TLB_FIFO_NUM_ENTRIES		4

#define EXTRACT_FIELD(val, which)		\
	(((val) & (which)) / ((which) & ~((which)-1)))
#define INSERT_FIELD(val, which, fieldval)	\
	(((val) & ~(which)) | ((fieldval) * ((which) & ~((which)-1))))

#ifndef __ASSEMBLY__
#include <target/atomic.h>
#include <target/spinlock.h>

typedef char			s8;
typedef unsigned char		u8;

typedef short			s16;
typedef unsigned short		u16;

typedef int			s32;
typedef unsigned int		u32;

#if __riscv_xlen == 64
typedef long			s64;
typedef unsigned long		u64;
#define PRILX			"016lx"
#elif __riscv_xlen == 32
typedef long long		s64;
typedef unsigned long long	u64;
#define PRILX			"08lx"
#else
#error "Unexpected __riscv_xlen"
#endif

typedef unsigned long		ulong;
typedef unsigned long		virtual_addr_t;
typedef unsigned long		virtual_size_t;
typedef unsigned long		physical_addr_t;
typedef unsigned long		physical_size_t;

#include <sbi/riscv_unpriv.h>

#define SBI_ECALL(__num, __a0, __a1, __a2)                                    \
	({                                                                    \
		register unsigned long a0 asm("a0") = (unsigned long)(__a0);  \
		register unsigned long a1 asm("a1") = (unsigned long)(__a1);  \
		register unsigned long a2 asm("a2") = (unsigned long)(__a2);  \
		register unsigned long a7 asm("a7") = (unsigned long)(__num); \
		asm volatile("ecall"                                          \
			     : "+r"(a0)                                       \
			     : "r"(a1), "r"(a2), "r"(a7)                      \
			     : "memory");                                     \
		a0;                                                           \
	})

#define SBI_ECALL_0(__num) SBI_ECALL(__num, 0, 0, 0)
#define SBI_ECALL_1(__num, __a0) SBI_ECALL(__num, __a0, 0, 0)
#define SBI_ECALL_2(__num, __a0, __a1) SBI_ECALL(__num, __a0, __a1, 0)

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

struct sbi_ipi_data {
	atomic_t ipi_type;
};

enum sbi_tlb_info_types {
	SBI_TLB_FLUSH_VMA,
	SBI_TLB_FLUSH_VMA_ASID,
	SBI_TLB_FLUSH_VMA_VMID
};

struct sbi_tlb_info {
	unsigned long start;
	unsigned long size;
	unsigned long asid;
	unsigned long type;
};

struct sbi_fifo {
	void *queue;
	spinlock_t qlock;
	u16 entry_size;
	u16 num_entries;
	u16 avail;
	u16 tail;
};

enum sbi_fifo_inplace_update_types {
	SBI_FIFO_SKIP,
	SBI_FIFO_UPDATED,
	SBI_FIFO_RESET,
	SBI_FIFO_UNCHANGED,
};

#define SBI_TLB_INFO_SIZE			sizeof(struct sbi_tlb_info)

#define sbi_ecall_console_putc(c) SBI_ECALL_1(SBI_ECALL_CONSOLE_PUTCHAR, (c));

static inline void sbi_ecall_console_puts(const char *str)
{
	while (str && *str)
		sbi_ecall_console_putc(*str++);
}

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
int sbi_ipi_send_many(struct sbi_scratch *scratch, struct unpriv_trap *uptrap,
		      ulong *pmask, u32 event, void *data);
void sbi_ipi_clear_smode(struct sbi_scratch *scratch);
void sbi_ipi_process(struct sbi_scratch *scratch);
int sbi_ipi_init(struct sbi_scratch *scratch, bool cold_boot);
#else
static inline int sbi_ipi_send_many(struct sbi_scratch *scratch,
				    struct unpriv_trap *uptrap,
				    ulong *pmask, u32 event, void *data)
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

u16 sbi_ecall_version_major(void);
u16 sbi_ecall_version_minor(void);
int sbi_ecall_handler(u32 hartid, ulong mcause, struct pt_regs *regs,
		      struct sbi_scratch *scratch);

int sbi_trap_redirect(struct pt_regs *regs, struct sbi_scratch *scratch,
		      ulong epc, ulong cause, ulong tval);
void sbi_trap_handler(struct pt_regs *regs, struct sbi_scratch *scratch);

int sbi_misaligned_load_handler(u32 hartid, ulong mcause,
				struct pt_regs *regs,
				struct sbi_scratch *scratch);
int sbi_misaligned_store_handler(u32 hartid, ulong mcause,
				 struct pt_regs *regs,
				 struct sbi_scratch *scratch);
int sbi_illegal_insn_handler(u32 hartid, ulong mcause,
			     struct pt_regs *regs,
			     struct sbi_scratch *scratch);

int sbi_emulate_csr_read(int csr_num, u32 hartid, ulong mstatus,
			 struct sbi_scratch *scratch, ulong *csr_val);
int sbi_emulate_csr_write(int csr_num, u32 hartid, ulong mstatus,
			  struct sbi_scratch *scratch, ulong csr_val);

int sbi_fifo_dequeue(struct sbi_fifo *fifo, void *data);
int sbi_fifo_enqueue(struct sbi_fifo *fifo, void *data);
void sbi_fifo_init(struct sbi_fifo *fifo, void *queue_mem, u16 entries,
		   u16 entry_size);
bool sbi_fifo_is_empty(struct sbi_fifo *fifo);
bool sbi_fifo_is_full(struct sbi_fifo *fifo);
int sbi_fifo_inplace_update(struct sbi_fifo *fifo, void *in,
			    int (*fptr)(void *in, void *data));
u16 sbi_fifo_avail(struct sbi_fifo *fifo);

int sbi_tlb_fifo_update(struct sbi_scratch *scratch, u32 event, void *data);
void sbi_tlb_fifo_process(struct sbi_scratch *scratch, u32 event);
int sbi_tlb_fifo_init(struct sbi_scratch *scratch, bool cold_boot);

u64 sbi_timer_value(struct sbi_scratch *scratch);
#ifdef CONFIG_ARCH_HAS_SBI_TIMER
void sbi_timer_event_stop(struct sbi_scratch *scratch);
void sbi_timer_event_start(struct sbi_scratch *scratch, u64 next_event);
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

int sbi_hart_init(struct sbi_scratch *scratch, u32 hartid, bool cold_boot);
void *sbi_hart_get_trap_info(struct sbi_scratch *scratch);
void sbi_hart_set_trap_info(struct sbi_scratch *scratch, void *data);
void sbi_hart_pmp_dump(struct sbi_scratch *scratch);
__noreturn void sbi_hart_switch_mode(unsigned long arg0, unsigned long arg1,
				     unsigned long next_addr,
				     unsigned long next_mode);
void sbi_hart_mark_available(u32 hartid);
ulong sbi_hart_available_mask(void);
void sbi_hart_unmark_available(u32 hartid);
struct sbi_scratch *sbi_hart_id_to_scratch(struct sbi_scratch *scratch,
					   u32 hartid);
void sbi_hart_wait_for_coldboot(struct sbi_scratch *scratch, u32 hartid);
void sbi_hart_wake_coldboot_harts(struct sbi_scratch *scratch, u32 hartid);
u32 sbi_current_hartid(void);
#ifdef CONFIG_SBI_MODIFY_PRIVILEGE
void sbi_modify_privilege(struct pt_regs *regs,
			  struct sbi_scratch *scratch);
#else
#define sbi_modify_privilege(reg, scratch)	do { } while (0)
#endif

int sbi_system_early_init(struct sbi_scratch *scratch, bool cold_boot);
int sbi_system_final_init(struct sbi_scratch *scratch, bool cold_boot);
__noreturn void sbi_system_reboot(struct sbi_scratch *scratch, u32 type);
__noreturn void sbi_system_shutdown(struct sbi_scratch *scratch, u32 type);

__noreturn void sbi_init(void);
#endif /* __ASSEMBLY__ */

#include <driver/sbi.h>
#include <asm/sbi.h>

#endif /* __SBI_H_INCLUDE__ */
