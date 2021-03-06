#ifndef __GPT_DRIVER_H_INCLUDE__
#define __GPT_DRIVER_H_INCLUDE__

#include <target/generic.h>

#ifndef __ASSEMBLY__
#ifdef CONFIG_ARCH_HAS_GPT
#include <asm/mach/gpt.h>
#endif

void gpt_hw_ctrl_init(void);

#ifdef CONFIG_TICK
void gpt_hw_periodic_start(void);
void gpt_hw_periodic_restart(void);
#else
#ifndef GPT_MAX_TIMEOUT
#define GPT_MAX_TIMEOUT 250
#endif
#ifdef SYS_BOOTLOAD
void gpt_hw_irq_poll(void);
#endif
void gpt_hw_oneshot_timeout(timeout_t tout_ms);
#endif
#endif /* __ASSEMBLY__ */

#endif /* __GPT_DRIVER_H_INCLUDE__ */
