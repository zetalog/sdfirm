#ifndef __STATE_H_INCLUDE__
#define __STATE_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

typedef uint8_t sid_t;

typedef void (*state_call_cb)(uint8_t event);

#define STATE_EVENT_WAKE	0x00	/* state wakeup */
#define STATE_EVENT_SHOT	0x01	/* timeout shot */
#define STATE_EVENT_POLL	0x02	/* irq polling */

struct state_entry {
	state_call_cb handler;		/* module entry */
};

#define NR_STATES			CONFIG_MAX_STATES

#define INVALID_SID			NR_STATES

/* state machine */
void state_run_all(void);
void state_run(sid_t sid, uint8_t event);
void __state_run(sid_t sid, uint8_t event);
void state_run_idle(void);
void bh_suspend(sid_t sid);
void bh_resume(sid_t sid);
boolean bh_resumed_any(void); 
sid_t state_register(state_call_cb handler);

/* hardware irq poller */
void poll_register(sid_t sid);
void poll_run(sid_t sid);

void state_init(void);

#endif /* __STATE_H_INCLUDE__ */
