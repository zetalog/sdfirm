#include <host/wdmsim.h>
#include <host/usbdfu.h>

#define DFU_TEST_XFR_LENGTH	200
#define DFU_TEST_FIRM_TOTAL	0x400
#define DFU_USER_BUFFER_SIZE	200

struct dfu_function_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bmAttributes;
#define DFU_ATTR_WILL_DETACH		0x08
#define DFU_ATTR_MANIFESTATION_TOLERANT	0x04
#define DFU_ATTR_CAN_UPLOAD		0x02
#define DFU_ATTR_CAN_DNLOAD		0x01
	uint16_t wDetachTimeOut;
	uint16_t wTransferSize;
	uint16_t bcdDFUVersion;
};
#define USB_DT_DFU_FUNCTION	0x21

struct dfu_status {
	/* An indication of the status resulting from the execution of
	 * the most recent request.
	 */
	uint8_t bStatus;
	/* Minimum time, in milliseconds, that the host should wait before
	 * sending a subsequent DFU_GETSTATUS request.
	 */
	uint8_t bwPollTime[3];
	/* An indication of the state. */
	uint8_t bState;
	/* Index of status description in string table. */
	uint8_t iString;
};

struct dfu_device {
	void *dev;
	int seq;
#define DFU_SEQ_IDLE		0x00
#define DFU_SEQ_DNLOAD		0x01 /* Write is allowed */
#define DFU_SEQ_UPLOAD		0x02 /* Read is allowed */
#define DFU_SEQ_DETACH		0x03
#define DFU_SEQ_ABORT		0x04

	/* attributes from GETSTATUS */
	uint8_t state;
	uint8_t status;
	uint8_t iString;	/* Error string ID */

	/* attributes from enumerations */
	uint8_t proto;
	char dev_sn[MAX_DFU_SN];
	char dev_name[MAX_DFU_NAME];
	struct dfu_function_desc *dfu_function_desc;

	/* asynchronous parameters */
	union {
		struct dfu_status status;
		uint8_t state;
	} params;

	usb_status_t usb_status;
	uint8_t dfu_status;

	/* The real block size should be wTransferSize, while the pseudo
	 * block size is a roundup of real one for optimization purpose.
	 */
	size_t xfr_size;
	size_t pad_size;
#define DFU_BUFFERED_XFRS	4
	/* buffer storing user's IO data */
	uint8_t *buffer;
	size_t length;
	/* XXX: Synchronizations of Variables
	 * These requirements are not apply to startup (dfu_buffer_init)
	 * and cleanup (dfu_buffer_exit) codes as they are protected by
	 * the DFU operation sequencing.
	 * If some variables are only used inside the serialized work
	 * queue, these requirements are not apply yet.
	 */
	/* When this is signalled for user space, it is required to check
	 * whether there's really a poll condition asserted.
	 * This is because the signal is caused by the condition evaluated
	 * of the circular buffer POS/EOF's snapshot, and the condition
	 * may not match the current POS/EOF's snapshot when the user
	 * space is signalled.
	 */
	os_waiter waiter;
	/* POS is the offset of the firmware, it is modified only when the
	 * DNLOAD/UPLOAD URB has completed.
	 */
	unsigned long pos;
	/* So we only get the following three variables synced across
	 * threads, since rpos/wpos of circular buffer is handled in
	 * seperated threads, its safe to call dfu_buffer_space or
	 * dfu_buffer_count without any lock for threads.  The problem
	 * occurs when the eof_user/eof_firm shoudl be set/got.  Please
	 * refer to the "Circular Buffer POS/EOF" comments.
	 */
	ASSIGN_CIRCBF16_MEMBER(circbf);
	int eof_user;
	int eof_firm;

	int sync_getstatus;		/* require DFU_GETSTATUS */
	int sync_clrstatus;		/* require DFU_CLRSTATUS */
	int seq_ended;
	int synced;

	os_timer *poll_timer;
	uint32_t poll_time;

	/* portable variables */
	struct dfu_status dev_status;
	LONG work_count;
	struct os_work_queue *work_queue;
	int busy;
};

struct dfu_urb {
	int request;
};

void dfu_timer_submit(struct dfu_device *dfu);
void dfu_timer_discard(struct dfu_device *dfu);
void dfu_timer_stop(struct dfu_device *dfu);
status_t dfu_timer_start(struct dfu_device *dfu);

void dfu_begin_seq(struct dfu_device *dfu, uint8_t proto, int seq);
void dfu_seq_ended(struct dfu_device *dfu);
void dfu_end_seq(struct dfu_device *dfu);

size_t dfu_buffer_count(struct dfu_device *dfu);
size_t dfu_buffer_space(struct dfu_device *dfu);
void dfu_buffer_read(struct dfu_device *dfu, size_t bytes);
void dfu_buffer_write(struct dfu_device *dfu, size_t bytes);
void dfu_buffer_calc(struct dfu_device *dfu,
		     uint16_t xfr_size);
