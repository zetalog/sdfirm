obj-y :=

# benchmarks
obj-$(CONFIG_DHRYSTONE) += dhrystone.rel
ifeq ($(CONFIG_DHRYSTONE_NO_INLINE),y)
CFLAGS_dhrystone.o += -fno-inline
endif
ifeq ($(CONFIG_DHRYSTONE_REG),y)
CFLAGS_dhrystone.o += -DREG=register
endif

obj-$(CONFIG_LINPACK) += linpack.rel

# Default data size need to be handled in Makefile while the others can be
# handled by core_portme_posix_overrides.h.
ifneq ($(CONFIG_COREMARK_TOTAL_DATA_SIZE),)
CFLAGS_coremark.o += -DTOTAL_DATA_SIZE=$(CONFIG_COREMARK_TOTAL_DATA_SIZE)
else
ifdef CONFIG_COREMARK_RUN_PROFILE
CFLAGS_coremark.o += -DTOTAL_DATA_SIZE=1200
endif
endif
# CoreMark should always be built with -O2
XCFLAGS_coremark.o = -O2 $(CONFIG_COREMARK_XCFLAGS)
FLAGS_STR_coremark.o="$(XCFLAGS_coremark.o)"
CFLAGS_coremark.o += $(XCFLAGS_coremark.o)
CFLAGS_coremark.o += -DFLAGS_STR=\"$(FLAGS_STR_coremark.o)\"
obj-$(CONFIG_COREMARK) += coremark.rel

obj-$(CONFIG_MEMSTREAM) += memstream.rel

obj-$(CONFIG_TEST_ALGO) += algo/
obj-$(CONFIG_TEST_SANITY) += sanity/
