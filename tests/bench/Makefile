obj-y :=

# sanity tests
obj-$(CONFIG_TLB_TEST) += tlbtest.rel

# benchmarks
obj-$(CONFIG_DHRYSTONE) += dhrystone.rel
ifeq ($(CONFIG_DHRYSTONE_NO_INLINE),y)
CFLAGS_dhrystone.o += -fno-inline
endif
ifeq ($(CONFIG_DHRYSTONE_REG),y)
CFLAGS_dhrystone.o += -DREG=register
endif

obj-$(CONFIG_LINPACK) += linpack.rel
obj-$(CONFIG_COREMARK) += coremark/
obj-$(CONFIG_MEMSTREAM) += memstream.rel

obj-$(CONFIG_HANOITOWER) += hanoitower.rel
obj-$(CONFIG_SHA256) += sha256.rel
obj-$(CONFIG_MD5) += md5.rel

obj-$(CONFIG_CFFT) += cfft/
obj-$(CONFIG_JPGENC) += jpgenc/
obj-$(CONFIG_GENANN) += gen_ann/
