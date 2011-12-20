
#include <initguid.h>
#include <wdm/os_dev.h>

#define wdm_dev_priv(dev)	((struct wdm_device *)((dev)->DeviceExtension))

DRIVER_DISPATCH wdm_dev_dispatch_pnp;
DRIVER_DISPATCH wdm_dev_dispatch_pm;
DRIVER_DISPATCH wdm_dev_dispatch_wmi;
DRIVER_DISPATCH wdm_dev_dispatch_ioctl;
DRIVER_ADD_DEVICE wdm_dev_add_device;

IO_WORKITEM_ROUTINE __wdm_dev_worker_queue;
IO_WORKITEM_ROUTINE __wdm_dev_worker_async;
PO_COMPLETION_ROUTINE __wdm_dev_pocmpl_req;
IO_COMPLETION_ROUTINE __wdm_dev_iocmpl_io;
IO_COMPLETION_ROUTINE __wdm_dev_iocmpl_async;
IO_COMPLETION_ROUTINE __wdm_dev_iocmpl_worker;

struct os_driver *wdm_fdo_driver;
size_t wdm_fdo_objsize;
unsigned long wdm_fdo_flags = 0;

struct wdm_driver wdm_drv_global_data;

reflck_t cmpxchg(reflck_t *dest, reflck_t oldv, reflck_t newv)
{
	return InterlockedCompareExchange(dest, newv, oldv);
}

reflck_t xchg(reflck_t *dest, reflck_t val)
{
	return InterlockedExchange(dest, val);
}

void *xchgptr(void **dest, void *val)
{
	return InterlockedExchangePointer(dest, val);
}

/*=========================================================================
 * memory
 *=======================================================================*/
#ifdef CONFIG_DEBUG
#define for_each_mem_chunk_safe(list, chunk, n)		\
	list_for_each_entry_safe(struct os_mem_chunk, chunk, n, &list, link)
#define for_each_mem_chunk(list, chunk)			\
	list_for_each_entry(struct os_mem_chunk, chunk, &list, link)

struct list_head os_mem_alloced;
struct list_head os_mem_freed;

void *__os_mem_alloc(size_t size, const char *what,
		     const char *file, int line)
{
	struct os_mem_chunk *chunk;

	/* Alloc os_mem_chunk and add it to os_mem_alloced.
	 */
	chunk = ExAllocatePool(NonPagedPool,sizeof(struct os_mem_chunk)+size);
	if (!chunk) {
		os_dbg(OS_DBG_ERR, "ExAllocatePool(%s) - failure\n", what);
		return NULL;
	}
	RtlZeroMemory(chunk->priv, size);
	os_dbg(OS_DBG_INFO, "ExAllocatePool(%s) - success\n", what);
	INIT_LIST_HEAD(&(chunk->link));
	chunk->file = file;
	chunk->line = line;
	chunk->size = size;
	chunk->what = what;
	list_add_tail(&(chunk->link), &os_mem_alloced);
	return chunk->priv;
}

void __os_mem_free(void *p, const char *file, int line)
{
	struct os_mem_chunk *chunk, *n;

	if (p) {
		/* Find p in os_mem_alloced, delete it from list if it is
		 * found, alloc os_mem_chunk and add it to os_mem_freed if
		 * it is not found.
		 */
		for_each_mem_chunk_safe(os_mem_alloced, chunk, n) {
			if (chunk->priv == p) {
				os_dbg(OS_DBG_INFO, "ExFreePool(%s) - success\n", chunk->what);
				list_del(&chunk->link);
				ExFreePool(chunk);
				return;
			}
		}
		os_dbg(OS_DBG_ERR, "ExFreePool - scrambled\n");
		chunk = ExAllocatePool(NonPagedPool, sizeof(struct os_mem_chunk));
		if (!chunk) {
			os_dbg(OS_DBG_INFO, "ExAllocatePool(scrambled) - failure\n");
			ExFreePool(p);
			return;
		}
		INIT_LIST_HEAD(&(chunk->link));
		chunk->file = file;
		chunk->line = line;
		chunk->size = 0;
		chunk->what = (const char *)p;
		list_add_tail(&(chunk->link), &os_mem_freed);
	}
}

void os_mem_leak_init(void)
{
	INIT_LIST_HEAD(&os_mem_alloced);
	INIT_LIST_HEAD(&os_mem_freed);
}

void os_mem_leak_dump(void)
{
	struct os_mem_chunk *chunk, *n;
	void *p;

#define MEM_DUMP_FMT		"%s(%d):0x%p(%d)\r\n"
#define MEM_DUMP_ARG(m, p)	(m)->file,(m)->line,(p),(m)->size
	/* Dump those not freed in os_mem_alloced, dump those not
	 * correctly freed in os_mem_freed.
	 */
	os_dbg(OS_DBG_INFO, "Leaked memory chunks:\n");
	for_each_mem_chunk_safe(os_mem_alloced, chunk, n) {
		p = chunk->priv;
		os_dbg(OS_DBG_INFO, MEM_DUMP_FMT, MEM_DUMP_ARG(chunk, p));
		list_del(&chunk->link);
		ExFreePool(chunk);
	}
	os_dbg(OS_DBG_INFO, "Scrambled memory chunks:\n");
	for_each_mem_chunk_safe(os_mem_freed, chunk, n) {
		p = (void **)chunk->what;
		os_dbg(OS_DBG_INFO, MEM_DUMP_FMT, MEM_DUMP_ARG(chunk, p));
		list_del(&chunk->link);
		ExFreePool(chunk);
		if (p) {
			/* Not managed allocation */
			ExFreePool(p);
		}
	}
}
#else
void *__os_mem_alloc(size_t size)
{
	void *p = ExAllocatePool(NonPagedPool, size);
	if (!p) return NULL;
	RtlZeroMemory(p, size);
	return p;
}

void __os_mem_free(void *p)
{
	if (p) ExFreePool(p);
}
#endif

void os_mem_zero(void *target, size_t length)
{
	RtlZeroMemory(target, length);
}

void os_mem_copy(void *target, const void *source, size_t length)
{
	RtlCopyMemory(target, source, length);
}

void os_mem_move(void *target, const void *source, size_t length)
{
	RtlMoveMemory(target, source, length);
}

/*=========================================================================
 * timer
 *=======================================================================*/
void wdm_timer_timeout_dpc(PKDPC dpc, 
			   void *ctx, 
			   void *arg1, void *arg2)
{
	os_timer *__t = (os_timer *)ctx;
	__t->timer_func((os_timer *)__t, __t->dev, __t->timer_data);
}

void os_timer_unregister(os_timer *__t)
{
	if (__t) {
		__t->stopping = 1;
		os_timer_discard(__t);
		if (__t->dev) {
			os_dev_put_kern(__t->dev, "TIMER_DPC");
			__t->dev = NULL;
		}
		os_mem_free(__t);
	}
}

os_timer *os_timer_register(os_device dev,
			    os_timer_cb call, void *data)
{
	os_timer *__t;
	__t = os_mem_alloc(sizeof(os_timer), "TIMER_DPC");
	if (__t) {
		__t->stopping = 0;
		__t->timer_func = call;
		__t->timer_data = data;
		__t->dev = os_dev_get_kern(dev, "TIMER_DPC");
		KeInitializeDpc(&__t->timer_dpc, wdm_timer_timeout_dpc, (void*)__t);
		KeInitializeTimer(&__t->timer_timer);
	}
	return (void*)__t;
}

void os_timer_discard(os_timer *__t)
{
	if (__t) {
		KeCancelTimer(&__t->timer_timer);
	}
}

void os_timer_schedule(os_timer *__t, uint32_t usecs)
{
	LARGE_INTEGER timeout;

	if (__t) {
		if (__t->stopping || !__t->dev) {
			return;
		}
		timeout.QuadPart = (int64_t)(usecs) * (int64_t)(-10);
		KeSetTimer(&__t->timer_timer, timeout, &__t->timer_dpc);
	}
}

void os_worker_free_item(struct os_work_item *__i)
{
	BUG_ON(!__i);
	if (__i->dev) os_dev_put_kern(__i->dev, __i->hint);
	if (__i->item) IoFreeWorkItem(__i->item);
	os_mem_free(__i);
}

struct os_work_item *os_worker_alloc_item(struct os_work_queue *__q,
					  int extrasize, const char *what)
{
	struct os_work_item *__i = NULL;

	if (__q->stopping) goto fail;
	__i = os_mem_alloc(sizeof (struct os_work_item) + extrasize,
			   what);
	if (!__i) goto fail;
	__i->hint = what;
	/* TODO: __q->dev? */
	BUG_ON(!__q->dev);
	__i->item = IoAllocateWorkItem(__q->dev);
	if (!__i->item) {
		os_dbg(OS_DBG_ERR, "IoAllocateWorkItem - failure\n");
		goto fail;
	}
	__i->dev = os_dev_get_kern(__q->dev, __i->hint);
	__i->work = __q;
	return __i;
fail:
	if (__i) os_worker_free_item(__i);
	return NULL;
}

PIRP __wdm_worker_alloc_irp(struct os_work_queue *__q,
			    struct os_work_item *__i)
{
	if (__q->wdm_alloc_irp)
		return __q->wdm_alloc_irp(__q->dev, __q->data, __i);
	return NULL;
}

void ____wdm_worker_free_irp(struct os_work_queue *__q, PIRP irp)
{
	if (__q->wdm_free_irp)
		__q->wdm_free_irp(__q->dev, __q->data, irp);
}

void __wdm_worker_free_irp(struct os_work_queue *__q, PIRP irp)
{
	____wdm_worker_free_irp(__q, irp);
	os_waiter_wake(&__q->waiter);
	KeMemoryBarrier();
	xchg(&__q->requesting, OS_WORK_IDLE);
	__q->nr_works--;
}

void wdm_worker_free_irp(struct os_work_queue *__q, PIRP irp)
{
	if (0 == InterlockedDecrement(&__q->refcnt)) {
		__wdm_worker_free_irp(__q, irp);
	}
}

void __os_worker_discard_queue(struct os_work_queue *__q)
{
	PIRP irp = NULL;

	if (cmpxchg(&__q->requesting,
		    OS_WORK_BUSY, OS_WORK_LOCK) != OS_WORK_BUSY) {
		return;
	}
	irp = xchgptr(&__q->irp, NULL);
	if (irp) {
		IoCancelIrp(irp);
		wdm_worker_free_irp(__q, irp);
	}
	return;
}

NTSTATUS __wdm_dev_iocmpl_worker(PDEVICE_OBJECT __dev, PIRP __irp, PVOID context)
{
	NTSTATUS status = __irp->IoStatus.Status;
	struct os_work_item *__i = (struct os_work_item *)context;
	struct os_work_queue *__q = __i->work;
	PIRP irp = NULL;
	LONG old_state;

	if (!NT_SUCCESS(status)) {
		os_dbg(OS_DBG_ERR, "IoCallDriver - failure(%X)", status);
	}

	old_state = cmpxchg(&__q->requesting, OS_WORK_BUSY, OS_WORK_LOCK);
	irp = xchgptr(&__q->irp, NULL);
	if (irp) {
		BUG_ON(old_state != OS_WORK_BUSY);
		__q->refcnt = 0;
		__wdm_worker_free_irp(__q, __irp);
	} else {
		BUG_ON(old_state == OS_WORK_BUSY);
		wdm_worker_free_irp(__q, __irp);
	}
	__q->work_cmpl(__i, __q->dev, status);
	os_worker_free_item(__i);
	return status;
}

VOID __wdm_dev_worker_queue(PDEVICE_OBJECT dev, PVOID ctx)
{
	status_t status = STATUS_SUCCESS;
	struct os_work_item *__i = (struct os_work_item *)ctx;
	struct os_work_queue *__q = __i->work;
	PIRP irp = NULL, old_irp = NULL;

	BUG_ON(!__i || !__q);

	irp = __wdm_worker_alloc_irp(__q, __i);
	if (!irp) {
		status = STATUS_DEVICE_BUSY;
		goto irp_busy;
	}
	BUG_ON(__q->requesting != OS_WORK_LOCK);
	os_waiter_idle(&__q->waiter);
	BUG_ON(__q->refcnt > 0);
	BUG_ON(__q->irp);

	/* Referenced by __wdm_dev_iocmpl_worker and os_worker_suspend_queue */
	__q->refcnt = 2;
	old_irp = xchgptr(&__q->irp, irp);
	BUG_ON(old_irp);
	BUG_ON(KeGetCurrentIrql() != PASSIVE_LEVEL);
	/* XXX: work_aval requires that the __q->irp is ready.
	 */
	status = __q->work_aval(__i, __q->dev);
	if (!NT_SUCCESS(status)) goto irp_failure;
	xchg(&__q->requesting, OS_WORK_BUSY);
	IoSetCompletionRoutine(irp,
			       __wdm_dev_iocmpl_worker,
			       __i, TRUE, TRUE, TRUE);
	/* If irp->CancelRoutine is NULL, and therefore the IRP is not
	 * cancelable, IoCancelIrp sets the IRP's cancel bit and returns
	 * FALSE. The IRP should be canceled at a later time when it
	 * becomes cancelable.
	 */
	status = IoCallDriver(wdm_dev_stacked(__q->dev), irp);
	irp = NULL;
	status = STATUS_PENDING;
	return;
irp_failure:
	irp = xchgptr(&__q->irp, NULL);
	__q->refcnt = 0;
	os_waiter_wake(&__q->waiter);
	KeMemoryBarrier();
	xchg(&__q->requesting, OS_WORK_IDLE);
irp_busy:
	/* Need to reschedule since we are the only running work item */
	__q->work_cmpl(__i, __q->dev, status);
	os_worker_free_item(__i);
	if (irp)
		____wdm_worker_free_irp(__q, irp);
	__q->nr_works--;
}

status_t os_worker_queue_item(struct os_work_queue *__q,
			      struct os_work_item *__i)
{
	if (!__q->stopping) {
		if (cmpxchg(&__q->requesting,
			    OS_WORK_IDLE, OS_WORK_LOCK) != OS_WORK_IDLE) {
			os_worker_free_item(__i);
			return STATUS_DEVICE_BUSY;
		}
		KeMemoryBarrier();
		__q->nr_works++;
		IoQueueWorkItem(__i->item,
				__wdm_dev_worker_queue,
				DelayedWorkQueue,
				__i);
	}
	return STATUS_SUCCESS;
}

void *os_worker_queue_data(struct os_work_queue *__q)
{
	if (__q)
		return __q->data;
	return NULL;
}

void *os_worker_item_data(struct os_work_item *item)
{
	return os_worker_queue_data(item->work);
}

void os_worker_create_queue(os_device dev,
			    struct os_work_queue *__q,
			    os_work_aval_cb aval,
			    os_work_cmpl_cb cmpl,
			    void *data)
{
	__q->data = data;
	__q->work_aval = aval;
	__q->work_cmpl = cmpl;
	__q->dev = os_dev_get_kern(dev, "WORK_QUEUE");
	xchg(&__q->requesting, OS_WORK_IDLE);
	xchgptr(&__q->irp, NULL);
	__q->refcnt = 0;
	__q->stopping = 0;
	__q->nr_works = 0;
	os_waiter_init(&__q->waiter, TRUE);
}

/* 50ms */
#define OS_WORKER_WAIT_TIMEOUT		50*1000

void os_worker_suspend_queue(struct os_work_queue *__q)
{
	BUG_ON(!__q);
	__q->stopping = 1;
	do {
		os_dbg(OS_DBG_DEBUG, "PENDING WORKS: %d\r\n", __q->nr_works);
		__os_worker_discard_queue(__q);
		os_waiter_wait(&__q->waiter, OS_WORKER_WAIT_TIMEOUT);
	} while (__q->nr_works > 0);
}

void os_worker_resume_queue(struct os_work_queue *__q)
{
	__q->stopping = 0;
}

void os_worker_delete_queue(struct os_work_queue *__q)
{
	BUG_ON(!__q);
	os_worker_suspend_queue(__q);
	if (__q->dev) {
		os_dev_put_kern(__q->dev, "WORK_QUEUE");
		__q->dev = NULL;
	}
}

void os_waiter_init(os_waiter *waiter, BOOLEAN wake)
{
	KeInitializeEvent(waiter, NotificationEvent, wake);
}

void os_waiter_wake(os_waiter *waiter)
{
	KeSetEvent(waiter, IO_NO_INCREMENT, FALSE);
}

