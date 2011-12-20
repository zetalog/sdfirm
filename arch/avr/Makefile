#
# arch/avr/Makefile
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2009 by Lv Zetalog Zheng

CPPFLAGS_sdfirm.lds =
OBJCOPYFLAGS	:=-O ihex
CFLAGS		+=-D__AVR__ -ffunction-sections -fsigned-char
AFLAGS		+=-D__AVR__ -D__ASSEMBLER__
LDFLAGS_sdfirm	:=--gc-sections --relax -nostdstartfiles

# Select a platform tht is kept up-to-date
KBUILD_DEFCONFIG := solmsd_defconfig

comma = ,

arch-$(CONFIG_CPU_8v1)		:=-mmcu=avr1
arch-$(CONFIG_CPU_8v2)		:=-mmcu=avr2
arch-$(CONFIG_CPU_8v25)		:=-mmcu=avr25
arch-$(CONFIG_CPU_8v3)		:=-mmcu=avr3
arch-$(CONFIG_CPU_8v31)		:=-mmcu=avr31
arch-$(CONFIG_CPU_8v35)		:=-mmcu=avr35
arch-$(CONFIG_CPU_8v4)		:=-mmcu=avr4
arch-$(CONFIG_CPU_8v5)		:=-mmcu=avr5
arch-$(CONFIG_CPU_8v51)		:=-mmcu=avr51
arch-$(CONFIG_CPU_8v6)		:=-mmcu=avr6

arch-$(CONFIG_AVR_ENHANCED)	+= -D__AVR_ENHANCED__
arch-$(CONFIG_AVR_MEGA)		+= -D__AVR_MEGA__
arch-$(CONFIG_AVR_TINY)		+= -D__AVR_ARCH_TINY__
arch-$(CONFIG_AVR_2_BYTE_PC)	+= -D__AVR_2_BYTE_PC__
arch-$(CONFIG_AVR_3_BYTE_PC)	+= -D__AVR_3_BYTE_PC__

CFLAGS		+=$(arch-y)
AFLAGS		+=$(arch-y)

CHECKFLAGS	+= -D__avr__

 machine-$(CONFIG_ARCH_AT90USB1287)	:= at90usb1287
#  incdir-$(CONFIG_ARCH_ATMEGA128)	:= atmega128
 machine-$(CONFIG_ARCH_AT90SCR100)	:= at90scr100

ifeq ($(incdir-y),)
incdir-y := $(machine-y)
endif
INCDIR   := mach-$(incdir-y)

ifneq ($(machine-y),)
MACHINE  := arch/avr/mach-$(machine-y)/
else
MACHINE  :=
endif

# Default program entry
head-y		:= arch/avr/common/head.rel

# If we have a machine-specific directory, then include it in the build.
core-y				+= arch/avr/common/
drivers-y			+= $(MACHINE)

#core-$(CONFIG_ARCH_ATMEGA128)	+= arch/mcs51/plat-atmega128/

# Default target when executing plain make
KBUILD_IMAGE := sdfirm.hex

all:	$(KBUILD_IMAGE)

boot := arch/avr/boot

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
