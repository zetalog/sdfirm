#ifndef __OS_DEV_H_INCLUDE__
#define __OS_DEV_H_INCLUDE__

#include <wdm/wdm_dev.h>
#include <host/wdm.h>
#include <host/bitops.h>
#include <host/list.h>

struct os_driver {
	status_t (*start)(os_device dev);
	void (*stop)(os_device dev);
	/* memory allocation */
	status_t (*init)(os_device dev);
	void (*exit)(os_device dev);
	/* power management */
	void (*suspend)(os_device dev);
	void (*resume)(os_device dev);

	status_t (*open)(os_device dev, os_file *file);
	void (*close)(os_device dev, os_file *file);
	status_t (*ioctl)(os_device dev, os_file *file,
			  unsigned long code, void *arg,
			  int *length);
	status_t (*read)(os_device dev, os_file *file,
			 char *buf, size_t *count);
	status_t (*write)(os_device dev, os_file *file,
			  const char *buf, size_t *count);
};

#ifdef CONFIG_DEBUG
struct os_mem_chunk {
	const char *file;
	int line;
	size_t size;
	const char *what;
	struct list_head link;
	uint8_t priv[1];
};

void *__os_mem_alloc(size_t size, const char *what,
		     const char *file, int line);
void __os_mem_free(void *p, const char *file, int line);
os_device os_dev_get_kern(os_device dev, const char *what);
refcnt_t os_dev_inc_kern(os_device dev, const char *what);
refcnt_t os_dev_dec_kern(os_device dev, const char *what);
refcnt_t os_dev_inc_user(os_device dev, const char *what);
refcnt_t os_dev_dec_user(os_device dev, const char *what);
void os_mem_leak_init(void);
void os_mem_leak_dump(void);

#define os_mem_alloc(size, what)	__os_mem_alloc(size, what, __FILE__, __LINE__)
#define os_mem_free(p)			__os_mem_free(p, __FILE__, __LINE__)
#else
void *__os_mem_alloc(size_t size);
void __os_mem_free(void *p);
os_device __os_dev_get_kern(os_device dev);
refcnt_t __os_dev_inc_kern(os_device dev);
refcnt_t __os_dev_dec_kern(os_device dev);
refcnt_t __os_dev_inc_user(os_device dev);
refcnt_t __os_dev_dec_user(os_device dev);
#define os_mem_leak_init()
#define os_mem_leak_dump()

#define os_mem_alloc(size, what)	__os_mem_alloc(size)
#define os_mem_free(p)			__os_mem_free(p)
#define os_dev_get_kern(dev, what)	__os_dev_get_kern(dev)
#define os_dev_inc_kern(dev, what)	__os_dev_inc_kern(dev)
#define os_dev_dec_kern(dev, what)	__os_dev_dec_kern(dev)
#define os_dev_inc_user(dev, what)	__os_dev_inc_user(dev)
#define os_dev_dec_user(dev, what)	__os_dev_dec_user(dev)
#endif
#define os_dev_put_kern(dev, what)	os_dev_dec_kern(dev, what)
void os_mem_zero(void *target, size_t length);
void os_mem_copy(void *target, const void *source, size_t length);
void os_mem_move(void *target, const void *source, size_t length);

os_timer *os_timer_register(os_device dev,
			    os_timer_cb call, void *data);
void os_timer_unregister(os_timer *timer);
void os_timer_schedule(os_timer *timer, uint32_t usecs);
void os_timer_discard(os_timer *timer);

void os_worker_create_queue(os_device dev,
			    struct os_work_queue *queue,
			    os_work_aval_cb aval,
			    os_work_cmpl_cb cmpl,
			    void *data);
void *os_worker_queue_data(struct os_work_queue *queue);
void os_worker_delete_queue(struct os_work_queue *queue);
struct os_work_item *os_worker_alloc_item(struct os_work_queue *__q,
					  int extrasize,
					  const char *what);
void os_worker_free_item(struct os_work_item *__i);
status_t os_worker_queue_item(struct os_work_queue *__q,
			      struct os_work_item *__i);
void os_worker_suspend_queue(struct os_work_queue *__q);
void os_worker_resume_queue(struct os_work_queue *__q);
void *os_worker_item_data(struct os_work_item *item);

#define OS_WAIT_INFINITE	0xFFFFFFFF

void os_waiter_init(os_waiter *waiter, BOOLEAN wake);
void os_waiter_wake(os_waiter *waiter);
void os_waiter_idle(os_waiter *waiter);
BOOLEAN os_waiter_wait(os_waiter *waiter, unsigned long usecs);

#define OS_DEVICE_REMOVE	0x01
#define OS_DEVICE_HOTPLUG	0x02
#define OS_DEVICE_WAKEUP	0x04
#define OS_DEVICE_SUSPEND	0x08

void *dev_priv(os_device dev);
const char *os_dev_id(os_device dev);
status_t os_dev_register_driver(struct os_driver *drv, size_t objsize,
				unsigned long flags);

int os_dev_can_stop(os_device dev);
void os_dev_wait_stopped(os_device dev);
int os_dev_can_remove(os_device dev);
void os_dev_wait_removed(os_device dev);
void os_dev_ref_init(os_device dev);
void os_dev_ref_exit(os_device dev);

#include "os_dbg.h"

/* driver entries */
extern const GUID *os_driver_fdo_guid;
status_t os_driver_init(void);
#ifdef CONFIG_SYSCTL
extern int os_driver_wmi_nrguids;
extern WMIGUIDREGINFO os_driver_wmi_guids[];
#endif

#endif /* __OS_DEV_H_INCLUDE__ */
