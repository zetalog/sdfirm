#ifndef __FDT_RISCV_H_INCLUDE__
#define __FDT_RISCV_H_INCLUDE__

#include <target/sbi.h>

#ifdef __CHECKER__
#define FDT_FORCE	__attribute__((force))
#define FDT_BITWISE	__attribute__((bitwise))
#else
#define FDT_FORCE
#define FDT_BITWISE
#endif

typedef uint16_t FDT_BITWISE fdt16_t;
typedef uint32_t FDT_BITWISE fdt32_t;
typedef uint64_t FDT_BITWISE fdt64_t;

#define EXTRACT_BYTE(x, n)	((unsigned long long)((uint8_t *)&x)[n])
#define CPU_TO_FDT16(x) ((EXTRACT_BYTE(x, 0) << 8) | EXTRACT_BYTE(x, 1))
#define CPU_TO_FDT32(x) ((EXTRACT_BYTE(x, 0) << 24) | (EXTRACT_BYTE(x, 1) << 16) | \
			 (EXTRACT_BYTE(x, 2) << 8) | EXTRACT_BYTE(x, 3))
#define CPU_TO_FDT64(x) ((EXTRACT_BYTE(x, 0) << 56) | (EXTRACT_BYTE(x, 1) << 48) | \
			 (EXTRACT_BYTE(x, 2) << 40) | (EXTRACT_BYTE(x, 3) << 32) | \
			 (EXTRACT_BYTE(x, 4) << 24) | (EXTRACT_BYTE(x, 5) << 16) | \
			 (EXTRACT_BYTE(x, 6) << 8) | EXTRACT_BYTE(x, 7))

static inline uint16_t fdt16_to_cpu(fdt16_t x)
{
	return (FDT_FORCE uint16_t)CPU_TO_FDT16(x);
}
static inline fdt16_t cpu_to_fdt16(uint16_t x)
{
	return (FDT_FORCE fdt16_t)CPU_TO_FDT16(x);
}

static inline uint32_t fdt32_to_cpu(fdt32_t x)
{
	return (FDT_FORCE uint32_t)CPU_TO_FDT32(x);
}
static inline fdt32_t cpu_to_fdt32(uint32_t x)
{
	return (FDT_FORCE fdt32_t)CPU_TO_FDT32(x);
}

static inline uint64_t fdt64_to_cpu(fdt64_t x)
{
	return (FDT_FORCE uint64_t)CPU_TO_FDT64(x);
}
static inline fdt64_t cpu_to_fdt64(uint64_t x)
{
	return (FDT_FORCE fdt64_t)CPU_TO_FDT64(x);
}
#undef CPU_TO_FDT64
#undef CPU_TO_FDT32
#undef CPU_TO_FDT16
#undef EXTRACT_BYTE

struct plic_data {
	unsigned long addr;
	unsigned long num_src;
};

struct clint_data {
	/* Public details */
	unsigned long addr;
	uint32_t first_hartid;
	uint32_t hart_count;
	bool has_64bit_mmio;
	/* Private details (initialized and used by CLINT library)*/
	uint32_t *ipi;
	struct clint_data *time_delta_reference;
	unsigned long time_delta_computed;
	uint64_t time_delta;
	uint64_t *time_val;
	uint64_t *time_cmp;
#if 0
	u64 (*time_rd)(volatile u64 *addr);
	void (*time_wr)(u64 value, volatile u64 *addr);
#endif
};

struct fdt_match {
	const char *compatible;
	void *data;
};

struct platform_uart_data {
	unsigned long addr;
	unsigned long freq;
	unsigned long baud;
	unsigned long reg_shift;
	unsigned long reg_io_width;
};

const struct fdt_match *fdt_match_node(void *fdt, int nodeoff,
				       const struct fdt_match *match_table);

int fdt_find_match(void *fdt, int startoff,
		   const struct fdt_match *match_table,
		   const struct fdt_match **out_match);

int fdt_get_node_addr_size(void *fdt, int node, unsigned long *addr,
			   unsigned long *size);

int fdt_parse_hart_id(void *fdt, int cpu_offset, u32 *hartid);

int fdt_parse_max_hart_id(void *fdt, u32 *max_hartid);

int fdt_parse_shakti_uart_node(void *fdt, int nodeoffset,
			       struct platform_uart_data *uart);

int fdt_parse_sifive_uart_node(void *fdt, int nodeoffset,
			       struct platform_uart_data *uart);

int fdt_parse_uart8250_node(void *fdt, int nodeoffset,
			    struct platform_uart_data *uart);

int fdt_parse_uart8250(void *fdt, struct platform_uart_data *uart,
		       const char *compatible);

struct plic_data;

int fdt_parse_plic_node(void *fdt, int nodeoffset, struct plic_data *plic);

int fdt_parse_plic(void *fdt, struct plic_data *plic, const char *compat);

struct clint_data;

int fdt_parse_clint_node(void *fdt, int nodeoffset, bool for_timer,
			 struct clint_data *clint);

int fdt_parse_compat_addr(void *fdt, unsigned long *addr,
			  const char *compatible);

/**
 * Fix up the CPU node in the device tree
 *
 * This routine updates the "status" property of a CPU node in the device tree
 * to "disabled" if that hart is in disabled state in OpenSBI.
 *
 * It is recommended that platform codes call this helper in their final_init()
 *
 * @param fdt: device tree blob
 */
void fdt_cpu_fixup(void *fdt);

/**
 * Fix up the PLIC node in the device tree
 *
 * This routine updates the "interrupt-extended" property of the PLIC node in
 * the device tree to hide the M-mode external interrupt from CPUs.
 *
 * It is recommended that platform codes call this helper in their final_init()
 *
 * @param fdt: device tree blob
 * @param compat: PLIC node compatible string
 */
void fdt_plic_fixup(void *fdt, const char *compat);

/**
 * Fix up the reserved memory node in the device tree
 *
 * This routine inserts a child node of the reserved memory node in the device
 * tree that describes the protected memory region done by OpenSBI via PMP.
 *
 * It is recommended that platform codes call this helper in their final_init()
 *
 * @param fdt: device tree blob
 * @return zero on success and -ve on failure
 */
int fdt_reserved_memory_fixup(void *fdt);

/**
 * Fix up the reserved memory subnodes in the device tree
 *
 * This routine adds the no-map property to the reserved memory subnodes so
 * that the OS does not map those PMP protected memory regions.
 *
 * Platform codes must call this helper in their final_init() after fdt_fixups()
 * if the OS should not map the PMP protected reserved regions.
 *
 * @param fdt: device tree blob
 * @return zero on success and -ve on failure
 */
int fdt_reserved_memory_nomap_fixup(void *fdt);

/**
 * General device tree fix-up
 *
 * This routine do all required device tree fix-ups for a typical platform.
 * It fixes up the PLIC node and the reserved memory node in the device tree
 * by calling the corresponding helper routines to accomplish the task.
 *
 * It is recommended that platform codes call this helper in their final_init()
 *
 * @param fdt: device tree blob
 */
void fdt_fixups(void *fdt);

#endif /* __FDT_RISCV_H_INCLUDE__ */