void dfu_buffer_run(struct dfu_device *dfu, int user);
void dfu_buffer_init(struct dfu_device *dfu);
void dfu_buffer_exit(struct dfu_device *dfu);
status_t dfu_buffer_poll(struct dfu_device *dfu, unsigned long *poll);

status_t dfu_poll_interface(void *dev, uint8_t request);

static void usb_worker_callback(os_device dev, struct os_work_item *work);
static void dfu_timer_callback(os_timer *timer, os_device dev, void *data);
static void dfu_control_aval(os_usbif *usb, struct os_usb_urb *urb);
static void dfu_control_cmpl(os_usbif *usb, struct os_usb_urb *urb);

void dfu_timer_submit(struct dfu_device *dfu)
{
	if (dfu->poll_timer)
		os_timer_schedule(dfu->poll_timer, dfu->poll_time*1000);
}

void dfu_timer_discard(struct dfu_device *dfu)
{
	if (dfu->poll_timer)
		os_timer_discard(dfu->poll_timer);
}

void dfu_timer_stop(struct dfu_device *dfu)
{
	if (dfu->poll_timer) {
		os_timer_unregister(dfu->poll_timer);
		dfu->poll_timer = NULL;
	}
}

status_t dfu_timer_start(struct dfu_device *dfu)
{
	dfu->poll_timer = os_timer_register(dfu->dev,
					    dfu_timer_callback,
					    NULL);
	if (!dfu->poll_timer) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	return STATUS_SUCCESS;
}

void dfu_get_status_sync(struct dfu_device *dfu)
{
	BUG_ON(dfu->sync_clrstatus);
	dfu->sync_getstatus = 1;
	dfu->synced = 0;
	dfu_poll_interface(dfu->dev, DFU_GETSTATUS);
}

void dfu_get_status_unsync(struct dfu_device *dfu)
{
	BUG_ON(dfu->sync_clrstatus);
	dfu->sync_getstatus = 0;
	dfu->synced = 1;
}

void dfu_clear_status_sync(struct dfu_device *dfu)
{
	dfu_get_status_unsync(dfu);
	dfu->sync_clrstatus = 1;
	dfu_poll_interface(dfu->dev, DFU_CLRSTATUS);
}

void dfu_clear_status_unsync(struct dfu_device *dfu)
{
	dfu->sync_clrstatus = 0;
}

void dfu_init_seq(struct dfu_device *dfu)
{
	dfu->seq = DFU_SEQ_IDLE;
}

void dfu_begin_seq(struct dfu_device *dfu, uint8_t proto, int seq)
{
	dfu->seq = seq;
	dfu->work_count = 0;
	dfu->busy = 0;
	dfu->seq_ended = 0;
}

void dfu_seq_ended(struct dfu_device *dfu)
{
	dfu->seq_ended = 1;
}

void dfu_end_seq(struct dfu_device *dfu)
{
	while (dfu->work_count > 0 || !dfu->seq_ended)
		Sleep(100);
	BUG_ON(dfu->pos != DFU_TEST_FIRM_TOTAL);
	dfu_buffer_exit(dfu);
	dfu->seq = DFU_SEQ_IDLE;
}

static status_t dfu_dnload_aval(os_device dev, struct os_usb_urb *urb,
				uint16_t blknr, uint8_t *buffer)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status = STATUS_SUCCESS;
	usb_status_t res = USBD_STATUS_SUCCESS;
	uint16_t len = 0;
	size_t count;
	int eof_firm, eof_user;
	
	/* XXX: Circular Buffer POS/EOF Getting Rule
	 * Getting user vars before getting firm vars in firm context.
	 * For user vars, getting eof should be done before count as
	 * setting eof is done after setting count.
	 */
	eof_user = dfu->eof_user;
	count = dfu_buffer_count(dfu);
	eof_firm = dfu->eof_firm;

	/* sanity checks */
	if (dfu->sync_clrstatus) {
		os_dbg(OS_DBG_DEBUG, "PIPE HALTED = yes\r\n");
		status = STATUS_DEVICE_BUSY;
		res = USBD_STATUS_ERROR_BUSY;
		goto end;
	}
	if (dfu->sync_getstatus) {
		os_dbg(OS_DBG_DEBUG, "DNLOAD SYNC\r\n");
		status = STATUS_DEVICE_BUSY;
		res = USBD_STATUS_ERROR_BUSY;
		goto end;
	}
	if (eof_firm ||
	    (count < dfu->xfr_size && !eof_user)) {
		os_dbg(OS_DBG_DEBUG, "DNLOAD BUSY\r\n");
		status = STATUS_DEVICE_BUSY;
		res = USBD_STATUS_ERROR_BUSY;
		goto end;
	}

	len = min(count, dfu->xfr_size);
	os_dbg(OS_DBG_DEBUG, "DNLOAD - %08x, %08x\r\n",
	       len, count);