void os_waiter_idle(os_waiter *waiter)
{
	KeClearEvent(waiter);
}

BOOLEAN os_waiter_wait(os_waiter *waiter, unsigned long usecs)
{
	LARGE_INTEGER timeout;
	PLARGE_INTEGER ptv = &timeout;

	if (usecs == OS_WAIT_INFINITE) {
		ptv = NULL;
	} else {
		timeout.QuadPart = (int64_t)(usecs) * (int64_t)(-10);
	}
	KeWaitForSingleObject(waiter, Executive, KernelMode, FALSE, ptv);
	return TRUE;
}

/*=========================================================================
 * IO manager IRPs
 *=======================================================================*/
struct wdm_irp *wdm_irp_get(os_device dev, const char *what)
{
	struct wdm_irp *ctx;
	ctx = os_mem_alloc(sizeof (struct wdm_irp), what);
	if (!ctx) return NULL;
	ctx->dev = os_dev_get_kern(dev, what);
	return ctx;
}

void wdm_irp_put(struct wdm_irp *ctx, const char *what)
{
	if (ctx) {
		os_dev_put_kern(ctx->dev, what);
		os_mem_free(ctx);
	}
}

BOOLEAN __wdm_dev_is_power_irp(PIRP irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	return stack->MajorFunction == IRP_MJ_POWER;
}

BOOLEAN __wdm_dev_is_ioctl_irp(PIRP irp)
{
	PIO_STACK_LOCATION stack = IoGetNextIrpStackLocation(irp);
	return stack->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL ||
	       stack->MajorFunction == IRP_MJ_DEVICE_CONTROL;
}

uint8_t wdm_io_stack_count(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (priv->attached_dev)
		return priv->attached_dev->StackSize;
	return 0;
}

status_t __wdm_dev_io_cmpl(os_device dev, IRP *irp)
{
	status_t status;

	if (__wdm_dev_is_power_irp(irp))
		PoStartNextPowerIrp(irp);
	status = irp->IoStatus.Status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS __wdm_dev_iocmpl_io(PDEVICE_OBJECT __dev, PIRP irp, PVOID context)
{
	struct wdm_irp *ctx = context;
	status_t status = irp->IoStatus.Status;
	LONG info = irp->IoStatus.Information;

	ASSERT(ctx->scenario != WDM_IRP_SKIPING &&
	       ctx->scenario != WDM_IRP_DISPING);

	if (irp->PendingReturned)
		IoMarkIrpPending(irp);
	os_dbg(OS_DBG_ERR, "IoCallDriver(cmpl) - status(%X)", status);
	if (!NT_SUCCESS(status)) {
		os_dbg(OS_DBG_ERR, "IoCallDriver - failure(%X)", status);
		status = STATUS_CONTINUE_COMPLETION;
		goto end;
	}
	if (ctx->routine)
		status = ctx->routine(ctx->dev, irp, ctx->context);
	if (ctx->scenario == WDM_IRP_WAITING) {
		if (!__wdm_dev_is_ioctl_irp(irp)) {
			os_waiter_wake(ctx->waiter);
		}
		status = STATUS_MORE_PROCESSING_REQUIRED;
	}
end:
	if (status == STATUS_MORE_PROCESSING_REQUIRED) {
		if (ctx->scenario == WDM_IRP_CMPLING) {
			/* Current IRP is not cared by us, thus complete
			 * it. -- The rare case.
			 */
			(void)__wdm_dev_io_cmpl(ctx->dev, irp);
		}
	}
	if (status == STATUS_CONTINUE_COMPLETION) {
		if (__wdm_dev_is_power_irp(irp))
			PoStartNextPowerIrp(irp);
	}
	wdm_irp_put(ctx, "IoCallDriver");
	return status;
}

status_t __wdm_dev_io_call(os_device dev, IRP *irp,
			   ULONG scenario,
			   PIO_COMPLETION_ROUTINE routine, void *context)
{
	status_t status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	struct wdm_irp *ctx;
	os_waiter waiter;

	if (scenario == WDM_IRP_SKIPING) {
		if (__wdm_dev_is_power_irp(irp))
			PoStartNextPowerIrp(irp);
		IoSkipCurrentIrpStackLocation(irp);
	} else {
		ctx = wdm_irp_get(dev, "IoCallDriver");
		if (!ctx) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			goto end;
		}
		ctx->scenario = scenario;
		ctx->routine = routine;
		ctx->context = context;
		ctx->waiter = NULL;
		if (scenario == WDM_IRP_WAITING) {
			os_waiter_init(&waiter, FALSE);
			ctx->waiter = &waiter;
			if (__wdm_dev_is_ioctl_irp(irp)) {
				irp->UserEvent = &waiter;
			}
		}
		if (scenario == WDM_IRP_PENDING) {
			IoMarkIrpPending(irp);
		}
		IoCopyCurrentIrpStackLocationToNext(irp);
		IoSetCompletionRoutine(irp, __wdm_dev_iocmpl_io, ctx,
				       TRUE, TRUE, TRUE);
	}
	if (__wdm_dev_is_power_irp(irp))
		status = PoCallDriver(priv->attached_dev, irp);
	else
		status = IoCallDriver(priv->attached_dev, irp);
	if (scenario == WDM_IRP_WAITING) {
		os_dbg(OS_DBG_DEBUG, "IoCallDriver(call) - status(%X)\n", status);
		if (status == STATUS_PENDING) {
			os_waiter_wait(&waiter, OS_WAIT_INFINITE);
			status = irp->IoStatus.Status;
			os_dbg(OS_DBG_DEBUG, "IoCallDriver(wait) - status(%X)\n", status);
		}
	}
	if (scenario == WDM_IRP_PENDING) {
		status = STATUS_PENDING;
	}
end:
	return status;
}

status_t wdm_dev_io_skip(os_device dev, IRP *irp)
{
	return __wdm_dev_io_call(dev, irp, WDM_IRP_SKIPING, NULL, NULL);
}

status_t wdm_dev_io_call(os_device dev, IRP *irp, BOOLEAN pending,
			 PIO_COMPLETION_ROUTINE routine, void *context)
{
	if (wdm_pnp_get_state(dev) == DEV_LOWER_REMOVED ||
	    wdm_pnp_get_state(dev) == DEV_REMOVED)
		return STATUS_INVALID_DEVICE_STATE;
	ASSERT(routine);
	if (pending) {
		return __wdm_dev_io_call(dev, irp, WDM_IRP_PENDING, routine, context);
	} else {
		return __wdm_dev_io_call(dev, irp, WDM_IRP_CMPLING, routine, context);
	}
}

status_t wdm_dev_io_wait(os_device dev, IRP *irp)
{
	if (wdm_pnp_get_state(dev) == DEV_LOWER_REMOVED ||
	    wdm_pnp_get_state(dev) == DEV_REMOVED)
		return STATUS_INVALID_DEVICE_STATE;
	return __wdm_dev_io_call(dev, irp, WDM_IRP_WAITING, NULL, NULL);
}

status_t wdm_dev_io_cmpl(os_device dev, IRP *irp,
			 status_t status, ULONG info)
{
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = info;
	return __wdm_dev_io_cmpl(dev, irp);
}

/*=========================================================================
 * windows management instrumentation
 *=======================================================================*/
#ifdef CONFIG_SYSCTL
#define OS_WMI_DRIVER_INFORMATION 0

NTSTATUS wdm_dev_wmi_query_reginfo(PDEVICE_OBJECT dev,
				   ULONG *reg_flags,
				   PUNICODE_STRING inst_name,
				   PUNICODE_STRING *reg_path,
				   PUNICODE_STRING mof_res_name,
				   PDEVICE_OBJECT *ppdo)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	os_dbg(OS_DBG_DEBUG, "QueryWmiRegInfo - begins\n");

	*reg_flags = WMIREG_FLAG_INSTANCE_PDO;
	*reg_path = &wdm_drv_global_data.reg_path;
	*ppdo = priv->pdo;
	RtlInitUnicodeString(mof_res_name, OS_MOF_RESOURCE_NAME);

	os_dbg(OS_DBG_DEBUG, "QueryWmiRegInfo - ends\n");
	return STATUS_SUCCESS;
}

status_t wdm_dev_wmi_get_datablock(PDEVICE_OBJECT dev, PIRP irp,
				     ULONG GuidIndex,
				     ULONG InstanceIndex,
				     ULONG InstanceCount,
				     PULONG InstanceLengthArray,
				     ULONG OutBufferSize,
				     PUCHAR Buffer)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	status_t status;
	unsigned long size = 0;
	WCHAR mode_name[] = L"Aishverya\0\0";
	uint16_t mode_length;

	os_dbg(OS_DBG_DEBUG, "QueryWmiDataBlock - begins\n");
	mode_length = (USHORT)((wcslen(mode_name) + 1) * sizeof(WCHAR));
	
	/* Only ever registers 1 instance per guid. */
	ASSERT((InstanceIndex == 0) && (InstanceCount == 1));
	
	switch (GuidIndex) {
	case OS_WMI_DRIVER_INFORMATION:
		size = sizeof(ULONG) + mode_length + sizeof(USHORT);
		if (OutBufferSize < size ) {
			os_dbg(OS_DBG_ERR, "OS_WMI_DRIVER_INFORMATION - buffer too small\n");
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		* (PULONG) Buffer = os_dbg_get_level();
		Buffer += sizeof(ULONG);
		/* Put length of string ahead of string. */
		*((PUSHORT)Buffer) = mode_length;
		Buffer = (PUCHAR)Buffer + sizeof(USHORT);
		RtlCopyBytes((PVOID)Buffer, (PVOID)mode_name, mode_length);
		*InstanceLengthArray = size ;
		status = STATUS_SUCCESS;
		break;
	default:
		status = STATUS_WMI_GUID_NOT_FOUND;
	}
	status = WmiCompleteRequest(dev, irp, status, size, IO_NO_INCREMENT);

	os_dbg(OS_DBG_DEBUG, "QueryWmiDataBlock - ends\n");
	return status;
}

status_t wdm_dev_wmi_set_datablock(PDEVICE_OBJECT dev, PIRP irp,
				   ULONG GuidIndex,
				   ULONG InstanceIndex,
				   ULONG BufferSize,
				   PUCHAR Buffer)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	status_t status;
	unsigned long info = 0;
	
	os_dbg(OS_DBG_DEBUG, "SetWmiDataBlock - begins\n");
	
	switch (GuidIndex) {
	case OS_WMI_DRIVER_INFORMATION:
		if (BufferSize == sizeof(ULONG)) {
			os_dbg_set_level(*(PULONG) Buffer);
			status = STATUS_SUCCESS;
			info = sizeof(ULONG);
		} else {
			status = STATUS_INFO_LENGTH_MISMATCH;
		}
		break;
	default:
		status = STATUS_WMI_GUID_NOT_FOUND;
	}
	status = WmiCompleteRequest(dev, irp, status, info, IO_NO_INCREMENT);

	os_dbg(OS_DBG_DEBUG, "SetWmiDataBlock - ends\n");
	return status;
}

status_t wdm_dev_wmi_set_dataitem(PDEVICE_OBJECT dev, PIRP irp,
				  ULONG GuidIndex,
				  ULONG InstanceIndex,
				  ULONG DataItemId,
				  ULONG BufferSize,
				  PUCHAR Buffer)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	status_t status;
	unsigned long info = 0;

	os_dbg(OS_DBG_DEBUG, "SetWmiDataItem - begins\n");

	switch (GuidIndex) {
	case OS_WMI_DRIVER_INFORMATION:
		if (DataItemId == 1) {
			if (BufferSize == sizeof(ULONG)) {
				os_dbg_set_level(*(PULONG) Buffer);
				status = STATUS_SUCCESS;
				info = sizeof(ULONG);
			} else {
				status = STATUS_INFO_LENGTH_MISMATCH;
			}
		} else {
			status = STATUS_WMI_READ_ONLY;
		}
		break;
	default:
		status = STATUS_WMI_GUID_NOT_FOUND;
	}
	status = WmiCompleteRequest(dev, irp, status, info, IO_NO_INCREMENT);

	os_dbg(OS_DBG_DEBUG, "SetWmiDataItem - ends\n");
	return status;
}

NTSTATUS wdm_dev_dispatch_wmi(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	SYSCTL_IRP_DISPOSITION disposition;
	uint8_t minor = stack->MinorFunction;

	if (!priv->is_fdo) {
		return __wdm_dev_io_cmpl(dev, irp);
	}

	os_dbg(OS_DBG_DEBUG, "IRP_MJ_SYSTEM_CONTROL %s - begins\n",
	       wdm_dev_wmi_string(minor));
	status = WmiSystemControl(&priv->wmi_context, dev, irp, &disposition);
	switch (disposition) {
	case IrpProcessed:
		/* This irp has been processed and may be completed or
		 * pending.
		 */
		break;
	case IrpNotCompleted:
		/* This irp has not been completed, but has been fully
		 * processed.  We will complete it now.
		 */
		(void)__wdm_dev_io_cmpl(dev, irp);
		break;
	case IrpForward:
	case IrpNotWmi:
	default:
		/* This irp is either not a WMI irp or is a WMI irp
		 * targeted at a device lower in the stack.
		 */
		status = wdm_dev_io_skip(dev, irp);
		break;
	}
	os_dbg(OS_DBG_DEBUG, "IRP_MJ_SYSTEM_CONTROL %s - ends\n",
	       wdm_dev_wmi_string(minor));
	return status;
}

void wdm_dev_wmi_exit(os_device dev)
{
	status_t status;
	struct wdm_device *priv;
	
	priv = wdm_dev_priv(dev);
	if (priv->wmi_registered) {
		status = IoWMIRegistrationControl(dev, WMIREG_ACTION_DEREGISTER);
		if (!NT_SUCCESS(status)) {
			os_dbg(OS_DBG_ERR,
			       "IoWMIRegistrationControl(WMIREG_ACTION_DEREGISTER) - failure(%X)\n",
			       status);
		}
	}
}

status_t wdm_dev_wmi_init(os_device dev)
{
	status_t status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);

	priv->wmi_registered = FALSE;
	priv->wmi_context.GuidCount          = os_driver_wmi_nrguids;
	priv->wmi_context.GuidList           = os_driver_wmi_guids;
	priv->wmi_context.QueryWmiRegInfo    = wdm_dev_wmi_query_reginfo;
	priv->wmi_context.QueryWmiDataBlock  = wdm_dev_wmi_get_datablock;
	priv->wmi_context.SetWmiDataBlock    = wdm_dev_wmi_set_datablock;
	priv->wmi_context.SetWmiDataItem     = wdm_dev_wmi_set_dataitem;
	priv->wmi_context.ExecuteWmiMethod   = NULL;
	priv->wmi_context.WmiFunctionControl = NULL;
	
	status = IoWMIRegistrationControl(dev, WMIREG_ACTION_REGISTER);
	if (!NT_SUCCESS(status)) {
		os_dbg(OS_DBG_ERR,
		       "IoWMIRegistrationControl(WMIREG_ACTION_REGISTER) - failure(%X)\n",
		       status);
		priv->wmi_registered = FALSE;
	} else {
		priv->wmi_registered = TRUE;
	}
	return status;
}
#else
NTSTATUS wdm_dev_dispatch_wmi(PDEVICE_OBJECT dev, PIRP irp)
{
	return wdm_dev_io_skip(dev, irp);
}

void wdm_dev_wmi_exit(os_device dev)
{
}

status_t wdm_dev_wmi_init(os_device dev)
{
	return STATUS_SUCCESS;
}
#endif

/*=========================================================================
 * input & output
 *=======================================================================*/
void wdm_dev_io_free_file(os_file *file)
{
	PUNICODE_STRING path = NULL;

	if (file) {
		path = &file->file_name;
		if (path->Buffer) {
			os_mem_free(path->Buffer);
			path->Buffer = NULL;
		}
		os_mem_free(file);
	}
}

