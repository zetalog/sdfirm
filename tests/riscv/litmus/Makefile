obj-y :=

obj-$(CONFIG_LITMUS_RISCV_DUMMY) += dummy.rel

include $(srctree)/tests/riscv/litmus/Makefile.litmus

CFLAGS += -DASS -DFORCE_AFFINITY
