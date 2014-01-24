#include <host/wdmsim.h>

void os_dbg(int level, const char *format, ...)
{
	char buf[OS_DBG_SIZE];
	va_list args;

	va_start(args, format);
	_vsnprintf(buf, OS_DBG_SIZE, format, args);
	printf(buf);
	va_end(args);
}

unsigned long cmpxchg(unsigned long *dest, unsigned long oldv, unsigned long newv)
{
	return (unsigned long)InterlockedCompareExchange(dest, newv, oldv);
}

unsigned long xchg(unsigned long *dest, unsigned long val)
{
	return InterlockedExchange(dest, val);
}

void os_waiter_init(os_waiter *waiter, BOOLEAN value)
{
	(*waiter) = CreateEvent(0, TRUE, value, 0);
}

void os_waiter_exit(os_waiter *waiter)
{
	CloseHandle(*waiter);
}

void os_waiter_wake(os_waiter *waiter)
{
	SetEvent(*waiter);
}

void os_waiter_idle(os_waiter *waiter)
{
	ResetEvent(*waiter);
}

BOOLEAN os_waiter_wait(os_waiter *waiter, unsigned long usecs)
{
	DWORD timeout;
	DWORD res;

	if (usecs == OS_WAIT_INFINITE) {
		timeout = INFINITE;
	} else {
		timeout = usecs / 1000;
	}
	res = WaitForSingleObject(*waiter, timeout);
	switch (res) {
	case WAIT_OBJECT_0:
		return TRUE;
	default:
		BUG();
	case WAIT_TIMEOUT:
		return FALSE;
	}
	return FALSE;
}

void os_mutex_init(os_mutex_t *mutex)
{
	InitializeCriticalSection(mutex);
}

void os_mutex_exit(os_mutex_t *mutex)
{
	DeleteCriticalSection(mutex);
}

void os_mutex_lock(os_mutex_t *mutex)
{
	EnterCriticalSection(mutex);
}

void os_mutex_unlock(os_mutex_t *mutex)
{
	LeaveCriticalSection(mutex);
}

struct os_work_queue {
	os_device dev;
	os_mutex_t queue_lock;
	struct list_head worklist;
	os_waiter alive;
	struct os_work_item *current_work;
	struct os_work_queue *wq;
	int run_depth;
	int stopping;
};

#define wdm_work_test_pending(_work)	\
	((_work)->pending)
#define wdm_work_clear_pending(_work)	\
	((_work)->pending = 0)

void os_worker_init_item(struct os_work_item *item, os_worker_cb func)
{
	item->pending = 0;
	item->data = 0;
	INIT_LIST_HEAD(&item->entry);
	item->func = func;
}

void wdm_worker_set_data(struct os_work_item *work,
			 struct os_work_queue *wq)
{
	BUG_ON(!wdm_work_test_pending(work));

	work->pending = 1;
	work->data = (long)wq;
}

struct os_work_queue *wdm_worker_get_data(struct os_work_item *work)
{
	return (void *) (work->data);
}

void wdm_worker_run_queue(struct os_work_queue *wq)
{
	os_mutex_lock(&wq->queue_lock);
	wq->run_depth++;
	if (wq->run_depth > 3) {
		printf("recursion depth exceeded: %d\n", wq->run_depth);
	}
	while (!list_empty(&wq->worklist)) {
		struct os_work_item *work = list_entry(wq->worklist.next,
						struct os_work_item, entry);
		os_worker_cb f = work->func;

		wq->current_work = work;
		list_del_init(&work->entry);
		os_mutex_unlock(&wq->queue_lock);

		BUG_ON(wdm_worker_get_data(work) != wq);
		wdm_work_clear_pending(work);
		f(wq->dev, work);

		os_mutex_lock(&wq->queue_lock);
		wq->current_work = NULL;
	}
	wq->run_depth--;
	os_mutex_unlock(&wq->queue_lock);
}

static int wdm_worker_thread(void *__wq)
{
	struct os_work_queue *wq = __wq;
	while (!wq->stopping) {
		wdm_worker_run_queue(wq);
	}
	os_waiter_wake(&wq->alive);
	_endthread();
	return 0;
}

struct os_work_queue *os_worker_create_queue(os_device dev)
{
	struct os_work_queue *wq;
	int err = 0;

	wq = malloc(sizeof(struct os_work_queue));
	if (!wq) {
		return NULL;
	}
	memset(wq, 0, sizeof(struct os_work_queue));
	os_mutex_init(&wq->queue_lock);
	wq->dev = dev;
	INIT_LIST_HEAD(&wq->worklist);
	os_waiter_init(&wq->alive, FALSE);
	_beginthread(wdm_worker_thread, 0, wq);
	return wq;
}

