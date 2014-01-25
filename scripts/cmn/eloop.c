#include <simul/eloop.h>
#include <stdio.h>
#include <signal.h>

/* WIN32 asynchronous event handler emulation */
typedef struct _eloop_sock_t {
	sockfd_t sock;
	void *eloop_data;
	void *user_data;
	eloop_sock_cb handler;
} eloop_sock_t;

typedef struct _eloop_alarm_t {
	struct timeval time;
	void *eloop_data;
	void *user_data;
	eloop_alarm_cb handler;
	struct _eloop_alarm_t *next;
} eloop_alarm_t;

typedef struct _eloop_signal_t {
	int sig;
	void *user_data;
	eloop_signal_cb handler;
	int signaled;
} eloop_signal_t;

typedef struct _eloop_fdset_t {
	int count;
	eloop_sock_t *table;
	int changed;
} eloop_fdset_t;

#define ELOOP_EVENT_MAX_PENDING	10

typedef struct _eloop_fsmevt_t {
	int event;
#define ELOOP_EVENT_STATE_ACTIVE		0
#define ELOOP_EVENT_STATE_DEACTIVE		1
#define ELOOP_EVENT_STATE_PENDING	2
	int state;
	/* pending event will only be sent on state changes */
	int pending_state;
	time_t last_pending;
	void *data;
	stm_instance_t *inst;
	struct list_head link;
} eloop_fsmevt_t;

struct _eloop_t {
	void *user_data;

	int max_sock;
	int notify_sock;
	struct sockaddr_in notify_addr;

	eloop_fdset_t readers;
	eloop_fdset_t writers;
	eloop_fdset_t exceptions;

	eloop_alarm_t *timeout;

	int signal_count;
	eloop_signal_t *signals;
	int signaled;
	int pending_terminate;

	struct list_head pending_queue;
	struct list_head running_queue;
	int events_pending;

	atomic_t refcnt;

	int terminate;
	int read_fdset_changed;
};

eloop_t *eloop_main = NULL;
static void eloop_context_ensure(eloop_t *eloop);
static void eloop_cleanup_events(eloop_t *eloop, stm_instance_t *fsmi);

static void eloop_notify_sock_recv(int sock, void *eloop_ctx, void *sock_ctx)
{
	eloop_t *eloop = (eloop_t *)sock_ctx;
	uint8_t buf[256];

	recv(eloop->notify_sock, buf, 256, 0);
}

static void eloop_reset_timeouts(eloop_t *eloop)
{
	uint8_t buf[1] = "";
	int res;

	if (eloop && eloop->notify_sock >= 0) {
		res = sendto(eloop->notify_sock, buf, 1, 0,
			     (struct sockaddr *)&eloop->notify_addr,
			     sizeof (struct sockaddr_in));
	}
}

