#ifndef __BH_H_INCLUDE__
#define __BH_H_INCLUDE__

#include <target/generic.h>
#include <target/smp.h>

#ifndef __ASSEMBLY__
typedef uint8_t bh_t;

typedef void (*bh_cb)(uint8_t event);

#define BH_WAKEUP	0x00	/* bottom half wakeup */
#define BH_POLLIRQ	0x01	/* irq polling */

struct bh_entry {
	bh_cb handler;			/* module entry */
};

#define NR_BHS				CONFIG_MAX_BHS
#define INVALID_BH			NR_BHS

/* bottom halves */
void bh_run(bh_t bh, uint8_t event);
void bh_panic(void);
void bh_suspend(bh_t bh);
void bh_resume(bh_t bh);
#ifdef CONFIG_SMP
void bh_resume_smp(bh_t bh, cpu_t cpu);
#else
#define bh_resume_smp(bh, cpu)		bh_resume(bh)
#endif
bh_t bh_register_handler(bh_cb handler);
void bh_sync(void);
void bh_loop(void);
void bh_init(void);
#endif /* __ASSEMBLY__ */

#endif /* __BH_H_INCLUDE__ */