NTSTATUS wdm_dev_io_open(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	PFILE_OBJECT filp;
	os_file *file = NULL;
	PUNICODE_STRING path = NULL;
	
	filp = stack->FileObject;
	if (!filp || !priv->drv->open) {
		status = STATUS_INVALID_PARAMETER;
		goto exit;
	}

	/* TODO: FsContext is not NULL? */
	filp->FsContext = NULL;
	file = os_mem_alloc(sizeof (os_file), "FILE_DESCRIPTOR");
	if (!file) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto exit;
	}

	path = &file->file_name;
	path->MaximumLength = filp->FileName.Length + sizeof(UNICODE_NULL);
	path->Length        = filp->FileName.Length;
	path->Buffer        = os_mem_alloc(path->MaximumLength, "UNICODE_STRING");
	if (!path->Buffer) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto exit;
	}
	os_mem_move(path->Buffer, filp->FileName.Buffer, filp->FileName.Length);

	status = priv->drv->open(dev, file);
	if (NT_SUCCESS(status)) {
		ASSERT(file->priv);
		filp->FsContext = file;
		file = NULL;
		os_dev_inc_user(dev, "OPEN_CLOSE");
	}

exit:
	if (file) wdm_dev_io_free_file(file);
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

NTSTATUS wdm_dev_io_close(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	PFILE_OBJECT filp;
	os_file *file;
	
	filp = stack->FileObject;
	if (!filp || !wdm_file_priv(filp) || !priv->drv->close) {
		status = STATUS_INVALID_PARAMETER;
		goto end;
	}
	file = wdm_file_priv(filp);
	priv->drv->close(dev, file);
	wdm_dev_io_free_file(file);
	filp->FsContext = NULL;
	os_dev_dec_user(dev, "OPEN_CLOSE");
end:
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

NTSTATUS wdm_dev_io_ioctl(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	PFILE_OBJECT filp = stack->FileObject;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG code;
	PVOID buffer;
	ULONG length;
	os_file *file;

	if (!filp || !wdm_file_priv(filp) || !priv->drv->ioctl) {
		status = STATUS_INVALID_PARAMETER;
		length = 0;
		goto end;
	}
	file = wdm_file_priv(filp);
	code = stack->Parameters.DeviceIoControl.IoControlCode;
	buffer = irp->AssociatedIrp.SystemBuffer;
	if (IOCTL_GET_DIR(code) == IOCTL_INPUT)
		length = stack->Parameters.DeviceIoControl.InputBufferLength;
	else
		length = stack->Parameters.DeviceIoControl.OutputBufferLength;
	status = priv->drv->ioctl(dev, file, IOCTL_GET_CODE(code),
				  buffer, &length);
end:
	return wdm_dev_io_cmpl(dev, irp, status, length);
}

NTSTATUS wdm_dev_io_write(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	PFILE_OBJECT filp = stack->FileObject;
	os_file *file;
	NTSTATUS status;
	const char *buf;
	size_t count = 0;

	if (!filp || !wdm_file_priv(filp) || !priv->drv->write) {
		status = STATUS_INVALID_HANDLE;
		goto failure;
	}
	file = wdm_file_priv(filp);

	buf = irp->AssociatedIrp.SystemBuffer;
	count = stack->Parameters.Write.Length;
	status = priv->drv->write(dev, file, buf, &count);
	if (!NT_SUCCESS(status)) goto failure;
	ASSERT(status != STATUS_PENDING);
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = count;
	return status;
failure:
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

NTSTATUS wdm_dev_io_read(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	PFILE_OBJECT filp = stack->FileObject;
	os_file *file;
	NTSTATUS status;
	char *buf;
	size_t count = 0;

	if (!filp || !wdm_file_priv(filp) || !priv->drv->read) {
		status = STATUS_INVALID_HANDLE;
		goto failure;
	}
	file = wdm_file_priv(filp);

	buf = irp->AssociatedIrp.SystemBuffer;
	count = stack->Parameters.Read.Length;
	status = priv->drv->read(dev, file, buf, &count);
	if (!NT_SUCCESS(status)) goto failure;
	ASSERT(status != STATUS_PENDING);
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = count;
	return status;
failure:
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

NTSTATUS wdm_dev_io_clean(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION clean_stack;
	KIRQL old_irpl;
	LIST_ENTRY cleanup_irps;
	PLIST_ENTRY curr, next, head;
	PIRP clean_irp;
	
	InitializeListHead(&cleanup_irps);
	
	KeAcquireSpinLock(&priv->queue_lock, &old_irpl);
	head = &priv->queue_irps;
	for (curr = head->Flink, next = curr->Flink;
	     curr != head; curr = next, next = curr->Flink) {
		clean_irp = CONTAINING_RECORD(curr, IRP, Tail.Overlay.ListEntry);
		clean_stack = IoGetCurrentIrpStackLocation(clean_irp);
		if (stack->FileObject == clean_stack->FileObject) {
			RemoveEntryList(curr);
			if (NULL == IoSetCancelRoutine(clean_irp, NULL)) {
				InitializeListHead(curr);
			} else {
				InsertTailList(&cleanup_irps, curr);
			}
		}
	}
	KeReleaseSpinLock(&priv->queue_lock, old_irpl);
	
	while (!IsListEmpty(&cleanup_irps)) {
		curr = RemoveHeadList(&cleanup_irps);
		clean_irp = CONTAINING_RECORD(curr, IRP, Tail.Overlay.ListEntry);
		wdm_dev_io_cmpl(dev, clean_irp, STATUS_CANCELLED, 0);
	}
	
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

NTSTATUS wdm_dev_dispatch_io(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (!priv->is_fdo)
		return wdm_dev_io_cmpl(dev, irp, STATUS_INVALID_DEVICE_REQUEST, 0);

	switch (stack->MajorFunction) {
	case IRP_MJ_CLEANUP:
		status = wdm_dev_io_clean(dev, irp);
		break;
	case IRP_MJ_CREATE:
		status = wdm_dev_io_open(dev, irp);
		break;
	case IRP_MJ_CLOSE:
		status = wdm_dev_io_close(dev, irp);
		break;
	case IRP_MJ_READ:
		status = wdm_dev_io_read(dev, irp);
		break;
	case IRP_MJ_WRITE:
		status = wdm_dev_io_write(dev, irp);
		break;
	case IRP_MJ_DEVICE_CONTROL:
		status = wdm_dev_io_ioctl(dev, irp);
		break;
	}
	return status;
}

status_t wdm_dev_io_start(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	status_t status = STATUS_SUCCESS;

	/* Enable the symbolic links for system components to open handles
	 * to the device
	 */
	status = IoSetDeviceInterfaceState(&priv->InterfaceName, TRUE);
	if (!NT_SUCCESS(status)) {
		os_dbg(OS_DBG_ERR, "IoSetDeviceInterfaceState - failure(%X)\n", status);
		goto end;
	}
end:
	return status;
}

status_t wdm_dev_io_stop(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	status_t status = STATUS_SUCCESS;

	/* Disable any device interfaces by calling
	 * IoSetDeviceInterfaceState.
	 */
	if (priv->io_registered) {
		status = IoSetDeviceInterfaceState(&priv->InterfaceName, FALSE);
		if (!NT_SUCCESS(status)) {
			os_dbg(OS_DBG_ERR,
			       "IoSetDeviceInterfaceState - failure(%X)\n", status);
		}
	}
	return status;
}

status_t wdm_dev_io_init(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	status_t status;

	status = IoRegisterDeviceInterface(priv->is_fdo ? priv->pdo : priv->fdo, 
					   priv->io_clsguid,
					   NULL,
					   &priv->InterfaceName);
	if (NT_SUCCESS(status)) {
		os_dbg(OS_DBG_INFO, "Device Name: %ws\n", priv->InterfaceName.Buffer);
		priv->io_registered = TRUE;
	} else {
		os_dbg(OS_DBG_ERR, "IoRegisterDeviceInterface(%d) - failure(%X)\n", status);
		priv->io_registered = FALSE;
	}
	dev->Flags |= DO_BUFFERED_IO;
	wdm_dev_wmi_init(dev);
	return status;
}

void wdm_dev_io_exit(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	wdm_dev_wmi_exit(dev);
	if (priv->io_registered) {
		RtlFreeUnicodeString(&priv->InterfaceName);
		priv->io_registered = FALSE;
	}
}

/*=========================================================================
 * plug & play
 *=======================================================================*/
wdm_irp_state_t wdm_irp_get_state(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	return priv->queue_state;
}

void wdm_irp_set_state(os_device dev, wdm_irp_state_t state)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	KIRQL old_irql;

	KeAcquireSpinLock(&priv->state_lock, &old_irql);
	priv->queue_state = state;
	KeReleaseSpinLock(&priv->state_lock, old_irql);

	if (state != HoldRequests)
		wdm_dev_io_async_discard(dev);
}

void wdm_dev_pnp_state_changed(os_device dev)
{
	switch (wdm_pnp_get_state(dev)) {
	case DEV_RUNNING:
		wdm_irp_set_state(dev, AllowRequests);
		break;
	case DEV_STOPPING:
	case DEV_STOPPED:
		wdm_irp_set_state(dev, HoldRequests);
		break;
	case DEV_LOWER_REMOVED:
	case DEV_REMOVED:
	case DEV_REMOVING:
		wdm_irp_set_state(dev, FailRequests);
		break;
	}
}

void wdm_dev_init_state(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	__wdm_dev_pnp_init_state(priv);
	wdm_dev_pnp_state_changed(dev);
}

void wdm_dev_save_state(os_device dev, wdm_dev_state_t state)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	KIRQL old_irql;

	KeAcquireSpinLock(&priv->state_lock, &old_irql);
	__wdm_dev_pnp_save_state(priv, state);
	KeReleaseSpinLock(&priv->state_lock, old_irql);
	wdm_dev_pnp_state_changed(dev);
}

wdm_dev_state_t wdm_pnp_get_state(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	return priv->curr_state;
}

void wdm_dev_restore_state(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	KIRQL old_irql;

	KeAcquireSpinLock(&priv->state_lock, &old_irql);
	__wdm_dev_pnp_restore_state(priv);
	KeReleaseSpinLock(&priv->state_lock, old_irql);
	wdm_dev_pnp_state_changed(dev);
}

NTSTATUS wdm_dev_pnp_start(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	
	if (wdm_pnp_get_state(dev) == DEV_REMOVING) {
		status = STATUS_DELETE_PENDING;
		goto end;
	}
	
	/* We cannot touch the device (send it any non pnp irps) until a
	 * start device has been passed down to the lower drivers.
	 */
	status = wdm_dev_io_wait(dev, irp);
	if (!NT_SUCCESS(status)) goto end;

	status = wdm_dev_start(dev);
	if (!NT_SUCCESS(status)) goto end;
	
	wdm_dev_save_state(dev, DEV_RUNNING);
end:
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

NTSTATUS wdm_dev_pnp_query_stop(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);

	wdm_dev_save_state(dev, DEV_STOPPING);
	status = wdm_dev_io_skip(dev, irp);
	return status;
}

NTSTATUS __wdm_pnp_cancel_stop(PDEVICE_OBJECT dev, PIRP irp,
			       wdm_dev_state_t valid_state)
{
	NTSTATUS status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

	if (valid_state != wdm_pnp_get_state(dev))
		goto end;

	status = wdm_dev_io_wait(dev, irp);
	if (NT_SUCCESS(status))
		wdm_dev_restore_state(dev);
end:
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

NTSTATUS wdm_dev_pnp_stop(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);

	wdm_dev_stop(dev, FALSE);
	wdm_dev_save_state(dev, DEV_STOPPED);
	status = wdm_dev_io_skip(dev, irp);
	return status;
}

NTSTATUS wdm_dev_pnp_cancel_stop(PDEVICE_OBJECT dev, PIRP irp)
{
	return __wdm_pnp_cancel_stop(dev, irp, DEV_STOPPING);
}

NTSTATUS wdm_dev_pnp_query_remove(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);
	
	/* Once a driver succeeds an IRP_MN_QUERY_REMOVE_DEVICE and it
	 * considers the device to be in the remove-pending state, the
	 * driver must fail any subsequent create requests for the device.
	 * The driver processes all other IRPs as usual, until the driver
	 * receives an IRP_MN_CANCEL_REMOVE_DEVICE or an
	 * IRP_MN_REMOVE_DEVICE.
	 */
	wdm_dev_save_state(dev, DEV_REMOVING);
	
	/* If the driver previously sent an IRP_MN_WAIT_WAKE request to
	 * enable the device for wake-up, cancel the wait-wake IRP.
	 */
	wdm_stop_wait_wake(dev);

	/* Record the previous PnP state of the device. */

	/* Finish the IRP in a function or filter driver. */
	status = wdm_dev_io_skip(dev, irp);
	return status;
}

NTSTATUS wdm_dev_pnp_remove(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	
	/* Has the driver already handled a previous
	 * IRP_MN_SURPRISE_REMOVAL request for this FDO?  If so, perform
	 * any remaining clean-up and skip to DEV_REMOVED handling.
	 */
	if (DEV_LOWER_REMOVED != wdm_pnp_get_state(dev)) {
		wdm_dev_stop(dev, TRUE);
	}
	status = wdm_dev_io_skip(dev, irp);

	/* XXX: IRP Reference Count Consistence
	 * Decrease to ensure that wdm_dev_remove can work correctly and
	 * then don't forget to increase it before return.
	 */
	os_dev_dec_kern(dev, "IRP_MJ_PNP(REMOVE)");
	wdm_dev_remove(dev);
	os_dev_inc_kern(dev, "IRP_MJ_PNP(REMOVE)");

	IoDetachDevice(priv->attached_dev);
	IoDeleteDevice(dev);
	wdm_dev_save_state(dev, DEV_REMOVED);
	
	/* Return from the DispatchPnP routine, propagating the return
	 * status from IoCallDriver.  A function driver does not specify
	 * an IoCompletion routine for a remove IRP, nor does it complete
	 * the IRP. Remove IRPs are completed by the parent bus driver.
	 */
	return status;
}

NTSTATUS wdm_dev_pnp_cancel_remove(PDEVICE_OBJECT dev, PIRP irp)
{
	return __wdm_pnp_cancel_stop(dev, irp, DEV_REMOVING);
}

NTSTATUS wdm_dev_pnp_surprise_removal(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	
	wdm_dev_save_state(dev, DEV_LOWER_REMOVED);
	wdm_dev_stop(dev, TRUE);
	status = wdm_dev_io_skip(dev, irp);
	return status;
}

NTSTATUS wdm_dev_pnp_query_capabilities(PDEVICE_OBJECT dev, PIRP irp)
{
	ULONG i;
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	PDEVICE_CAPABILITIES caps;
	
	caps = stack->Parameters.DeviceCapabilities.Capabilities;
	/* We will provide here an example of an IRP that is processed
	 * both on its way down and on its way up: there might be no need
	 * for a function driver process this irp (the bus driver will do
	 * that).  The driver will wait for the lower drivers (the bus
	 * driver among  them) to process this IRP, then it processes it
	 * again.
	 */
	if (caps->Version < 1 || caps->Size < sizeof(DEVICE_CAPABILITIES)) {
		status = STATUS_UNSUCCESSFUL;
		goto end;
	}
	if (priv->drv_flags & OS_DEVICE_HOTPLUG)
		caps->SurpriseRemovalOK = TRUE;
	else
		caps->SurpriseRemovalOK = FALSE;
	if (priv->drv_flags & OS_DEVICE_REMOVE)
		caps->Removable = TRUE;
	else
		caps->Removable = FALSE;
	if (priv->is_fdo) {
		/* Pass the IRP down and let the lower driver determine the
		 * capabilities.
		 */
		status = wdm_dev_io_wait(dev, irp);
	} else {
		/* Determine our capabilities here. */
	}
	priv->caps = *caps;
	wdm_dev_power_caps(dev);
end:
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

#ifdef CONFIG_BUS
NTSTATUS wdm_dev_pnp_skip(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (priv->is_fdo)
		return wdm_dev_io_skip(dev, irp);
	else
		return __wdm_dev_io_cmpl(dev, irp);
}

NTSTATUS wdm_dev_dispatch_ioctl(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (priv->is_fdo)
		return wdm_dev_io_cmpl(dev, irp, STATUS_INVALID_DEVICE_REQUEST, 0);
	return wdm_bus_dispatch_ioctl(dev, irp);
}

/* IRP_MN_QUERY_DEVICE_RELATIONS
 * The PnP manager sends this request to determine certain relationships
 * among devices. The following types of drivers handle this request:
 * 1. Bus drivers MUST handle BusRelations requests for their adapter or
 *    controller (bus FDO).
 *    Filter drivers MIGHT handle BusRelations requests.
 * 2. Bus drivers MUST handle TargetDeviceRelation requests for their
 *    child devices (child PDOs). 
 * 3. Function and filter drivers MIGHT handle RemovalRelations requests. 
 * 4. Bus drivers MIGHT handle EjectionRelations requests for their child
 *    devices (child PDOs).
 * This implementation will not implement "MIGHT" requirements.
 */
NTSTATUS wdm_fdo_pnp_query_relations(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	PDEVICE_RELATIONS rels = (PDEVICE_RELATIONS)irp->IoStatus.Information;
	rels = wdm_bus_query_childs(dev, rels);
	if (!rels)
		status = STATUS_INSUFFICIENT_RESOURCES;
	return wdm_dev_io_cmpl(dev, irp, status, (ULONG_PTR)rels);
}

NTSTATUS wdm_pdo_pnp_query_relations(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	PDEVICE_RELATIONS rels = (PDEVICE_RELATIONS)irp->IoStatus.Information;
	rels = wdm_bus_query_parent(dev, rels);
	if (!rels)
		status = STATUS_INSUFFICIENT_RESOURCES;
	return wdm_dev_io_cmpl(dev, irp, status, (ULONG_PTR)rels);
}

NTSTATUS wdm_dev_pnp_query_relations(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	switch (stack->Parameters.QueryDeviceRelations.Type) {
	case TargetDeviceRelation:
		if (!priv->is_fdo)
			return wdm_pdo_pnp_query_relations(dev, irp);
		break;
	case BusRelations:
		if (priv->is_fdo)
			return wdm_fdo_pnp_query_relations(dev, irp);
		break;
	}
	return wdm_dev_pnp_skip(dev, irp);
}

/* IRP_MN_QUERY_ID
 * 1. Bus drivers must handle requests for BusQueryDeviceID for their
 *    child devices (child PDOs).
 * 2. Bus drivers can handle requests for BusQueryHardwareIDs,
 *    BusQueryCompatibleIDs, and BusQueryInstanceID for their child
 *    devices.
 * 3. Beginning with Windows 7, bus drivers must also handle requests for
 *    BusQueryContainerID for their child PDOs. 
 * Note  Function drivers and filter drivers do not handle this IRP.
 */

NTSTATUS wdm_dev_pnp_query_id(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (!priv->fdo) {
		PWCHAR buffer = NULL;
		PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

		switch (stack->Parameters.QueryId.IdType) {
		case BusQueryDeviceID:
		case BusQueryHardwareIDs:
			buffer = os_mem_alloc(priv->u.dev.hardware_id_len,
					      "HARDWARE_ID");
			if (!buffer) {
				status = STATUS_INSUFFICIENT_RESOURCES;
				break;
			}
			os_mem_copy(buffer, priv->u.dev.hardware_id,
				    priv->u.dev.hardware_id_len);
			break;
		case BusQueryCompatibleIDs:
			buffer = os_mem_alloc(priv->u.dev.compatible_id_len,
					      "COMPATIBLE_ID");
			if (!buffer) {
				status = STATUS_INSUFFICIENT_RESOURCES;
				break;
			}
			os_mem_copy(buffer, priv->u.dev.compatible_id,
				    priv->u.dev.compatible_id_len);
			break;
		case BusQueryInstanceID:
			buffer = os_mem_alloc(MAX_PNP_STRING_SIZE * sizeof(WCHAR),
					      "INSTANCE_ID");
			if (!buffer) {
				status = STATUS_INSUFFICIENT_RESOURCES;
				break;
			}
			RtlStringCchPrintfW(buffer, MAX_PNP_STRING_SIZE, L"%02d",
					    priv->u.dev.serial_no);
			break;
#ifdef BusQueryContainerID
		case BusQueryContainerID:
			break;
		default:
			return wdm_dev_pnp_skip(dev, irp);
#endif
		}
		if (buffer) {
			os_dbg(OS_DBG_DEBUG, "%s - %LS\n",
			       wdm_bus_id_string(stack->Parameters.QueryId.IdType),
			       buffer);
		}
		return wdm_dev_io_cmpl(dev, irp, status, (LONG_PTR)buffer);
	}
	return wdm_dev_pnp_skip(dev, irp);
}

/* IRP_MN_QUERY_DEVICE_TEXT
 * The PnP manager uses this IRP to get a device's description or location
 * information.
 * 1. Bus drivers must handle this request for their child devices if the
 *    bus supports this information.
 * 2. Function and filter drivers do not handle this IRP.
 * 3. Bus drivers are strongly encouraged to return device descriptions
 *    for their child devices.
 * 4. Bus drivers are also encouraged to return LocationInformation for
 *    their child devices, but this information is optional.
 */
NTSTATUS wdm_dev_pnp_query_text(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (!priv->fdo) {
		PWCHAR buffer = (PWCHAR)irp->IoStatus.Information;
		PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

		switch (stack->Parameters.QueryDeviceText.DeviceTextType) {
		case DeviceTextDescription:
			if (!buffer) {
				buffer = os_mem_alloc(MAX_PNP_STRING_SIZE * sizeof(WCHAR),
						      "TEXT_DESCRIPTION");
				if (buffer == NULL) {
					status = STATUS_INSUFFICIENT_RESOURCES;
					break;
				}
				RtlStringCchPrintfW(buffer, MAX_PNP_STRING_SIZE,
						    OS_DRIVER_TEXT_DESC);
			}
			break;
		case DeviceTextLocationInformation:
			if (!buffer) {
				buffer = os_mem_alloc(MAX_PNP_STRING_SIZE * sizeof(WCHAR),
						      "TEXT_LOCATION_INFORMATION");
				if (buffer == NULL) {
					status = STATUS_INSUFFICIENT_RESOURCES;
					break;
				}
				RtlStringCchPrintfW(buffer, MAX_PNP_STRING_SIZE,
						    OS_DRIVER_TEXT_DESC);
			}
			break;
		default:
			return wdm_dev_pnp_skip(dev, irp);
		}
		if (buffer) {
			os_dbg(OS_DBG_DEBUG, "%s - %LS\n",
			       wdm_bus_text_string(stack->Parameters.QueryId.IdType),
			       buffer);
		}
		return wdm_dev_io_cmpl(dev, irp, status, (LONG_PTR)buffer);
	}
	return wdm_dev_pnp_skip(dev, irp);
}

#ifdef CONFIG_BUS_RES
/* TODO: Hardware Resource Allocations
 *
 * Require a more flexible machanism for real hardwares.
 */
NTSTATUS wdm_dev_pnp_query_resources(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (!priv->fdo) {
		NTSTATUS status = STATUS_SUCCESS;
		PCM_RESOURCE_LIST rl;
		PCM_FULL_RESOURCE_DESCRIPTOR frd;
		PCM_PARTIAL_RESOURCE_DESCRIPTOR prd;

		rl = os_mem_alloc(sizeof (CM_RESOURCE_LIST), "CM_RESOURCE_LIST");
		if (!rl) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			goto end;
		}
		
		rl->Count = 1;
		frd = &rl->List[0];
		frd->InterfaceType = InterfaceTypeUndefined;
		frd->BusNumber = 0;
		frd->PartialResourceList.Count = 1;
		prd = &frd->PartialResourceList.PartialDescriptors[0];
		prd->Type = CmResourceTypePort;
		prd->ShareDisposition = CmResourceShareShared;
		prd->Flags = CM_RESOURCE_PORT_IO | CM_RESOURCE_PORT_16_BIT_DECODE;
		prd->u.Port.Start.QuadPart = 0xBAD; /* some random port number */
		prd->u.Port.Length = 1;
end:
		return wdm_dev_io_cmpl(dev, irp, status, (ULONG_PTR)rl);
	}
	return wdm_dev_pnp_skip(dev, irp);
}

NTSTATUS wdm_dev_pnp_query_requirements(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (!priv->fdo) {
		NTSTATUS status = STATUS_SUCCESS;
		PIO_RESOURCE_REQUIREMENTS_LIST rrl;
		PIO_RESOURCE_DESCRIPTOR rd;
		ULONG rrl_size;
		
		rrl_size = sizeof(IO_RESOURCE_REQUIREMENTS_LIST);
		rrl = os_mem_alloc(rrl_size, "IO_RESOURCE_REQUIREMENTS_LIST");
		if (!rrl) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			goto end;
		}

		rrl->ListSize = rrl_size;
		rrl->AlternativeLists = 1;
		rrl->InterfaceType = InterfaceTypeUndefined;
		rrl->BusNumber = 0;
		rrl->List[0].Version = 1;
		rrl->List[0].Revision = 1;
		rrl->List[0].Count = 1;
		rd = rrl->List[0].Descriptors;
		rd->Type = CmResourceTypePort;
		rd->ShareDisposition = CmResourceShareDeviceExclusive;
		rd->Flags = CM_RESOURCE_PORT_IO|CM_RESOURCE_PORT_16_BIT_DECODE;
		rd->u.Port.Length = 1;
		rd->u.Port.Alignment = 0x01;
		rd->u.Port.MinimumAddress.QuadPart = 0;
		rd->u.Port.MaximumAddress.QuadPart = 0xFFFF;
end:
		return wdm_dev_io_cmpl(dev, irp, status, (ULONG_PTR)rrl);
	}
	return wdm_dev_pnp_skip(dev, irp);
}
#else
#define wdm_dev_pnp_query_resources(dev, irp)		wdm_dev_pnp_skip(dev, irp)
#define wdm_dev_pnp_query_requirements(dev, irp)	wdm_dev_pnp_skip(dev, irp)
#endif

