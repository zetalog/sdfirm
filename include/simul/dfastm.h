/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2008
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)dfastm.h: event driven deterministic finite-state machine 
 * $Id: dfastm.h,v 1.1 2011-04-26 03:33:19 zhenglv Exp $
 */

#ifndef __DFASTM_H_INCLUDE__
#define __DFASTM_H_INCLUDE__

#include <simul\types.h>
#include <simul\atomic.h>
#include <simul\list.h>

typedef struct _stm_instance_t stm_instance_t;
typedef struct _stm_entry_t stm_entry_t;
typedef struct _stm_table_t stm_table_t;
typedef	int (*stm_action_fn)(stm_instance_t *fsmi, void *data);

/* TODO: the table will be too big
 * should be state array and per-state event arrays
 */
struct _stm_entry_t {
	int state;
	int event;
	const stm_action_fn *action;
	int n_actions;
	int new_state;
};

#define STM_LOG_ERR	1
#define STM_LOG_INFO	2

struct _stm_table_t {
	const char *name;
	void (*log)(stm_instance_t const *fsmi, int level, const char *fmt, ...);
	int num_states;
	const char **state_names;
	int num_events;
	const char **event_names;
	const stm_entry_t *stm;
};

typedef int (*stm_callback_cb)(stm_instance_t *inst,
			       int event, void *data);
struct _stm_instance_t {
	atomic_t refcnt;
	char name[16];
	int state;
	stm_callback_cb func;
	unsigned long data;
	const stm_table_t *table;
};

stm_instance_t *stm_table_new(const char name[16],
			      stm_callback_cb func, unsigned long data,
			      const stm_table_t *table, int state);
void stm_table_free(stm_instance_t *fsmi);
static inline stm_instance_t *stm_table_get(stm_instance_t *fsmi)
{
	atomic_inc(&fsmi->refcnt);
	return fsmi;
}
static inline void stm_table_put(stm_instance_t *fsmi)
{
	atomic_dec(&fsmi->refcnt);
}

/* XXX: don't call a event handler directly, 
 * should use eloop_post_event instead
 */
int stm_raise_event(stm_instance_t *fsmi, int event, void *data);
void stm_enter_state(stm_instance_t *fsmi, int new_state);
const char *stm_state_name(stm_instance_t *fsmi);
const char *stm_event_name(stm_instance_t *fsmi, int event);

#endif /* __DFASTM_H_INCLUDE__ */
