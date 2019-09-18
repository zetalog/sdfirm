#ifndef __PAGE_UTILS_ARM64_H_INCLUDE__
#define __PAGE_UTILS_ARM64_H_INCLUDE__

#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <target/arch.h>
#include <asm/assembler.h>
#include <asm/asm-offsets.h>
#include <asm/sysreg.h>
#include <target/paging.h>

/*
 * Macro to create a table entry to the next page.
 *	tbl:	page table address
 *	virt:	virtual address
 *	shift:	#imm page table shift
 *	ptrs:	#imm pointers per table page
 * Preserves:	virt
 * Corrupts:	tmp1, tmp2
 * Returns:	tbl -> next level table page address
 */
	.macro	create_table_entry, tbl, virt, shift, ptrs, tmp1, tmp2
	lsr	\tmp1, \virt, #\shift
	and	\tmp1, \tmp1, #\ptrs - 1	// table index
	add	\tmp2, \tbl, #PAGE_SIZE
	orr	\tmp2, \tmp2, #PMD_TYPE_TABLE	// address of next table and entry type
	str	\tmp2, [\tbl, \tmp1, lsl #3]
	add	\tbl, \tbl, #PAGE_SIZE		// next level table page
	.endm

/*
 * Macro to populate the PGD (and possibily PUD) for the corresponding
 * block entry in the next level (tbl) for the given virtual address.
 * Preserves:	tbl, next, virt
 * Corrupts:	tmp1, tmp2
 */
	.macro	create_pgd_entry, tbl, virt, tmp1, tmp2
	create_table_entry \tbl, \virt, PGDIR_SHIFT, PTRS_PER_PGD, \tmp1, \tmp2
#if BPGT_PGTABLE_LEVELS > 3
	create_table_entry \tbl, \virt, PUD_SHIFT, PTRS_PER_PUD, \tmp1, \tmp2
#endif
#if BPGT_PGTABLE_LEVELS > 2
	create_table_entry \tbl, \virt, BPGT_TABLE_SHIFT, PTRS_PER_PTE, \tmp1, \tmp2
#endif
	.endm

	.macro	locate_pgd_entry, tbl
	add	\tbl, \tbl, #PAGE_SIZE		// next level table page
#if BPGT_PGTABLE_LEVELS > 3
	add	\tbl, \tbl, #PAGE_SIZE		// next level table page
#endif
#if BPGT_PGTABLE_LEVELS > 2
	add	\tbl, \tbl, #PAGE_SIZE		// next level table page
#endif
	.endm

/*
 * Macro to populate block entries in the page table for the start..end
 * virtual range (inclusive).
 * Preserves:	tbl, flags
 * Corrupts:	phys, start, end, pstate
 */
	.macro	create_block_map, tbl, flags, phys, start, end
	lsr	\phys, \phys, #BPGT_BLOCK_SHIFT
	lsr	\start, \start, #BPGT_BLOCK_SHIFT
	and	\start, \start, #PTRS_PER_PTE - 1		// table index
	orr	\phys, \flags, \phys, lsl #BPGT_BLOCK_SHIFT	// table entry
	lsr	\end, \end, #BPGT_BLOCK_SHIFT
	and	\end, \end, #PTRS_PER_PTE - 1			// table end index
9999:	str	\phys, [\tbl, \start, lsl #3]			// store the entry
	add	\start, \start, #1				// next entry
	add	\phys, \phys, #BPGT_BLOCK_SIZE			// next block
	cmp	\start, \end
	b.ls	9999b
	.endm

#endif
