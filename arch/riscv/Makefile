# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies. Remember to do have actions
# for "archclean" and "archdep" for cleaning up and making dependencies for
# this architecture
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#

OBJCOPYFLAGS    := -R .comment
LDFLAGS_sdfirm	:=

export BITS
ifeq ($(CONFIG_RISCV_64I),y)
	BITS := 64
	UTS_MACHINE := riscv64

	CFLAGS += -mabi=lp64
	AFLAGS += -mabi=lp64
	LDFLAGS += -melf64lriscv
else
	BITS := 32
	UTS_MACHINE := riscv32

	CFLAGS += -mabi=ilp32
	AFLAGS += -mabi=ilp32
	LDFLAGS += -melf32lriscv
endif

# ISA string setting
riscv-march-$(CONFIG_RISCV_32I)	:= rv32ima
riscv-march-$(CONFIG_RISCV_64I)	:= rv64ima
riscv-march-$(CONFIG_RISCV_F)	:= $(riscv-march-y)f
riscv-march-$(CONFIG_RISCV_D)	:= $(riscv-march-y)d
riscv-march-$(CONFIG_RISCV_C)	:= $(riscv-march-y)c
CFLAGS += -march=$(riscv-march-y)
AFLAGS += -march=$(riscv-march-y)

CFLAGS += -mno-save-restore
CFLAGS += -DCONFIG_PAGE_OFFSET=$(CONFIG_PAGE_OFFSET)

ifeq ($(CONFIG_CMODEL_MEDLOW),y)
	CFLAGS += -mcmodel=medlow
endif
ifeq ($(CONFIG_CMODEL_MEDANY),y)
	CFLAGS += -mcmodel=medany
endif

ifeq ($(CONFIG_CC_NO_RELAX),y)
	CFLAGS += -Wl,--no-relax
else
#	CFLAGS += -msmall-data-limit=n
endif

# GCC versions that support the "-mstrict-align" option default to allowing
# unaligned accesses.  While unaligned accesses are explicitly allowed in the
# RISC-V ISA, they're emulated by machine mode traps on all extant
# architectures.  It's faster to have GCC emit only aligned accesses.
CFLAGS += $(call cc-option,-mstrict-align)

# Default target when executing plain make
boot		:= arch/riscv/boot
KBUILD_IMAGE	:= sdfirm.bin sdfirm.srec
ifneq ($(CONFIG_VERILOG_DATA_WIDTH),)
KBUILD_IMAGE	+= sdfirm.ram sdfirm.rom
endif

# arch specific predefines for sparse
CHECKFLAGS += -D__riscv -D__riscv_xlen=$(BITS)

head-y := arch/riscv/common/head.rel

ifeq ($(CONFIG_ARCH_DUOWEN),y)
 machine-$(CONFIG_ARCH_DUOWEN)		:= duowen
 incdir-$(CONFIG_ARCH_DUOWEN)		:= duowen
endif
ifeq ($(CONFIG_ARCH_DPU),y)
 machine-$(CONFIG_ARCH_DPU)		:= dpu
 incdir-$(CONFIG_ARCH_DPU)		:= dpu
endif
ifeq ($(CONFIG_ARCH_VAISRA),y)
 machine-$(CONFIG_ARCH_VAISRA)		:= vaisra
 incdir-$(CONFIG_ARCH_VAISRA)		:= vaisra
endif
ifeq ($(CONFIG_ARCH_DPULP),y)
 machine-$(CONFIG_ARCH_DPULP)		:= dpulp
 incdir-$(CONFIG_ARCH_DPULP)		:= dpulp
endif
ifeq ($(CONFIG_ARCH_UNLEASHED),y)
 machine-$(CONFIG_ARCH_UNLEASHED)	:= unleashed
 incdir-$(CONFIG_ARCH_UNLEASHED)	:= unleashed
endif
ifeq ($(CONFIG_ARCH_K210),y)
 machine-$(CONFIG_ARCH_K210)		:= k210
 incdir-$(CONFIG_ARCH_K210)		:= k210
endif
ifeq ($(CONFIG_ARCH_COREV),y)
 machine-$(CONFIG_ARCH_COREV)		:= corev
 incdir-$(CONFIG_ARCH_COREV)		:= corev
endif
ifeq ($(CONFIG_ARCH_VEGA),y)
 machine-$(CONFIG_ARCH_VEGA)		:= vega
 incdir-$(CONFIG_ARCH_VEGA)		:= vega
endif
ifeq ($(CONFIG_ARCH_SPIKE),y)
 machine-$(CONFIG_ARCH_SPIKE)		:= spike
 incdir-$(CONFIG_ARCH_SPIKE)		:= spike
endif
ifeq ($(CONFIG_ARCH_VIRT),y)
 machine-$(CONFIG_ARCH_VIRT)		:= virt
 incdir-$(CONFIG_ARCH_VIRT)		:= virt
endif

ifeq ($(incdir-y),)
incdir-y := $(machine-y)
endif
INCDIR   := mach-$(incdir-y)

ifneq ($(machine-y),)
MACHINE  := arch/riscv/mach-$(machine-y)/
else
MACHINE  :=
endif

machdirs := $(patsubst %,arch/riscv/mach-%/,$(machine-y))
platdirs := $(patsubst %,arch/riscv/plat-%/,$(plat-y))

core-y				+= arch/riscv/common/
core-$(CONFIG_SBI)		+= arch/riscv/sbi/
core-$(CONFIG_FDT)		+= arch/riscv/fdt/
drivers-y			+= $(MACHINE)

all: $(KBUILD_IMAGE)

#	Update machine arch and proc symlinks if something which affects
#	them changed.  We use .arch to indicate when they were updated
#	last, otherwise make uses the target directory mtime.

include/asm/mach: $(wildcard include/config/arch/*.h) \
		include/config/auto.conf include/asm
	@echo '  SYMLINK include/asm/mach -> include/asm/$(INCDIR)'
	$(Q)$(SYMLINK) $(objtree)/include/asm/$(INCDIR) $@

ifeq ($(CONFIG_FDT_BUILTIN),y)
DTS_FILE := arch/$(ARCH)/common/fdt_builtin.dts
$(DTS_FILE): $(MACHINE)$(machine-y).dts
	@echo '  COPY    $(DTS_FILE) -> $(MACHINE)$(machine-y).dts'
	$(Q)mkdir -p $(objtree)/arch/riscv/common
	$(Q)cp -f $< $@
endif
CLEAN_FILES += arch/$(ARCH)/common/fdt_builtin.dts

archprepare: maketools

sdfirm.bin: sdfirm.strip
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@
sdfirm.srec: sdfirm.strip
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@
ifneq ($(CONFIG_VERILOG_DATA_WIDTH),)
sdfirm.rom: sdfirm.bin
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@
sdfirm.ram: sdfirm.srec
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@
endif

PHONY += maketools FORCE
maketools: include/asm/mach $(DTS_FILE) FORCE

CLEAN_FILES += arch/$(ARCH)/include/asm/mach-types.h
CLEAN_FILES += arch/$(ARCH)/boot/sdfirm.rom
CLEAN_FILES += arch/$(ARCH)/boot/sdfirm.ram
CLEAN_FILES += arch/$(ARCH)/boot/sdfirm.srec
CLEAN_FILES += arch/$(ARCH)/boot/sdfirm.bin

# We use MRPROPER_FILES and CLEAN_FILES now
archclean:
	$(Q)$(MAKE) $(clean)=$(boot)