NTSTATUS wdm_dev_pnp_query_information(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (!priv->fdo) {
		NTSTATUS status = STATUS_SUCCESS;
		PPNP_BUS_INFORMATION bi;

		bi = os_mem_alloc(sizeof(PNP_BUS_INFORMATION), "BUS_INFORMATION");
		if (bi == NULL) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			goto end;
		}
		bi->BusTypeGuid = OS_DRIVER_BUS_TYPE;
		bi->LegacyBusType = PNPBus;
		bi->BusNumber = os_driver_bus_num(dev);
end:
		return wdm_dev_io_cmpl(dev, irp, status, (ULONG_PTR)bi);
	}
	return wdm_dev_pnp_skip(dev, irp);
}

NTSTATUS wdm_dev_pnp_query_interface(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (!priv->fdo) {
		PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
		char guid_string[2048];
		GUID *if_guid;
		PINTERFACE *if_bus;
		USHORT if_size, if_ver;
		int i;
		NTSTATUS status;

		if_guid = (GUID *)stack->Parameters.QueryInterface.InterfaceType;
		for (i = 0; i < sizeof(GUID); i++) {
			RtlStringCchPrintfA(guid_string+2*i, 3, "%02X",
					    *((unsigned char *)if_guid+i));
		}
		if_size = stack->Parameters.QueryInterface.Size;
		if_ver = stack->Parameters.QueryInterface.Version;
		if_bus = (PINTERFACE *)stack->Parameters.QueryInterface.Interface;
		if (!IsEqualGUID(if_guid, OS_DRIVER_BUSIF_GUID)) {
			os_dbg(OS_DBG_ERR, "QUERY_INTERFACE: unsupported GUID %s\n",
			       guid_string);
			return __wdm_dev_io_cmpl(dev, irp);
		}
		status = wdm_bus_query_interface(dev, if_size, if_ver, if_bus);
		return wdm_dev_io_cmpl(dev, irp, status, 0);
	}
	return wdm_dev_pnp_skip(dev, irp);
}

NTSTATUS wdm_dev_pnp_eject(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (!priv->is_fdo) {
		priv->u.dev.present = FALSE;
		return wdm_dev_io_cmpl(dev, irp, STATUS_SUCCESS, 0);
	}
	return wdm_dev_pnp_skip(dev, irp);
}

NTSTATUS wdm_dev_pnp_usage_notification(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (!priv->is_fdo)
		return wdm_dev_io_cmpl(dev, irp, STATUS_UNSUCCESSFUL, 0);
	return wdm_dev_pnp_skip(dev, irp);
}
#else
NTSTATUS wdm_dev_pnp_skip(PDEVICE_OBJECT dev, PIRP irp)
{
	return wdm_dev_io_skip(dev, irp);
}

NTSTATUS wdm_dev_dispatch_ioctl(PDEVICE_OBJECT dev, PIRP irp)
{
	return wdm_dev_io_cmpl(dev, irp, STATUS_INVALID_DEVICE_REQUEST, 0);
}

#define wdm_dev_pnp_query_relations(dev, irp)		wdm_dev_pnp_skip(dev, irp)
#define wdm_dev_pnp_query_id(dev, irp)			wdm_dev_pnp_skip(dev, irp)
#define wdm_dev_pnp_query_text(dev, irp)		wdm_dev_pnp_skip(dev, irp)
#define wdm_dev_pnp_query_resources(dev, irp)		wdm_dev_pnp_skip(dev, irp)
#define wdm_dev_pnp_query_requirements(dev, irp)	wdm_dev_pnp_skip(dev, irp)
#define wdm_dev_pnp_query_information(dev, irp)		wdm_dev_pnp_skip(dev, irp)
#define wdm_dev_pnp_query_interface(dev, irp)		wdm_dev_pnp_skip(dev, irp)
#define wdm_dev_pnp_usage_notification(dev, irp)	wdm_dev_pnp_skip(dev, irp)
#define wdm_dev_pnp_eject(dev, irp)			wdm_dev_pnp_skip(dev, irp)
#endif

NTSTATUS wdm_fdo_dispatch_pnp(PDEVICE_OBJECT dev, PIRP irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	struct wdm_device *priv = wdm_dev_priv(dev);
	NTSTATUS status;
	uint8_t minor = stack->MinorFunction;
	
	os_dbg(OS_DBG_INFO, "IRP_MJ_PNP %s(%d) - begins\n",
	       wdm_dev_pnp_string(minor), minor);
	
	switch (stack->MinorFunction) {
	case IRP_MN_QUERY_CAPABILITIES:
		status = wdm_dev_pnp_query_capabilities(dev, irp);
		break;
	case IRP_MN_QUERY_DEVICE_RELATIONS:
		status = wdm_dev_pnp_query_relations(dev, irp);
		break;
	case IRP_MN_QUERY_ID:
		status = wdm_dev_pnp_query_id(dev, irp);
		break;
	case IRP_MN_QUERY_DEVICE_TEXT:
		status = wdm_dev_pnp_query_text(dev, irp);
		break;
	case IRP_MN_QUERY_RESOURCES:
		status = wdm_dev_pnp_query_resources(dev, irp);
		break;
	case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
		status = wdm_dev_pnp_query_requirements(dev, irp);
		break;
	case IRP_MN_QUERY_BUS_INFORMATION:
		status = wdm_dev_pnp_query_information(dev, irp);
		break;
	case IRP_MN_QUERY_INTERFACE:
		status = wdm_dev_pnp_query_interface(dev, irp);
		break;
	case IRP_MN_EJECT:
		status = wdm_dev_pnp_eject(dev, irp);
		break;
	case IRP_MN_DEVICE_USAGE_NOTIFICATION:
		status = wdm_dev_pnp_usage_notification(dev, irp);
		break;

	/* Drivers that run only on Windows 2000 and later versions of
	 * Windows (WDM versions 0x10 and greater) receive stop IRPs only
	 * when the PnP manager rebalances resources.
	 */
	case IRP_MN_QUERY_STOP_DEVICE:
		/* XXX: IRP Reference Count Consistence
		 * Decrease to ensure that os_dev_can_stop can work
		 * correctly and then don't forget to increase it before
		 * return.
		 */
		os_dev_dec_kern(dev, "IRP_MJ_PNP(REMOVE)");
		if (os_dev_can_stop(dev)) {
			status = wdm_dev_pnp_query_stop(dev, irp);
			os_dev_inc_kern(dev, "IRP_MJ_PNP(REMOVE)");
			break;
		}
		os_dev_inc_kern(dev, "IRP_MJ_PNP(REMOVE)");
		status = wdm_dev_io_cmpl(dev, irp, STATUS_UNSUCCESSFUL, 0);
		break;
	case IRP_MN_CANCEL_STOP_DEVICE:
		status = wdm_dev_pnp_cancel_stop(dev, irp);
		break;
	case IRP_MN_STOP_DEVICE:
		status = wdm_dev_pnp_stop(dev, irp);
		break;
	case IRP_MN_QUERY_REMOVE_DEVICE:
		/* XXX: IRP Reference Count Consistence
		 * Decrease to ensure that os_dev_can_stop can work
		 * correctly and then don't forget to increase it before
		 * return.
		 */
		os_dev_dec_kern(dev, "IRP_MJ_PNP(REMOVE)");
		if (os_dev_can_stop(dev)) {
			os_dev_inc_kern(dev, "IRP_MJ_PNP(REMOVE)");
			status = wdm_dev_pnp_query_remove(dev, irp);
			break;
		}
		os_dev_inc_kern(dev, "IRP_MJ_PNP(REMOVE)");
		status = wdm_dev_io_cmpl(dev, irp, STATUS_UNSUCCESSFUL, 0);
		break;
	case IRP_MN_CANCEL_REMOVE_DEVICE:
		status = wdm_dev_pnp_cancel_remove(dev, irp);
		break;
	case IRP_MN_REMOVE_DEVICE:
		status = wdm_dev_pnp_remove(dev, irp);
		break;
	case IRP_MN_SURPRISE_REMOVAL:
		status = wdm_dev_pnp_surprise_removal(dev, irp);
		break;
	case IRP_MN_START_DEVICE:
		status = wdm_dev_pnp_start(dev, irp);
		break;
	default:
		status = wdm_dev_pnp_skip(dev, irp);
		break;
	}

	os_dbg(OS_DBG_INFO, "IRP_MJ_PNP %s(%d) - ends\n",
	       wdm_dev_pnp_string(minor), minor);
	return status;
}

