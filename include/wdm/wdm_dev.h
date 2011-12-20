#ifndef __WDM_DEV_H_INCLUDE__
#define __WDM_DEV_H_INCLUDE__

#include <wdm.h>
#include <ntddk.h>
#include <wmilib.h>
#include <wmistr.h>
#include <wdmguid.h>
#include <ntintsafe.h>
#include <wdmsec.h> /* for IoCreateDeviceSecure */

/* Let us use newly introduced (Windows Server 2003 DDK) safe string
 * function to avoid security issues related buffer overrun.  The
 * advantages of the RtlStrsafe functions include:
 * 1) The size of the destination buffer is always provided to the
 *    function to ensure that the function does not write past the end of
 *    the buffer.
 * 2) Buffers are guaranteed to be null-terminated, even if the operation
 * truncates the intended result.
 */
/* In this driver we are using a safe version swprintf, which is
 * RtlStringCchPrintfW.  To use strsafe function on 9x, ME, and Win2K
 * Oses, we have to define NTSTRSAFE_LIB before including this header file
 * and explicitly linking to ntstrsafe.lib. If your driver is just
 * targeted for XP and above, there is no need to define NTSTRSAFE_LIB and
 * link to the lib.
 */
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <dontuse.h>

#include <host/circbf.h>
#include <host/list.h>

typedef NTSTATUS status_t;

typedef unsigned long		refcnt_t;
typedef unsigned long		reflck_t;

reflck_t cmpxchg(reflck_t *dest, reflck_t oldv, reflck_t newv);
reflck_t xchg(reflck_t *dest, reflck_t val);
void *xchgptr(void **dest, void *val);

#define POLL_READ		0x01
#define POLL_WRITE		0x02
#define POLL_ERROR		0x04

typedef PDEVICE_OBJECT os_device;
typedef struct os_file os_file;
typedef KEVENT os_waiter;

struct os_file {
	PFILE_OBJECT file;
	UNICODE_STRING file_name;
	void *priv;
};
#define wdm_file_priv(filp)	((os_file *)(filp)->FsContext)
#define file_priv(file)		((file)->priv)

typedef struct os_timer os_timer;

typedef void (*os_timer_cb)(os_timer *timer, os_device dev, void *data);
typedef status_t (*os_work_aval_cb)(struct os_work_item *item,
				    os_device dev);
typedef void (*os_work_cmpl_cb)(struct os_work_item *item,
				os_device dev, status_t status);

#define OS_IS_SUCCESS(status)	NT_SUCCESS(status)

typedef VOID (PO_COMPLETION_ROUTINE)(PDEVICE_OBJECT dev, UCHAR minor,
				     POWER_STATE power,
				     PVOID ctx, PIO_STATUS_BLOCK iosb);
typedef VOID (KDEFERRED_ROUTINE)(PKDPC, PVOID, PVOID, PVOID);

typedef enum wdm_dev_state {
	DEV_STOPPED,		/* device stopped */
	DEV_RUNNING,            /* started and working */
	DEV_STOPPING,		/* stop pending */
	DEV_REMOVING,		/* remove pending */
	DEV_LOWER_REMOVED,	/* removed by surprise */
	DEV_REMOVED,		/* removed */
} wdm_dev_state_t;

typedef enum wdm_irp_state {
	HoldRequests,		/* device is not started yet */
	AllowRequests,		/* device is ready to process */
	FailRequests,		/* fail both existing and queued up requests */
} wdm_irp_state_t;

struct wdm_irp {
	ULONG scenario;
#define WDM_IRP_SKIPING		0x01 /* Scenario 1, forward and forget */
#define WDM_IRP_WAITING		0x02 /* Scenario 2, forward and wait */
#define WDM_IRP_CMPLING		0x03 /* Scenario 3, forward with a completion routine */
#define WDM_IRP_PENDING		0x04 /* Scenario 4, queue for later, or forward and reuse */
#define WDM_IRP_DISPING		0x00 /* Scenario 5, complete the IRP in the dispatch routine */
	os_waiter *waiter;
	PDEVICE_OBJECT dev;
	PIO_COMPLETION_ROUTINE routine;
	PVOID *context;
};

