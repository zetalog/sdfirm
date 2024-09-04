
obj-y :=

obj-$(CONFIG_TEST_BENCH) += bench.rel
obj-$(CONFIG_TEST_BENCH) += bench/

obj-$(CONFIG_TEST_LITMUS) += litmus.rel
CFLAGS_litmus.o += -DASS -DFORCE_AFFINITY

obj-$(CONFIG_TEST_MEMTESTER) += memtester.rel

obj-$(CONFIG_RAVEN) += arm64/
obj-$(CONFIG_RISCV) += riscv/