end:
	os_usb_set_urb_length(urb, len);
	os_usb_set_urb_status(urb, res);
	return status;
}

static status_t dfu_upload_aval(os_device dev, struct os_usb_urb *urb,
				uint16_t blknr, uint8_t *buffer)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status = STATUS_SUCCESS;
	usb_status_t res = USBD_STATUS_SUCCESS;
	uint16_t len = 0;
	size_t space;
	int eof_firm;
	
	/* XXX: Circular Buffer POS/EOF Getting Rule
	 * Getting user vars before getting firm vars in firm context.
	 * For user vars, getting eof should be done before space as
	 * setting eof is done after setting space.
	 */
	space = dfu_buffer_space(dfu);
	eof_firm = dfu->eof_firm;

	/* sanity checks */
	if (dfu->sync_clrstatus) {
		os_dbg(OS_DBG_DEBUG, "PIPE HALTED = yes\r\n");
		status = STATUS_DEVICE_BUSY;
		res = USBD_STATUS_ERROR_BUSY;
		goto end;
	}
	if (eof_firm || space < dfu->xfr_size) {
		os_dbg(OS_DBG_DEBUG, "UPLOAD BUSY\r\n");
		status = STATUS_DEVICE_BUSY;
		res = USBD_STATUS_ERROR_BUSY;
		goto end;
	}

	if (dfu->pos <= DFU_TEST_FIRM_TOTAL)
		len = min(space, dfu->xfr_size);
	if (len + dfu->pos > DFU_TEST_FIRM_TOTAL)
		len = (uint16_t)(DFU_TEST_FIRM_TOTAL - dfu->pos);
	os_dbg(OS_DBG_DEBUG, "UPLOAD - %08x, %08x\r\n",
	       len, space);
end:
	os_usb_set_urb_length(urb, len);
	os_usb_set_urb_status(urb, res);
	return status;
}

static status_t dfu_get_status_aval(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status = STATUS_SUCCESS;
	usb_status_t res = USBD_STATUS_SUCCESS;
	uint16_t len = 0;

	/* sanity checks */
	if (dfu->seq_ended) {
		os_dbg(OS_DBG_DEBUG, "SEQ ENDED\r\n");
		status = STATUS_DEVICE_BUSY;
		res = USBD_STATUS_ERROR_BUSY;
		goto end;
	}
	if (dfu->sync_clrstatus) {
		os_dbg(OS_DBG_DEBUG, "PIPE HALTED = yes\r\n");
		status = STATUS_DEVICE_BUSY;
		res = USBD_STATUS_ERROR_BUSY;
		goto end;
	}
	if (dfu->synced) {
		if (!dfu->eof_firm) {
			if (dfu->seq == DFU_SEQ_DNLOAD)
				dfu->dev_status.bState = DFU_STATE_dfuDNLOAD_IDLE;
			if (dfu->seq == DFU_SEQ_UPLOAD)
				dfu->dev_status.bState = DFU_STATE_dfuUPLOAD_IDLE;
		} else {
			dfu->dev_status.bState = DFU_STATE_dfuIDLE;
		}
	} else if (dfu->sync_getstatus) {
		if (dfu->seq == DFU_SEQ_DNLOAD)
			dfu->dev_status.bState = DFU_STATE_dfuDNLOAD_SYNC;
	}

	len = sizeof (struct dfu_status);
	os_mem_copy(&dfu->params.status, &dfu->dev_status, len);

end:
	os_usb_set_urb_length(urb, len);
	os_usb_set_urb_status(urb, res);
	return status;
}

static status_t dfu_clear_status_aval(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t res = USBD_STATUS_SUCCESS;
	uint16_t len = 0;
	status_t status = STATUS_SUCCESS;

	/* sanity checks */
	if (!dfu->sync_clrstatus) {
		os_dbg(OS_DBG_DEBUG, "PIPE HALTED = no\r\n");
		res = USBD_STATUS_ERROR_BUSY;
		status = STATUS_DEVICE_BUSY;
		goto end;
	}

	/* Record last error status. */
	dfu->dfu_status = dfu->status;

end:
	os_usb_set_urb_length(urb, len);
	os_usb_set_urb_status(urb, res);
	return status;
}

static void dfu_control_aval(os_usbif *usb, struct os_usb_urb *urb)
{
	struct dfu_device *dfu = usb_dev_priv(usb);
	struct dfu_urb *dfu_urb = urb_priv(urb);

	switch (dfu_urb->request) {
	default:
	case DFU_GETSTATUS:
		dfu_get_status_aval(dfu->dev, urb);
		break;
	case DFU_CLRSTATUS:
		dfu_clear_status_aval(dfu->dev, urb);
		break;
	case DFU_DNLOAD:
		dfu_dnload_aval(dfu->dev, urb,
				(uint16_t)(dfu->pos/dfu->xfr_size),
				circbf_rpos(&dfu->circbf));
		break;
	case DFU_UPLOAD:
		dfu_upload_aval(dfu->dev, urb,
				(uint16_t)(dfu->pos/dfu->xfr_size),
				circbf_wpos(&dfu->circbf));
		break;
#if 0
	case DFU_DETACH:
		dfu_detach_aval(dfu->dev);
		break;
	case DFU_ABORT:
		dfu_abort_aval(dfu->dev);
		break;
	case DFU_RESET:
		dfu_reset_aval(dfu->dev);
		break;
#endif
	}
}