static void eloop_context_ensure(eloop_t *eloop)
{
	int eusure_fd = -1;
	struct sockaddr_in saddr;
	int on = 1;

	if (eloop->readers.count <= 0) {
		if (eloop->notify_sock >= 0) {
			closesocket(eloop->notify_sock);
			eloop->notify_sock = -1;
		}

		eusure_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (eusure_fd < 0) {
			return;
		}

		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(54321);
		saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

 		(void) setsockopt(eusure_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

		if (bind(eusure_fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
			closesocket(eusure_fd);
			eusure_fd = -1;
		}
		eloop->notify_sock = eusure_fd;
		memcpy(&eloop->notify_addr, &saddr, sizeof (struct sockaddr_in));
		eloop_register_read_sock(eloop, eusure_fd, eloop_notify_sock_recv,
					 eloop, eloop);
	}
}

eloop_t *eloop_context_init(void *user_data)
{
	eloop_t *eloop = NULL;

	eloop = malloc(sizeof (eloop_t));
	if (eloop) {
		memset(eloop, 0, sizeof(eloop_t));
		eloop->user_data = user_data;
		INIT_LIST_HEAD(&eloop->pending_queue);
		INIT_LIST_HEAD(&eloop->running_queue);
		eloop->notify_sock = -1;
		atomic_set(&eloop->refcnt, 0);
	}

	return eloop;
}

static int eloop_fdset_t_add_sock(eloop_fdset_t *table,
				  int sock, eloop_sock_cb handler,
				  void *eloop_data, void *user_data)
{
	eloop_sock_t *tmp;

	if (table == NULL)
		return -1;

	tmp = (eloop_sock_t *)
		realloc(table->table,
			(table->count + 1) * sizeof(eloop_sock_t));
	if (tmp == NULL)
		return -1;

	tmp[table->count].sock = sock;
	tmp[table->count].eloop_data = eloop_data;
	tmp[table->count].user_data = user_data;
	tmp[table->count].handler = handler;
	table->count++;
	table->table = tmp;
	table->changed = 1;

	return 0;
}

static void eloop_fdset_t_remove_sock(eloop_fdset_t *table, sockfd_t sock)
{
	int i;

	if (table == NULL || table->table == NULL || table->count == 0)
		return;

	for (i = 0; i < table->count; i++) {
		if (table->table[i].sock == sock)
			break;
	}
	if (i == table->count)
		return;
	if (i != table->count - 1) {
		memmove(&table->table[i], &table->table[i + 1],
			(table->count - i - 1) *
			sizeof(eloop_sock_t));
	}
	table->count--;
	table->changed = 1;
}

static void eloop_fdset_t_set_fds(eloop_fdset_t *table, fd_set *fds)
{
	int i;

	FD_ZERO(fds);

	if (table->table == NULL)
		return;

	for (i = 0; i < table->count; i++)
		FD_SET(table->table[i].sock, fds);
}

static void eloop_fdset_t_dispatch(eloop_fdset_t *table, fd_set *fds)
{
	int i;

	if (table == NULL || table->table == NULL)
		return;

	table->changed = 0;
	for (i = 0; i < table->count; i++) {
		if (FD_ISSET(table->table[i].sock, fds)) {
			table->table[i].handler(table->table[i].sock,
						table->table[i].eloop_data,
						table->table[i].user_data);
			if (table->changed)
				break;
		}
	}
}

static void eloop_fdset_destroy(eloop_fdset_t *table)
{
	if (table)
		free(table->table);
}

void eloop_context_hold(eloop_t *eloop)
{
	if (!eloop)
		eloop = eloop_main;

	if (eloop)
		atomic_inc(&eloop->refcnt);
}

void eloop_context_unhold(eloop_t *eloop)
{
	if (!eloop)
		eloop = eloop_main;

	if (eloop)
		atomic_dec(&eloop->refcnt);
}

int eloop_register_read_sock(eloop_t *eloop, int sock, eloop_sock_cb handler,
			     void *eloop_data, void *user_data)
{
	if (!eloop)
		eloop = eloop_main;

	return eloop_register_sock(eloop, sock, EVENT_TYPE_READ, handler,
				   eloop_data, user_data);
}

int eloop_register_write_sock(eloop_t *eloop, int sock, eloop_sock_cb handler,
			      void *eloop_data, void *user_data)
{
	if (!eloop)
		eloop = eloop_main;

	return eloop_register_sock(eloop, sock, EVENT_TYPE_WRITE, handler,
				   eloop_data, user_data);
}

void eloop_unregister_read_sock(eloop_t *eloop, int sock)
{
	if (!eloop)
		eloop = eloop_main;

	eloop_unregister_sock(eloop, sock, EVENT_TYPE_READ);
}


void eloop_unregister_write_sock(eloop_t *eloop, int sock)
{
	if (!eloop)
		eloop = eloop_main;

	eloop_unregister_sock(eloop, sock, EVENT_TYPE_WRITE);
}

static eloop_fdset_t *eloop_get_sock_table(eloop_t *eloop, eloop_event_t type)
{
	if (!eloop)
		eloop = eloop_main;

	switch (type) {
	case EVENT_TYPE_READ:
		return &eloop->readers;
	case EVENT_TYPE_WRITE:
		return &eloop->writers;
	case EVENT_TYPE_EXCEPTION:
		return &eloop->exceptions;
	}

	return NULL;
}

int eloop_register_sock(eloop_t *eloop, int sock, eloop_event_t type,
			eloop_sock_cb handler,
			void *eloop_data, void *user_data)
{
	int res;
	eloop_fdset_t *table;

	if (!eloop)
		eloop = eloop_main;

	table = eloop_get_sock_table(eloop, type);
	res = eloop_fdset_t_add_sock(table, sock, handler,
				     eloop_data, user_data);
	if (sock > eloop->max_sock)
		eloop->max_sock = sock;
	return res;
}

void eloop_unregister_sock(eloop_t *eloop, int sock, eloop_event_t type)
{
	eloop_fdset_t *table;

	if (!eloop)
		eloop = eloop_main;

	table = eloop_get_sock_table(eloop, type);
	eloop_fdset_t_remove_sock(table, sock);
}

static eloop_alarm_t *__eloop_find_remove_timeout(eloop_t *eloop,
						  eloop_alarm_cb handler,
						  void *eloop_data,
						  void *user_data)
{
	eloop_alarm_t *timeout, *prev, *next;

	if (!eloop)
		eloop = eloop_main;

	prev = NULL;
	timeout = eloop->timeout;
	while (timeout != NULL) {
		next = timeout->next;

		if (timeout->handler == handler &&
		    (timeout->eloop_data == eloop_data ||
		     eloop_data == ELOOP_ALL_CTX) &&
		    (timeout->user_data == user_data ||
		     user_data == ELOOP_ALL_CTX)) {
			if (prev == NULL)
				eloop->timeout = next;
			else
				prev->next = next;
			return timeout;
		} else {
			prev = timeout;
		}
		timeout = next;
	}
	return NULL;
}

int eloop_register_timeout(eloop_t *eloop, int secs,
			   int usecs, eloop_alarm_cb handler,
			   void *eloop_data, void *user_data)
{
	eloop_alarm_t *timeout, *tmp, *prev;
	struct timeval tv;

	if (!eloop)
		eloop = eloop_main;

	BUG_ON(secs < 0);

	gettimeofday(&tv, NULL);
	tv.tv_sec += secs;
	tv.tv_usec += usecs;
	while (tv.tv_usec >= 1000000) {
		tv.tv_sec++;
		tv.tv_usec -= 1000000;
	}

	timeout = __eloop_find_remove_timeout(eloop, handler,
					      eloop_data, user_data);
	if (timeout) {
		if (timeval_before(&tv, &timeout->time))
			timeout->time = tv;
	} else {
		timeout = malloc(sizeof(*timeout));
		if (timeout == NULL)
			return -1;
		timeout->time = tv;
	}

	timeout->eloop_data = eloop_data;
	timeout->user_data = user_data;
	timeout->handler = handler;
	timeout->next = NULL;

	if (eloop->timeout == NULL) {
		eloop->timeout = timeout;
		eloop_reset_timeouts(eloop);
		return 0;
	}

	prev = NULL;
	tmp = eloop->timeout;
	while (tmp != NULL) {
		if (timeval_before(&timeout->time, &tmp->time))
			break;
		prev = tmp;
		tmp = tmp->next;
	}

	if (prev == NULL) {
		timeout->next = eloop->timeout;
		eloop->timeout = timeout;
	} else {
		timeout->next = prev->next;
		prev->next = timeout;
	}

	eloop_reset_timeouts(eloop);
	return 0;
}

void eloop_cancel_timeout(eloop_t *eloop, eloop_alarm_cb handler,
			  void *eloop_data, void *user_data)
{
	eloop_alarm_t *timeout;

	if (!eloop)
		eloop = eloop_main;

	timeout = __eloop_find_remove_timeout(eloop, handler,
					      eloop_data, user_data);
	if (timeout)
		free(timeout);
}

static void eloop_handle_alarm(int sig)
{
	fprintf(stderr,
		"eloop: could not process SIGINT or SIGTERM in two "
		"seconds. Looks like there\n"
		"is a bug that ends up in a busy loop that "
		"prevents clean shutdown.\n"
		"Killing program forcefully.\n");
	exit(1);
}

static void eloop_handle_signal(int sig)
{
	int i;

#ifndef WIN32
	if ((sig == SIGINT || sig == SIGTERM) && !eloop_main->pending_terminate) {
		/* use SIGALRM to break out from potential busy loops that
		 * would not allow the program to be killed. */
		eloop_main->pending_terminate = 1;
		signal(SIGALRM, eloop_handle_alarm);
		alarm(2);
	}
#endif /* WIN32 */

	eloop_main->signaled++;
	for (i = 0; i < eloop_main->signal_count; i++) {
		if (eloop_main->signals[i].sig == sig) {
			eloop_main->signals[i].signaled++;
			break;
		}
	}
}

static void eloop_process_pending_signals(void)
{
	int i;

	if (eloop_main->signaled == 0)
		return;
	eloop_main->signaled = 0;

	if (eloop_main->pending_terminate) {
#ifndef WIN32
		alarm(0);
#endif /* WIN32 */
		eloop_main->pending_terminate = 0;
	}

	for (i = 0; i < eloop_main->signal_count; i++) {
		if (eloop_main->signals[i].signaled) {
			eloop_main->signals[i].signaled = 0;
			eloop_main->signals[i].handler(eloop_main->signals[i].sig,
						       eloop_main->user_data,
						       eloop_main->signals[i].user_data);
		}
	}
}

int eloop_register_signal(int sig, eloop_signal_cb handler,
			  void *user_data)
{
	eloop_signal_t *tmp;

	tmp = (eloop_signal_t *)
		realloc(eloop_main->signals,
			(eloop_main->signal_count + 1) *
			sizeof(eloop_signal_t));
	if (tmp == NULL)
		return -1;

	tmp[eloop_main->signal_count].sig = sig;
	tmp[eloop_main->signal_count].user_data = user_data;
	tmp[eloop_main->signal_count].handler = handler;
	tmp[eloop_main->signal_count].signaled = 0;
	eloop_main->signal_count++;
	eloop_main->signals = tmp;
	signal(sig, eloop_handle_signal);

	return 0;
}

int eloop_register_signal_terminate(eloop_signal_cb handler,
				    void *user_data)
{
	int ret = eloop_register_signal(SIGINT, handler, user_data);
	if (ret == 0)
		ret = eloop_register_signal(SIGTERM, handler, user_data);
	return ret;
}

int eloop_register_signal_reconfig(eloop_signal_cb handler,
				   void *user_data)
{
#ifdef SIGHUP
	return eloop_register_signal(SIGHUP, handler, user_data);
#else
	return 0;
#endif
}

#define for_each_event(q, t)		\
	list_for_each_entry(eloop_fsmevt_t, t, &q, link)
#define for_each_event_safe(q, t, n)	\
	list_for_each_entry_safe(eloop_fsmevt_t, t, n, &q, link)

#define ELOOP_EVENT_RES_PENDING		-1
#define ELOOP_EVENT_RES_UNKNOWN		0
#define ELOOP_EVENT_RES_HANDLED		1

static int eloop_raise_event(eloop_fsmevt_t *fsme)
{
	stm_instance_t *stmi = fsme->inst;
	int event = fsme->event;
	void *data = fsme->data;
	stm_table_t const *stm; 
	stm_entry_t const *stme;

	if (stmi->func) {
		stmi->func(stmi, event, data);
		return ELOOP_EVENT_RES_HANDLED;
	}

	stm = stmi->table;
	stme = &stm->stm[0];
	
	assert(event >= 0 && stmi->state < stm->num_states);

	if ((event >= stm->num_events)) {
		if (stm->log) {
			(*stm->log)(stmi, STM_LOG_ERR,
				    "STM [%s(%s)]: invalid event, event=%s",
				    stm->name, stmi->name,
				    stm->event_names[event]);
		}
		return ELOOP_EVENT_RES_UNKNOWN;
	}

	while (stme->action != NULL) {
		if ((event == stme->event) && (stmi->state == stme->state)) {
			int idx;

			/* run all registered actions */
			for (idx = 0;
			     stme->action && idx < stme->n_actions; idx++) {
				if (stme->action[idx]) {
					if (!(*stme->action[idx])(stmi, data) ||
					    fsme->state == ELOOP_EVENT_STATE_DEACTIVE) {
						return ELOOP_EVENT_RES_PENDING;
					}
					if (fsme->state == ELOOP_EVENT_STATE_DEACTIVE)
						return ELOOP_EVENT_RES_HANDLED;
				}
			}
			stm_enter_state(stmi, stme->new_state);
			return ELOOP_EVENT_RES_HANDLED;
		}
		stme++;
	}

	/* undefined event */
	if (stm->log) {
		(*stm->log)(stmi, STM_LOG_ERR,
			    "STM [%s(%s)]: unhandled event, state=%s, event=%s",
			    stm->name, stmi->name,
			    stm->state_names[stmi->state],
			    stm->event_names[event]);
	}
	return ELOOP_EVENT_RES_UNKNOWN;
}

static void eloop_push_event(struct list_head *head, eloop_fsmevt_t *fsme)
{
	/* add to tail */
	INIT_LIST_HEAD(&fsme->link);
	list_add_tail(&fsme->link, head);
}

static eloop_fsmevt_t *eloop_pop_event(struct list_head *head)
{
	eloop_fsmevt_t *fsme;

	if (list_empty(head)) {
		return NULL;
	}
	fsme = list_entry(head->next, eloop_fsmevt_t, link);
	list_del_init(&fsme->link);
	return fsme;
}

#if 0
static const char *eloop_stm_state2str(int state)
{
	switch (state) {
	case ELOOP_EVENT_STATE_ACTIVE:
		return "ACTIVE";
	case ELOOP_EVENT_STATE_DEACTIVE:
		return "DEACTIVE";
	case ELOOP_EVENT_STATE_PENDING:
		return "PENDING";
	default:
		return "UNKNOWN";
	}
}
#endif

static void eloop_process_pending_events(void *ununsed, void *user_data)
{
	eloop_t *eloop = (eloop_t *)user_data;
	eloop_fsmevt_t *fsme, *n;
	LIST_HEAD(pending_events);
	LIST_HEAD(dead_events);
	int res;
	time_t now;

	if (!eloop)
		eloop = eloop_main;

	fsme = eloop_pop_event(&eloop->pending_queue);

	while (fsme) {
		time(&now);

		if (fsme->state != ELOOP_EVENT_STATE_DEACTIVE) {
			if (fsme->state == ELOOP_EVENT_STATE_PENDING) {
				if (now - fsme->last_pending > ELOOP_EVENT_MAX_PENDING) {
					fsme->state = ELOOP_EVENT_STATE_DEACTIVE;
					eloop_push_event(&dead_events, fsme);
					goto next;
				} else {
					if (fsme->pending_state == fsme->inst->state) {
						eloop_push_event(&pending_events, fsme);
						goto next;
					}
				}
			}

			fsme->last_pending = now;
			fsme->pending_state = fsme->inst->state;

			if (!fsme->inst->func) {
#if 0
				log_kern(LOG_DEBUG,
					 "STM [%s(%s)]: process event, state=%s, event=%s",
					 fsme->inst->table->name,
					 fsme->inst->name,
					 fsme->inst->table->state_names[fsme->inst->state],
					 fsme->inst->table->event_names[fsme->event]);
#endif
			}

			eloop_push_event(&eloop->running_queue, fsme);
			res = eloop_raise_event(fsme);
			list_del_init(&fsme->link);

			if (ELOOP_EVENT_RES_PENDING == res &&
			    fsme->state != ELOOP_EVENT_STATE_DEACTIVE) {
				if (now - fsme->last_pending > ELOOP_EVENT_MAX_PENDING) {
					fsme->state = ELOOP_EVENT_STATE_DEACTIVE;
					eloop_push_event(&dead_events, fsme);
				} else {
					/* event handler want pending */
					fsme->state = ELOOP_EVENT_STATE_PENDING;
					eloop_push_event(&pending_events, fsme);
				}
			} else {
				/* unknown event: res == 0
				 * handled event: res == 1
				 * deleted event: state == ELOOP_EVENT_STATE_DEACTIVE
				 */
				fsme->state = ELOOP_EVENT_STATE_DEACTIVE;
				eloop_push_event(&dead_events, fsme);
			}
		} else {
			eloop_push_event(&dead_events, fsme);
		}
next:
		fsme = eloop_pop_event(&eloop->pending_queue);
	}

	for_each_event_safe(dead_events, fsme, n) {
		if (fsme->state == ELOOP_EVENT_STATE_DEACTIVE) {
			list_del(&fsme->link);
			stm_table_free(fsme->inst);
			free(fsme);
		}
	}

	list_splice(&pending_events, &eloop->pending_queue);

	if (list_empty(&eloop->pending_queue)) {
		eloop->events_pending = 0;
		eloop_cancel_timeout(eloop, eloop_process_pending_events,
				     NULL, eloop);
	} else {
		eloop_register_timeout(eloop, 1, 0,
				       eloop_process_pending_events,
				       NULL, eloop);
	}
}

stm_instance_t *eloop_create_automaton(const char name[16],
				       stm_callback_cb func, unsigned long data,
				       const stm_table_t *table, int state)
{
	return stm_table_new(name, func, data, table, state);
}

void eloop_delete_automaton(stm_instance_t *fsmi)
{
	if (fsmi) {
		eloop_cleanup_events(NULL, fsmi);
		stm_table_free(fsmi);
	}
}

void eloop_send_event(eloop_t *eloop, stm_instance_t *fsmi,
		      int event, void *data)
{
	if (!eloop)
		eloop = eloop_main;

	stm_raise_event(fsmi, event, data);
}

void eloop_post_event(eloop_t *eloop, stm_instance_t *fsmi,
		      int event, void *data)
{
	eloop_fsmevt_t *fsme = malloc(sizeof (eloop_fsmevt_t));

	if (!eloop)
		eloop = eloop_main;

	if (fsme) {
		fsme->event = event;
		fsme->data = data;
		fsme->inst = stm_table_get(fsmi);
		fsme->state = ELOOP_EVENT_STATE_ACTIVE;
		time(&fsme->last_pending);

		BUG_ON(!fsme->inst);

		INIT_LIST_HEAD(&fsme->link);
		eloop_push_event(&eloop->pending_queue, fsme);

		if (!eloop->events_pending) {
			eloop->events_pending = 1;
			eloop_register_timeout(eloop, 0, 0,
					       eloop_process_pending_events,
					       NULL, eloop);
		}
	}
}

void eloop_cancel_event(eloop_t *eloop, stm_instance_t *fsmi,
			int event, void *data)
{
	eloop_fsmevt_t *fsme;

	if (!eloop)
		eloop = eloop_main;

	for_each_event(eloop->pending_queue, fsme) {
		if (fsme->inst == fsmi && fsme->event == event &&
		    fsme->data == data) {
#if 0
			log_kern(LOG_DEBUG,
				 "STM [%s(%s)]: cancel event, event=%s",
				 fsmi->table->name,
				 fsmi->name,
				 fsmi->table->event_names[fsme->event]);
#endif
			fsme->state = ELOOP_EVENT_STATE_DEACTIVE;
			fsme->data = NULL;
		}
	}
	for_each_event(eloop->running_queue, fsme) {
		if (fsme->inst == fsmi && fsme->event == event &&
		    fsme->data == data) {
#if 0
			log_kern(LOG_DEBUG,
				 "STM [%s(%s)]: cancel event, event=%s",
				 fsmi->table->name,
				 fsmi->name,
				 fsmi->table->event_names[fsme->event]);
#endif
			fsme->state = ELOOP_EVENT_STATE_DEACTIVE;
			fsme->data = NULL;
		}
	}
}

void eloop_cleanup_events(eloop_t *eloop, stm_instance_t *fsmi)
{
	eloop_fsmevt_t *fsme;

	if (!eloop)
		eloop = eloop_main;

	for_each_event(eloop->pending_queue, fsme) {
		if (fsme->inst == fsmi) {
#if 0
			log_kern(LOG_DEBUG,
				 "STM [%s(%s)]: cancel event, event=%s",
				 fsmi->table->name,
				 fsmi->name,
				 fsmi->table->event_names[fsme->event]);
#endif
			fsme->state = ELOOP_EVENT_STATE_DEACTIVE;
			fsme->data = NULL;
		}
	}
	for_each_event(eloop->running_queue, fsme) {
		if (fsme->inst == fsmi) {
#if 0
			log_kern(LOG_DEBUG,
				 "STM [%s(%s)]: cancel event, event=%s",
				 fsmi->table->name,
				 fsmi->name,
				 fsmi->table->event_names[fsme->event]);
#endif
			fsme->state = ELOOP_EVENT_STATE_DEACTIVE;
			fsme->data = NULL;
		}
	}
}

static void eloop_events_destroy(eloop_t *eloop)
{
	eloop_fsmevt_t *fsme, *n;

	if (!eloop)
		eloop = eloop_main;

	for_each_event_safe(eloop->pending_queue, fsme, n) {
		list_del_init(&fsme->link);
		free(fsme);
	}
}

int eloop_sock_running(eloop_t *eloop)
{
	BUG_ON(!eloop);

	return eloop->readers.count > 0 ||
	       eloop->writers.count > 0 || eloop->exceptions.count > 0;
}

void eloop_context_run(eloop_t *eloop)
{
	fd_set *rfds, *wfds, *efds;
	int res;
	struct timeval tv, now;

	if (!eloop)
		eloop = eloop_main;

	eloop_context_ensure(eloop);

	rfds = malloc(sizeof(*rfds));
	wfds = malloc(sizeof(*wfds));
	efds = malloc(sizeof(*efds));
	if (rfds == NULL || wfds == NULL || efds == NULL) {
		printf("eloop_run - malloc failed\n");
		goto out;
	}

	while ((atomic_read(&eloop->refcnt) != 0) ||
	       !eloop->terminate ||
	       eloop->timeout || eloop_sock_running(eloop)) {

		if (eloop->timeout) {
			gettimeofday(&now, NULL);
			if (timeval_before(&now, &eloop->timeout->time))
				timeval_sub(&eloop->timeout->time, &now, &tv);
			else {
				tv.tv_sec = tv.tv_usec = 0;
			}
#if 0
			printf("next timeout in %lu.%06lu sec\n",
			       tv.tv_sec, tv.tv_usec);
#endif
		} else {
			tv.tv_usec = 0;
			tv.tv_sec = 1;
		}

		if (eloop_sock_running(eloop)) {
			eloop_fdset_t_set_fds(&eloop->readers, rfds);
			eloop_fdset_t_set_fds(&eloop->writers, wfds);
			eloop_fdset_t_set_fds(&eloop->exceptions, efds);
			res = select(eloop->max_sock + 1, rfds, wfds, efds, &tv);
			if (res < 0 &&
#ifdef EINTR
			    errno != EINTR &&
#endif
			    errno != 0) {
				perror("select");
				goto out;
			}
		} else {
			sleep(tv.tv_sec);
			usleep(tv.tv_usec);
		}
		eloop_process_pending_signals();

		/* check if some registered timeouts have occurred */
		/* TODO: add time precision */
		if (eloop->timeout) {
			eloop_alarm_t *tmp;

			gettimeofday(&now, NULL);
			if (!timeval_before(&now, &eloop->timeout->time)) {
				tmp = eloop->timeout;
				eloop->timeout = eloop->timeout->next;
				tmp->handler(tmp->eloop_data,
					     tmp->user_data);
				free(tmp);
			}
		}

		if (res <= 0)
			continue;

		eloop_fdset_t_dispatch(&eloop->readers, rfds);
		eloop_fdset_t_dispatch(&eloop->writers, wfds);
		eloop_fdset_t_dispatch(&eloop->exceptions, efds);
	}

out:
	free(rfds);
	free(wfds);
	free(efds);
}

void eloop_terminate(eloop_t *eloop)
{
	if (!eloop)
		eloop = eloop_main;

	eloop->terminate = 1;
}

void eloop_context_free(eloop_t *eloop)
{
	eloop_alarm_t *timeout, *prev;

	if (!eloop)
		eloop = eloop_main;

	timeout = eloop->timeout;
	while (timeout != NULL) {
		prev = timeout;
		timeout = timeout->next;
		free(prev);
	}
	eloop_fdset_destroy(&eloop->readers);
	eloop_fdset_destroy(&eloop->writers);
	eloop_fdset_destroy(&eloop->exceptions);
	eloop_events_destroy(eloop);
	free(eloop->signals);
	free(eloop);
}

int eloop_terminated(eloop_t *eloop)
{
	return eloop ? eloop->terminate : eloop_main->terminate;
}

void *eloop_get_user_data(eloop_t *eloop)
{
	return eloop ? eloop->user_data : eloop_main->user_data;
}

int __endianess = 0;

static void detect_endianess(void)
{
	union {
		uint16_t i;
		char ch;
	} eb_test = {1};
	if (eb_test.ch == 0) {
		__endianess = ENDIAN_BIG;
	} else {
		__endianess = ENDIAN_LITTLE;
	}
}

int __init eloop_init(void)
{
	detect_endianess();
	if (!eloop_main)
		eloop_main = eloop_context_init(NULL);
	BUG_ON(!eloop_main);
	return eloop_main ? 0 : 1;
}

void __exit eloop_exit(void)
{
	if (eloop_main)
		eloop_context_free(eloop_main);
}

#ifdef CONFIG_TEST_BYTEORDER
void test_byteorder(void)
{
	__u16 w = 0x0123U;
	__u32 q = 0x01234567UL;
	__u64 o = ULL(0x0123456789abcdef);

	printf("Endianess: %s\n", 
	       (__endianess == ENDIAN_BIG) ? "Big Endian" : "Little Endian");
	printf("cpu(%016x) -> le16(%016x)\r\n", w, cpu_to_le16(w));
	printf("cpu(%016x) -> be16(%016x)\r\n", w, cpu_to_be16(w));
	printf("cpu(%016lx) -> le32(%016lx)\r\n", q, cpu_to_le32(q));
	printf("cpu(%016lx) -> be32(%016lx)\r\n", q, cpu_to_be32(q));
	printf("cpu(%016I64x) -> le64(%016I64x)\r\n", o, cpu_to_le64(o));
	printf("cpu(%016I64x) -> be64(%016I64x)\r\n", o, cpu_to_be64(o));

	printf("le16(%016x) -> cpu(%016x)\r\n", w, le16_to_cpu(w));
	printf("be16(%016x) -> cpu(%016x)\r\n", w, be16_to_cpu(w));
	printf("le32(%016lx) -> cpu(%016lx)\r\n", q, le32_to_cpu(q));
	printf("be32(%016lx) -> cpu(%016lx)\r\n", q, be32_to_cpu(q));
	printf("le64(%016I64x) -> cpu(%016I64x)\r\n", o, le64_to_cpu(o));
	printf("be64(%016I64x) -> cpu(%016I64x)\r\n", o, be64_to_cpu(o));
}
#endif