struct wdm_power_request {
	os_waiter *waiter;
	PDEVICE_OBJECT dev;
	PO_COMPLETION_ROUTINE *cmpl;
	PVOID ctx;
};

typedef void (*wdm_io_async_cb)(os_device dev, ULONG ctx);
struct wdm_io_async {
	PDEVICE_OBJECT dev;
	PIRP irp;
	PIO_WORKITEM item;
	/* IO routines */
	PIO_COMPLETION_ROUTINE routine;
	PVOID context;
	/* Asynchronous calls */
	wdm_io_async_cb async_call;
	ULONG async_data;
};

typedef enum wdm_version {
	Win2kOrBetter = 1,
	WinXpOrBetter,
	WinVistaOrBetter
} wdm_version_t;

struct os_work_queue {
	os_device dev;
	PIRP (*wdm_alloc_irp)(os_device, void *q_data,
			      struct os_work_item *item);
	void (*wdm_free_irp)(os_device, void *, PIRP);
	os_work_aval_cb work_aval;
	os_work_cmpl_cb work_cmpl;
	void *data;
	reflck_t requesting;
#define OS_WORK_IDLE		0x00
#define OS_WORK_LOCK		0x01
#define OS_WORK_BUSY		0x02
	LONG stopping;
	LONG nr_works;
	PIRP irp;
	os_waiter waiter;
	LONG refcnt;
};

struct os_work_item {
	const char *hint;
	os_device dev;
	struct os_work_queue *work;
	PIO_WORKITEM item;
	uint8_t priv[1];
};

#define work_priv(__item)	((void *)((__item)->priv))

struct os_timer {
	os_device dev;
	os_timer_cb timer_func;
	void *timer_data;
	KTIMER timer_timer;
	KDPC timer_dpc;
	LONG stopping;
};

typedef FAST_MUTEX os_mutex_t;
#define os_mutex_init(mutex)	ExInitializeFastMutex(mutex)
#define os_mutex_lock(mutex)	ExAcquireFastMutex(mutex)
#define os_mutex_unlock(mutex)	ExReleaseFastMutex(mutex)

#define __wdm_dev_pnp_init_state(_priv_)		\
        (_priv_)->curr_state = DEV_STOPPED;		\
        (_priv_)->prev_state = DEV_STOPPED;
#define __wdm_dev_pnp_save_state(_priv_, _state_)	\
        (_priv_)->prev_state = (_priv_)->curr_state;	\
        (_priv_)->curr_state = (_state_);
#define __wdm_dev_pnp_restore_state(_priv_)		\
        (_priv_)->curr_state = (_priv_)->prev_state;

struct wdm_device {
	struct os_driver *drv;
	/* Function device created by us */
	PDEVICE_OBJECT fdo;
	/* Physical device created by bus */
	PDEVICE_OBJECT pdo;
	/* next stack device for irps */
	PDEVICE_OBJECT attached_dev;

	unsigned long drv_flags;

	/* Bus drivers set the appropriate values in this structure in
	 * response to an IRP_MN_QUERY_CAPABILITIES IRP. Function and
	 * filter drivers might alter the capabilities set by the bus
	 * driver.
	 */
	DEVICE_CAPABILITIES caps;

	/* Name buffer for our named Functional device object link.
	 * The name is generated based on the driver's class GUID.
	 */
	char id[256];
	int minor;
	UNICODE_STRING InterfaceName;
	int io_registered;

	/* power management */
	wdm_dev_state_t curr_state;
	wdm_dev_state_t prev_state;
	KSPIN_LOCK state_lock;