static void dfu_get_status_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;

	if (!USBD_IS_SUCCESS(urbstatus)) {
		dfu_poll_interface(dfu->dev, DFU_GETSTATUS);
		return;
	}

	dfu_get_status_unsync(dfu);
	dfu->state = dfu->params.status.bState;
	dfu->status = dfu->params.status.bStatus;
	dfu->iString = dfu->params.status.iString;
	dfu->poll_time = MAKELONG((uint16_t)dfu->params.status.bwPollTime[0],
				  MAKEWORD(dfu->params.status.bwPollTime[1],
					   dfu->params.status.bwPollTime[2]));
	os_dbg(OS_DBG_DEBUG, "bState = %02X\n", dfu->state);
	os_dbg(OS_DBG_DEBUG, "bStatus = %02X\n", dfu->status);
	os_dbg(OS_DBG_DEBUG, "ulPollTime = %d\n", dfu->poll_time);

	if (dfu->state == DFU_STATE_dfuERROR) {
		dfu_clear_status_sync(dfu);
		return;
	}
	switch (dfu->seq) {
	case DFU_SEQ_DNLOAD:
		switch (dfu->state) {
		case DFU_STATE_dfuDNBUSY:
		case DFU_STATE_dfuDNLOAD_SYNC:
		case DFU_STATE_dfuMANIFEST_SYNC:
		case DFU_STATE_dfuMANIFEST:
			dfu_poll_interface(dfu->dev, DFU_GETSTATUS);
			break;
		case DFU_STATE_dfuMANIFEST_WAIT_RESET:
			dfu_poll_interface(dfu->dev, DFU_RESET);
			break;
		case DFU_STATE_dfuDNLOAD_IDLE:
			dfu_buffer_run(dfu, 0);
			break;
		case DFU_STATE_dfuIDLE:
			dfu_seq_ended(dfu);
			break;
		default:
			dfu_poll_interface(dfu->dev, DFU_RESET);
			break;
		}
		break;
	case DFU_SEQ_UPLOAD:
		switch (dfu->state) {
		case DFU_STATE_dfuUPLOAD_IDLE:
			dfu_buffer_run(dfu, 0);
			break;
		case DFU_STATE_dfuIDLE:
			dfu_seq_ended(dfu);
			break;
		default:
			dfu_poll_interface(dfu->dev, DFU_RESET);
			break;
		}
		break;
	case DFU_SEQ_DETACH:
		switch (dfu->state) {
		case DFU_STATE_appDETACH:
			dfu_poll_interface(dfu->dev, DFU_RESET);
			break;
		case DFU_STATE_dfuIDLE:
			dfu_seq_ended(dfu);
			break;
		default:
			dfu_poll_interface(dfu->dev, DFU_RESET);
			break;
		}
		break;
	case DFU_SEQ_ABORT:
		switch (dfu->state) {
		case DFU_STATE_dfuIDLE:
			dfu_seq_ended(dfu);
			break;
		default:
			dfu_poll_interface(dfu->dev, DFU_ABORT);
			break;
		}
		break;
	default:
		dfu_seq_ended(dfu);
		dfu_init_seq(dfu);
		break;
	}
}

static void dfu_clear_status_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	dfu_clear_status_unsync(dfu);
	dfu_poll_interface(dfu->dev, DFU_GETSTATUS);
}

static void dfu_dnload_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;
	size_t len = urb->length;

	if (!USBD_IS_SUCCESS(urbstatus)) {
		if (urbstatus != USBD_STATUS_ERROR_BUSY)
			dfu->eof_firm = 1;
		dfu_buffer_run(dfu, 0);
		return;
	}
	dfu->pos += len;
	os_dbg(OS_DBG_INFO, "DNLOAD length = 0x%08x, 0x%08x\n",
	       len, dfu->pos);
	/* XXX: Circular Buffer POS/EOF Setting Rule
	 * EOF is set after circular buffer positions.
	 */
	dfu_buffer_read(dfu, len);
	if (len == 0) {
		os_dbg(OS_DBG_DEBUG, "====================\r\n");
		os_dbg(OS_DBG_INFO, "DNLOAD total = 0x%08x\r\n", dfu->pos);
		os_dbg(OS_DBG_DEBUG, "====================\r\n");
		dfu->eof_firm = 1;
	}
	dfu_get_status_sync(dfu);
}

