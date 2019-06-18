#
# arch/arm/Makefile
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2018 by Lv Zetalog Zheng

OBJCOPYFLAGS	:=-O binary -R .note -R .note.gnu.build-id -R .comment -S
CFLAGS		+=-g
AFLAGS		+=-g
LDFLAGS_sdfirm	:=-p --no-undefined -X
CFLAGS		+= \
	-mtune=cortex-a53 \
	-mstrict-align \
	-mcmodel=large \
	-mlittle-endian \
	-emit-cp-at-end \
	-fno-jump-tables \
	-ffunction-sections \
	-fdata-sections \
	-fshort-wchar \
	-funsigned-char \
	-fno-strict-aliasing \
	-fno-common \
	-fno-short-enums \
	-fverbose-asm \
	-fomit-frame-pointer \
	-fno-builtin \
	-fno-asynchronous-unwind-tables \
	-fstack-protector \
	-ffixed-x18\
	-Werror \
	-Wno-array-bounds \
	-Wno-address \
	-Wno-unused-parameter \
	-Wno-unused-function \
	-Wno-return-type \
	-Wno-unused-but-set-variable

ifeq ($(CONFIG_ARM64_ERRATUM_843419),y)
  ifeq ($(call ld-option, --fix-cortex-a53-843419),)
$(warning ld does not support --fix-cortex-a53-843419; kernel may be susceptible to erratum)
  else
LDFLAGS_vmlinux	+= --fix-cortex-a53-843419
  endif
endif

# Select a platform tht is kept up-to-date
KBUILD_DEFCONFIG := imc_defconfig

# Check for binutils support for specific extensions
lseinstr := $(call as-instr,.arch_extension lse,-DCONFIG_AS_LSE=1)

ifeq ($(CONFIG_ARM64_LSE_ATOMICS), y)
  ifeq ($(lseinstr),)
$(warning LSE atomics not supported by binutils)
  endif
endif

ifeq ($(CONFIG_ARM64), y)
brokengasinst := $(call as-instr,1:\n.inst 0\n.rept . - 1b\n\nnop\n.endr\n,,-DCONFIG_BROKEN_GAS_INST=1)

  ifneq ($(brokengasinst),)
$(warning Detected assembler with broken .inst; disassembly will be unreliable)
  endif
endif

CFLAGS	+= -mgeneral-regs-only $(lseinstr) $(brokengasinst)
CFLAGS	+= -fno-asynchronous-unwind-tables
CFLAGS	+= $(call cc-option, -mpc-relative-literal-loads)
AFLAGS	+= $(lseinstr) $(brokengasinst)

CFLAGS	+= $(call cc-option,-mabi=lp64)
AFLAGS	+= $(call cc-option,-mabi=lp64)

CFLAGS	+= $(call cc-ifversion, -ge, 0500, -DCONFIG_ARCH_SUPPORTS_INT128)

ifeq ($(CONFIG_CPU_BIG_ENDIAN), y)
CPPFLAGS	+= -mbig-endian
CHECKFLAGS	+= -D__AARCH64EB__
AS		+= -EB
LD		+= -EB
LDFLAGS		+= -maarch64linuxb
UTS_MACHINE	:= aarch64_be
else
CPPFLAGS	+= -mlittle-endian
CHECKFLAGS	+= -D__AARCH64EL__
AS		+= -EL
LD		+= -EL
LDFLAGS		+= -maarch64linux
UTS_MACHINE	:= aarch64
endif

CHECKFLAGS	+= -D__aarch64__ -m64

head-y		:= arch/arm64/common/head.rel

ifeq ($(CONFIG_ARCH_QDF2400),y)
 machine-$(CONFIG_ARCH_QDF2400)    := qdf2400
 incdir-$(CONFIG_ARCH_QDF2400)	   := qdf2400
endif
ifeq ($(CONFIG_ARCH_GEM5),y)
 machine-$(CONFIG_ARCH_GEM5)       := gem5
 incdir-$(CONFIG_ARCH_GEM5)	   := gem5
endif

ifeq ($(incdir-y),)
incdir-y := $(machine-y)
endif
INCDIR   := mach-$(incdir-y)

ifneq ($(machine-y),)
MACHINE  := arch/arm64/mach-$(machine-y)/
else
MACHINE  :=
endif

machdirs := $(patsubst %,arch/arm64/mach-%/,$(machine-y))
platdirs := $(patsubst %,arch/arm64/plat-%/,$(plat-y))

export	GZFLAGS

# If we have a machine-specific directory, then include it in the build.
core-y				+= arch/arm64/common/
drivers-y			+= $(MACHINE)

# Default target when executing plain make
KBUILD_IMAGE := sdfirm.bin

all:	$(KBUILD_IMAGE)

boot := arch/arm64/boot

#	Update machine arch and proc symlinks if something which affects
#	them changed.  We use .arch to indicate when they were updated
#	last, otherwise make uses the target directory mtime.

include/asm/mach: $(wildcard include/config/arch/*.h) \
		include/config/auto.conf include/asm
	@echo '  SYMLINK include/asm/mach -> include/asm/$(INCDIR)'
	$(Q)$(SYMLINK) $(TOPDIR)/include/asm/$(INCDIR) $@

archprepare: maketools

sdfirm.bin: sdfirm
	$(Q)$(MAKE) $(build)=$(boot) MACHINE=$(MACHINE) $(boot)/$@

PHONY += maketools FORCE
maketools: include/asm/mach FORCE

CLEAN_FILES += arch/$(ARCH)/include/asm/mach-types.h
CLEAN_FILES += arch/$(ARCH)/boot/sdfirm.bin

# We use MRPROPER_FILES and CLEAN_FILES now
archclean:
	$(Q)$(MAKE) $(clean)=$(boot)

define archhelp
  echo  '  sdfirm.bin      - Binary image (arch/$(ARCH)/boot/sdfirm.bin)'
endef
