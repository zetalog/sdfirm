#
# arch/mcs51/Makefile
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2009 by Lv Zetalog Zheng

OBJCOPYFLAGS	:=
GZFLAGS		:=-9
CFLAGS		+=-mmcs51 --model-small --iram-size 0x100 --xram-size 0x200 --code-size 0x8000
LDFLAGS_sdfirm	:=--out-fmt-ihx $(CFLAGS)

# Select a platform tht is kept up-to-date
KBUILD_DEFCONFIG := pinpad_defconfig

comma = ,

CHECKFLAGS	+= -D__mcs51__

 machine-$(CONFIG_ARCH_AT8XC5122)  := at8xc5122
#  incdir-$(CONFIG_ARCH_AT8051)	   := at8051

ifeq ($(incdir-y),)
incdir-y := $(machine-y)
endif
INCDIR   := mach-$(incdir-y)

ifneq ($(machine-y),)
MACHINE  := arch/mcs51/mach-$(machine-y)/
else
MACHINE  :=
endif

#Default value
head-y		:= $(MACHINE)/head.rel

export	GZFLAGS

# If we have a machine-specific directory, then include it in the build.
core-y				+= arch/mcs51/common/
drivers-y			+= $(MACHINE)

#core-$(CONFIG_ARCH_AT8051)	+= arch/mcs51/plat-at8051/

# Default target when executing plain make
KBUILD_IMAGE := sdfirm.hex

all:	$(KBUILD_IMAGE)

boot := arch/mcs51/boot

#	Update machine arch and proc symlinks if something which affects
#	them changed.  We use .arch to indicate when they were updated
#	last, otherwise make uses the target directory mtime.

include/asm/mach: $(wildcard include/config/arch/*.h) \
		include/config/auto.conf include/asm
	@echo '  SYMLINK include/asm/mach -> include/asm/$(INCDIR)'
	$(Q)$(SYMLINK) include/asm/$(INCDIR) $@

archprepare: maketools

PHONY += maketools FORCE
maketools: include/asm/mach FORCE

sdfirm.hex: sdfirm
	$(Q)$(MAKE) $(build)=$(boot) MACHINE=$(MACHINE) $(boot)/$@

CLEAN_FILES += arch/$(ARCH)/include/asm/mach-types.h
CLEAN_FILES += arch/$(ARCH)/boot/sdfirm.hex

# We use MRPROPER_FILES and CLEAN_FILES now
archclean:
	$(Q)$(MAKE) $(clean)=$(boot)

define archhelp
  echo  '  sdfirm.hex     - Packed hex image (arch/$(ARCH)/boot/sdfirm.hex)'
endef
