obj-y :=

obj-y += run.rel
obj-$(CONFIG_LITMUS_RISCV_DUMMY) += dummy.rel
obj-$(CONFIG_LB_2B_amoadd_2D_data_2D_amoadd_2E_rl_2B_amoadd_2E_aq_2D_data_2D_amoadd) += LB+amoadd-data-amoadd.rl+amoadd.aq-data-amoadd.rel
obj-$(CONFIG_WRC_2B_fence_2E_rw_2E_rws_2B_fence_2E_rw_2E_rwsxx) += WRC+fence.rw.rws+fence.rw.rwsxx.rel

include $(srctree)/tests/riscv/litmus/Makefile.litmus

CFLAGS += -DASS -DFORCE_AFFINITY