NTSTATUS wdm_pdo_dispatch_pnp(PDEVICE_OBJECT dev, PIRP irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	struct wdm_device *priv = wdm_dev_priv(dev);
	NTSTATUS status;
	uint8_t minor = stack->MinorFunction;
	
	os_dbg(OS_DBG_INFO, "IRP_MJ_PNP %s(%d) - begins\n",
	       wdm_dev_pnp_string(minor), minor);
	
	switch (stack->MinorFunction) {
	case IRP_MN_QUERY_CAPABILITIES:
		status = wdm_dev_pnp_query_capabilities(dev, irp);
		break;
	case IRP_MN_QUERY_DEVICE_RELATIONS:
		status = wdm_dev_pnp_query_relations(dev, irp);
		break;

	/* Drivers that run only on Windows 2000 and later versions of
	 * Windows (WDM versions 0x10 and greater) receive stop IRPs only
	 * when the PnP manager rebalances resources.
	 */
	case IRP_MN_QUERY_STOP_DEVICE:
		/* XXX: IRP Reference Count Consistence
		 * Decrease to ensure that os_dev_can_stop can work
		 * correctly and then don't forget to increase it before
		 * return.
		 */
		os_dev_dec_kern(dev, "IRP_MJ_PNP(REMOVE)");
		if (os_dev_can_stop(dev)) {
			status = wdm_dev_pnp_query_stop(dev, irp);
			os_dev_inc_kern(dev, "IRP_MJ_PNP(REMOVE)");
			break;
		}
		os_dev_inc_kern(dev, "IRP_MJ_PNP(REMOVE)");
		status = wdm_dev_io_cmpl(dev, irp, STATUS_UNSUCCESSFUL, 0);
		break;
	case IRP_MN_CANCEL_STOP_DEVICE:
		status = wdm_dev_pnp_cancel_stop(dev, irp);
		break;
	case IRP_MN_STOP_DEVICE:
		status = wdm_dev_pnp_stop(dev, irp);
		break;
	case IRP_MN_QUERY_REMOVE_DEVICE:
		/* XXX: IRP Reference Count Consistence
		 * Decrease to ensure that os_dev_can_stop can work
		 * correctly and then don't forget to increase it before
		 * return.
		 */
		os_dev_dec_kern(dev, "IRP_MJ_PNP(REMOVE)");
		if (os_dev_can_stop(dev)) {
			os_dev_inc_kern(dev, "IRP_MJ_PNP(REMOVE)");
			status = wdm_dev_pnp_query_remove(dev, irp);
			break;
		}
		os_dev_inc_kern(dev, "IRP_MJ_PNP(REMOVE)");
		status = wdm_dev_io_cmpl(dev, irp, STATUS_UNSUCCESSFUL, 0);
		break;
	case IRP_MN_CANCEL_REMOVE_DEVICE:
		status = wdm_dev_pnp_cancel_remove(dev, irp);
		break;
	case IRP_MN_REMOVE_DEVICE:
		status = wdm_dev_pnp_remove(dev, irp);
		break;
	case IRP_MN_SURPRISE_REMOVAL:
		status = wdm_dev_pnp_surprise_removal(dev, irp);
		break;
	case IRP_MN_START_DEVICE:
		status = wdm_dev_pnp_start(dev, irp);
		break;
	default:
		status = wdm_dev_io_skip(dev, irp);
		break;
	}

	os_dbg(OS_DBG_INFO, "IRP_MJ_PNP %s(%d) - ends\n",
	       wdm_dev_pnp_string(minor), minor);
	return status;
}

NTSTATUS wdm_dev_dispatch_pnp(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (priv->is_fdo)
		return wdm_fdo_dispatch_pnp(dev, irp);
	else
		return wdm_pdo_dispatch_pnp(dev, irp);
}

status_t wdm_dev_pnp_init(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	KeInitializeSpinLock(&priv->state_lock);
	InitializeListHead(&priv->queue_irps);
	KeInitializeSpinLock(&priv->queue_lock);
	wdm_dev_init_state(dev);
	return STATUS_SUCCESS;
}

void wdm_dev_pnp_exit(os_device dev)
{
	/* TODO: check priv->queue_irps */
}

/*=========================================================================
 * power management
 *=======================================================================*/
struct wdm_power_request *wdm_irp_power_get(os_device dev, const char *what)
{
	struct wdm_power_request *ctx;
	ctx = os_mem_alloc(sizeof (struct wdm_power_request), what);
	if (!ctx) return NULL;
	ctx->dev = os_dev_get_kern(dev, what);
	return ctx;
}

void wdm_irp_power_put(struct wdm_power_request *ctx, const char *what)
{
	os_device dev;
	if (ctx) {
		/* XXX: Free/Put Orders
		 *
		 * Free/put operations are strictly ordered to make
		 * os_mem_leak_dump happy.
		 */
		dev = ctx->dev;
		os_mem_free(ctx);
		os_dev_put_kern(dev, what);
	}
}

VOID __wdm_dev_pocmpl_req(PDEVICE_OBJECT pdo,
			  UCHAR minor, POWER_STATE power,
			  PVOID context, PIO_STATUS_BLOCK iosb)
{
	struct wdm_power_request *ctx = context;
	struct wdm_device *priv = wdm_dev_priv(ctx->dev);
	NTSTATUS status = iosb->Status;

	if (!NT_SUCCESS(status))
		os_dbg(OS_DBG_ERR, "PoRequestPowerIrp - failure(%X)", status);
	if (ctx->cmpl)
		ctx->cmpl(ctx->dev, minor, power, ctx->ctx, iosb);
	if (ctx->waiter)
		os_waiter_wake(ctx->waiter);
	wdm_irp_power_put(ctx, "PoRequestPowerIrp");
}

status_t wdm_dev_po_request(os_device dev, BOOLEAN wait,
			    UCHAR minor, POWER_STATE power,
			    PO_COMPLETION_ROUTINE *cmpl, PVOID context)
{
	NTSTATUS status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);
	struct wdm_power_request *ctx = NULL;
	os_waiter waiter;

	ctx = wdm_irp_power_get(dev, "PoRequestPowerIrp");
	if (!ctx) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto end;
	}
	ctx->cmpl = cmpl;
	ctx->ctx = context;
	if (wait) {
		os_waiter_init(&waiter, FALSE);
		ctx->waiter = &waiter;
	} else {
		ctx->waiter = NULL;
	}
	status = PoRequestPowerIrp(priv->pdo,
				   minor, 
				   power,
				   __wdm_dev_pocmpl_req,
				   ctx,
				   NULL);
	if (!NT_SUCCESS(status)) {
		os_dbg(OS_DBG_ERR,
		       "PoRequestPowerIrp(%s) - failure(%X)\n",
		       wdm_dev_pm_string(minor), status);
		wdm_irp_power_put(ctx, "PoRequestPowerIrp");
		goto end;
	}
	if (wait) {
		if (STATUS_PENDING == status) {
			os_waiter_wait(&waiter, OS_WAIT_INFINITE);
		}
	} else {
		status = STATUS_PENDING;
	}

end:
	return status;
}

status_t wdm_dev_set_power(os_device dev, BOOLEAN wait,
			   DEVICE_POWER_STATE state,
			   PO_COMPLETION_ROUTINE *cmpl, PVOID context)
{
	NTSTATUS status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);
	POWER_STATE power;
	struct wdm_power_request *ctx = NULL;

	if (wdm_dev_get_power(dev) == state) {
		os_dbg(OS_DBG_DEBUG, "IRP_MN_SET_POWER(DEV) - already stable\n");
		return STATUS_SUCCESS;
	}
	power.DeviceState = state;
	return wdm_dev_po_request(dev, wait, IRP_MN_SET_POWER, power,
				  cmpl, context);
}

status_t wdm_dev_resume(os_device dev, BOOLEAN wait,
			PO_COMPLETION_ROUTINE *cmpl, PVOID context)
{
	return wdm_dev_set_power(dev, wait, PowerDeviceD0, cmpl, context);
}

status_t wdm_dev_suspend(os_device dev, BOOLEAN wait,
			 PO_COMPLETION_ROUTINE *cmpl, PVOID context)
{
	int i;
	struct wdm_device *priv = wdm_dev_priv(dev);
	DEVICE_POWER_STATE power = priv->caps.DeviceWake;
	if (power == PowerDeviceUnspecified) {
		for (i = PowerSystemSleeping1; i <= PowerSystemSleeping3; i++) {
			if (priv->caps.DeviceState[i] < PowerDeviceD3) {
				power = priv->caps.DeviceState[i];
			}
		}
	}
	if (power == PowerDeviceUnspecified || power <= PowerDeviceD0) {
		power = PowerDeviceD2;
	}
	return wdm_dev_set_power(dev, wait, power, cmpl, context);
}

struct wdm_io_async *wdm_io_async_get(os_device dev)
{
	struct wdm_io_async *async;
	PIO_WORKITEM item;
	async = os_mem_alloc(sizeof(struct wdm_io_async), "IO_ASYNC");
	if (!async) return NULL;
	item = IoAllocateWorkItem(dev);
	if (!item) {
		os_dbg(OS_DBG_ERR, "IoAllocateWorkItem - failure\n");
		os_mem_free((PVOID)async);
		return NULL;
	}
	async->dev = os_dev_get_kern(dev, "IO_ASYNC");
	async->item = item;
	return async;
}

void wdm_io_async_put(struct wdm_io_async *async)
{
	if (async) {
		if (async->dev) os_dev_put_kern(async->dev, "IO_ASYNC");
		if (async->item) IoFreeWorkItem(async->item);
		os_mem_free((PVOID)async);
	}
}

NTSTATUS __wdm_dev_iocmpl_async(PDEVICE_OBJECT __dev, PIRP irp, PVOID context)
{
	struct wdm_io_async *async = context;
	status_t status = irp->IoStatus.Status;
	LONG info = irp->IoStatus.Information;

	if (async->routine)
		status = async->routine(async->dev, irp, async->context);
	wdm_io_async_put(async);
	return status;
}

/* Routine Description:
 *     This routine waits for the I/O in progress to finish and then
 *     sends the device power irp (query/set) down the stack.
 * Arguments:
 *     dev - pointer to device object
 *     Context - context passed to the work-item.
 * Return Value:
 *     None
 */
VOID __wdm_dev_worker_async(PDEVICE_OBJECT dev, PVOID ctx)
{
	PIRP irp;
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	struct wdm_io_async *async = (struct wdm_io_async *)ctx;
	int i;

	os_dbg(OS_DBG_DEBUG, "IO_ASYNC(hold) - running\n");
	irp = (PIRP)async->irp;

#if 0
	/* XXX: Correct Worker Consistence
	 * Our driver architecture must ensure that the device has already
	 * been suspended.  So that the following codes could be useless.
	 */
	/* XXX: IRP Reference Count Consistence
	 * Decrease to ensure that os_dev_wait_stopped can work correctly
	 * and then don't forget to increase it before return.
	 */
	os_dev_dec_kern(dev, "IO_ASYNC");
	for (i = 0; i < async->nr_irps; i++) {
		/* XXX: IRP Reference Count Consistence
		 * Decrease to ensure that os_dev_wait_stopped can work
		 * correctly and then don't forget to increase it before
		 * return.
		 */
		os_dev_dec_kern(dev, "IRP_HOLDED");
	}
	os_dev_wait_stopped(dev);
	for (i = 0; i < async->nr_irps; i++) {
		/* XXX: IRP Reference Count Consistence
		 * Decrease to ensure that os_dev_wait_stopped can work
		 * correctly and then don't forget to increase it before
		 * return.
		 */
		os_dev_inc_kern(dev, "IRP_HOLDED");
	}
	/* Increase to restore the count. */
	os_dev_inc_kern(dev, "IO_ASYNC");
#endif
	if (async->async_call)
		async->async_call(async->dev, async->async_data);

	/* Now send the irp down. */
	status = wdm_dev_io_call(dev, irp, FALSE,
				 __wdm_dev_iocmpl_async, async);
	if (!NT_SUCCESS(status)) {
		__wdm_dev_iocmpl_async(priv->attached_dev, irp, async);
		os_dbg(OS_DBG_DEBUG, "IO_ASYNC(hold) - ends\n");
	}
}

/* Routine Description:
 *     This routine is called on query or set power DOWN irp for the
 *     device.  This routine queues a workitem.
 * Arguments:
 *     dev - pointer to device object
 *     irp - I/O request packet
 * Return Value:
 *     NT status value
 */
status_t wdm_dev_io_async_submit(os_device dev, PIRP irp,
				 wdm_io_async_cb async_call,
				 ULONG async_data,
				 PIO_COMPLETION_ROUTINE routine,
				 void *context)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	struct wdm_io_async *async;
	
	os_dbg(OS_DBG_DEBUG, "IO_ASYNC(hold) - begins\n");

	wdm_irp_set_state(dev, HoldRequests);
	async = wdm_io_async_get(dev);
	if (!async) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto end;
	}
	async->irp = irp;
	async->routine = routine;
	async->context = context;
	async->async_call = async_call;
	async->async_data = async_data;
	os_dbg(OS_DBG_DEBUG, "IO_ASYNC(hold) - queuing\n");
	IoMarkIrpPending(irp);
	IoQueueWorkItem(async->item, __wdm_dev_worker_async,
			DelayedWorkQueue, async);
	status = STATUS_PENDING;

end:
	if (!NT_SUCCESS(status))
		os_dbg(OS_DBG_DEBUG, "IO_ASYNC(hold) - ends\n");
	return status;
}

/* Routine Description:
 *     Remove and process the entries in the queue. If this routine is called
 *     when processing IRP_MN_CANCEL_STOP_DEVICE, IRP_MN_CANCEL_REMOVE_DEVICE
 *     or IRP_MN_START_DEVICE, the requests are passed to the next lower driver.
 *     If the routine is called when IRP_MN_REMOVE_DEVICE is received, the IRPs
 *     are complete with STATUS_DELETE_PENDING
 * Arguments:
 *     priv - pointer to device extension
 * Return Value:
 *     None
 */
