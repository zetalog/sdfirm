#
# arch/arm64/boot/Makefile
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2009 Lv Zheng
#

sdfirm := sdfirm.bin sdfirm.srec sdfirm.vhx

$(obj)/sdfirm.bin: sdfirm.strip FORCE
	$(call if_changed,objcopy)
	@echo '  Small Device Firmware: $@ is ready'
$(obj)/sdfirm.srec: sdfirm.strip FORCE
	$(call if_changed,objcopy)
	@echo '  Small Device Firmware: $@ is ready'
ifdef CONFIG_SUNXI_SPL
$(obj)/sunxi-spl.bin: $(obj)/sdfirm.bin FORCE
	$(call if_changed,mksunxiboot)
	@echo '  Small Device Firmware: $@ is ready'
MKSUNXIBOOTFLAGS_sunxi-spl.bin	+="sun50i-h5-nanopi-neo2"
endif
OBJCOPYFLAGS_sdfirm.bin		+=-O binary
OBJCOPYFLAGS_sdfirm.srec	+=-O srec
ifneq ($(CONFIG_VERILOG_DATA_WIDTH),)
$(obj)/sdfirm.vhx: $(obj)/sdfirm.srec FORCE
	$(call if_changed,srec2vhx)
	@echo '  Small Device Firmware: $@ is ready'
SREC2VHXFLAGS_sdfirm.vhx	+=-w $(CONFIG_VERILOG_DATA_WIDTH)
endif