static void dfu_upload_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;
	size_t len = urb->length;

	if (!USBD_IS_SUCCESS(urbstatus)) {
		if (urbstatus != USBD_STATUS_ERROR_BUSY)
			dfu->eof_firm = 1;
		dfu_buffer_run(dfu, 0);
		return;
	}
	dfu->pos += len;
	os_dbg(OS_DBG_INFO, "UPLOAD length = 0x%08x, 0x%08x\n",
	       len, dfu->pos);
	/* XXX: Circular Buffer POS/EOF Setting Rule
	 * EOF is set after circular buffer positions.
	 */
	dfu_buffer_write(dfu, len);
	if (len < dfu->xfr_size) {
		os_dbg(OS_DBG_DEBUG, "====================\r\n");
		os_dbg(OS_DBG_INFO, "UPLOAD total = 0x%08x\r\n", dfu->pos);
		os_dbg(OS_DBG_DEBUG, "====================\r\n");
		dfu->eof_firm = 1;
	}
	dfu_buffer_run(dfu, 0);
}

static void dfu_control_cmpl(os_usbif *usb, struct os_usb_urb *urb)
{
	struct dfu_device *dfu = usb_dev_priv(usb);
	struct dfu_urb *dfu_urb = urb_priv(urb);
	usb_status_t urbstatus = os_usb_get_urb_status(dfu->dev, USB_EID_DEFAULT);

	xchg(&dfu->busy, 0);
	if (!USBD_IS_SUCCESS(urb->status)) {
		if (urb->status != USBD_STATUS_ERROR_BUSY) {
			dfu_get_status_sync(dfu);
		}
	}
	switch (dfu_urb->request) {
	case DFU_GETSTATUS:
		dfu_get_status_cmpl(dfu->dev, urb);
		break;
	case DFU_CLRSTATUS:
		dfu_clear_status_cmpl(dfu->dev, urb);
		break;
	case DFU_DNLOAD:
		dfu_dnload_cmpl(dfu->dev, urb);
		break;
	case DFU_UPLOAD:
		dfu_upload_cmpl(dfu->dev, urb);
		break;
	default:
		BUG();
		break;
	}
}

static void usb_worker_callback(os_device dev, struct os_work_item *work)
{
	struct os_usb_urb *urb = work_priv(work);
	struct dfu_urb *dfu_urb = urb_priv(urb);
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);

	if (cmpxchg(&dfu->busy, 0, 1) != 0)
		goto end;
	dfu_control_aval(dfu, urb);
	dfu_control_cmpl(dfu, urb);
end:
	dfu->work_count--;
	free(work);
}

void dfu_file_idle(struct dfu_device *dfu)
{
	os_dbg(OS_DBG_DEBUG, "DFU IDLE: seq=%d\r\n", dfu->seq);
	os_waiter_idle(&dfu->waiter);
}

void dfu_file_wake(struct dfu_device *dfu)
{
	os_dbg(OS_DBG_DEBUG, "DFU WAKE: seq=%d\r\n", dfu->seq);
	os_waiter_wake(&dfu->waiter);
}

status_t __dfu_poll_interface(os_device dev, uint8_t request)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	struct os_work_item *work;
	struct os_usb_urb *urb;
	struct dfu_urb *dfu_urb;

	if (dfu->seq_ended)
		return STATUS_INVALID_DEVICE_STATE;
	work = malloc(sizeof (struct os_work_item)+sizeof (struct dfu_urb)+
		      sizeof (struct os_usb_urb));
	if (!work) return STATUS_INSUFFICIENT_RESOURCES;
	os_worker_init_item(work, usb_worker_callback);
	urb = work_priv(work);
	os_usb_urb_init(urb);
	dfu_urb = urb_priv(urb);
	dfu_urb->request = request;
	dfu_file_idle(dfu);
	dfu->work_count++;
	os_worker_queue_item(dfu->work_queue, work);
	return STATUS_SUCCESS;
}

status_t dfu_poll_interface(os_device dev, uint8_t request)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status = STATUS_SUCCESS;

#if 0
	if (request == DFU_GETSTATUS) {
		dfu_timer_submit(dfu);
	} else
#endif
	{
		status = __dfu_poll_interface(dev, request);
	}
	return status;
}

static void dfu_timer_callback(os_timer *timer, os_device dev, void *data)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	__dfu_poll_interface(dev, DFU_GETSTATUS);
}

void dfu_buffer_calc(struct dfu_device *dfu, uint16_t xfr_size)
{
	size_t size;

	BUG_ON(DFU_BUFFERED_XFRS<=1);
	
	dfu->xfr_size = xfr_size;
	size = __roundup32(dfu->xfr_size*DFU_BUFFERED_XFRS);
	dfu->length = size;
	dfu->pad_size = size-(dfu->xfr_size*DFU_BUFFERED_XFRS);
}