void wdm_dev_io_async_discard(os_device dev)
{
	KIRQL old_irql;
	PIRP next_irp, cancelled_irp;
	PDRIVER_CANCEL cancel_routine;
	LIST_ENTRY cancelled_irps;
	PLIST_ENTRY list;
	struct wdm_device *priv = wdm_dev_priv(dev);
	
	os_dbg(OS_DBG_DEBUG, "IO_ASYNC(unhold) - begins\n");

	ASSERT(wdm_irp_get_state(dev) != HoldRequests);
	
	cancel_routine = NULL;
	InitializeListHead(&cancelled_irps);
	
	/* 1.  dequeue the entries in the queue
	 * 2.  reset the cancel routine
	 * 3.  process them
	 * 3a. if the device is active, send them down
	 * 3b. else complete with STATUS_DELETE_PENDING
	 */
	while (1) {
		KeAcquireSpinLock(&priv->queue_lock, &old_irql);
		if (IsListEmpty(&priv->queue_irps)) {
			KeReleaseSpinLock(&priv->queue_lock, old_irql);
			break;
		}
		
		/* Remove a request from the queue. */
		list = RemoveHeadList(&priv->queue_irps);
		next_irp = CONTAINING_RECORD(list, IRP, Tail.Overlay.ListEntry);
		/* Set the cancel routine to NULL. */
		cancel_routine = IoSetCancelRoutine(next_irp, NULL);
		/* Check if its already cancelled. */
		if (next_irp->Cancel) {
			if (cancel_routine) {
				/* The cancel routine for this IRP hasn't
				 * been called yet so queue the IRP in the
				 * cancelled_irp list and complete after
				 * releasing the lock.
				 */
				InsertTailList(&cancelled_irps, list);
			} else {
				/* The cancel routine has run it must be
				 * waiting to hold the queue lock so
				 * initialize the IRPs list.
				 */
				InitializeListHead(list);
			}
			KeReleaseSpinLock(&priv->queue_lock, old_irql);
		} else {
			KeReleaseSpinLock(&priv->queue_lock, old_irql);
			if (FailRequests == wdm_irp_get_state(dev)) {
				wdm_dev_io_cmpl(dev, next_irp, STATUS_DELETE_PENDING, 0);
			} else {
				/* PIO_STACK_LOCATION stack; */
				os_dev_inc_kern(dev, "IO_ASYNC(unhold)");
				wdm_dev_io_skip(dev, next_irp);
				os_dev_dec_kern(dev, "IO_ASYNC(unhold)");
			}
		}
	}

	/* Walk through the cancelled_irp list and cancel them */
	while (!IsListEmpty(&cancelled_irps)) {
		PLIST_ENTRY cancelEntry = RemoveHeadList(&cancelled_irps);
		cancelled_irp = CONTAINING_RECORD(cancelEntry, IRP, Tail.Overlay.ListEntry);
		wdm_dev_io_cmpl(dev, cancelled_irp, STATUS_CANCELLED, 0);
	}
	
	os_dbg(OS_DBG_DEBUG, "IO_ASYNC(unhold) - ends\n");
}

void wdm_dev_power_caps(os_device dev)
{
	INT i;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PDEVICE_CAPABILITIES caps = &priv->caps;

	os_dbg(OS_DBG_DEBUG, "DeviceD1 = %s\n", wdm_dev_bool_string(caps->DeviceD1));
	os_dbg(OS_DBG_DEBUG, "DeviceD2 = %s\n", wdm_dev_bool_string(caps->DeviceD2));
	os_dbg(OS_DBG_DEBUG, "WakeFromD0 = %s\n", wdm_dev_bool_string(caps->WakeFromD0));
	os_dbg(OS_DBG_DEBUG, "WakeFromD1 = %s\n", wdm_dev_bool_string(caps->WakeFromD1));
	os_dbg(OS_DBG_DEBUG, "WakeFromD2 = %s\n", wdm_dev_bool_string(caps->WakeFromD2));
	os_dbg(OS_DBG_DEBUG, "SystemWake = %s\n", wdm_sys_power_string(caps->SystemWake));
	os_dbg(OS_DBG_DEBUG, "DeviceWake = %s\n", wdm_dev_power_string(caps->DeviceWake));
	for (i = PowerDeviceUnspecified; i < PowerDeviceMaximum; i++) {
		os_dbg(OS_DBG_DEBUG, "DeviceState[%s] = %s\n",
		       wdm_sys_power_string(i), wdm_dev_power_string(caps->DeviceState[i]));
	}
}

#ifdef CONFIG_PM_WAIT_WAKE
PO_COMPLETION_ROUTINE __wdm_dev_pocmpl_ww;
IO_COMPLETION_ROUTINE __wdm_dev_iocmpl_ww;

status_t __wdm_dev_iocmpl_ww(PDEVICE_OBJECT dev, PIRP irp, PVOID ctx)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	os_dbg(OS_DBG_DEBUG, "IRP_MN_WAIT_WAKE - ends\n");
	return STATUS_CONTINUE_COMPLETION;
}

VOID __wdm_dev_pocmpl_ww(PDEVICE_OBJECT dev, UCHAR minor,
			 POWER_STATE power,
			 PVOID ctx, PIO_STATUS_BLOCK iosb)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	
	xchg(&priv->wait_wake_outstanding, 0);
	os_dbg(OS_DBG_DEBUG, "WAIT_WAKE - ends\n");

	if (!NT_SUCCESS(iosb->Status)) return;
	status = wdm_dev_resume(dev, FALSE, NULL, NULL);
}

void wdm_enable_wait_wake(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (priv->drv_flags & OS_DEVICE_WAKEUP) {
		priv->wait_wake_enabled = 1;
		wdm_start_wait_wake(dev);
	}
}

void wdm_wait_wake_init(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	priv->wait_wake_enabled = 0;
	xchg(&priv->wait_wake_outstanding, 0);
	xchg(&priv->wait_wake_stopped, 0);
	xchgptr(&priv->wait_wake_irp, NULL);
}

void wdm_wait_wake_exit(os_device dev)
{
}

status_t wdm_start_wait_wake(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	POWER_STATE state;
	NTSTATUS status;
	
	if (!priv->wait_wake_enabled) return STATUS_SUCCESS;
	if (xchg(&priv->wait_wake_outstanding, 1)) {
		return STATUS_DEVICE_BUSY;
	}

	os_dbg(OS_DBG_DEBUG, "WAIT_WAKE - begins\n");
	xchg(&priv->wait_wake_stopped, 0);
	xchg(&priv->wait_wake_started, 0);

	/* Lowest state from which this irp will wake the system. */
	state.SystemState = priv->caps.SystemWake;
	status = wdm_dev_po_request(dev, FALSE,
				    IRP_MN_WAIT_WAKE, state,
				    __wdm_dev_pocmpl_ww, NULL);
	if (!NT_SUCCESS(status)) {
		xchg(&priv->wait_wake_outstanding, 0);
		os_dbg(OS_DBG_DEBUG, "WAIT_WAKE - ends\n");
		return status;
	}
	return status;
}

status_t wdm_stop_wait_wake(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIRP irp;
	
	if (!priv->wait_wake_enabled) return STATUS_SUCCESS;

	xchg(&priv->wait_wake_started, 1);
	irp = (PIRP)xchgptr(&priv->wait_wake_irp, NULL);
	if (irp) {
		os_dbg(OS_DBG_DEBUG, "WAIT_WAKE - stopping\n");
		IoCancelIrp(irp);
		if (xchg(&priv->wait_wake_stopped, 1)) {
			wdm_dev_io_cmpl(dev, irp, STATUS_CANCELLED, 0);
		}    
	}
	
	os_dbg(OS_DBG_DEBUG, "WAIT_WAKE - ends\n");
	return STATUS_SUCCESS;
}

BOOLEAN wdm_dev_allow_wait_wake(os_device dev, DEVICE_POWER_STATE power)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (!priv->wait_wake_enabled) return FALSE;
	return power <= priv->caps.DeviceWake;
}

BOOLEAN wdm_sys_allow_wait_wake(os_device dev, SYSTEM_POWER_STATE power)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (!priv->wait_wake_enabled) return FALSE;
	return power <= priv->caps.SystemWake;
}

NTSTATUS wdm_dev_pm_wait_wake(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status;
	SYSTEM_POWER_STATE power = stack->Parameters.WaitWake.PowerState;

	os_dbg(OS_DBG_DEBUG, "IRP_MN_WAIT_WAKE - begins\n");

	if (!wdm_sys_allow_wait_wake(dev, power)) {
		status = STATUS_INVALID_DEVICE_STATE;
		goto end;
	}
	status = wdm_dev_io_call(dev, irp, FALSE,
				 __wdm_dev_iocmpl_ww, NULL);
	if (!NT_SUCCESS(status)) goto end;
	return STATUS_PENDING;
end:
	os_dbg(OS_DBG_DEBUG, "IRP_MN_WAIT_WAKE - ends\n");
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}
#else
NTSTATUS wdm_dev_pm_wait_wake(PDEVICE_OBJECT dev, PIRP irp)
{
	return wdm_dev_io_cmpl(dev, irp, STATUS_NOT_SUPPORTED, 0);
}
#endif

#ifdef CONFIG_PM
IO_COMPLETION_ROUTINE __wdm_sys_iocmpl_owner;
PO_COMPLETION_ROUTINE __wdm_sys_pocmpl_owner;

/* Device power policy owners should call following routines for:
 * 1. IRP_MN_QUERY_POWER(SystemPowerState)
 * 2. IRP_MN_SET_POWER(SystemPowerState)
 * to dispatch power requests to its lower devices.
 * Routines related to device power policy owners:
 * 1. wdm_sys_dispatch_owner
 * 2. wdm_sys_complete_owner
 * 3. __wdm_sys_pocmpl_owner
 * 4. __wdm_sys_iocmpl_owner
 */
VOID __wdm_sys_pocmpl_owner(PDEVICE_OBJECT dev, UCHAR minor,
			    POWER_STATE power,
			    PVOID ctx, PIO_STATUS_BLOCK iosb)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIRP irp = ctx;
	os_dbg(OS_DBG_DEBUG, "%s(SystemPowerState) - ends\n",
	       wdm_dev_pm_string(minor));
	/* copy the D-irp(in iosb) status into S-irp(in ctx) */
	irp->IoStatus.Status = iosb->Status;
}

status_t wdm_sys_complete_owner(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	SYSTEM_POWER_STATE sys_state;
	POWER_STATE power_state;
	NTSTATUS status;

	sys_state = stack->Parameters.Power.State.SystemState;
	power_state.DeviceState = priv->caps.DeviceState[sys_state];

	status = wdm_dev_po_request(dev, FALSE,
				    stack->MinorFunction, power_state,
				    __wdm_sys_pocmpl_owner, irp);
	return status;
}

status_t __wdm_sys_iocmpl_owner(PDEVICE_OBJECT dev, PIRP irp, PVOID ctx)
{
	NTSTATUS status = irp->IoStatus.Status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

	if (!NT_SUCCESS(status)) goto end;
	
	status = wdm_sys_complete_owner(dev, irp);
	if (!NT_SUCCESS(status)) goto end;

	ASSERT(status == STATUS_PENDING);
	return STATUS_MORE_PROCESSING_REQUIRED;

end:
	os_dbg(OS_DBG_DEBUG, "%s(SystemPowerState) - ends\n",
	       wdm_dev_pm_string(stack->MinorFunction));
	return STATUS_CONTINUE_COMPLETION;
}

NTSTATUS wdm_sys_dispatch_owner(PDEVICE_OBJECT dev, PIRP irp,
				SYSTEM_POWER_STATE sys_state)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

	status = wdm_dev_io_call(dev, irp, FALSE,
				 __wdm_sys_iocmpl_owner, NULL);
	if (!NT_SUCCESS(status)) {
		goto end;
	}
	return STATUS_PENDING;
end:
	os_dbg(OS_DBG_DEBUG, "%s(SystemPowerState) - ends\n",
	       wdm_dev_pm_string(stack->MinorFunction));
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

IO_COMPLETION_ROUTINE __wdm_dev_iocmpl_resume;
IO_COMPLETION_ROUTINE __wdm_dev_iocmpl_suspend;

NTSTATUS __wdm_dev_iocmpl_resume(PDEVICE_OBJECT dev, PIRP irp, PVOID ctx)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = irp->IoStatus.Status;
	struct wdm_device *priv = wdm_dev_priv(dev);

	wdm_dev_apply_power(dev, stack->Parameters.Power.State.DeviceState);
	os_dbg(OS_DBG_DEBUG, "IRP_MN_SET_POWER(DevicePowerState) - ends\n");
	return status;
}

NTSTATUS __wdm_dev_iocmpl_suspend(PDEVICE_OBJECT dev, PIRP irp, PVOID ctx)
{
	os_dbg(OS_DBG_DEBUG, "IRP_MN_SET_POWER(DevicePowerState) - ends\n");
	return irp->IoStatus.Status;
}

NTSTATUS wdm_dev_dispatch_resume(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	/* Driver should apply power up IRP in IRP_MN_SET_POWER
	 * completion.
	 */
	status = wdm_dev_io_call(dev, irp, FALSE,
				 __wdm_dev_iocmpl_resume, NULL);
	if (!NT_SUCCESS(status)) goto end;
	return status;
end:
	os_dbg(OS_DBG_DEBUG, "IRP_MN_SET_POWER(DevicePowerState) - ends\n");
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

static void __wdm_dev_ioasync_suspend(PDEVICE_OBJECT dev, ULONG ctx)
{
	DEVICE_POWER_STATE state = (DEVICE_POWER_STATE)ctx;
	wdm_dev_apply_power(dev, state);
}

NTSTATUS wdm_dev_dispatch_suspend(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

	/* Driver should wait other IRPs completion before
	 * IRP_MN_SET_POWER completes.  But the wait operation should be
	 * called asynchronously.
	 */
	status = wdm_dev_io_async_submit(dev, irp,
					 __wdm_dev_ioasync_suspend,
					 stack->Parameters.Power.State.DeviceState,
					 __wdm_dev_iocmpl_suspend, NULL);
	if (!NT_SUCCESS(status)) goto end;
	return status;
end:
	os_dbg(OS_DBG_DEBUG, "IRP_MN_SET_POWER(DevicePowerState) - ends\n");
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

void wdm_dev_po_apply(os_device dev, DEVICE_POWER_STATE power)
{
	POWER_STATE state;
	state.DeviceState = power;
	PoSetPowerState(dev, DevicePowerState, state);
}
#else
#define wdm_dev_po_apply(dev, power)
#define wdm_sys_dispatch_owner(dev, irp, sys_state)	\
	wdm_dev_io_skip(dev, irp)
#define wdm_dev_dispatch_resume(dev, irp)		\
	wdm_dev_dispatch_owner(dev, irp);
#define wdm_dev_dispatch_suspend(dev, irp)		\
	wdm_dev_dispatch_owner(dev, irp);

NTSTATUS wdm_dev_dispatch_owner(PDEVICE_OBJECT dev, PIRP irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	wdm_dev_apply_power(dev, stack->Parameters.Power.State.DeviceState);
	return wdm_dev_io_skip(dev, irp);
}
#endif

NTSTATUS wdm_dev_pm_set_sys(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	SYSTEM_POWER_STATE sys_state = stack->Parameters.Power.State.SystemState;
	
	os_dbg(OS_DBG_DEBUG, "IRP_MN_SET_POWER(SystemPowerState) - begins\n");
	os_dbg(OS_DBG_DEBUG, "IRP[%s]<-DEV[%s]\n",
	       wdm_sys_power_string(sys_state),
	       wdm_sys_power_string(priv->SysPower));

	__wdm_sys_apply_power(dev, sys_state);
	return wdm_sys_dispatch_owner(dev, irp, sys_state);
}

NTSTATUS wdm_dev_pm_set_dev(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	DEVICE_POWER_STATE dev_state = stack->Parameters.Power.State.DeviceState;
	
	os_dbg(OS_DBG_DEBUG, "IRP_MN_SET_POWER(DevicePowerState) - begins\n");
	os_dbg(OS_DBG_DEBUG, "IRP[%s]<-DEV[%s]\n",
	       wdm_dev_power_string(dev_state),
	       wdm_dev_power_string(wdm_dev_get_power(dev)));

	if (dev_state == PowerDeviceD0) {
		return wdm_dev_dispatch_resume(dev, irp);
	} else {
		return wdm_dev_dispatch_suspend(dev, irp);
	}
}

NTSTATUS wdm_dev_pm_query_sys(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	SYSTEM_POWER_STATE sys_state = stack->Parameters.Power.State.SystemState;
	
	os_dbg(OS_DBG_DEBUG, "IRP_MN_QUERY_POWER(SystemPowerState) - begins\n");
	os_dbg(OS_DBG_DEBUG, "IRP[%s]<-DEV[%s]\n",
	       wdm_sys_power_string(sys_state),
	       wdm_sys_power_string(priv->SysPower));

	return wdm_sys_dispatch_owner(dev, irp, sys_state);
}

NTSTATUS wdm_dev_pm_query_dev(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	DEVICE_POWER_STATE dev_state = stack->Parameters.Power.State.DeviceState;
	
	os_dbg(OS_DBG_DEBUG, "IRP_MN_QUERY_POWER(DevicePowerState) - begins\n");
	os_dbg(OS_DBG_DEBUG, "IRP[%s]<-DEV[%s]\n",
	       wdm_dev_power_string(dev_state),
	       wdm_dev_power_string(priv->DevPower));

#if 0
	/* TODO: System Suspending Prevention
	 *
	 * Since it's not possible to prevent system from suspending, we
	 * comment following codes out.
	 */
	if (!wdm_dev_allow_wait_wake(dev, dev_state) &&
	    !os_dev_can_stop(dev)) {
		return wdm_dev_io_cmpl(dev, irp, STATUS_DEVICE_BUSY, 0);
	}
#endif

	os_dbg(OS_DBG_DEBUG, "IRP_MN_QUERY_POWER(DevicePowerState) - ends\n");
	return wdm_dev_io_skip(dev, irp);
}

NTSTATUS wdm_dev_dispatch_pm(PDEVICE_OBJECT dev, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	struct wdm_device *priv = wdm_dev_priv(dev);
	uint8_t minor = stack->MinorFunction;

	os_dbg(OS_DBG_INFO, "IRP_MJ_POWER - %s(%d)\n",
	       wdm_dev_pm_string(minor), minor);

	switch (stack->MinorFunction) {
	case IRP_MN_SET_POWER:
		switch (stack->Parameters.Power.Type) {
		case SystemPowerState:
			status = wdm_dev_pm_set_sys(dev, irp);
			break;
		case DevicePowerState:
			status = wdm_dev_pm_set_dev(dev, irp);
			break;
		}
		break;
	case IRP_MN_QUERY_POWER:
		switch (stack->Parameters.Power.Type) {
		case SystemPowerState:
			status = wdm_dev_pm_query_sys(dev, irp);
			break;
		case DevicePowerState:
			status = wdm_dev_pm_query_dev(dev, irp);
			break;
		}
		break;
	case IRP_MN_WAIT_WAKE:
		status = wdm_dev_pm_wait_wake(dev, irp);
		break;
	case IRP_MN_POWER_SEQUENCE:
	default:
		wdm_dev_io_skip(dev, irp);
		break;
	}
	return status;
}

void wdm_dev_apply_power(os_device dev, DEVICE_POWER_STATE power)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (power == PowerDeviceD0) {
		priv->drv->resume(dev);
	} else {
		priv->drv->suspend(dev);
	}
	__wdm_dev_apply_power(dev, power);
}

void __wdm_sys_apply_power(os_device dev, SYSTEM_POWER_STATE power)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	priv->SysPower = power;
	if (power > priv->caps.SystemWake) {
		wdm_stop_wait_wake(dev);
	} else {
		wdm_start_wait_wake(dev);
	}
}

