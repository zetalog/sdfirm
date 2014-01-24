
#include "usb_int.h"

static int calculate_timeout(struct usbi_transfer *transfer)
{
	int r;
	struct timespec current_time;
	unsigned int timeout =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(transfer)->timeout;

	if (!timeout)
		return 0;

	r = usbi_backend->clock_gettime(USBI_CLOCK_MONOTONIC, &current_time);
	if (r < 0) {
		usbi_err(ITRANSFER_CTX(transfer),
			"failed to read monotonic clock, errno=%d", errno);
		return r;
	}

	current_time.tv_sec += timeout / 1000;
	current_time.tv_nsec += (timeout % 1000) * 1000000;

	if (current_time.tv_nsec > 1000000000) {
		current_time.tv_nsec -= 1000000000;
		current_time.tv_sec++;
	}

	TIMESPEC_TO_TIMEVAL(&transfer->timeout, &current_time);
	return 0;
}

static void add_to_flying_list(struct usbi_transfer *transfer)
{
	struct usbi_transfer *cur;
	struct timeval *timeout = &transfer->timeout;
	struct libusb_context *ctx = ITRANSFER_CTX(transfer);
	
	/* if we have no other flying transfers, start the list with this one */
	if (list_empty(&ctx->flying_transfers)) {
		list_add(&transfer->list, &ctx->flying_transfers);
		goto out;
	}

	/* if we have infinite timeout, append to end of list */
	if (!timerisset(timeout)) {
		list_add_tail(&transfer->list, &ctx->flying_transfers);
		goto out;
	}

	/* otherwise, find appropriate place in list */
	list_for_each_entry(struct usbi_transfer, cur, &ctx->flying_transfers, list) {
		/* find first timeout that occurs after the transfer in question */
		struct timeval *cur_tv = &cur->timeout;

		if (!timerisset(cur_tv) || (cur_tv->tv_sec > timeout->tv_sec) ||
		    (cur_tv->tv_sec == timeout->tv_sec &&
		     cur_tv->tv_usec > timeout->tv_usec)) {
			list_add_tail(&transfer->list, &cur->list);
			goto out;
		}
	}

	/* otherwise we need to be inserted at the end */
	list_add_tail(&transfer->list, &ctx->flying_transfers);
out:
	;
}

struct libusb_transfer *libusb_alloc_transfer(int iso_packets)
{
	size_t os_alloc_size = usbi_backend->transfer_priv_size
		+ (usbi_backend->add_iso_packet_size * iso_packets);
	int alloc_size = sizeof(struct usbi_transfer)
		+ sizeof(struct libusb_transfer)
		+ (sizeof(struct libusb_iso_packet_descriptor) * iso_packets)
		+ os_alloc_size;
	struct usbi_transfer *itransfer = malloc(alloc_size);
	if (!itransfer)
		return NULL;

	memset(itransfer, 0, alloc_size);
	itransfer->num_iso_packets = iso_packets;
	return __USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
}

void libusb_free_transfer(struct libusb_transfer *transfer)
{
	struct usbi_transfer *itransfer;

	if (!transfer)
		return;

	itransfer = __LIBUSB_TRANSFER_TO_USBI_TRANSFER(transfer);

	if (usbi_backend->free_transfer)
		usbi_backend->free_transfer(itransfer);
	if (transfer->flags & LIBUSB_TRANSFER_FREE_BUFFER && transfer->buffer)
		free(transfer->buffer);

	free(itransfer);
}

int libusb_submit_transfer(struct libusb_transfer *transfer)
{
	struct usbi_transfer *itransfer =
		__LIBUSB_TRANSFER_TO_USBI_TRANSFER(transfer);
	int r;

	itransfer->transferred = 0;
	itransfer->flags = 0;
	r = calculate_timeout(itransfer);
	if (r < 0)
		return LIBUSB_ERROR_OTHER;

	add_to_flying_list(itransfer);
	r = usbi_backend->submit_transfer(itransfer);
	if (r) {
		list_del(&itransfer->list);
	}

	return r;
}

int libusb_cancel_transfer(struct libusb_transfer *transfer)
{
	struct usbi_transfer *itransfer =
		__LIBUSB_TRANSFER_TO_USBI_TRANSFER(transfer);
	int r;

	r = usbi_backend->cancel_transfer(itransfer);
	if (r < 0) {
		usbi_err(TRANSFER_CTX(transfer),
			 "cancel transfer failed error %d", r);
	}
	return r;
}