	wdm_irp_state_t queue_state;
	LIST_ENTRY queue_irps;
	KSPIN_LOCK queue_lock;

	SYSTEM_POWER_STATE SysPower;
	DEVICE_POWER_STATE DevPower;

	refcnt_t ref_init;
	KSPIN_LOCK ref_lock;
	/* user space reference count */
	refcnt_t nr_users;
	/* kernel space reference count */
	refcnt_t nr_kerns;
	os_waiter removed_waiter;
	os_waiter stopped_waiter;

	LPCGUID io_clsguid;

	int is_fdo;
#ifdef CONFIG_BUS
	/* bus driver specific */
	union {
		struct wdm_bus_device {
			struct list_head devs;
			int nr_devs;
			os_mutex_t mutex;
		} bus;
		struct wdm_dev_device {
			struct list_head link;
			os_device bus;
			int present;
			int reported_missing;
			PWCHAR hardware_id;
			ULONG hardware_id_len;
			PWCHAR compatible_id;
			ULONG compatible_id_len;
			ULONG serial_no;
		} dev;
	} u;
#endif

#ifdef CONFIG_PM_WAIT_WAKE
	/* wait wake */
	PIRP wait_wake_irp;
	reflck_t wait_wake_outstanding;
	reflck_t wait_wake_stopped;
	reflck_t wait_wake_started;
	LONG wait_wake_enabled;
#endif

#ifdef CONFIG_SYSCTL
	/* windows management instrumentation */
	WMILIB_CONTEXT wmi_context;
	int wmi_registered;
#endif

	uint8_t priv[1];
};

struct wdm_driver {
	UNICODE_STRING reg_path;
	wdm_version_t wdm_version;
};

#define MAX_PNP_STRING_SIZE		256

extern struct wdm_driver wdm_drv_global_data;

void wdm_drv_enum_version(void);
wdm_version_t wdm_drv_get_version(void);

os_device wdm_dev_stacked(os_device dev);

struct wdm_irp *wdm_irp_get(os_device dev, const char *what);
void wdm_irp_put(struct wdm_irp *ctx, const char *what);

/* http://support.microsoft.com/kb/320275/en-us?fr=1
 * See different ways to handling IRPs.
 */
uint8_t wdm_io_stack_count(os_device dev);
/* scenario 1 */
status_t wdm_dev_io_skip(os_device dev, IRP *irp);
/* scenario 2 */
status_t wdm_dev_io_wait(os_device dev, IRP *irp);
/* scenario 3(pending=FALSE) & 4(pending=TRUE) */
status_t wdm_dev_io_call(os_device dev, IRP *irp, BOOLEAN pending,
			 PIO_COMPLETION_ROUTINE routine,
			 void *context);
/* scenario 5 */
status_t __wdm_dev_io_cmpl(os_device dev, IRP *irp);
status_t wdm_dev_io_cmpl(os_device dev, IRP *irp,
			 status_t status, ULONG info);

/* XXX: Asynchronous IO Handling for Power Management:
 *
 * IRP_MN_SET_POWER should be completed as fast as it could, thus
 * suspend/resume process should be put into a seperate context (e.x. in a
 * work queue), the power IRP should be passed down after the completion
 * of the suspend/resume process.
 */
void wdm_dev_io_async_discard(os_device dev);
status_t wdm_dev_io_async_submit(os_device dev, PIRP irp,
				 wdm_io_async_cb async_call,
				 ULONG async_data,
				 PIO_COMPLETION_ROUTINE routine,
				 void *context);

status_t wdm_dev_po_request(os_device dev, BOOLEAN wait,
			    UCHAR minor, POWER_STATE state,
			    PO_COMPLETION_ROUTINE *cmpl, PVOID context);

