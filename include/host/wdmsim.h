#ifndef __WDMSIM_H_INCLUDE__
#define __WDMSIM_H_INCLUDE__

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <assert.h>
#include <host/circbf.h>
#include <host/bitops.h>
#include <host/list.h>

#define BUG_ON(exp)	assert(!(exp))
#define BUG()		BUG_ON(TRUE);

unsigned long cmpxchg(unsigned long *dest,
		      unsigned long oldv, unsigned long newv);
unsigned long xchg(unsigned long *dest, unsigned long val);

typedef int status_t;
#define STATUS_SUCCESS			ERROR_SUCCESS
#define STATUS_DEVICE_BUSY		ERROR_BUSY
#define STATUS_INVALID_DEVICE_STATE	ERROR_ACCESS_DENIED
#define STATUS_INSUFFICIENT_RESOURCES	ERROR_OUTOFMEMORY
#ifdef WIN32_NO_STATUS
#define STATUS_TIMEOUT			ERROR_TIMEOUT
#endif
#define OS_IS_SUCCESS(status)		((status) == STATUS_SUCCESS)

#define OS_DBG_SIZE		1024
#define OS_DBG_FAIL		0
#define OS_DBG_ERR		0
#define OS_DBG_WARN		0
#define OS_DBG_INFO		0
#define OS_DBG_DEBUG		0

void os_dbg(int level, const char *format, ...);

#define POLL_READ		0x01
#define POLL_WRITE		0x02
#define POLL_ERROR		0x04

void os_mem_copy(void *target, const void *source, size_t length);
void os_mem_move(void *target, const void *source, size_t length);

typedef void *os_device;
#define dev_priv(dev)		(dev)
typedef HANDLE os_waiter;

#define OS_WAIT_INFINITE	0xFFFFFFFF
void os_waiter_init(os_waiter *waiter, BOOLEAN value);
void os_waiter_exit(os_waiter *waiter);
void os_waiter_wake(os_waiter *waiter);
void os_waiter_idle(os_waiter *waiter);
BOOLEAN os_waiter_wait(os_waiter *waiter, unsigned long usecs);

typedef CRITICAL_SECTION os_mutex_t;

void os_mutex_init(os_mutex_t *mutex);
void os_mutex_exit(os_mutex_t *mutex);
void os_mutex_lock(os_mutex_t *mutex);
void os_mutex_unlock(os_mutex_t *mutex);

typedef void (*os_worker_cb)(os_device dev, struct os_work_item *work);

struct os_work_item {
	long data;
	int pending;
	struct list_head entry;
	os_worker_cb func;
	char user[1];
};

#define work_priv(_work)	((void *)((_work)->user))

struct os_work_queue *os_worker_create_queue(os_device dev);
void os_worker_delete_queue(struct os_work_queue *wq);
int os_worker_queue_item(struct os_work_queue *wq, struct os_work_item *work);
void os_worker_init_item(struct os_work_item *item, os_worker_cb func);

#if 0
#define wait_for_interact()	getch()
#else
#define wait_for_interact()
#endif

typedef struct os_timer os_timer;

typedef void (*os_timer_cb)(os_timer *timer, os_device dev, void *data);

struct os_timer {
	int stopping;
	os_timer_cb timer_func;
	void *timer_data;
	os_device dev;
	HANDLE timer_queue;
	HANDLE timer;
	os_mutex_t lock;
};

os_timer *os_timer_register(os_device dev,
			    os_timer_cb call, void *data);
void os_timer_unregister(os_timer *timer);
void os_timer_schedule(os_timer *timer, uint32_t usecs);
void os_timer_discard(os_timer *timer);

typedef int usb_status_t;
typedef void os_usbif;
#define usb_dev_priv(usb)	(usb)

struct os_usb_urb {
	uint16_t length;
	usb_status_t status;
	char priv[1];
};
#define urb_priv(_urb)		((void *)((_urb)->priv))

#define USB_EID_DEFAULT		0

struct os_work_queue *os_usb_get_pipe(void *usb);
void os_usb_urb_init(struct os_usb_urb *urb);
void os_usb_set_urb_length(struct os_usb_urb *urb, size_t length);
void os_usb_set_urb_status(struct os_usb_urb *urb, usb_status_t status);
size_t os_usb_get_urb_length(os_device dev, uint8_t eid);
usb_status_t os_usb_get_urb_status(os_device dev, uint8_t eid);
struct os_usb_urb *os_usb_urb_by_eid(os_device dev, uint8_t eid);

#define USBD_STATUS_SUCCESS		0x00
#define USBD_STATUS_HALTED		0x01
#define USBD_STATUS_ERROR_BUSY		0x02
#define USBD_STATUS_STATUS_NOT_MAPPED	0x03
#define USBD_IS_SUCCESS(status)		((status) == USBD_STATUS_SUCCESS)

#endif /* __WDMSIM_H_INCLUDE__ */
