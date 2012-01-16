#ifndef __USB_INT_H_INCLUDE__
#define __USB_INT_H_INCLUDE__

#ifdef WIN32
#include <windows.h>
#include <winioctl.h>
#include <io.h>
#include <direct.h>
#define snprintf	_snprintf
#define PATH_MAX	_MAX_PATH
#ifndef S_ISDIR
#define S_ISDIR(m)	(m & _S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(x)	(x & _S_IFREG)
#endif

/*
 * The following is for compatibility across the various Linux
 * platforms.  The generic ioctl numbering scheme doesn't really enforce
 * a type field.  De facto, however, the top 8 bits of the lower 16
 * bits are indeed used as a type field, so we might just as well make
 * this explicit here.  Please be sure to use the decoding macros
 * below from now on.
 */
#define _IOC_NRBITS	8
#define _IOC_TYPEBITS	8
#define _IOC_SIZEBITS	14
#define _IOC_DIRBITS	2

#define _IOC_NRMASK	((1 << _IOC_NRBITS)-1)
#define _IOC_TYPEMASK	((1 << _IOC_TYPEBITS)-1)
#define _IOC_SIZEMASK	((1 << _IOC_SIZEBITS)-1)
#define _IOC_DIRMASK	((1 << _IOC_DIRBITS)-1)

#define _IOC_NRSHIFT	0
#define _IOC_TYPESHIFT	(_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT	(_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT	(_IOC_SIZESHIFT+_IOC_SIZEBITS)

/*
 * Direction bits.
 */
#define _IOC_NONE	0U
#define _IOC_WRITE	1U
#define _IOC_READ	2U

#define _IOC(dir,type,nr,size) \
	(((dir)  << _IOC_DIRSHIFT) | \
	 ((type) << _IOC_TYPESHIFT) | \
	 ((nr)   << _IOC_NRSHIFT) | \
	 ((size) << _IOC_SIZESHIFT))

/* provoke compile error for invalid uses of size argument */
extern unsigned int __invalid_size_argument_for_IOC;
#define _IOC_TYPECHECK(t) \
	((sizeof(t) == sizeof(t[1]) && \
	  sizeof(t) < (1 << _IOC_SIZEBITS)) ? \
	  sizeof(t) : __invalid_size_argument_for_IOC)

#ifndef _IOWR
#define _IOWR(type,nr,size)	_IOC(_IOC_READ|_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#endif

static long ioctl(unsigned int fd, unsigned int cmd,
		  unsigned long arg)
{
	return 0;
}

#define ETIME 62	/* Timer expired */
#define ENODATA 61	/* No data (for no delay io) */
#define EPROTO 71	/* Protocol error */
#define ETIMEDOUT 116	/* Connection timed out */
#define EOVERFLOW 139	/* Value too large for defined data type */

#else
#include <unistd.h>
#include <sys/ioctl.h>
#define _dirent		dirent
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <stdarg.h>
#include <dirent.h>
#include <fcntl.h>
#include <host/libusb.h>

#define USB_MAXENDPOINTS		32
#define USB_MAXINTERFACES		32
#define USB_MAXALTSETTING		128	/* Hard limit */
#define USB_MAXCONFIG			8

struct list_head {
	struct list_head *prev, *next;
};

/* Get an entry from the list 
 * 	ptr - the address of this list_head element in "type" 
 * 	type - the data type that contains "member"
 * 	member - the list_head element in "type" 
 */
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr) - (unsigned long)(&((type *)0L)->member)))

/* Get each entry from a list
 *	pos - A structure pointer has a "member" element
 *	head - list head
 *	member - the list_head element in "pos"
 */
#define list_for_each_entry(type, pos, head, member)			\
	for (pos = list_entry((head)->next, type, member);		\
	     &pos->member != (head);					\
	     pos = list_entry(pos->member.next, type, member))

#define list_for_each_entry_safe(type, pos, n, head, member)		\
        for (pos = list_entry((head)->next, type, member),		\
		n = list_entry(pos->member.next, type, member);		\
	     &pos->member != (head);					\
	     pos = n, n = list_entry(n->member.next, type, member))

#define list_empty(entry) ((entry)->next == (entry))

static inline void list_init(struct list_head *entry)
{
	entry->prev = entry->next = entry;
}

static inline void list_add(struct list_head *entry, struct list_head *head)
{
	entry->next = head->next;
	entry->prev = head;

	head->next->prev = entry;
	head->next = entry;
}

