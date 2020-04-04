#ifdef CONFIG_DPU_TMR_FIRQ
define_virq __handle_tmr_virq do_riscv_interrupt
#else
#define __handle_tmr_virq	__handle_bad_virq
#endif
#ifdef CONFIG_DPU_PLIC_FIRQ
define_virq __handle_ext_virq do_riscv_interrupt
#else
#define __handle_ext_virq	__handle_bad_virq
#endif