/* Handle completion of a transfer (completion might be an error condition).
 * This will invoke the user-supplied callback function, which may end up
 * freeing the transfer. Therefore you cannot use the transfer structure
 * after calling this function, and you should free all backend-specific
 * data before calling it. */
void usbi_handle_transfer_completion(struct usbi_transfer *itransfer,
				     enum libusb_transfer_status status)
{
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	uint8_t flags;

	list_del(&itransfer->list);

	if (status == LIBUSB_TRANSFER_COMPLETED &&
	    transfer->flags & LIBUSB_TRANSFER_SHORT_NOT_OK) {
		int rqlen = transfer->length;
		if (transfer->type == LIBUSB_TRANSFER_TYPE_CONTROL)
			rqlen -= LIBUSB_CONTROL_SETUP_SIZE;
		if (rqlen != itransfer->transferred) {
			usbi_dbg("interpreting short transfer as error");
			status = LIBUSB_TRANSFER_ERROR;
		}
	}

	flags = transfer->flags;
	transfer->status = status;
	transfer->actual_length = itransfer->transferred;
	if (transfer->callback)
		transfer->callback(transfer);
	/* transfer might have been freed by the above call, do not use from
	 * this point. */
	if (flags & LIBUSB_TRANSFER_FREE_TRANSFER)
		libusb_free_transfer(transfer);
}

/* Similar to usbi_handle_transfer_completion() but exclusively for transfers
 * that were asynchronously cancelled. The same concerns w.r.t. freeing of
 * transfers exist here.
 */
void usbi_handle_transfer_cancellation(struct usbi_transfer *transfer)
{
	/* if the URB was cancelled due to timeout, report timeout to the user */
	if (transfer->flags & USBI_TRANSFER_TIMED_OUT) {
		usbi_dbg("detected timeout cancellation");
		usbi_handle_transfer_completion(transfer, LIBUSB_TRANSFER_TIMED_OUT);
		return;
	}

	/* otherwise its a normal async cancel */
	usbi_handle_transfer_completion(transfer, LIBUSB_TRANSFER_CANCELLED);
}

static void handle_timeout(struct usbi_transfer *itransfer)
{
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	int r;

	itransfer->flags |= USBI_TRANSFER_TIMED_OUT;
	r = libusb_cancel_transfer(transfer);
	if (r < 0) {
		usbi_warn(TRANSFER_CTX(transfer),
			  "async cancel failed %d errno=%d", r, errno);
	}
}

static int handle_timeouts(struct libusb_context *ctx)
{
	struct timespec systime_ts;
	struct timeval systime;
	struct usbi_transfer *transfer;
	int r = 0;

	USBI_GET_CONTEXT(ctx);
	if (list_empty(&ctx->flying_transfers))
		goto out;

	/* get current time */
	r = usbi_backend->clock_gettime(USBI_CLOCK_MONOTONIC, &systime_ts);
	if (r < 0)
		goto out;

	TIMESPEC_TO_TIMEVAL(&systime, &systime_ts);

	/* iterate through flying transfers list, finding all transfers that
	 * have expired timeouts */
	list_for_each_entry(struct usbi_transfer, transfer,
			    &ctx->flying_transfers, list) {
		struct timeval *cur_tv = &transfer->timeout;

		/* if we've reached transfers of infinite timeout, we're all done */
		if (!timerisset(cur_tv))
			goto out;

		/* ignore timeouts we've already handled */
		if (transfer->flags & USBI_TRANSFER_TIMED_OUT)
			continue;

		/* if transfer has non-expired timeout, nothing more to do */
		if ((cur_tv->tv_sec > systime.tv_sec) ||
		    (cur_tv->tv_sec == systime.tv_sec &&
		     cur_tv->tv_usec > systime.tv_usec))
			goto out;
	
		/* otherwise, we've got an expired timeout to handle */
		handle_timeout(transfer);
	}

out:
	return r;
}

/* do the actual event handling. assumes that no other thread is concurrently
 * doing the same thing. */