void __wdm_sys_apply_power(os_device dev, SYSTEM_POWER_STATE power);
void __wdm_dev_apply_power(os_device dev, DEVICE_POWER_STATE power);
void wdm_dev_apply_power(os_device dev, DEVICE_POWER_STATE power);
DEVICE_POWER_STATE wdm_dev_get_power(os_device dev);
DEVICE_POWER_STATE wdm_sys_get_power(os_device dev);
status_t wdm_dev_set_power(os_device dev, BOOLEAN wait,
			   DEVICE_POWER_STATE state,
			   PO_COMPLETION_ROUTINE *cmpl, PVOID context);
status_t wdm_dev_resume(os_device dev, BOOLEAN wait,
			PO_COMPLETION_ROUTINE *cmpl, PVOID context);
status_t wdm_dev_suspend(os_device dev, BOOLEAN wait,
			 PO_COMPLETION_ROUTINE *cmpl, PVOID context);
BOOLEAN os_dev_is_powered(os_device dev);
void wdm_dev_power_caps(os_device dev);

#ifdef CONFIG_PM_WAIT_WAKE
void wdm_enable_wait_wake(os_device dev);
status_t wdm_start_wait_wake(os_device dev);
status_t wdm_stop_wait_wake(os_device dev);
void wdm_wait_wake_init(os_device dev);
void wdm_wait_wake_exit(os_device dev);
BOOLEAN wdm_dev_allow_wait_wake(os_device dev, DEVICE_POWER_STATE power);
BOOLEAN wdm_sys_allow_wait_wake(os_device dev, SYSTEM_POWER_STATE power);
#else
#define wdm_enable_wait_wake(dev)
#define wdm_start_wait_wake(dev)		STATUS_SUCCESS
#define wdm_stop_wait_wake(dev)			STATUS_SUCCESS
#define wdm_wait_wake_init(dev)
#define wdm_wait_wake_exit(dev)
#define wdm_dev_allow_wait_wake(dev, power)	FALSE
#define wdm_sys_allow_wait_wake(dev, power)	FALSE
#endif

status_t wdm_reg_get_word(PWCHAR path, PWCHAR name, PULONG value);
status_t wdm_reg_get_property(os_device pdo, int property,
			      uint8_t *data, size_t size);

void wdm_dev_init_state(os_device dev);
void wdm_dev_save_state(os_device dev, wdm_dev_state_t state);
void wdm_dev_restore_state(os_device dev);
wdm_dev_state_t wdm_pnp_get_state(os_device dev);

status_t wdm_dev_add(os_device dev);
void wdm_dev_remove(os_device dev);
status_t wdm_dev_start(os_device dev);
void wdm_dev_stop(os_device dev, int removing);

void wdm_irp_set_state(os_device dev, wdm_irp_state_t state);
wdm_irp_state_t wdm_irp_get_state(os_device dev);
int wdm_irp_is_accepted(os_device dev, IRP *irp);

#ifdef CONFIG_BUS
void wdm_bus_add(os_device dev);
void wdm_bus_remove(os_device dev);
status_t wdm_bus_start(os_device dev);
void wdm_bus_stop(os_device dev);
PDEVICE_RELATIONS wdm_bus_query_childs(PDEVICE_OBJECT dev,
				       PDEVICE_RELATIONS rels);
PDEVICE_RELATIONS wdm_bus_query_parent(PDEVICE_OBJECT dev,
				       PDEVICE_RELATIONS rels);

#define foreach_wdm_bus_dev(b, d)		\
	list_for_each_entry(struct wdm_device, d, &b->u.bus.devs, u.dev.link)
#define foreach_wdm_bus_dev_safe(b, d, n)	\
	list_for_each_entry_safe(struct wdm_device, d, n, &b->u.bus.devs, u.dev.link)
#else
#define wdm_bus_add(dev)
#define wdm_bus_remove(dev)
#define wdm_bus_start(dev)	STATUS_SUCCESS
#define wdm_bus_stop(dev)
#endif

#endif /* __WDM_DEV_H_INCLUDE__ */