void __wdm_dev_apply_power(os_device dev, DEVICE_POWER_STATE power)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	wdm_dev_po_apply(dev, power);
	priv->DevPower = power;
	if (os_dev_is_powered(dev)) {
		wdm_irp_set_state(dev, AllowRequests);
	} else {
		wdm_irp_set_state(dev, HoldRequests);
	}
	if (priv->caps.DeviceWake > power) {
		wdm_stop_wait_wake(dev);
	} else {
		wdm_start_wait_wake(dev);
	}
}

DEVICE_POWER_STATE wdm_dev_get_power(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	return priv->DevPower;
}

BOOLEAN os_dev_is_powered(os_device dev)
{
	return wdm_dev_get_power(dev) == PowerDeviceD0;
}

DEVICE_POWER_STATE wdm_sys_get_power(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	return priv->SysPower;
}

status_t wdm_dev_pm_start(os_device dev)
{
	wdm_wait_wake_init(dev);
	return STATUS_SUCCESS;
}

void wdm_dev_pm_stop(os_device dev)
{
	wdm_stop_wait_wake(dev);
}

status_t wdm_dev_pm_init(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	wdm_wait_wake_init(dev);
	/* Set the flags as underlying PDO. */
	if (priv->pdo->Flags & DO_POWER_PAGABLE) {
		dev->Flags |= DO_POWER_PAGABLE;
	}
	/* Typically, the function driver for a device is its power policy
	 * owner, although for some devices another driver or system
	 * component may assume this role.
	 */
	__wdm_sys_apply_power(dev, PowerSystemWorking);
	/* Set the initial power state of the device, if known. */
	__wdm_dev_apply_power(dev, PowerDeviceD0);
	return STATUS_SUCCESS;
}

void wdm_dev_pm_exit(os_device dev)
{
	__wdm_dev_apply_power(dev, PowerDeviceD3);
	__wdm_sys_apply_power(dev, PowerSystemUnspecified);
	wdm_wait_wake_exit(dev);
}

/*=========================================================================
 * informational
 *=======================================================================*/
status_t wdm_reg_get_property(os_device pdo, int property,
			      uint8_t *data, size_t size)
{
	WCHAR tmp[512];
	ULONG ret;
	ULONG i;
	status_t status = STATUS_SUCCESS;
	
	if (!pdo || !data || !size) {
		return STATUS_UNSUCCESSFUL;
	}
	
	memset(data, 0, size);
	memset(tmp, 0, sizeof(tmp));
	
	status = IoGetDeviceProperty(pdo, property, sizeof(tmp) - 2, tmp, &ret);
	if (NT_SUCCESS(status) && ret){
		/* convert unicode string to normal character string */
		for (i = 0; (i < ret/2) && (i < ((ULONG)size - 1)); i++){
			data[i] = (char)tmp[i];
		}
		_strlwr(data);
	} else {
		os_dbg(OS_DBG_ERR, "IoGetDeviceProperty(%d) - failure(%X)",
		       property, status);
	}
	return status;
}

status_t wdm_reg_get_word(PWCHAR path, PWCHAR name, PULONG value)
{
	ULONG default_data;
	WCHAR buffer[MAXIMUM_FILENAME_LENGTH];
	NTSTATUS status;
	UNICODE_STRING reg_path;
	RTL_QUERY_REGISTRY_TABLE param_table[2];
	
	reg_path.Length = 0;
	reg_path.MaximumLength = MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR);
	reg_path.Buffer = buffer;
	
	RtlZeroMemory(reg_path.Buffer, reg_path.MaximumLength);
	os_mem_move(reg_path.Buffer, path,
		    wcslen(path) * sizeof(WCHAR));
	
	RtlZeroMemory(param_table, sizeof(param_table));
	
	param_table[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
	param_table[0].Name = name;
	param_table[0].EntryContext = value;
	param_table[0].DefaultType = REG_DWORD;
	param_table[0].DefaultData = &default_data;
	param_table[0].DefaultLength = sizeof(ULONG);
	
	status = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE |
					RTL_REGISTRY_OPTIONAL,
					reg_path.Buffer,
					param_table, NULL, NULL);
	if (NT_SUCCESS(status)) {
		os_dbg(OS_DBG_INFO, "RtlQueryRegistryValues - success(value=%X)\n", *value);
		return STATUS_SUCCESS;
	} else {
		os_dbg(OS_DBG_ERR, "RtlQueryRegistryValues - failure(status=%X)\n", status);
		*value = 0;
		return STATUS_UNSUCCESSFUL;
	}
#if 0
	/* usage */
	wdm_reg_get_word(os_driver_reg_path, L"ConfigName", (PULONG)&priv->ConfigValue);
#endif
}

/*=========================================================================
 * device and IRP
 *=======================================================================*/
os_device wdm_dev_stacked(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	return priv->attached_dev;
}

void *dev_priv(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	return priv->priv;
}

const char *os_dev_id(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	return priv->id;
}

int wdm_irp_is_accepted(os_device dev, IRP *irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	/* Check if the IRP is sent to our device object or to the lower
	 * one.  This check is neccassary since the device object might be
	 * a filter.
	 */
	if (irp->Tail.Overlay.OriginalFileObject) {
		return irp->Tail.Overlay.OriginalFileObject->DeviceObject == priv->fdo ?
		       TRUE : FALSE;
	}
	return FALSE;
}

status_t os_dev_register_driver(struct os_driver *drv, size_t objsize,
				unsigned long flags)
{
	wdm_fdo_driver = drv;
	wdm_fdo_objsize = objsize;
	wdm_fdo_flags = flags;
	return STATUS_SUCCESS;
}

void os_dev_ref_exit(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	os_dev_dec_kern(dev, "THIS_DEVICE");
	priv->ref_init = 0;
	os_dev_wait_removed(dev);
}

void os_dev_ref_init(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	priv->nr_users = 0;
	priv->nr_kerns = 0;
	KeInitializeSpinLock(&priv->ref_lock);
	os_waiter_init(&priv->removed_waiter, FALSE);
	os_waiter_init(&priv->stopped_waiter, FALSE);

	/* kernel reference count biased to 1.
	 * Transition to 0 during remove device means IO is finished.
	 * Transition to 1 means the device can be stopped.
	 */
	priv->ref_init = 1;
	os_dev_inc_kern(dev, "THIS_DEVICE");
}

#ifdef CONFIG_DEBUG
refcnt_t os_dev_dec_user(os_device dev, const char *what)
{
	refcnt_t refcnt;
	struct wdm_device *priv = wdm_dev_priv(dev);
	refcnt = InterlockedDecrement(&priv->nr_users);
	os_dbg(OS_DBG_DEBUG, "InterlockedDecrement(USER) - %s[%d]\n", what, refcnt);
	return refcnt;
}

refcnt_t os_dev_inc_user(os_device dev, const char *what)
{
	refcnt_t refcnt;
	struct wdm_device *priv = wdm_dev_priv(dev);
	refcnt = InterlockedIncrement(&priv->nr_users);
	os_dbg(OS_DBG_DEBUG, "InterlockedIncrement(USER) - %s[%d]\n", what, refcnt);
	return refcnt;
}

/* Routine Description:
 *     This routine bumps up the I/O count.
 *     This routine is typically invoked when any of the
 *     dispatch routines handle new irps for the driver.
 * Arguments:
 *     dev - pointer to device
 * Return Value:
 *     new value
 */
refcnt_t os_dev_inc_kern(os_device dev, const char *what)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	refcnt_t refcnt = 0;
	KIRQL old_irql;
	
	KeAcquireSpinLock(&priv->ref_lock, &old_irql);
	refcnt = InterlockedIncrement((PLONG)&priv->nr_kerns);
	/* when refcnt bumps from 1 to 2, clear the stopped_waiter */
	if (refcnt == (1+priv->ref_init)) {
		os_waiter_idle(&priv->stopped_waiter);
		os_dbg(OS_DBG_DEBUG, "WAITER_IDLE - STOPPED\n");
	}
	KeReleaseSpinLock(&priv->ref_lock, old_irql);

	os_dbg(OS_DBG_DEBUG, "InterlockedIncrement(KERN) - %s[%d]\n", what, refcnt);
	return refcnt;
}

os_device os_dev_get_kern(os_device dev, const char *what)
{
	os_dev_inc_kern(dev, what);
	return dev;
}

/* Routine Description:
 *     This routine decrements the outstanding I/O count.  This is
 *     typically invoked after the dispatch routine has finished
 *     processing the irp.
 * Arguments:
 *     dev - pointer to devicen
 * Return Value:
 *     new value
 */
refcnt_t os_dev_dec_kern(os_device dev, const char *what)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	refcnt_t refcnt = 0;
	KIRQL old_irql;
	
	KeAcquireSpinLock(&priv->ref_lock, &old_irql);
	refcnt = InterlockedDecrement((PLONG)&priv->nr_kerns);
	if (refcnt == priv->ref_init) {
		os_waiter_wake(&priv->stopped_waiter);
		os_dbg(OS_DBG_DEBUG, "WAITER_WAKE - STOPPED\n");
	}
	if (refcnt == 0) {
		/* ASSERT(DEV_REMOVED == wdm_pnp_get_state(dev)); */
		os_waiter_wake(&priv->removed_waiter);
		os_dbg(OS_DBG_DEBUG, "WAITER_WAKE - REMOVED\n");
	}
	KeReleaseSpinLock(&priv->ref_lock, old_irql);

	os_dbg(OS_DBG_DEBUG, "InterlockedDecrement(KERN) - %s[%d]\n", what, refcnt);
	return refcnt;
}
#else
refcnt_t __os_dev_dec_user(os_device dev)
{
	refcnt_t refcnt;
	struct wdm_device *priv = wdm_dev_priv(dev);
	refcnt = InterlockedDecrement(&priv->nr_users);
	os_dbg(OS_DBG_DEBUG, "InterlockedDecrement(USER) - [%d]\n", refcnt);
	return refcnt;
}

refcnt_t __os_dev_inc_user(os_device dev)
{
	refcnt_t refcnt;
	struct wdm_device *priv = wdm_dev_priv(dev);
	refcnt = InterlockedIncrement(&priv->nr_users);
	os_dbg(OS_DBG_DEBUG, "InterlockedIncrement(USER) - [%d]\n", refcnt);
	return refcnt;
}

/* Routine Description:
 *     This routine bumps up the I/O count.
 *     This routine is typically invoked when any of the
 *     dispatch routines handle new irps for the driver.
 * Arguments:
 *     dev - pointer to device
 * Return Value:
 *     new value
 */
refcnt_t __os_dev_inc_kern(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	refcnt_t refcnt = 0;
	KIRQL old_irql;
	
	KeAcquireSpinLock(&priv->ref_lock, &old_irql);
	refcnt = InterlockedIncrement((PLONG)&priv->nr_kerns);
	/* when refcnt bumps from 1 to 2, clear the stopped_waiter */
	if (refcnt == (1+priv->ref_init)) {
		os_waiter_idle(&priv->stopped_waiter);
		os_dbg(OS_DBG_DEBUG, "WAITER_IDLE - STOPPED\n");
	}
	KeReleaseSpinLock(&priv->ref_lock, old_irql);

	os_dbg(OS_DBG_DEBUG, "InterlockedIncrement(KERN) - [%d]\n", refcnt);
	return refcnt;
}

os_device __os_dev_get_kern(os_device dev)
{
	__os_dev_inc_kern(dev);
	return dev;
}

refcnt_t __os_dev_dec_kern(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	refcnt_t refcnt = 0;
	KIRQL old_irql;
	
	KeAcquireSpinLock(&priv->ref_lock, &old_irql);
	refcnt = InterlockedDecrement((PLONG)&priv->nr_kerns);
	if (refcnt == (priv->ref_init)) {
		os_waiter_wake(&priv->stopped_waiter);
		os_dbg(OS_DBG_DEBUG, "WAITER_WAKE - STOPPED\n");
	}
	if (refcnt == 0) {
		ASSERT(DEV_REMOVED == wdm_pnp_get_state(dev));
		os_waiter_wake(&priv->removed_waiter);
		os_dbg(OS_DBG_DEBUG, "WAITER_WAKE - REMOVED\n");
	}
	KeReleaseSpinLock(&priv->ref_lock, old_irql);

	os_dbg(OS_DBG_DEBUG, "InterlockedDecrement(KERN) - [%d]\n", refcnt);
	return refcnt;
}
#endif

int os_dev_can_remove(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	return (priv->nr_users == 0) && (priv->nr_kerns == 1);
}

int os_dev_can_stop(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	return priv->nr_users == 0;
}

void os_dev_wait_stopped(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	os_waiter_wait(&priv->stopped_waiter, OS_WAIT_INFINITE);
}

void os_dev_wait_removed(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	os_waiter_wait(&priv->removed_waiter, OS_WAIT_INFINITE);
}

/*=========================================================================
 * driver entry
 *=======================================================================*/
wdm_version_t wdm_drv_get_version(void)
{
	return wdm_drv_global_data.wdm_version;
}

void wdm_drv_enum_version(void)
{
	if (RtlIsNtDdiVersionAvailable(NTDDI_VISTA)) {
		wdm_drv_global_data.wdm_version = WinVistaOrBetter;
	} else if (RtlIsNtDdiVersionAvailable(NTDDI_WINXP)) {
		wdm_drv_global_data.wdm_version = WinXpOrBetter;
	} else if (RtlIsNtDdiVersionAvailable(NTDDI_WIN2K)) {
		wdm_drv_global_data.wdm_version = Win2kOrBetter;
	}
}

status_t wdm_dev_start(os_device dev)
{
	status_t status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	status = wdm_dev_pm_start(dev);
	if (!NT_SUCCESS(status)) goto end;
	status = wdm_dev_io_start(dev);
	if (!NT_SUCCESS(status)) goto end;
	status = priv->drv->start(dev);
	wdm_irp_set_state(dev, AllowRequests);
end:
	return status;
}