static int handle_events(struct libusb_context *ctx, struct timeval *tv)
{
	int r;
	struct usbi_pollfd *ipollfd;
	nfds_t nfds = 0;
	struct pollfd *fds;
	int i = -1;
	int timeout_ms;

	list_for_each_entry(struct usbi_pollfd, ipollfd, &ctx->pollfds, list)
		nfds++;

	/* TODO: malloc when number of fd's changes, not on every poll */
	fds = malloc(sizeof(*fds) * nfds);
	if (!fds)
		return LIBUSB_ERROR_NO_MEM;

	list_for_each_entry(struct usbi_pollfd, ipollfd, &ctx->pollfds, list) {
		struct libusb_pollfd *pollfd = &ipollfd->pollfd;
		int fd = pollfd->fd;
		i++;
		fds[i].fd = fd;
		fds[i].events = pollfd->events;
		fds[i].revents = 0;
	}

	timeout_ms = (tv->tv_sec * 1000) + (tv->tv_usec / 1000);

	/* round up to next millisecond */
	if (tv->tv_usec % 1000)
		timeout_ms++;

	usbi_dbg("poll() %d fds with timeout in %dms", nfds, timeout_ms);
	r = poll(fds, nfds, timeout_ms);
	usbi_dbg("poll() returned %d", r);
	if (r == 0) {
		free(fds);
		return handle_timeouts(ctx);
	} else if (r == -1 && errno == EINTR) {
		free(fds);
		return LIBUSB_ERROR_INTERRUPTED;
	} else if (r < 0) {
		free(fds);
		usbi_err(ctx, "poll failed %d err=%d\n", r, errno);
		return LIBUSB_ERROR_IO;
	}

	r = usbi_backend->handle_events(ctx, fds, nfds, r);
	if (r)
		usbi_err(ctx, "backend handle_events failed with error %d", r);

	free(fds);
	return r;
}

/* returns the smallest of:
 *  1. timeout of next URB
 *  2. user-supplied timeout
 * returns 1 if there is an already-expired timeout, otherwise returns 0
 * and populates out
 */
static int get_next_timeout(libusb_context *ctx, struct timeval *tv,
			    struct timeval *out)
{
	struct timeval timeout;
	int r = libusb_get_next_timeout(ctx, &timeout);
	if (r) {
		/* timeout already expired? */
		if (!timerisset(&timeout))
			return 1;

		/* choose the smallest of next URB timeout or user specified timeout */
		if (timercmp(&timeout, tv, <))
			*out = timeout;
		else
			*out = *tv;
	} else {
		*out = *tv;
	}
	return 0;
}

int libusb_handle_events_timeout(libusb_context *ctx,
				 struct timeval *tv)
{
	int r;
	struct timeval poll_timeout;

	USBI_GET_CONTEXT(ctx);
	r = get_next_timeout(ctx, tv, &poll_timeout);
	if (r) {
		/* timeout already expired */
		return handle_timeouts(ctx);
	}
	return handle_events(ctx, &poll_timeout);
}

int libusb_handle_events(libusb_context *ctx)
{
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	return libusb_handle_events_timeout(ctx, &tv);
}

int libusb_get_next_timeout(libusb_context *ctx,
			    struct timeval *tv)
{
	struct usbi_transfer *transfer;
	struct timespec cur_ts;
	struct timeval cur_tv;
	struct timeval *next_timeout;
	int r;
	int found = 0;

	USBI_GET_CONTEXT(ctx);
	if (list_empty(&ctx->flying_transfers)) {
		usbi_dbg("no URBs, no timeout!");
		return 0;
	}

	/* find next transfer which hasn't already been processed as timed out */
	list_for_each_entry(struct usbi_transfer, transfer,
			    &ctx->flying_transfers, list) {
		if (!(transfer->flags & USBI_TRANSFER_TIMED_OUT)) {
			found = 1;
			break;
		}
	}

	if (!found) {
		usbi_dbg("all URBs have already been processed for timeouts");
		return 0;
	}

	next_timeout = &transfer->timeout;

	/* no timeout for next transfer */
	if (!timerisset(next_timeout)) {
		usbi_dbg("no URBs with timeouts, no timeout!");
		return 0;
	}

	r = usbi_backend->clock_gettime(USBI_CLOCK_MONOTONIC, &cur_ts);
	if (r < 0) {
		usbi_err(ctx, "failed to read monotonic clock, errno=%d", errno);
		return LIBUSB_ERROR_OTHER;
	}
	TIMESPEC_TO_TIMEVAL(&cur_tv, &cur_ts);