static inline void list_add_tail(struct list_head *entry,
	struct list_head *head)
{
	entry->next = head;
	entry->prev = head->prev;

	head->prev->next = entry;
	head->prev = entry;
}

static inline void list_del(struct list_head *entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
}

#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))

#define TIMESPEC_IS_SET(ts) ((ts)->tv_sec != 0 || (ts)->tv_nsec != 0)

enum usbi_log_level {
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
};

void usbi_log(struct libusb_context *ctx, enum usbi_log_level,
	      const char *format, ...);
void usbi_dbg(const char *fmt, ...);
void _usbi_log(libusb_context *ctx, enum usbi_log_level level,
	       const char *fmt, ...);
void usbi_info(libusb_context *ctx, const char *fmt, ...);
void usbi_warn(libusb_context *ctx, const char *fmt, ...);
void usbi_err(libusb_context *ctx, const char *fmt, ...);

#define USBI_GET_CONTEXT(ctx) if (!(ctx)) (ctx) = usbi_default_context
#define DEVICE_CTX(dev) ((dev)->ctx)
#define HANDLE_CTX(handle) (DEVICE_CTX((handle)->dev))
#define TRANSFER_CTX(transfer) (HANDLE_CTX((transfer)->dev_handle))
#define ITRANSFER_CTX(transfer) \
	(TRANSFER_CTX(__USBI_TRANSFER_TO_LIBUSB_TRANSFER(transfer)))

extern struct libusb_context *usbi_default_context;

struct libusb_context {
	int debug;
	int debug_fixed;

	struct list_head usb_devs;

	/* A list of open handles.
	 * Backends are free to traverse this if required.
	 */
	struct list_head open_devs;

	/* this is a list of in-flight transfer handles, sorted by timeout
	 * expiration. URBs to timeout the soonest are placed at the
	 * beginning of the list, URBs that will time out later are placed
	 * after, and urbs with infinite timeout are always placed at the
	 * very end.
	 */
	struct list_head flying_transfers;

	/* list of poll fds */
	struct list_head pollfds;

	/* user callbacks for pollfd changes */
	libusb_pollfd_added_cb fd_added_cb;
	libusb_pollfd_removed_cb fd_removed_cb;
	void *fd_cb_user_data;
};

struct libusb_device {
	/* lock protects refcnt, everything else is finalized at
	 * initialization time
	 */
	int refcnt;

	struct libusb_context *ctx;

	uint8_t bus_number;
	uint8_t device_address;
	uint8_t num_configurations;

	struct list_head list;
	unsigned long session_data;
	unsigned char os_priv[0];
};

struct libusb_device_handle {
	unsigned long claimed_interfaces;

	struct list_head list;
	struct libusb_device *dev;
	unsigned char os_priv[0];
};

#define USBI_TRANSFER_TIMED_OUT	 			(1<<0)

enum {
	USBI_CLOCK_MONOTONIC,
	USBI_CLOCK_REALTIME
};

/* in-memory transfer layout:
 *
 * 1. struct usbi_transfer
 * 2. struct libusb_transfer (which includes iso packets) [variable size]
 * 3. os private data [variable size]
 *
 * from a libusb_transfer, you can get the usbi_transfer by rewinding the
 * appropriate number of bytes.
 * the usbi_transfer includes the number of allocated packets, so you can
 * determine the size of the transfer and hence the start and length of
 * the OS-private data.
 */

struct usbi_transfer {
	int num_iso_packets;
	struct list_head list;
	struct timeval timeout;
	int transferred;
	uint8_t flags;
};

#define __USBI_TRANSFER_TO_LIBUSB_TRANSFER(transfer) \
	((struct libusb_transfer *)(((uint8_t *)(transfer)) \
		+ sizeof(struct usbi_transfer)))
#define __LIBUSB_TRANSFER_TO_USBI_TRANSFER(transfer) \
	((struct usbi_transfer *)(((uint8_t *)(transfer)) \
		- sizeof(struct usbi_transfer)))

static inline void *usbi_transfer_get_os_priv(struct usbi_transfer *transfer)
{
	return ((uint8_t *)transfer) + sizeof(struct usbi_transfer)
		+ sizeof(struct libusb_transfer)
		+ (transfer->num_iso_packets
			* sizeof(struct libusb_iso_packet_descriptor));
}

#ifdef WIN32
#include <pshpack1.h> 
#endif

/* All standard descriptors have these 2 fields in common */
struct usb_descriptor_header {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
};

#ifdef WIN32
#include <poppack.h>
#endif

