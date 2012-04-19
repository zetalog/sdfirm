#ifndef __TIMER_H_INCLUDE__
#define __TIMER_H_INCLUDE__

/* TODO:
 * 1. promptly timer TIMER_IRQ.
 */
#include <target/config.h>
#include <target/generic.h>
#include <target/state.h>
#include <target/gpt.h>

typedef uint8_t tid_t;

/* The max timers that can be registered */
#ifdef CONFIG_MAX_TIMERS
#define NR_TIMERS				CONFIG_MAX_TIMERS
#else
#define NR_TIMERS				NR_BHS
#endif
#define INVALID_TID				NR_TIMERS

/* TIMER_BH: Background running timers, which should be delayed to the
 *           process context.  No interrupt masking is required for such
 *           timers.  This is also known as generic timers.
 * TIMER_IRQ: Foreground running timers, which should be handled in the
 *            interrupt context without delay to ensure that it can be
 *            handled in time.  This is also known as realtime timers.
 */
#define TIMER_BH	0x01
#define TIMER_IRQ	0x02

#ifdef CONFIG_TIMER
tid_t timer_register(bh_t bh, uint8_t type);
void timer_schedule_shot(tid_t tid, timeout_t tout_ms);

void timer_run(uint8_t type);
boolean timer_timeout_raised(tid_t tid, uint8_t type);
#else
#define timer_register(sid, type)		INVALID_TID
#define timer_timeout_raised(tid, type)		false
#define timer_schedule_shot(tid, tout_ms)
#define timer_run(type)
#endif

#endif /* __TIMER_H_INCLUDE__ */
