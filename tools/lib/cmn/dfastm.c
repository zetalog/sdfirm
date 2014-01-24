#include <simul\dfastm.h>

stm_instance_t *stm_table_new(const char name[16],
			      stm_callback_cb func, unsigned long data,
			      const stm_table_t *table, int state)
{
	stm_instance_t *fsmi = NULL;

	fsmi = malloc(sizeof (stm_instance_t));
	if (fsmi) {
		memset(fsmi, 0, sizeof (stm_instance_t *));
		strlcpy(fsmi->name, name, sizeof (fsmi->name));
		fsmi->table = table;
		fsmi->func = func;
		fsmi->data = data;
		fsmi->state = state;
		atomic_set(&fsmi->refcnt, 1);
	}
	return fsmi;
}

void stm_table_free(stm_instance_t *fsmi)
{
	if (fsmi) {
		atomic_dec(&fsmi->refcnt);
		if (atomic_read(&fsmi->refcnt) == 0) {
			free(fsmi);
		}
	}
}

int stm_raise_event(stm_instance_t *stmi, int event, void *data)
{
	stm_table_t const *stm = stmi->table; 
	stm_entry_t const *stme = &stm->stm[0];

	if (stmi->func) {
		stmi->func(stmi, event, data);
		return 1;
	}

	assert(event >= 0);

	if ((stmi->state >= stm->num_states)) {
		if (stm->log) {
			(*stm->log)(stmi, STM_LOG_ERR,
				    "STM [%s(%s)]: state=%d out of range",
				    stm->name, stmi->name, stmi->state);
		}
		return 1;
	}
	if ((event >= stm->num_events)) {
		if (stm->log) {
			(*stm->log)(stmi, STM_LOG_ERR,
				    "STM [%s(%s)]: event=%d out of range",
				    stm->name, stmi->name, event);
		}
		return 1;
	}

	if (stm->log) {
		(*stm->log)(stmi, STM_LOG_INFO,
			    "STM [%s(%s)]: event %s in state %s",
			    stm->name, stmi->name, stm->event_names[event],
			    stm->state_names[stmi->state]);
	}
	while (stme->action != NULL) {
		if ((event == stme->event) && (stmi->state == stme->state)) {
			int idx;
			int prev_state = stmi->state;

			/* run all registered actions */
			for (idx = 0; stme->action && idx < stme->n_actions;
			     idx++) {
				if (stme->action[idx])
					if (!(*stme->action[idx])(stmi, data))
						return 0;
			}
			stmi->state = stme->new_state;
			if (stme->new_state != prev_state) {
				if (stm->log) {
					(*stm->log)(stmi, STM_LOG_INFO,
						    "STM [%s(%s)]: %s -> %s",
						    stm->name, stmi->name,
						    stm->state_names[prev_state], 
						    stm->state_names[stme->new_state]);
				}
			}
			return 1;
		}
		stme++;
	}
	if (stm->log) {
		(*stm->log)(stmi, STM_LOG_ERR,
			    "STM [%s(%s)]: unhandled event %d in state %d",
			    stm->name, stmi->name, event, stmi->state);
	}
	return 1;
}

void stm_enter_state(stm_instance_t *stmi, int new_state)
{
	const stm_table_t *stm = stmi->table;

	if (new_state >= stm->num_states) {
		if (stm->log) {
			(*stm->log)(stmi, STM_LOG_ERR,
				    "STM [%s(%s)]: new state=%d out of range",
				    stm->name, stmi->name, new_state);
		}
		return;
	}

	if (new_state != stmi->state) {
		if (stm->log) {
			(*stm->log)(stmi, STM_LOG_INFO,
				    "STM [%s(%s)]: %s -> %s",
				    stm->name, stmi->name,
				    stm->state_names[stmi->state], 
				    stm->state_names[new_state]);
		}
		stmi->state = new_state;
	}
}

const char *stm_state_name(stm_instance_t *stmi)
{
	if (stmi->state > stmi->table->num_states) {
		return "???";
	}
	return stmi->table->state_names[stmi->state];
}

const char *stm_event_name(stm_instance_t *stmi, int event)
{
	if (event > stmi->table->num_events) {
		return "???";
	}
	return stmi->table->event_names[event];
}
