#ifndef __SBI_H_INCLUDE__
#define __SBI_H_INCLUDE__

#include <target/arch.h>
#include <target/irq.h>
#include <target/jiffies.h>
#include <target/console.h>
#include <target/atomic.h>
#include <target/clk.h>

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
	SBI_ITLB_FLUSH
};

struct sbi_tlb_info {
	unsigned long start;
	unsigned long size;
	unsigned long asid;
	unsigned long type;
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

#define SBI_TLB_INFO_SIZE			sizeof(struct sbi_tlb_info)

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
int sbi_ipi_send_many(struct sbi_scratch *scratch, struct unpriv_trap *uptrap,
		      ulong *pmask, uint32_t event, void *data);
void sbi_ipi_clear_smode(struct sbi_scratch *scratch);
void sbi_ipi_process(struct sbi_scratch *scratch);
int sbi_ipi_init(struct sbi_scratch *scratch, bool cold_boot);
#else
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

uint16_t sbi_ecall_version_major(void);
uint16_t sbi_ecall_version_minor(void);
int sbi_ecall_handler(uint32_t hartid, ulong mcause, struct pt_regs *regs,
		      struct sbi_scratch *scratch);

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
