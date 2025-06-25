#ifndef __FDT_RISCV_H_INCLUDE__
#define __FDT_RISCV_H_INCLUDE__

#include <target/generic.h>
#include <target/sbi.h>
#ifdef CONFIG_RISCV_SMAIA
#include <asm/imsic.h>
#include <asm/aplic.h>
#endif

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
};

struct fdt_match {
	const char *compatible;
	void *data;
};

#define FDT_MAX_PHANDLE_ARGS 16
struct fdt_phandle_args {
	int node_offset;
	int args_count;
	u32 args[FDT_MAX_PHANDLE_ARGS];
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
int fdt_get_node_addr_size(const void *fdt, int node, uint64_t *addr,
			   uint64_t *size);
int fdt_parse_hart_id(void *fdt, int cpu_offset, uint32_t *hartid);
int fdt_parse_max_hart_id(void *fdt, uint32_t *max_hartid);
int fdt_parse_shakti_uart_node(void *fdt, int nodeoffset,
			       struct platform_uart_data *uart);
int fdt_parse_sifive_uart_node(void *fdt, int nodeoffset,
			       struct platform_uart_data *uart);
int fdt_parse_uart8250_node(void *fdt, int nodeoffset,
			    struct platform_uart_data *uart);
int fdt_parse_uart8250(void *fdt, struct platform_uart_data *uart,
		       const char *compatible);
int fdt_parse_plic_node(void *fdt, int nodeoffset, struct plic_data *plic);
int fdt_parse_plic(void *fdt, struct plic_data *plic, const char *compat);
int fdt_parse_clint_node(void *fdt, int nodeoffset, bool for_timer,
			 struct clint_data *clint);
int fdt_parse_compat_addr(void *fdt, uint64_t *addr,
			  const char *compatible);
bool fdt_node_is_enabled(const void *fdt, int nodeoff);
int fdt_parse_phandle_with_args(const void *fdt, int nodeoff,
				const char *prop, const char *cells_prop,
				int index, struct fdt_phandle_args *out_args);
#ifdef CONFIG_RISCV_SMAIA
int fdt_parse_aplic_node(void *fdt, int nodeoff, struct aplic_data *aplic);
int fdt_parse_imsic_node(void *fdt, int nodeoff, struct imsic_data *imsic);
#endif

#ifdef CONFIG_FDT_PMU
int fdt_pmu_setup(void *fdt);
uint64_t fdt_pmu_get_select_value(uint32_t event_idx);
int fdt_pmu_fixup(void *fdt);
#else
#define fdt_pmu_setup(fdt)			0
#define fdt_pmu_get_select_value(event_idx)	0
#define fdt_pmu_fixup(fdt)			do { } while (0)
#endif

static inline void *fdt_get_address(void)
{
	return sbi_scratch_thishart_arg1_ptr();
}

#ifdef CONFIG_SBI_FDT
void fdt_cpu_fixup(void *fdt);
void fdt_irqs_fixup(void *fdt, const char *compat, int num);
void fdt_irq_fixup(void *fdt, const char *compat);
int fdt_reserved_memory_fixup(void *fdt);
int fdt_reserved_memory_nomap_fixup(void *fdt);
void fdt_fixups(void *fdt);
#else
#define fdt_cpu_fixup(fdt)			do { } while (0)
#define fdt_irqs_fixup(fdt, compat, num)	do { } while (0)
#define fdt_irq_fixup(fdt, compat)		do { } while (0)
#define fdt_reserved_memory_fixup(fdt)		do { } while (0)
#define fdt_reserved_memory_nomap_fixup(fdt)	do { } while (0)
#define fdt_fixups(fdt)				do { } while (0)
#endif

#ifdef CONFIG_UEFI_DXE
void fdt_efi_fixup(void *fdt);
#else
#define fdt_efi_fixup(fdt)			do { } while (0)
#endif

#endif /* __FDT_RISCV_H_INCLUDE__ */