void dfu_buffer_run(struct dfu_device *dfu, int user)
{
	size_t space, count;
	int eof_user, eof_firm;

	/* XXX: Circular Buffer POS/EOF Snapshot Rule
	 * If this is called from user context, then eof_user can be
	 * ensured and count(DNLOAD)/space(UPLOAD) can be ensured big
	 * enough.  Getting eof_firm first can ensure space/count is
	 * correct.
	 * If this is called from firm context, then eof_firm can be
	 * ensured and space(DNLOAD)/count(UPLOAD) can be ensured big
	 * enough.  Getting eof_user first can ensure count/space is
	 * correct.
	 */
	eof_user = dfu->eof_user;
	eof_firm = dfu->eof_firm;
	space = dfu_buffer_space(dfu);
	count = dfu_buffer_count(dfu);

	os_dbg(OS_DBG_DEBUG, "COUNT: 0x%04x (0...size)\r\n", count);
	os_dbg(OS_DBG_DEBUG, "SPACE: 0x%04x (0...size-1)\r\n", space);
	os_dbg(OS_DBG_DEBUG, "EOF: user=%s, firm=%s\r\n",
	       eof_user ? "yes" : "no",
	       eof_firm ? "yes" : "no");

	if (dfu->seq == DFU_DNLOAD) {
		if (space > 0 || eof_firm) {
			dfu_file_wake(dfu);
		} else if (!user) {
			dfu_file_idle(dfu);
		}
		if (count >= dfu->xfr_size || eof_user) {
			if (!eof_firm)
				dfu_poll_interface(dfu->dev, DFU_DNLOAD);
		}
	} else {
		if (count > 0 || eof_firm) {
			dfu_file_wake(dfu);
		} else if (!user) {
			dfu_file_idle(dfu);
		}
		if (space >= dfu->xfr_size) {
			if (!eof_firm)
				dfu_poll_interface(dfu->dev, DFU_UPLOAD);
		}
	}
	if (eof_firm && !user) {
		dfu_poll_interface(dfu->dev, DFU_GETSTATUS);
	}
}

void dfu_buffer_init(struct dfu_device *dfu)
{
	dfu_clear_status_unsync(dfu);
	dfu_get_status_unsync(dfu);
	dfu->usb_status = USBD_STATUS_SUCCESS;
	dfu->dfu_status = DFU_STATUS_OK;

	BUG_ON((dfu->seq != DFU_DNLOAD) &&
	       (dfu->seq != DFU_UPLOAD));
	os_waiter_init(&dfu->waiter, FALSE);

	INIT_CIRCBF_DECLARE(&dfu->circbf);
	dfu->eof_user = 0;
	dfu->eof_firm = 0;
	dfu->pos = 0;
	dfu_buffer_run(dfu, 1);
}

void dfu_buffer_exit(struct dfu_device *dfu)
{
	os_waiter_exit(&dfu->waiter);
}

void dfu_buffer_read(struct dfu_device *dfu, size_t bytes)
{
	size_t count;
	count = circbf_count_end(&dfu->circbf, dfu->length);
	BUG_ON(bytes > count);
	circbf_read(&dfu->circbf, dfu->length, bytes);
	if (circbf_roff(&dfu->circbf) == (dfu->length-dfu->pad_size))
		circbf_read(&dfu->circbf, dfu->length, dfu->pad_size);
}

void dfu_buffer_write(struct dfu_device *dfu, size_t bytes)
{
	size_t space;
	space = circbf_space_end(&dfu->circbf, dfu->length);
	BUG_ON(bytes > space);
	circbf_write(&dfu->circbf, dfu->length, bytes);
	if (circbf_woff(&dfu->circbf) == (dfu->length-dfu->pad_size))
		circbf_write(&dfu->circbf, dfu->length, dfu->pad_size);
}

size_t dfu_buffer_space(struct dfu_device *dfu)
{
	size_t space = circbf_space_end(&dfu->circbf, dfu->length);
	BUG_ON(circbf_woff(&dfu->circbf) >= dfu->xfr_size*DFU_BUFFERED_XFRS);
	BUG_ON(circbf_roff(&dfu->circbf) >= dfu->xfr_size*DFU_BUFFERED_XFRS);
	if (circbf_roff(&dfu->circbf) <= circbf_woff(&dfu->circbf))
		space -= dfu->pad_size;
	BUG_ON(space > (dfu->xfr_size*DFU_BUFFERED_XFRS-1));
	return space;
}

size_t dfu_buffer_count(struct dfu_device *dfu)
{
	size_t count = circbf_count_end(&dfu->circbf, dfu->length);
	BUG_ON(circbf_woff(&dfu->circbf) >= dfu->xfr_size*DFU_BUFFERED_XFRS);
	BUG_ON(circbf_roff(&dfu->circbf) >= dfu->xfr_size*DFU_BUFFERED_XFRS);
	if (circbf_woff(&dfu->circbf) < circbf_roff(&dfu->circbf))
		count -= dfu->pad_size;
	BUG_ON(count > dfu->xfr_size*DFU_BUFFERED_XFRS);
	return count;
}