	if (timercmp(&cur_tv, next_timeout, >=)) {
		usbi_dbg("first timeout already expired");
		timerclear(tv);
	} else {
		timersub(next_timeout, &cur_tv, tv);
		usbi_dbg("next timeout in %d.%06ds", tv->tv_sec, tv->tv_usec);
	}

	return 1;
}

void libusb_set_pollfd_notifiers(libusb_context *ctx,
				 libusb_pollfd_added_cb added_cb,
				 libusb_pollfd_removed_cb removed_cb,
				 void *user_data)
{
	USBI_GET_CONTEXT(ctx);
	ctx->fd_added_cb = added_cb;
	ctx->fd_removed_cb = removed_cb;
	ctx->fd_cb_user_data = user_data;
}

/* Add a file descriptor to the list of file descriptors to be monitored.
 * events should be specified as a bitmask of events passed to poll(), e.g.
 * POLLIN and/or POLLOUT. */
int usbi_add_pollfd(struct libusb_context *ctx, int fd, short events)
{
	struct usbi_pollfd *ipollfd = malloc(sizeof(*ipollfd));
	if (!ipollfd)
		return LIBUSB_ERROR_NO_MEM;

	usbi_dbg("add fd %d events %d", fd, events);
	ipollfd->pollfd.fd = fd;
	ipollfd->pollfd.events = events;
	list_add_tail(&ipollfd->list, &ctx->pollfds);

	if (ctx->fd_added_cb)
		ctx->fd_added_cb(fd, events, ctx->fd_cb_user_data);
	return 0;
}

/* Remove a file descriptor from the list of file descriptors to be polled. */
void usbi_remove_pollfd(struct libusb_context *ctx, int fd)
{
	struct usbi_pollfd *ipollfd;
	int found = 0;

	usbi_dbg("remove fd %d", fd);
	list_for_each_entry(struct usbi_pollfd, ipollfd, &ctx->pollfds, list)
		if (ipollfd->pollfd.fd == fd) {
			found = 1;
			break;
		}

	if (!found) {
		usbi_dbg("couldn't find fd %d to remove", fd);
		return;
	}

	list_del(&ipollfd->list);
	free(ipollfd);
	if (ctx->fd_removed_cb)
		ctx->fd_removed_cb(fd, ctx->fd_cb_user_data);
}

const struct libusb_pollfd **libusb_get_pollfds(libusb_context *ctx)
{
	struct libusb_pollfd **ret = NULL;
	struct usbi_pollfd *ipollfd;
	size_t i = 0;
	size_t cnt = 0;
	USBI_GET_CONTEXT(ctx);

	list_for_each_entry(struct usbi_pollfd, ipollfd, &ctx->pollfds, list)
		cnt++;

	ret = calloc(cnt + 1, sizeof(struct libusb_pollfd *));
	if (!ret)
		goto out;

	list_for_each_entry(struct usbi_pollfd, ipollfd, &ctx->pollfds, list)
		ret[i++] = (struct libusb_pollfd *) ipollfd;
	ret[cnt] = NULL;

out:
	return (const struct libusb_pollfd **) ret;
}

/* Backends call this from handle_events to report disconnection of a device.
 * The transfers get cancelled appropriately.
 */
void usbi_handle_disconnect(struct libusb_device_handle *handle)
{
	struct usbi_transfer *cur;
	struct usbi_transfer *to_cancel;

	usbi_dbg("device %d.%d",
		handle->dev->bus_number, handle->dev->device_address);

	/* terminate all pending transfers with the LIBUSB_TRANSFER_NO_DEVICE
	 * status code.
	 * 
	 * this is a bit tricky because:
	 * 1. we can't do transfer completion while holding flying_transfers_lock
	 * 2. the transfers list can change underneath us - if we were to build a
	 *    list of transfers to complete (while holding look), the situation
	 *    might be different by the time we come to free them
	 *
	 * so we resort to a loop-based approach as below
	 * FIXME: is this still potentially racy?
	 */

	while (1) {
		to_cancel = NULL;
		list_for_each_entry(struct usbi_transfer, cur, &HANDLE_CTX(handle)->flying_transfers, list)
			if (__USBI_TRANSFER_TO_LIBUSB_TRANSFER(cur)->dev_handle == handle) {
				to_cancel = cur;
				break;
			}

		if (!to_cancel)
			break;

		usbi_backend->clear_transfer_priv(to_cancel);
		usbi_handle_transfer_completion(to_cancel, LIBUSB_TRANSFER_NO_DEVICE);
	}
}