void os_worker_delete_queue(struct os_work_queue *wq)
{
	if (wq) {
		wq->stopping = TRUE;
		os_waiter_wait(&wq->alive, OS_WAIT_INFINITE);
		os_waiter_exit(&wq->alive);
		os_mutex_exit(&wq->queue_lock);
		free(wq);
	}
}

static void wdm_worker_insert_item(struct os_work_queue *wq,
				   struct os_work_item *work, int tail)
{
	wdm_worker_set_data(work, wq);
	/*
	 * Ensure that we get the right work->data if we see the
	 * result of list_add() below, see try_to_grab_pending().
	 */
	if (tail)
		list_add_tail(&work->entry, &wq->worklist);
	else
		list_add(&work->entry, &wq->worklist);
}

static void wdm_worker_queue_item(struct os_work_queue *wq,
				  struct os_work_item *work)
{
	os_mutex_lock(&wq->queue_lock);
	wdm_worker_insert_item(wq, work, 1);
	os_mutex_unlock(&wq->queue_lock);
}

int os_worker_queue_item(struct os_work_queue *wq,
			 struct os_work_item *work)
{
	int ret = 0;

	if (cmpxchg(&work->pending, 0, 1) == 0) {
		BUG_ON(!list_empty(&work->entry));
		wdm_worker_queue_item(wq, work);
		ret = 1;
	}
	return ret;
}

void os_usb_urb_init(struct os_usb_urb *urb)
{
	urb->length = 0;
	urb->status = 0;
}

void os_usb_set_urb_length(struct os_usb_urb *urb, size_t length)
{
	urb->length = length;
}

void os_usb_set_urb_status(struct os_usb_urb *urb, usb_status_t status)
{
	urb->status = status;
}

usb_status_t os_usb_get_urb_status(os_device dev, uint8_t eid)
{
	os_usbif *usb = dev_priv(dev);
	struct os_work_queue *work = os_usb_get_pipe(usb);
	struct os_usb_urb *urb = work_priv(work->current_work);
	return urb->status;
}

void os_timer_unregister(os_timer *__t)
{
	if (__t) {
		__t->stopping = 1;
		os_timer_discard(__t);
		os_mutex_exit(&__t->lock);
		if (__t->dev) {
			__t->dev = NULL;
		}
		if (__t->timer_queue)
			DeleteTimerQueue(__t->timer_queue);
		free(__t);
	}
}

os_timer *os_timer_register(os_device dev,
			    os_timer_cb call, void *data)
{
	os_timer *__t;
	__t = malloc(sizeof(os_timer));
	if (__t) {
		memset(__t, 0, sizeof(os_timer));
		__t->stopping = 0;
		__t->timer_func = call;
		__t->timer_data = data;
		__t->dev = dev;
		__t->timer_queue = CreateTimerQueue();
		if (!__t->timer_queue) {
			os_timer_unregister(__t);
			return NULL;
		}
		os_mutex_init(&__t->lock);
	}
	return (void*)__t;
}

VOID CALLBACK wdm_timer_callback(PVOID param, BOOLEAN TimerOrWaitFired)
{
	os_timer *__t = (os_timer *)param;
	os_timer_discard(__t);
	BUG_ON(!__t->timer_func);
	__t->timer_func(__t, __t->dev, __t->timer_data);
}

void os_timer_schedule(os_timer *__t, uint32_t usecs)
{
	os_mutex_lock(&__t->lock);
	if (!__t->stopping && !__t->timer) {
		CreateTimerQueueTimer(&__t->timer,
				      __t->timer_queue,
				      wdm_timer_callback,
				      __t,
				      usecs/1000,
				      0,
				      WT_EXECUTEINTIMERTHREAD | WT_EXECUTEONLYONCE);
	}
	os_mutex_unlock(&__t->lock);
}

void os_timer_discard(os_timer *__t)
{
	os_mutex_lock(&__t->lock);
	if (__t->timer) {
		DeleteTimerQueueTimer(__t->timer_queue,
				      __t->timer,
				      NULL);
		__t->timer = NULL;
	}
	os_mutex_unlock(&__t->lock);
}

void os_mem_copy(void *target, const void *source, size_t length)
{
	memcpy(target, source, length);
}

void os_mem_move(void *target, const void *source, size_t length)
{
	memmove(target, source, length);
}
