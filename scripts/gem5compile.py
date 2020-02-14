#!/usr/bin/python3

import sys
import os
from enum import Enum, auto

class BlockType(Enum):
	ASM = auto()
	MEM = auto()
	OTHERS = auto()


class ItemType(Enum):
	ASM_INST = auto()
	ASM_COMMENT = auto()
	ASM_LABEL = auto()
	ASM_OTHERS = auto()
	MEM_ADDR_VAL = auto()
	MEM_OTHERS = auto()


class ArchType(Enum):
	RISCV = auto()
	ARM64 = auto()


class PageInfo:

	def __init__(self, base): 
		self.base = base
		self.offset_val = {}


class Gem5Compile:
	'''Compile a Gem5 slicing result into source codes for sdfirm'''

	PAGE_SIZE_BITS = 12
	PAGE_SIZE = 1 << PAGE_SIZE_BITS
	ADDR_FULL_MASK = 0xFFFFFFFFFFFFFFFF
	PAGE_BASE_MASK = (ADDR_FULL_MASK << PAGE_SIZE_BITS) & ADDR_FULL_MASK
	PAGE_OFFSET_MASK = PAGE_BASE_MASK ^ ADDR_FULL_MASK 

	def __init__(self, arch, slice_filename): 
		self.arch = arch
		self.filename_slice = slice_filename
		self.filename_asm = slice_filename + "_func.S"
		self.filename_c = slice_filename + "_heap.c"
		self.mem_pages = []
		self.mem_info_cnt = 0
		print("Compile Gem5 slice", slice_filename)
		return

	def prepare(self):
		print("Status: Prepare input and output files...")
		try:
			self.fd_slice = open(self.filename_slice, 'r')
		except IOError:
			print("Failed to open slice file:", self.filename_slice)
			sys.exit(1)
		try:
			self.fd_asm = open(self.filename_asm, 'w')
		except IOError:
			print("Failed to open output file:", self.filename_asm)
			sys.exit(1)
		try:
			self.fd_c = open(self.filename_c, 'w')
		except IOError:
			print("Failed to open output file:", self.filename_c)
			sys.exit(1)


		return

	def oneline_asm(self, slice_line):
		''' Replace and output '''
		line_words = slice_line.split()
		word_cnt = len(line_words)
		if (word_cnt <= 0):
			item_type = ItemType.ASM_OTHERS
		elif (".section" in line_words[0] or \
			  ".balign" in line_words[0] or \
			  ".global" in line_words[0]):
			item_type = ItemType.ASM_OTHERS
		elif (line_words[0] == "/*" or \
			  line_words[0] == "*" or \
			  line_words[0] == "*/" or \
			  line_words[0] == "#" or \
			  line_words[0] == "//"):
			item_type = ItemType.ASM_COMMENT
		elif (":" in  line_words[0]):
			item_type = ItemType.ASM_LABEL
		else:
			item_type = ItemType.ASM_INST

		if (item_type == ItemType.ASM_INST):
			line_words[0] = line_words[0].replace('_', '.')
			out_line = " ".join(line_words)
			out_line = "    " + out_line + "\n"
		else:
			out_line = slice_line

		self.fd_asm.write(out_line)
		return

	def oneline_mem(self, slice_line):
		''' Record only '''
		line_words = slice_line.split()
		word_cnt = len(line_words)
		if (word_cnt != 2):
			print("Warn: Invalid MEM line \"", slice_line.replace("\n", ""), "\"")
			return
		addr = int(line_words[0], 16)
		val = int(line_words[1], 16)
		page_base = addr & self.PAGE_BASE_MASK
		offset = addr & self.PAGE_OFFSET_MASK
		#print("Debug: new MEM line ", hex(page_base), hex(offset), hex(val))

		if (len(self.mem_pages) <= 0 or self.mem_pages[-1].base != page_base):
			print("Status: Start new page, base = ", hex(page_base))
			new_page = PageInfo(page_base)
			new_page.offset_val[offset] = val
			self.mem_pages.append(new_page)
		else:
			self.mem_pages[-1].offset_val[offset] = val
		return

	def output_page_data(self):
		page_cnt = len(self.mem_pages)
		print("Status: Output memory page data. Page count =", page_cnt)

		# Header of C file
		out_str = \
"""
/*
 * Private data for Simpoint
 */
#include <stdint.h>
/* Counter used in simpoint_chk2exit */
uint64_t simpoint_exit_cnt = 1;

"""
		self.fd_c.write(out_str)

		# Memory information as array in C file
		out_str = \
"""
#ifndef CONFIG_GEM5_STATIC_PAGES
/*
 * Storage for address-value pairs.
 */

#include <stdint.h>

static uint64_t simpoint_mem_info[] = {
"""
		self.fd_c.write(out_str)

		for page in self.mem_pages:
			for offset, val in page.offset_val.items():
				self.mem_info_cnt += 1
				out_str = hex(page.base + offset) + ", " + hex(val) + ","
				out_str = "    " + out_str + "\n"
				self.fd_c.write(out_str)

		out_str = \
"""
};
#endif

"""
		self.fd_c.write(out_str)

		# Page data as array in C file
		out_str = \
