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
# Copyright (C) 2010 by Lv Zetalog Zheng

OBJCOPYFLAGS	:=-O binary -R .note -R .note.gnu.build-id -R .comment -S
CFLAGS		+=-ffixed-r8
AFLAGS		+=
LDFLAGS_sdfirm	:=--gc-sections -p --no-undefined -X

# Never generate .eh_frame
CFLAGS		+= $(call cc-option,-fno-dwarf2-cfi-asm)

# Select a platform tht is kept up-to-date
KBUILD_DEFCONFIG := solmsd_defconfig

ifeq ($(CONFIG_FRAME_POINTER),y)
CFLAGS		+=-fno-omit-frame-pointer -mapcs -mno-sched-prolog
endif

ifeq ($(CONFIG_CPU_BIG_ENDIAN),y)
CPPFLAGS	+= -mbig-endian
AS		+= -EB
LD		+= -EB
else
CPPFLAGS	+= -mlittle-endian
AS		+= -EL
LD		+= -EL
endif

comma = ,

# This selects which instruction set is used.
# Note that GCC does not numerically define an architecture version
# macro, but instead defines a whole series of macros which makes
# testing for a specific architecture or later rather impossible.
arch-$(CONFIG_CPU_32v7M)	:=-D__LINUX_ARM_ARCH__=7 -D__LINUX_ARM_M_ARCH__ $(call cc-option,-march=armv7-m -Wa$(comma)-march=armv7-m)
arch-$(CONFIG_CPU_32v7)		:=-D__LINUX_ARM_ARCH__=7 $(call cc-option,-march=armv7-a,-march=armv5t -Wa$(comma)-march=armv7-a)
arch-$(CONFIG_CPU_32v6)		:=-D__LINUX_ARM_ARCH__=6 $(call cc-option,-march=armv6,-march=armv5t -Wa$(comma)-march=armv6)
# Only override the compiler option if ARMv6. The ARMv6K extensions are
# always available in ARMv7
ifeq ($(CONFIG_CPU_32v6),y)
arch-$(CONFIG_CPU_32v6K)	:=-D__LINUX_ARM_ARCH__=6 $(call cc-option,-march=armv6k,-march=armv5t -Wa$(comma)-march=armv6k)
endif
arch-$(CONFIG_CPU_32v5)		:=-D__LINUX_ARM_ARCH__=5 $(call cc-option,-march=armv5te,-march=armv4t)
arch-$(CONFIG_CPU_32v4T)	:=-D__LINUX_ARM_ARCH__=4 -march=armv4t
arch-$(CONFIG_CPU_32v4)		:=-D__LINUX_ARM_ARCH__=4 -march=armv4
arch-$(CONFIG_CPU_32v3M)	:=-D__LINUX_ARM_ARCH__=3 -march=armv3m
arch-$(CONFIG_CPU_32v3)		:=-D__LINUX_ARM_ARCH__=3 -march=armv3

# This selects how we optimise for the processor.
tune-$(CONFIG_CPU_CORTEXM3)	:=-mcpu=cortex-m3

ifeq ($(CONFIG_ARM_EABI),y)
CFLAGS_ABI	:=-mabi=aapcs-linux -mno-thumb-interwork
else
CFLAGS_ABI	:=$(call cc-option,-mapcs-32,-mabi=apcs-gnu) $(call cc-option,-mno-thumb-interwork,)
endif

ifeq ($(CONFIG_ARM_THUMB2),y)
AFLAGS_AUTOIT	:=$(call as-option,-Wa$(comma)-mimplicit-it=always,-Wa$(comma)-mauto-it)
AFLAGS_NOWARN	:=$(call as-option,-Wa$(comma)-mno-warn-deprecated,-Wa$(comma)-W)
CFLAGS_THUMB	:=-mthumb $(AFLAGS_AUTOIT) $(AFLAGS_NOWARN)
AFLAGS_THUMB	:=$(CFLAGS_THUMB) -Wa$(comma)-mthumb
else
CFLAGS_ARM	:=$(call cc-option,-marm,)
AFLAGS_ARM	:=$(CFLAGS_ARM) $(call cc-option,-marm,)
endif

# Need -Uarm for gcc < 3.x
CFLAGS		+=$(CFLAGS_ABI) $(CFLAGS_ARM) $(CFLAGS_THUMB) $(arch-y) $(tune-y) $(call cc-option,-mshort-load-bytes,$(call cc-option,-malignment-traps,)) -msoft-float -Uarm
AFLAGS		+=$(CFLAGS_ABI) $(AFLAGS_ARM) $(AFLAGS_THUMB) $(arch-y) $(tune-y) -include asm/unified.h -msoft-float

CHECKFLAGS	+= -D__arm__

# Default program entry
head-y		:= arch/arm/common/head.rel

ifeq ($(CONFIG_ARCH_LM3S9B92),y)
 machine-$(CONFIG_ARCH_LM3S9B92)   := lm3s9b92
 incdir-$(CONFIG_ARCH_LM3S9B92)	   := lm3s9b92
endif
ifeq ($(CONFIG_ARCH_LM3S3826),y)
 machine-$(CONFIG_ARCH_LM3S3826)   := lm3s3826
 incdir-$(CONFIG_ARCH_LM3S3826)	   := lm3s3826
endif

ifeq ($(incdir-y),)
incdir-y := $(machine-y)
endif
INCDIR   := mach-$(incdir-y)

ifneq ($(machine-y),)
MACHINE  := arch/arm/mach-$(machine-y)/
else
MACHINE  :=
endif

machdirs := $(patsubst %,arch/arm/mach-%/,$(machine-y))
platdirs := $(patsubst %,arch/arm/plat-%/,$(plat-y))

export	GZFLAGS

# If we have a machine-specific directory, then include it in the build.
core-y				+= arch/arm/common/
drivers-y			+= $(MACHINE)

#core-$(CONFIG_ARCH_CORTEXM3)	+= arch/arm/plat-cortexm3/

# Default target when executing plain make
KBUILD_IMAGE := sdfirm.bin

all:	$(KBUILD_IMAGE)

boot := arch/arm/boot

#	Update machine arch and proc symlinks if something which affects
#	them changed.  We use .arch to indicate when they were updated
#	last, otherwise make uses the target directory mtime.

include/asm/mach: $(wildcard include/config/arch/*.h) \
		include/config/auto.conf include/asm
	@echo '  SYMLINK include/asm/mach -> include/asm/$(INCDIR)'
	$(Q)$(SYMLINK) $(TOPDIR)/include/asm/$(INCDIR) $@

archprepare: maketools

PHONY += maketools FORCE
maketools: include/asm/mach FORCE

sdfirm.bin: sdfirm
	$(Q)$(MAKE) $(build)=$(boot) MACHINE=$(MACHINE) $(boot)/$@

CLEAN_FILES += arch/$(ARCH)/include/asm/mach-types.h
CLEAN_FILES += arch/$(ARCH)/boot/sdfirm.bin

# We use MRPROPER_FILES and CLEAN_FILES now
archclean:
	$(Q)$(MAKE) $(clean)=$(boot)

define archhelp
  echo  '  sdfirm.bin     - Binary image (arch/$(ARCH)/boot/sdfirm.bin)'
endef
