#ifndef __BH_H_INCLUDE__
#define __BH_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

typedef uint8_t bh_t;

typedef void (*bh_cb)(uint8_t event);

#define BH_WAKEUP	0x00	/* bottom half wakeup */
#define BH_TIMEOUT	0x01	/* timeout shot */
#define BH_POLLIRQ	0x02	/* irq polling */

struct bh_entry {
	bh_cb handler;			/* module entry */
};

#define NR_BHS				CONFIG_MAX_BHS
#define INVALID_BH			NR_BHS

/* bottom halves */
void bh_run_all(void);
void bh_run(bh_t bh, uint8_t event);
void __bh_run(bh_t bh, uint8_t event);
void bh_panic(void);
void bh_suspend(bh_t bh);
void bh_resume(bh_t bh);
boolean bh_resumed_any(void); 
bh_t bh_register_handler(bh_cb handler);

/* irq pollers */
void poll_register(bh_t bh);
void poll_run(bh_t bh);

void bh_init(void);

#endif /* __BH_H_INCLUDE__ */