"""
#ifdef CONFIG_GEM5_STATIC_PAGES
#include <target/paging.h>
/*
 * Memory pages in an array
 */
/* Start address */
uint64_t simpoint_pages_start []                                                                                                                                                       
__attribute__((__section__(\".simpoint_pages\"), __aligned__(""" + str(self.PAGE_SIZE) + """))) = {
"""
		self.fd_c.write(out_str)

		for page in self.mem_pages:
			out_str = "/* page of base = " + hex(page.base) + " */\n"
			self.fd_c.write(out_str)
			for i in list(range(self.PAGE_SIZE >> 3)):
				offset = i * 8
				if (offset in page.offset_val):
					out_str = hex(page.offset_val[offset]) + ", /* offset = " + hex(offset) + " */"
				else:
					out_str = "0x0, "
				out_str = "    " + out_str + "\n"
				self.fd_c.write(out_str)

		out_str = \
"""
};
/* End address */
uint64_t simpoint_pages_end [0]__attribute__((__section__(".simpoint_pages")))= {};
#endif
"""
		self.fd_c.write(out_str)
		return

	def output_page_func(self):
		page_cnt = len(self.mem_pages)
		print("Status: Output memory page functions...")

		# Page allocating function
		out_str = \
"""
#include <target/paging.h>

phys_addr_t simpoint_new_pages[""" + str(page_cnt) + """] = {0};
phys_addr_t *simpoint_pages_alloc(void)
{
    for (int i = 0; i < """ + str(page_cnt) + """; i++) {
        simpoint_new_pages[i] = mem_alloc(""" + str(self.PAGE_SIZE) + """, """ + str(self.PAGE_SIZE) + """);
        con_dbg("New page at 0x%lx\\n", simpoint_new_pages[i]);
    }
    return simpoint_new_pages;
}

#ifdef CONFIG_GEM5_STATIC_PAGES
#include <target/paging.h>
static phys_addr_t simpoint_dumped_pages[""" + str(page_cnt) + """] = {0};

phys_addr_t *simpoint_pages_dump(void)
{
    phys_addr_t pa = (phys_addr_t)simpoint_pages_start;
    for (int i = 0; i < """ + str(page_cnt) + """; i++) {
        simpoint_dumped_pages[i] = pa;
        pa += """ + str(self.PAGE_SIZE) + """;
    }
    return simpoint_dumped_pages;
}
#endif

"""
		self.fd_c.write(out_str)

		# Page mapping function
		out_str = \
"""
void simpoint_pages_map(pgd_t *pgdp, phys_addr_t *pages_list)
{
"""
		self.fd_c.write(out_str)

		i = 0
		for page in self.mem_pages:
			out_str = \
"""
    con_dbg("Maping page PA = 0x%lx, VA = """ + hex(page.base) + """\\n", pages_list[""" + str(i) + """]);
    create_pgd_mapping(pgdp, pages_list[""" + str(i) + """], """ + hex(page.base) + """, """ + str(self.PAGE_SIZE) + """, PAGE_KERNEL, false);
"""
			self.fd_c.write(out_str)

		out_str = \
"""
    return;
}

"""
		self.fd_c.write(out_str)


		# Memory restore function
		out_str = \
"""
#ifndef CONFIG_GEM5_STATIC_PAGES
#include <target/compiler.h>
void simpoint_mem_restore(void)
{
    uint64_t *ptr = simpoint_mem_info;
    for (int i = 0; i < """ + str(self.mem_info_cnt) + """; i++) {
        uint64_t *addr = (uint64_t *)ptr[i*2];
        uint64_t value = ptr[i*2+1];
        *addr = value;
    }
}
#endif    
"""
		self.fd_c.write(out_str)

		return

	def process(self):
		print("Status: Process slice...")

		# First, step line by line
		block_type = BlockType.OTHERS
		line_num = 0
		for slice_line in self.fd_slice:
			line_num += 1
			if ("SIMPOINT_ASM_BEGIN" in slice_line):
				block_type = BlockType.ASM
				print("Status: Switch to SIMPOINT_ASM block at line", line_num)
				continue
			elif ("SIMPOINT_MEM_BEGIN" in slice_line):
				print("Status: Switch to SIMPOINT_MEM block at line", line_num)
				block_type = BlockType.MEM
				continue

			if (block_type == BlockType.ASM):
				self.oneline_asm(slice_line)
			elif (block_type == BlockType.MEM):
				self.oneline_mem(slice_line)
			else:
				print("Warn: Unknown line", line_num)

		# Then, carry out a summary
		self.output_page_data()
		self.output_page_func()
		return

	def cleanup(self):
		print("Status: Cleanup...")
		self.fd_slice.close()
		self.fd_asm.close()
		self.fd_c.close()
		return

def usage():
		print("Usage:", sys.argv[0], "riscv|arm64 gem5-slice-file")
		print("output:")
		print("  {gem5-slice-file}_func.S")
		print("  {gem5-slice-file}_heap.c")
		sys.exit(0)

def main():
	if (len(sys.argv) < 3):
		usage()

	if (sys.argv[1] == "riscv"):
		arch = ArchType.RISCV
	elif (sys.argv[1] == "arm64"):
		arch = ArchType.ARM64
	else:
		print("Invalid arch = ", sys.argv[1])
		usage()

	slice_file = sys.argv[2]

	compiler = Gem5Compile(arch, slice_file)
	compiler.prepare()
	compiler.process()
	compiler.cleanup()
	sys.exit(0)

if __name__== "__main__":
  main()
