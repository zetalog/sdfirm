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

obj-$(CONFIG_COREMARK) += core_main.rel
obj-$(CONFIG_COREMARK) += core_list_join.rel
obj-$(CONFIG_COREMARK) += core_matrix.rel
obj-$(CONFIG_COREMARK) += core_state.rel
obj-$(CONFIG_COREMARK) += core_util.rel
obj-$(CONFIG_COREMARK) += core_portme.rel

obj-$(CONFIG_MEMSTREAM) += memstream.rel

obj-$(CONFIG_TEST_ALGO) += algo/
obj-$(CONFIG_TEST_SANITY) += sanity/
