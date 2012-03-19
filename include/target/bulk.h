#ifndef __BULK_H_INCLUDE__
#define __BULK_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>

#ifdef CONFIG_BULK_MAX_CHANS
#define NR_BULK_CHANS	CONFIG_BULK_MAX_CHANS
#else
#define NR_BULK_CHANS	1
#endif

/* bulk channel ID */
typedef uint8_t bulk_cid_t;
typedef uint16_t bulk_size_t;

typedef void (*bulk_open_cb)(size_t);
typedef void (*bulk_close_cb)(bulk_size_t);

struct bulk_channel {
	/* io flags defined in the io.h */
	uint8_t flags;
	bulk_size_t threshold;

	io_cb open;	/* enable hardware ant its IRQ/event */
	io_cb close;	/* disable hardware ant its IRQ/event */
	io_cb start;	/* before the transfer */
	io_cb stop;	/* after the transfer  */
	io_cb halt;	/* halt the channel */
	io_cb unhalt;	/* unhalt the channel */
	iotest_cb testpoll;
	iordwr_cb xmitbyte;
	iobulk_cb xmitbulk;
};
__TEXT_TYPE__(const struct bulk_channel, bulk_channel_t);

struct bulk_user {
	io_cb poll;
	io_cb iocb;
	io_cb done;
};
__TEXT_TYPE__(const struct bulk_user, bulk_user_t);

/*=========================================================================
 * bulk APIs
 *=======================================================================*/
bulk_cid_t bulk_register_channel(bulk_channel_t *chan);
boolean bulk_open_channel(bulk_cid_t bulk,
			  bulk_user_t *user,
			  uint8_t *buffer,
			  bulk_size_t length,
			  bulk_size_t threshold);
void bulk_close_channel(bulk_cid_t bulk);

void bulk_restore_channel(bulk_cid_t bulk);
bulk_cid_t bulk_save_channel(bulk_cid_t bulk);
#define bulk_select_channel(bulk)	bulk_restore_channel(bulk)

boolean bulk_request_syncing(void);
void bulk_request_set_sync(void);
void bulk_request_clear_sync(void);

boolean bulk_channel_halting(bulk_cid_t bulk);
void bulk_channel_halt(bulk_cid_t bulk);
void bulk_channel_unhalt(bulk_cid_t bulk, bulk_size_t resync_bytes);

boolean bulk_request_submit(size_t length);
void bulk_request_commit(size_t length);
void bulk_request_discard(void);

size_t bulk_request_handled(void);
size_t bulk_request_unhandled(bulk_cid_t bulk);
bulk_size_t bulk_transfer_handled(void);
bulk_size_t bulk_transfer_unhandled(void);

void bulk_transfer_write(bulk_cid_t bulk);
void bulk_transfer_read(bulk_cid_t bulk);
void bulk_transfer_submit(bulk_cid_t bulk, bulk_size_t bytes);

void bulk_writeb(uint8_t byte);
void bulk_writew(uint16_t word);
void bulk_writel(uint32_t dword);
uint8_t bulk_readb(uint8_t byte);
uint16_t bulk_readw(uint16_t word);
uint32_t bulk_readl(uint32_t dword);

#define BULK_WRITEB(byte)		(bulk_writeb(byte))
#define BULK_WRITEW(word)		(bulk_writew(word))
#define BULK_WRITEL(dword)		(bulk_writel(dword))
#define BULK_READB(byte)		(byte = bulk_readb(byte))
#define BULK_READW(word)		(word = bulk_readw(word))
#define BULK_READL(dword)		(dword = bulk_readl(dword))

#define BULK_READ_BEGIN(byte)					\
	{							\
		size_t __l = bulk_request_handled();		\
		BULK_READB(byte);				\
		if (bulk_request_handled() != __l)
#define BULK_READ_END						\
	}
#define BULK_WRITE_BEGIN(byte)					\
	{							\
		size_t __l = bulk_request_handled();		\
		BULK_WRITEB(byte);				\
		if (bulk_request_handled() != __l)
#define BULK_WRITE_END						\
	}

/* bulk APIs */
boolean bulk_transmit_submit(void);
void bulk_transmit_complete(void);
int bulk_putchar(uint8_t *c);
int bulk_getchar(uint8_t *c);
int bulk_write(uint8_t *buf, size_t len);
int bulk_read(uint8_t *buf, size_t len);

void bulk_reset_fifo(bulk_cid_t bulk);

/*=========================================================================
 * deprecated bulk APIs
 *=======================================================================*/
/* Both sides are CPU transfer */
void bulk_config_read_byte(bulk_cid_t bulk, bulk_size_t flush,
			   bulk_open_cb open,
			   iord_cb read,
			   bulk_close_cb close);
void bulk_config_write_byte(bulk_cid_t bulk, bulk_size_t flush,
			    bulk_open_cb open,
			    iowr_cb write,
			    bulk_close_cb close);
void bulk_transfer_sync(uint8_t bulk, size_t size);
bulk_cid_t bulk_alloc_fifo(uint8_t *buffer, bulk_size_t length);
void bulk_free_fifo(bulk_cid_t bulk);

#endif /* __BULK_H_INCLUDE__ */