/* shared data and functions */

struct libusb_device *usbi_alloc_device(struct libusb_context *ctx,
	unsigned long session_id);
struct libusb_device *usbi_get_device_by_session_id(struct libusb_context *ctx,
	unsigned long session_id);
int usbi_sanitize_device(struct libusb_device *dev);
void usbi_handle_disconnect(struct libusb_device_handle *handle);

void usbi_handle_transfer_completion(struct usbi_transfer *itransfer,
				     enum libusb_transfer_status status);
void usbi_handle_transfer_cancellation(struct usbi_transfer *transfer);

int usbi_parse_descriptor(unsigned char *source, char *descriptor,
			  void *dest, int host_endian);
int usbi_get_config_index_by_value(struct libusb_device *dev,
				   uint8_t bConfigurationValue,
				   uint8_t *idx);

/* polling */
struct usbi_pollfd {
	/* must come first */
	struct libusb_pollfd pollfd;
	struct list_head list;
};

int usbi_add_pollfd(struct libusb_context *ctx, int fd, short events);
void usbi_remove_pollfd(struct libusb_context *ctx, int fd);

/* device discovery */

/* we traverse usbfs without knowing how many devices we are going to find.
 * so we create this discovered_devs model which is similar to a linked-list
 * which grows when required. it can be freed once discovery has completed,
 * eliminating the need for a list node in the libusb_device structure
 * itself. */
struct discovered_devs {
	size_t len;
	size_t capacity;
	struct libusb_device *devices[0];
};

struct discovered_devs *discovered_devs_append(struct discovered_devs *discdevs,
					       struct libusb_device *dev);

/* OS abstraction */

/* This is the interface that OS backends need to implement.
 * All fields are mandatory, except ones explicitly noted as optional. */
struct usbi_os_backend {
	/* A human-readable name for your backend, e.g. "Linux usbfs" */
	const char *name;

	int (*init)(struct libusb_context *ctx);
	void (*exit)(void);

	int (*get_device_list)(struct libusb_context *ctx,
			       struct discovered_devs **discdevs);

	int (*open)(struct libusb_device_handle *handle);
	void (*close)(struct libusb_device_handle *handle);

	int (*get_device_descriptor)(struct libusb_device *device,
				     unsigned char *buffer,
				     int *host_endian);
	int (*get_active_config_descriptor)(struct libusb_device *device,
					    unsigned char *buffer,
					    size_t len, int *host_endian);
	int (*get_config_descriptor)(struct libusb_device *device,
				     uint8_t config_index,
				     unsigned char *buffer, size_t len,
				     int *host_endian);

	int (*get_configuration)(struct libusb_device_handle *handle,
				 uint8_t *config);
	int (*set_configuration)(struct libusb_device_handle *handle,
				 uint8_t config);
	int (*claim_interface)(struct libusb_device_handle *handle, int iface);
	int (*release_interface)(struct libusb_device_handle *handle, int iface);
	int (*set_interface_altsetting)(struct libusb_device_handle *handle,
					int iface, int altsetting);
	int (*clear_halt)(struct libusb_device_handle *handle,
			  unsigned char endpoint);
	int (*reset_device)(struct libusb_device_handle *handle);

	int (*kernel_driver_active)(struct libusb_device_handle *handle,
				    int Interface);
	int (*detach_kernel_driver)(struct libusb_device_handle *handle,
				    int Interface);
	int (*attach_kernel_driver)(struct libusb_device_handle *handle,
				    int Interface);

	void (*destroy_device)(struct libusb_device *dev);

	int (*submit_transfer)(struct usbi_transfer *itransfer);
	int (*cancel_transfer)(struct usbi_transfer *itransfer);
	void (*clear_transfer_priv)(struct usbi_transfer *itransfer);
	int (*handle_events)(struct libusb_context *ctx,
			     struct pollfd *fds, nfds_t nfds, int num_ready);
	void (*free_transfer)(struct usbi_transfer *itransfer);

	int (*clock_gettime)(int clkid, struct timespec *tp);

	size_t device_priv_size;
	size_t device_handle_priv_size;
	size_t transfer_priv_size;
	/* FIXME: linux can't use this any more. if other OS's cannot either,
	 * then remove this */
	size_t add_iso_packet_size;
};

extern const struct usbi_os_backend * const usbi_backend;

extern const struct usbi_os_backend linux_usbfs_backend;
extern const struct usbi_os_backend win32_backend;

#endif /* __USB_INT_H_INCLUDE__ */