status_t dfu_buffer_poll(struct dfu_device *dfu, unsigned long *poll)
{
	size_t count, space;
	int eof_firm;
	unsigned long cond;

	BUG_ON(!poll);
	cond = *poll, *poll = 0;
again:
	dfu_buffer_run(dfu, 1);
	os_dbg(OS_DBG_DEBUG, "DFU_BUFFER - Waiting\r\n");
	if (os_waiter_wait(&dfu->waiter, OS_WAIT_INFINITE)) {
		os_dbg(OS_DBG_INFO, "DFU_BUFFER - Awaking\r\n");
		if (cond & POLL_READ) {
			/* XXX: Circular Buffer POS/EOF Getting Rule
			 * EOF is get before circular buffer positions.
			 */
			eof_firm = dfu->eof_firm;
			count = dfu_buffer_count(dfu);
			if (count > 0 || eof_firm)
				(*poll) |= POLL_READ;
		}
		if (cond & POLL_WRITE) {
			/* XXX: Circular Buffer POS/EOF Getting Rule
			 * EOF is get before circular buffer positions.
			 */
			eof_firm = dfu->eof_firm;
			space = dfu_buffer_space(dfu);
			if (space > 0 || eof_firm)
				(*poll) |= POLL_WRITE;
		}
		if (*poll)
			return STATUS_SUCCESS;
		goto again;
	} else {
		/* TODO: Allow Timeout IO for User Space
		 * Is this enough for breaking the timed out pipe?
		 */
		os_dbg(OS_DBG_ERR, "DFU_BUFFER - Timeout\r\n");
		return STATUS_TIMEOUT;
	}
}

static status_t dfu_read_file(struct dfu_device *dfu,
			      char *buf, size_t *count)
{
	status_t status = STATUS_SUCCESS;
	size_t bytes = 0;
	unsigned long poll = POLL_READ;

	if ((dfu->proto != USBDFU_PROTO_DFUMODE) ||
	    (dfu->seq != DFU_SEQ_UPLOAD)) {
		status = STATUS_INVALID_DEVICE_STATE;
		goto end;
	}

	status = dfu_buffer_poll(dfu, &poll);
	if (!OS_IS_SUCCESS(status)) goto end;

	bytes = dfu_buffer_count(dfu);
	if (bytes > 0)
		bytes = min(*count, bytes);
	if (bytes > 0)
		memcpy(buf, circbf_rpos(&dfu->circbf), bytes);
	/* XXX: Circular Buffer POS/EOF Setting Rule
	 * EOF is set after circular buffer positions.
	 */
	dfu_buffer_read(dfu, bytes);
	if (bytes == 0)
		dfu->eof_user = 1;
	dfu_buffer_run(dfu, 1);
	*count = bytes;
end:
	return status;
}

static status_t dfu_write_file(struct dfu_device *dfu,
			       char *buf, size_t *count)
{
	status_t status = STATUS_SUCCESS;
	size_t bytes = 0;
	unsigned long poll = POLL_WRITE;

	if ((dfu->proto != USBDFU_PROTO_DFUMODE) ||
	    (dfu->seq != DFU_SEQ_DNLOAD)) {
		status = STATUS_INVALID_DEVICE_STATE;
		*count = 0;
		goto end;
	}

	status = dfu_buffer_poll(dfu, &poll);
	if (status != STATUS_SUCCESS) goto end;

	bytes = dfu_buffer_space(dfu);
	if (bytes > 0)
		bytes = min(*count, bytes);
	if (bytes > 0)
		memcpy(circbf_wpos(&dfu->circbf), buf, bytes);
	/* XXX: Circular Buffer POS/EOF Setting Rule
	 * EOF is set after circular buffer positions.
	 */
	dfu_buffer_write(dfu, bytes);
	if (bytes == 0)
		dfu->eof_user = 1;
	dfu_buffer_run(dfu, 1);
	*count = bytes;
end:
	return status;
}

void dfu_test_read(struct dfu_device *dfu)
{
	size_t offset = 0, total = 0;
	char buffer[DFU_USER_BUFFER_SIZE];
	size_t len;
	char *pos;
	status_t status;

	dfu_begin_seq(dfu, USBDFU_PROTO_DFUMODE, DFU_UPLOAD);
	dfu_buffer_init(dfu);
	do {
		len = DFU_USER_BUFFER_SIZE-offset;
		pos = buffer+offset;
		status = dfu_read_file(dfu, pos, &len);
		if (status != STATUS_SUCCESS) {
			os_dbg(OS_DBG_ERR, "READ - failure(%X)\r\n", status);
			break;
		}
		total += len;
		offset += len;
		if (len == 0) {
			os_dbg(OS_DBG_DEBUG, "====================\r\n");
			os_dbg(OS_DBG_INFO, "READ total = 0x%08x\r\n",
			       total);
			os_dbg(OS_DBG_DEBUG, "====================\r\n");
			break;
		} else {
			os_dbg(OS_DBG_INFO, "READ length = 0x%08x, 0x%08x\r\n",
			       len, total);
		}
		BUG_ON(offset > DFU_USER_BUFFER_SIZE);
		if (offset == DFU_USER_BUFFER_SIZE) {
			offset = 0;
		}
	} while (1);
	BUG_ON(total != DFU_TEST_FIRM_TOTAL);
	dfu_end_seq(dfu);
	wait_for_interact();
}

