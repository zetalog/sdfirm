#
# Makefile for the target kernel.
#

obj-y :=
obj-$(CONFIG_ARCH_IS_BOOT_EXE) += main.rel
obj-$(CONFIG_ARCH_IS_BOOT_EXE) += init.rel
obj-${CONFIG_GEM5}	+= gem5/
obj-${CONFIG_RIS}	+= ris/

# files to be removed upon make clean
clean-files := ../include/target/compile.h

# dependencies on generated files need to be listed explicitly

$(obj)/version.rel: include/target/compile.h

# compile.h changes depending on hostname, generation number, etc,
# so we regenerate it always.
# mkcompile_h will make sure to only update the
# actual file if its content has changed.

include/target/compile.h: FORCE
	@echo '  CHK     $@'
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkcompile_h $@ \
	"$(UTS_MACHINE)" "$(CONFIG_SMP)" "$(CONFIG_PREEMPT)" "$(CC) $(CFLAGS)"