void wdm_dev_stop(os_device dev, int removing)
{
	status_t status;
	struct wdm_device *priv = wdm_dev_priv(dev);
	if (removing)
		wdm_irp_set_state(dev, FailRequests);
	else
		wdm_irp_set_state(dev, HoldRequests);
	wdm_dev_pm_stop(dev);
	priv->drv->stop(dev);
	wdm_dev_io_stop(dev);
}

void wdm_dev_remove(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	os_dev_wait_stopped(dev);

	wdm_bus_remove(dev);
	/* Free any hardware resources for the device in use by the
	 * driver.  The exact operations depend on the device and the
	 * driver but can include disconnecting an interrupt with
	 * IoDisconnectInterrupt, freeing physical address ranges with
	 * MmUnmapIoSpace, and freeing I/O ports.
	 */
	priv->drv->exit(dev);
	wdm_dev_io_exit(dev);
	wdm_dev_pm_exit(dev);
	wdm_dev_pnp_exit(dev);
	os_dev_ref_exit(dev);
}

status_t wdm_dev_add(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	status_t status = STATUS_SUCCESS;
	
	os_dev_ref_init(dev);

	status = wdm_dev_pnp_init(dev);
	if (!NT_SUCCESS(status)) goto end;
	status = wdm_dev_pm_init(dev);
	if (!NT_SUCCESS(status)) goto end;
	status = wdm_dev_io_init(dev);
	if (!NT_SUCCESS(status)) goto end;
	status = priv->drv->init(dev);
	wdm_bus_add(dev);
end:
	if (!NT_SUCCESS(status)) {
		priv->drv->exit(dev);
		wdm_dev_io_exit(dev);
		wdm_dev_pm_exit(dev);
		wdm_dev_pnp_exit(dev);
	}
	return status;
}

#ifdef CONFIG_BUS
PDEVICE_RELATIONS wdm_bus_query_childs(PDEVICE_OBJECT dev,
				       PDEVICE_RELATIONS rels)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PDEVICE_RELATIONS orels = NULL;
	struct wdm_device *pdo;
	ULONG nr_pres = 0, nr_prev = 0, nr_rels;

	os_mutex_lock(&priv->u.bus.mutex);
	if (!priv->u.bus.nr_devs) goto end;
	orels = rels, rels = NULL;
	if (orels)
		nr_prev = orels->Count;
	foreach_wdm_bus_dev(priv, pdo) {
		if (pdo->u.dev.present)
			nr_pres++;
	}

	os_dbg(OS_DBG_INFO, "Childs releations - prev=%d, pres=%d\n",
	       nr_prev, nr_pres);

	nr_rels = nr_pres+nr_prev;
	rels = (PDEVICE_RELATIONS)os_mem_alloc(sizeof(DEVICE_RELATIONS)+
					       (nr_rels*sizeof (PDEVICE_OBJECT))-1,
					       "DEVICE_RELATIONS");
	if (NULL == rels) goto end;
	rels->Count = nr_rels;
	if (nr_prev) {
		os_mem_copy(rels->Objects, orels->Objects,
			    nr_prev * sizeof (PDEVICE_OBJECT));
	}
	foreach_wdm_bus_dev(priv, pdo) {
		if (pdo->u.dev.present) {
			rels->Objects[nr_prev] = pdo->pdo;
			ObReferenceObject(pdo->pdo);
			nr_prev++;
		} else {
			pdo->u.dev.reported_missing = TRUE;
		}
	}
end:
	if (orels)
		os_mem_free(orels);
	os_mutex_unlock(&priv->u.bus.mutex);
	return rels;
}

PDEVICE_RELATIONS wdm_bus_query_parent(PDEVICE_OBJECT dev,
				       PDEVICE_RELATIONS rels)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	BUG_ON(rels);
	rels = (PDEVICE_RELATIONS)os_mem_alloc(sizeof(DEVICE_RELATIONS),
					       "DEVICE_RELATIONS");
	if (!rels) return NULL;
	rels->Count = 1;
	rels->Objects[0] = priv->pdo;
	ObReferenceObject(priv->pdo);
	return rels;
}

void wdm_bus_add(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (priv->is_fdo) {
		priv->u.bus.nr_devs = 0;
		INIT_LIST_HEAD(&priv->u.bus.devs);
		os_mutex_init(&priv->u.bus.mutex);
	} else {
		INIT_LIST_HEAD(&priv->u.dev.link);
		priv->u.dev.bus = NULL;
		/* attached to the bus */
		priv->u.dev.present = TRUE;
		/* not yet reported missing */
		priv->u.dev.reported_missing = FALSE;
	}
}

void wdm_bus_remove(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (priv->is_fdo) {
		struct wdm_device *pdo, *n;
		os_mutex_lock(&priv->u.bus.mutex);
		foreach_wdm_bus_dev_safe(priv, pdo, n) {
			//wdm_bus_remove_dev(pdo->dev);
			list_del_init(&pdo->u.dev.link);
			pdo->u.dev.bus = NULL;
			pdo->u.dev.reported_missing = TRUE;
			priv->u.bus.nr_devs--;
		}
		os_mutex_unlock(&priv->u.bus.mutex);
	}
}

status_t wdm_bus_start(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (priv->is_fdo) {
		struct wdm_device *pdo, *n;
		os_mutex_lock(&priv->u.bus.mutex);
		foreach_wdm_bus_dev(priv, pdo) {
			//wdm_bus_start_dev(pdo->pdo);
		}
		os_mutex_unlock(&priv->u.bus.mutex);
	}
	return STATUS_SUCCESS;
}

void wdm_bus_stop(os_device dev)
{
	struct wdm_device *priv = wdm_dev_priv(dev);

	if (priv->is_fdo) {
		struct wdm_device *pdo, *n;
		os_mutex_lock(&priv->u.bus.mutex);
		foreach_wdm_bus_dev(priv, pdo) {
			//wdm_bus_stop_dev(pdo->pdo);
		}
		os_mutex_unlock(&priv->u.bus.mutex);
	}
}
#endif

NTSTATUS __wdm_dev_add_device(PDRIVER_OBJECT driver,
			      PDEVICE_OBJECT pdo,
			      size_t objsize,
			      struct os_driver *drv,
			      LPCGUID io_clsguid,
			      int minor,
			      unsigned long flags,
			      LPCGUID dev_clsguid,
			      void (*later_init)(os_device dev, void *init_data),
			      void *later_data)
{
	NTSTATUS status;
	PDEVICE_OBJECT dev = NULL;
	struct wdm_device *priv;
	char id[256];
	int i;
	WCHAR tmp_name[128];
	UNICODE_STRING dev_name;
	
	os_dbg(OS_DBG_DEBUG, "DRIVER_ADD_DEVICE - begins\n");

	if (pdo != NULL) {
		status = wdm_reg_get_property(pdo, DevicePropertyHardwareID,
					      id, sizeof(id));
		if (!NT_SUCCESS(status)) return STATUS_NO_SUCH_DEVICE;
		os_dbg(OS_DBG_INFO, "Device Name: %s\n", id);

		for (i = 1; i < 256; i++) {
			RtlStringCchPrintfW(tmp_name,
					    sizeof(tmp_name)/sizeof(WCHAR),
					    L"%s%04d", OS_DRIVER_FDO_NAME, i);
			RtlInitUnicodeString(&dev_name, tmp_name);
			status = IoCreateDevice(driver,
						sizeof(struct wdm_device) + objsize,
						&dev_name,
						OS_DRIVER_FDO_TYPE,
						OS_DRIVER_FDO_CHAR,
						FALSE, &dev);
			if (NT_SUCCESS(status)) {
				os_dbg(OS_DBG_INFO, "IoCreateDevice - success\n", i);
				os_dbg(OS_DBG_DEBUG, "Device Minor - %i\n", i);
				os_dbg(OS_DBG_DEBUG, "Device Name - %ws\n", dev_name.Buffer);
				break;
			}
			dev = NULL;
		}
	} else {
		/* PDO must have a name. You should let the system auto
		 * generate a name by specifying
		 * FILE_AUTOGENERATED_DEVICE_NAME in the
		 * DeviceCharacteristics parameter.
		 */
		/* Create a secure device, in case the child gets
		 * installed as a raw device (RawDeviceOK), to prevent an
		 * unpriviledged user accessing our device.
		 */
		/* Just make sure that the GUID specified here is not a
		 * setup class GUID.  If you specify a setup class guid,
		 * you must make sure that class is installed before
		 * enumerating the PDO.
		 */
		status = IoCreateDeviceSecure(driver,
					      sizeof (struct wdm_device) + objsize,
					      NULL,
					      FILE_DEVICE_BUS_EXTENDER,
					      OS_DRIVER_FDO_CHAR,
					      FALSE,
					      &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RWX_RES_RWX,
					      (LPCGUID)dev_clsguid,
					      &pdo);
	}
	if (!dev) {
		os_dbg(OS_DBG_ERR, "IoCreateDevice - failure(Too much devices)\n");
		return STATUS_NO_SUCH_DEVICE;
	}

	priv = wdm_dev_priv(dev);
	priv->drv = drv;
	priv->is_fdo = (pdo != NULL);
	priv->io_clsguid = io_clsguid;
	priv->fdo = dev;
	priv->pdo = pdo;
	priv->drv_flags = flags;
	RtlStringCbCopyNA(priv->id, 256, id, 256);

	if (priv->is_fdo) {
		priv->minor = i;
		priv->attached_dev = IoAttachDeviceToDeviceStack(dev, pdo);
		if (NULL == priv->attached_dev) {
			os_dbg(OS_DBG_ERR, "IoAttachDeviceToDeviceStack - failure\n");
			IoDeleteDevice(dev);
			status = STATUS_NO_SUCH_DEVICE;
			goto end;
		}
	} else {
		priv->minor = minor;
	}

	status = wdm_dev_add(dev);
	if (!NT_SUCCESS(status)) {
		if (priv->is_fdo) {
			IoDetachDevice(priv->attached_dev);
		}
		IoDeleteDevice(dev);
		goto end;
	}

	if (later_init)
		later_init(dev, later_data);

	dev->Flags &= ~DO_DEVICE_INITIALIZING;
end:
	os_dbg(OS_DBG_DEBUG, "DRIVER_ADD_DEVICE - ends\n");
	return status;
}

NTSTATUS wdm_dev_add_device(PDRIVER_OBJECT driver,
			    PDEVICE_OBJECT pdo)
{
	return __wdm_dev_add_device(driver, pdo,
				    wdm_fdo_objsize,
				    wdm_fdo_driver,
				    os_driver_fdo_guid,
				    -1,
				    wdm_fdo_flags,
				    NULL,
				    NULL, NULL);
}

DRIVER_UNLOAD DriverUnload;
DRIVER_INITIALIZE DriverEntry;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, DriverEntry)
#pragma alloc_text(PAGE, DriverUnload)
#endif

static NTSTATUS wdm_dev_dispatch_fail(PDEVICE_OBJECT dev, PIRP irp,
				      NTSTATUS status)
{
	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

NTSTATUS wdm_dev_dispatch_none(PDEVICE_OBJECT dev, PIRP irp)
{
	return wdm_dev_io_cmpl(dev, irp, STATUS_NOT_SUPPORTED, 0);
}

os_device wdm_dev_get_irp(os_device dev, unsigned long major)
{
	if (major != IRP_MJ_SYSTEM_CONTROL) {
		os_dbg(OS_DBG_DEBUG, "%s - begins\n", wdm_dev_irp_string(major));
		return os_dev_get_kern(dev, wdm_dev_irp_string(major));
	}
	return NULL;
}

void wdm_dev_put_irp(os_device dev, unsigned long major)
{
	if (major != IRP_MJ_SYSTEM_CONTROL) {
		os_dev_put_kern(dev, wdm_dev_irp_string(major));
		os_dbg(OS_DBG_DEBUG, "%s - ends\n", wdm_dev_irp_string(major));
	}
}

NTSTATUS wdm_dev_dispatch_irp(PDEVICE_OBJECT dev, IRP *irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status;
	ULONG major, minor;
	
	major = stack->MajorFunction;
	minor = stack->MinorFunction;

	if (major != IRP_MJ_SYSTEM_CONTROL) {
		os_dbg(OS_DBG_DEBUG, "DISPATCH_IRP - %d, %d, %s\n",
		       major, minor,
		       wdm_dev_state_string(wdm_pnp_get_state(dev)));
	}
	
	switch (wdm_pnp_get_state(dev)) {
	case DEV_REMOVED:
		return wdm_dev_io_cmpl(dev, irp, STATUS_DELETE_PENDING, 0);
	case DEV_RUNNING:
		break;
	default:
		/* PNP IRPs will affect the pnp state, allow CLOSE IRP to
		 * be handled to free those memory allocated by the IO
		 * codes.
		 * TODO: CLEANUP?
		 */
		if ((major != IRP_MJ_PNP) && (major != IRP_MJ_CLOSE))
			return wdm_dev_dispatch_fail(dev, irp,
						     STATUS_INVALID_DEVICE_STATE);
	}

#if 0
	/* Handle filter driver IRPs. */
	if (!wdm_irp_is_accepted(dev, irp))
		return wdm_dev_io_skip(dev, irp);
#endif

	wdm_dev_get_irp(dev, major);

	switch (major)  {
	case IRP_MJ_PNP:
		status = wdm_dev_dispatch_pnp(dev, irp);
		break;
	case IRP_MJ_POWER:
		status = wdm_dev_dispatch_pm(dev, irp);
		break;
	case IRP_MJ_CLEANUP:
	case IRP_MJ_CREATE:
	case IRP_MJ_CLOSE:
	case IRP_MJ_READ:
	case IRP_MJ_WRITE:
	case IRP_MJ_DEVICE_CONTROL:
		status = wdm_dev_dispatch_io(dev, irp);
		break;
	case IRP_MJ_SYSTEM_CONTROL:
		status = wdm_dev_dispatch_wmi(dev, irp);
		break;
	case IRP_MJ_INTERNAL_DEVICE_CONTROL:
		status = wdm_dev_dispatch_ioctl(dev, irp);
		break;
	default:
		status = wdm_dev_dispatch_fail(dev, irp, STATUS_NOT_SUPPORTED);
		break;
	}

	wdm_dev_put_irp(dev, major);
	return status;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING registry)
{
	NTSTATUS status;
	PUNICODE_STRING path;
	INT i;

	os_dbg(OS_DBG_DEBUG, "DRIVER_INITIALIZE - begins\n");
	os_dbg(OS_DBG_DEBUG, "DRIVER_INITIALIZE - version "OS_DRIVER_VERSION_STR"\n");

	os_mem_leak_init();

	/* Parameter 'registry' contains a unicode string representing the
	 * path to driver specific key in the registry.
	 */
	path = &wdm_drv_global_data.reg_path;
	path->MaximumLength = registry->Length + sizeof(UNICODE_NULL);
	path->Length        = registry->Length;
	path->Buffer        = os_mem_alloc(path->MaximumLength, "UNICODE_STRING");
	if (!path->Buffer) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto end;
	}
	os_mem_move(path->Buffer, registry->Buffer, registry->Length);
	os_dbg(OS_DBG_DEBUG, "Registry - %ws\n", registry->Buffer);
	wdm_drv_enum_version();

	status = STATUS_SUCCESS;
	/* Initialize the driver object with this driver's entry points. */
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)  {
		driver->MajorFunction[i] = wdm_dev_dispatch_irp;
	}

	driver->DriverUnload = DriverUnload;
	driver->DriverExtension->AddDevice = (PDRIVER_ADD_DEVICE)wdm_dev_add_device;

	status = os_driver_init();
end:
	os_dbg(OS_DBG_DEBUG, "DRIVER_INITIALIZE - ends\n");
	return status;
}

VOID DriverUnload(PDRIVER_OBJECT driver)
{
	PUNICODE_STRING path;

	os_dbg(OS_DBG_DEBUG, "DRIVER_UNLOAD - begins\n");

	path = &wdm_drv_global_data.reg_path;
	if (path->Buffer) {
		os_mem_free(path->Buffer);
		path->Buffer = NULL;
	}
	os_mem_leak_dump();
	os_dbg(OS_DBG_DEBUG, "DRIVER_UNLOAD - ends\n");
}