void dfu_test_write(struct dfu_device *dfu)
{
	size_t offset = 0, total = 0;
	char buffer[DFU_USER_BUFFER_SIZE];
	size_t len;
	char *pos;
	status_t status;

	dfu_begin_seq(dfu, USBDFU_PROTO_DFUMODE, DFU_DNLOAD);
	dfu_buffer_init(dfu);
	do {
		len = DFU_USER_BUFFER_SIZE-offset;
		if (total + len > DFU_TEST_FIRM_TOTAL)
			len = DFU_TEST_FIRM_TOTAL-total;

		pos = buffer+offset;
		status = dfu_write_file(dfu, pos, &len);
		if (status != STATUS_SUCCESS) {
			os_dbg(OS_DBG_ERR, "WRITE - failure(%X)\r\n", status);
			break;
		}
		if (len == 0) {
			os_dbg(OS_DBG_DEBUG, "====================\r\n");
			os_dbg(OS_DBG_INFO, "WRITE total = 0x%08x\r\n",
			       total);
			os_dbg(OS_DBG_DEBUG, "====================\r\n");
			break;
		} else {
			os_dbg(OS_DBG_INFO, "WRITE length = 0x%08x, 0x%08x\r\n",
			       len, total);
		}
		total += len;
		offset += len;
		BUG_ON(offset > DFU_USER_BUFFER_SIZE);
		if (offset == DFU_USER_BUFFER_SIZE) {
			offset = 0;
		}
	} while (1);
	BUG_ON(total != DFU_TEST_FIRM_TOTAL);
	dfu_end_seq(dfu);
	wait_for_interact();
}

void dfu_delete_device(struct dfu_device *dfu)
{
	if (dfu) {
		dfu_timer_stop(dfu);
		if (dfu->work_queue)
			os_worker_delete_queue(dfu->work_queue);
		if (dfu->buffer)
			free(dfu->buffer);
		if (dfu->dfu_function_desc)
			free(dfu->dfu_function_desc);
		free(dfu);
	}
}

struct dfu_device *dfu_create_device(uint16_t transfer_size)
{
	struct dfu_device *dfu = NULL;

	dfu = malloc(sizeof (struct dfu_device));
	if (!dfu) goto fail;
	memset(dfu, 0, sizeof (struct dfu_device));

	dfu->dev = (void *)dfu;
	dfu->seq = DFU_SEQ_IDLE;
	dfu->state = DFU_STATE_dfuIDLE;
	dfu->proto = USBDFU_PROTO_DFUMODE;

	dfu->dfu_function_desc = malloc(sizeof (struct dfu_function_desc));
	if (!dfu->dfu_function_desc) goto fail;
	dfu->dfu_function_desc->wTransferSize = transfer_size;

	dfu_buffer_calc(dfu, dfu->dfu_function_desc->wTransferSize);
	dfu->buffer = malloc(dfu->xfr_size*DFU_BUFFERED_XFRS);
	if (!dfu->buffer) goto fail;
	INIT_CIRCBF_ASSIGN(&dfu->circbf, dfu->buffer);

	dfu->work_queue = os_worker_create_queue(dfu->dev);
	if (!dfu->work_queue) goto fail;

	dfu_timer_start(dfu);
	return dfu;

fail:
	dfu_delete_device(dfu);
	return NULL;
}

#define DFU_TEST_DNLOAD		0x01
#define DFU_TEST_UPLOAD		0x02
void dfu_run_test(struct dfu_device *dfu, int test)
{
	int count = 0;
	while (1) {
		count++;
		printf("********************\r\n");
		printf("test round - %d\r\n", count);
		printf("********************\r\n");
		if (test & DFU_TEST_UPLOAD)
			dfu_test_read(dfu);
		if (test & DFU_TEST_DNLOAD)
			dfu_test_write(dfu);
	}
}

struct os_work_queue *os_usb_get_pipe(os_usbif *usb)
{
	struct dfu_device *dfu = usb_dev_priv(usb);
	return dfu->work_queue;
}

void main(void)
{
	struct dfu_device *dfu;
	dfu = dfu_create_device(DFU_TEST_XFR_LENGTH);
	if (!dfu) return;
	dfu_test_read(dfu);
//	dfu_run_test(dfu, DFU_TEST_DNLOAD | DFU_TEST_UPLOAD);
	dfu_delete_device(dfu);
}
