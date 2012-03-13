#include <target/bulk.h>
#include <target/state.h>
#include <target/timer.h>

#define BULK_POLL_INTERVAL	10

struct bulk {
	bulk_size_t length;
	ASSIGN_CIRCBF16_MEMBER(buffer);
	bulk_size_t flush;

	uint8_t flags;
#define BULK_FLAG_MASK		0x0F
#define BULK_FLAG_SOFT		0x01
#define BULK_FLAG_HARD		0x02
#define BULK_FLAG_ASYNC		0x04	/* asynchronous TX/RXAVAL events occured */
#define BULK_FLAG_SYNC		0x08	/* synchronous TX/RXAVAL events occured */
#define BULK_FLAG_HALT		0x10
#define BULK_FLAG_BUFFER	0x20

	size_t req_all;
	size_t req_cur;
	size_t xfr_all;
	size_t xfr_cur;
	size_t iter;
};

#define bulk_channel_set_soft()			\
	raise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SOFT)
#define bulk_channel_clear_soft()		\
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SOFT)
#define bulk_channel_pending()			\
	bits_raised(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SOFT)
#define bulk_channel_set_hard()			\
	raise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HARD)
#define bulk_channel_clear_hard()		\
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HARD)
#define bulk_channel_running()			\
	bits_raised(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HARD)
#define __bulk_channel_set_async()		\
	raise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_ASYNC)
#define __bulk_channel_clear_async()		\
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_ASYNC)
#define bulk_channel_asyncing()			\
	bits_raised(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_ASYNC)
#define bulk_channel_set_sync()			\
	raise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SYNC)
#define bulk_channel_clear_sync()		\
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SYNC)
#define __bulk_channel_syncing()			\
	bits_raised(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SYNC)
#define bulk_channel_halting()			\
	bits_raised(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HALT)
#define bulk_channel_halt()			\
	raise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HALT)
#define bulk_channel_unhalt()		\
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HALT)

#define bulk_channel_handled()			\
	(bulk_chan_ctrls[bulk_cid].req_cur)
#define bulk_channel_unhandled()		\
	(bulk_chan_ctrls[bulk_cid].req_all -	\
	 bulk_chan_ctrls[bulk_cid].req_cur)
#define bulk_transfer_handled()			\
	(bulk_chan_ctrls[bulk_cid].xfr_cur)
#define bulk_transfer_unhandled()		\
	(bulk_chan_ctrls[bulk_cid].xfr_all -	\
	 bulk_transfer_handled())

struct bulk bulk_chan_ctrls[NR_BULK_CHANS];
DECLARE_BITMAP(bulk_chan_regs, NR_BULK_CHANS);

bulk_user_t *bulk_users[NR_BULK_CHANS];
bulk_channel_t *bulk_channels[NR_BULK_CHANS];
bulk_cid_t bulk_nr_chans = 0;
bulk_cid_t bulk_cid;

sid_t bulk_sid = INVALID_SID;
tid_t bulk_tid = INVALID_TID;

void bulk_channel_set_async(void);
void bulk_channel_clear_async(void);

static void bulk_iter_reset(void);
static void bulk_iter_accel(void);
static void bulk_transfer_reset(void);
static void bulk_channel_reset(void);
static void bulk_transfer_limit(void);
static void bulk_channel_limit(size_t bytes);

static uint8_t bulk_channel_size(void);
static uint8_t bulk_channel_dir(void);

static void bulk_channel_poll(void);
static void bulk_channel_reap(void);
static void bulk_channel_open(void);
static void bulk_channel_close(void);
static void __bulk_channel_init(void);
static void __bulk_channel_exit(void);
#ifdef CONFIG_BULK_ASYNC_WRITE
static void bulk_channel_begin(void);
#else
#define bulk_channel_begin()
#endif
static void bulk_channel_end(void);

static void bulk_hw_write_byte(uint8_t c);
static uint8_t bulk_hw_read_byte(void);
static void bulk_hw_channel_start(void);
static void bulk_hw_channel_stop(void);
static void bulk_hw_channel_open(void);
static void bulk_hw_channel_close(void);

static inline void __bulk_alloc_fifo(bulk_cid_t bulk,
				     uint8_t *buffer,
				     bulk_size_t length)
{
	bulk_chan_ctrls[bulk].length = length;
	INIT_CIRCBF_ASSIGN(&(bulk_chan_ctrls[bulk].buffer), buffer);
}

static inline void __bulk_free_fifo(bulk_cid_t bulk)
{
	bulk_chan_ctrls[bulk].length = 0;
	INIT_CIRCBF_ASSIGN(&(bulk_chan_ctrls[bulk].buffer), NULL);
}

void bulk_reset_fifo(bulk_cid_t bulk)
{
	INIT_CIRCBF_DECLARE(&(bulk_chan_ctrls[bulk].buffer));
}

bulk_cid_t bulk_register_channel(bulk_channel_t *chan)
{
	bulk_cid_t bulk = bulk_nr_chans;

	BUG_ON(bulk_nr_chans >= NR_BULK_CHANS);
	bulk_channels[bulk] = chan;
	set_bit(bulk, bulk_chan_regs);
	bulk_nr_chans++;

	return bulk;
}

boolean bulk_open_channel(bulk_cid_t bulk,
			  bulk_user_t *user,
			  uint8_t *buffer,
			  bulk_size_t length,
			  bulk_size_t threshold)
{
	bulk_channel_t *chan;

	BUG_ON(bulk >= bulk_nr_chans);
	BUG_ON(bulk_users[bulk]);
	chan = bulk_channels[bulk];

	bulk_users[bulk] = user;
	bulk_chan_ctrls[bulk].flush = threshold;
	if (buffer) {
		__bulk_alloc_fifo(bulk, buffer, length);
	}

	return true;
}

void bulk_close_channel(bulk_cid_t bulk)
{
	BUG_ON(bulk >= bulk_nr_chans);
	BUG_ON(!bulk_users[bulk]);
	__bulk_free_fifo(bulk);
	bulk_users[bulk] = NULL;
}

void bulk_restore_channel(bulk_cid_t bulk)
{
	bulk_cid = bulk;
}

bulk_cid_t bulk_save_channel(bulk_cid_t bulk)
{
	bulk_cid_t sbulk = bulk_cid;
	bulk_restore_channel(bulk);
	return sbulk;
}

boolean bulk_channel_syncing(void)
{
	return __bulk_channel_syncing();
}

static void bulk_channel_set_async(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->stop);
	__bulk_channel_set_async();
	chan->stop();
	state_wakeup(bulk_sid);
}

static void bulk_channel_clear_async(void)
{
	bulk_channel_t *chan;

	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->start);
	__bulk_channel_clear_async();
	chan->start();
}

static void bulk_hw_channel_open(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->open);
	chan->open();
}

static void bulk_hw_channel_close(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->close);
	chan->close();
}

static void bulk_hw_channel_start(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->start);
	chan->start();
}

static void bulk_hw_channel_stop(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->stop);
	chan->stop();
}

static uint8_t bulk_hw_channel_getchar(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->getchar);
	return chan->getchar();
}

static void bulk_hw_channel_putchar(uint8_t c)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->putchar);
	chan->putchar(c);
}

static uint8_t bulk_channel_dir(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan ||
	       ((chan->flags & O_RDWR) != O_RDONLY &&
	        (chan->flags & O_RDWR) != O_WRONLY));
	return chan->flags & O_RDWR;
}

static uint8_t bulk_channel_size(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan);
	return chan->threshold;
}

static void bulk_handle_halt(void)
{
	bulk_user_t *user;
	
	user = bulk_users[bulk_cid];
	BUG_ON(!user || !user->halt);
	user->halt();
}

static void bulk_handle_poll(void)
{
	bulk_user_t *user;
	
	user = bulk_users[bulk_cid];
	BUG_ON(!user || !user->poll);
	user->poll();
}

static void bulk_handle_data(void)
{
	bulk_user_t *user;
	
	user = bulk_users[bulk_cid];
	BUG_ON(!user || !user->iocb);
	user->iocb();
}

static void bulk_handle_done(void)
{
	bulk_user_t *user;
	
	user = bulk_users[bulk_cid];
	BUG_ON(!user || !user->done);
	user->done();
}

static void bulk_iter_reset(void)
{
	bulk_chan_ctrls[bulk_cid].iter = 0;
}

static void bulk_iter_accel(void)
{
	bulk_chan_ctrls[bulk_cid].iter =
	bulk_chan_ctrls[bulk_cid].req_cur;
}

boolean bulk_transfer_last(void)
{
	return bulk_channel_unhandled() == 0;
}

void bulk_transfer_submit(size_t bytes)
{
	bulk_chan_ctrls[bulk_cid].xfr_all = bytes;
}

static void bulk_channel_reap(void)
{
	uint8_t reap = 0;

	if (bulk_channel_dir() != O_RDONLY)
		return;
#if 0
	while (bulk_transfer_unhandled() > 0) {
		BULK_READB(reap);
	}
#endif
}

static void bulk_transfer_reset(void)
{
	bulk_chan_ctrls[bulk_cid].xfr_cur = 0;
	bulk_transfer_limit();
	bulk_hw_channel_open();
}

static void bulk_channel_reset(void)
{
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_MASK);
	bulk_chan_ctrls[bulk_cid].req_all = 0;
	bulk_chan_ctrls[bulk_cid].req_cur = 0;
	bulk_chan_ctrls[bulk_cid].xfr_all = 0;
	bulk_chan_ctrls[bulk_cid].xfr_cur = 0;
	bulk_channel_poll();
}

static void bulk_transfer_limit(void)
{
	if (bulk_channel_dir() == O_WRONLY) {
		bulk_chan_ctrls[bulk_cid].xfr_all = bulk_channel_size();
		if ((size_t)(bulk_chan_ctrls[bulk_cid].xfr_all) >
		    (size_t)(bulk_channel_unhandled() - bulk_transfer_handled())) {
			bulk_chan_ctrls[bulk_cid].xfr_all =
				(size_t)(bulk_channel_unhandled() -
					 bulk_transfer_handled());
		}
	}
}

static void bulk_channel_limit(size_t bytes)
{
	bulk_chan_ctrls[bulk_cid].req_all = bytes;
	bulk_transfer_limit();
}

static void bulk_channel_open(void)
{
	bulk_hw_channel_start();
	bulk_channel_begin();
}

static void bulk_channel_close(void)
{
	if (bulk_channel_running()) {
		bulk_hw_channel_stop();
		bulk_channel_clear_hard();
		if (bulk_channel_pending()) {
			bulk_handle_done();
			bulk_channel_clear_soft();
		}
	}
}

#ifdef CONFIG_BULK_ASYNC_WRITE
static void bulk_channel_begin(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk];
	if (bulk_channel_dir() == O_WRONLY) {
		bulk_channel_set_async();
	}
}
#endif

static void bulk_channel_end(void)
{
	if (bulk_channel_halting()) {
		bulk_channel_set_async();
		return;
	}
	if (bulk_channel_syncing())
		bulk_transfer_reset();
	if (!bulk_channel_asyncing()) {
		bulk_hw_channel_close();
	}
	if (bulk_transfer_last()) {
		if (bulk_channel_dir() == O_RDONLY ||
		    bulk_channel_syncing()) {
			bulk_channel_set_async();
		}
	}
}

static void __bulk_channel_init(void)
{
	bulk_channel_set_hard();
}

static void __bulk_channel_exit(void)
{
	bulk_channel_close();
	/* XXX: Automatically FIFO Reset by Hardware
	 *
	 * Do not call usbd_hw_request_reset() here as most of the
	 * controllers must have internally reset the data toggle bit by
	 * itself to achieve the maximum throughput for error-free data
	 * requests.
	 */
	bulk_channel_reset();
}

static void __bulk_transfer_data(void)
{
	if (bulk_channel_halting()) {
		bulk_handle_halt();
		return;
	}
	bulk_iter_reset();
	bulk_transfer_reset();
	BUG_ON(bulk_chan_ctrls[bulk_cid].req_cur >
	       bulk_chan_ctrls[bulk_cid].req_all);
	bulk_handle_data();
	bulk_channel_reap();
	bulk_channel_end();
}

void bulk_transfer_iocb(void)
{
	if (!bulk_channel_running()) {
		__bulk_channel_init();
		__bulk_transfer_data();
	} else if (bulk_channel_halting() || bulk_transfer_last()) {
		__bulk_channel_exit();
	} else {
		__bulk_transfer_data();
	}
}

void bulk_writeb_sync(uint8_t byte)
{
	if (bulk_transfer_handled() == bulk_channel_size())
		bulk_transfer_reset();
	if (bulk_channel_unhandled() > 0 &&
	    bulk_chan_ctrls[bulk_cid].req_cur ==
	    bulk_chan_ctrls[bulk_cid].iter) {
		bulk_hw_write_byte(byte);
		bulk_chan_ctrls[bulk_cid].xfr_cur++;
		bulk_chan_ctrls[bulk_cid].req_cur++;
		if (bulk_transfer_handled() == bulk_channel_size()) {
			bulk_hw_channel_close();
		}
	}
	bulk_chan_ctrls[bulk_cid].iter++;
}

uint8_t bulk_readb_sync(void)
{
	uint8_t newval = 0;

	if (bulk_transfer_handled() == bulk_channel_size())
		bulk_transfer_reset();
	if (bulk_channel_unhandled() > 0 &&
	    bulk_chan_ctrls[bulk_cid].req_cur ==
	    bulk_chan_ctrls[bulk_cid].iter) {
		newval = bulk_hw_read_byte();
		bulk_chan_ctrls[bulk_cid].xfr_cur++;
		bulk_chan_ctrls[bulk_cid].req_cur++;
		if (bulk_transfer_handled() == bulk_channel_size()) {
			bulk_hw_channel_close();
		}
	}
	bulk_chan_ctrls[bulk_cid].iter++;

	return newval;
}

void bulk_writeb_async(uint8_t byte)
{
	if (bulk_transfer_unhandled() > 0 &&
	    bulk_chan_ctrls[bulk_cid].req_cur ==
	    bulk_chan_ctrls[bulk_cid].iter) {
		bulk_hw_write_byte(byte);
		bulk_chan_ctrls[bulk_cid].xfr_cur++;
		bulk_chan_ctrls[bulk_cid].req_cur++;
		if (bulk_transfer_handled() == bulk_channel_size()) {
			bulk_hw_channel_close();
		}
	}
	bulk_chan_ctrls[bulk_cid].iter++;
}

uint8_t bulk_readb_async(uint8_t byte)
{
	uint8_t newval = byte;

	if (bulk_transfer_unhandled() > 0 &&
	    bulk_chan_ctrls[bulk_cid].req_cur ==
	    bulk_chan_ctrls[bulk_cid].iter) {
		newval = bulk_hw_read_byte();
		bulk_chan_ctrls[bulk_cid].xfr_cur++;
		bulk_chan_ctrls[bulk_cid].req_cur++;
		if (bulk_transfer_handled() == bulk_channel_size()) {
			bulk_hw_channel_close();
		}
	}
	bulk_chan_ctrls[bulk_cid].iter++;

	return newval;
}

void bulk_writeb(uint8_t byte)
{
	if (bulk_channel_syncing()) {
		bulk_writeb_sync(byte);
	} else {
		bulk_writeb_async(byte);
	}
}

void bulk_writew(uint16_t word)
{
	bulk_writeb(LOBYTE(word));
	bulk_writeb(HIBYTE(word));
}

void bulk_writel(uint32_t dword)
{
	bulk_writew(LOWORD(dword));
	bulk_writew(HIWORD(dword));
}

uint8_t bulk_readb(uint8_t byte)
{
	if (bulk_channel_syncing()) {
		return bulk_readb_sync();
	} else {
		return bulk_readb_async(byte);
	}
}

uint16_t bulk_readw(uint16_t word)
{
	uint8_t lo = LOBYTE(word);
	uint8_t hi = HIBYTE(word);
	lo = bulk_readb(lo);
	hi = bulk_readb(hi);
	return lo | ((uint16_t)hi << 8);
}

uint32_t bulk_readl(uint32_t dword)
{
	uint16_t lo = LOWORD(dword);
	uint16_t hi = HIWORD(dword);
	lo = bulk_readw(lo);
	hi = bulk_readw(hi);
	return lo | ((uint32_t)hi << 16);
}

void bulk_transfer_write(bulk_cid_t bulk)
{
	bulk_select_channel(bulk);
	bulk_transfer_iocb();
}

void bulk_transfer_read(bulk_cid_t bulk)
{
	bulk_select_channel(bulk);
	bulk_transfer_iocb();
}

boolean bulk_submit_channel(bulk_cid_t bulk, size_t length)
{
	uint8_t sbulk;
	boolean result = false;

	sbulk = bulk_save_channel(bulk);

	if (bulk_channel_pending() || bulk_channel_halting())
		goto end;

	bulk_channel_limit(length);

	bulk_channel_set_soft();
	bulk_channel_open();
	result = true;
end:
	bulk_restore_channel(sbulk);
	return result;
}

void bulk_commit_channel(size_t length)
{
	BUG_ON(!bulk_channel_pending());
	bulk_channel_limit(length);
}

static void bulk_hw_write_byte(uint8_t c)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf,
			    &bulk_chan_ctrls[bulk_cid].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk_cid].length;

	if (!circbf->buffer) {
		/* direct IO */
		bulk_hw_channel_putchar(c);
	} else {
		/* buffer IO */
		BUG_ON(circbf_space(circbf, length) == 0);
		*(circbf_wpos(circbf)) = c;
		circbf_write(circbf, length, 1);
	}
}

static uint8_t bulk_hw_read_byte(void)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf,
			    &bulk_chan_ctrls[bulk_cid].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk_cid].length;

	if (!circbf->buffer) {
		/* direct IO */
		return bulk_hw_channel_getchar();
	} else {
		/* buffer IO */
		uint8_t c;
		BUG_ON(circbf_count(circbf, length) == 0);
		c = *(circbf_rpos(circbf));
		circbf_read(circbf, length, 1);
		return c;
	}
}

void bulk_async_iocb(void)
{
	if (bulk_channel_asyncing()) {
		bulk_channel_clear_async();
		bulk_transfer_iocb();
	}
}

static void bulk_async_handler(void)
{
	bulk_cid_t bulk, sbulk;

	for (bulk = 0; bulk < bulk_nr_chans; bulk++) {
		sbulk = bulk_save_channel(bulk);
		if (bulk_users[bulk_cid]) {
			bulk_async_iocb();
		}
		bulk_restore_channel(sbulk);
	}
}

static void bulk_channel_poll(void)
{
	if (bulk_users[bulk_cid] &&
	    !bulk_channel_pending()) {
		bulk_handle_poll();
	}
}

static void bulk_timer_handler(void)
{
	if (timer_timeout_raised(bulk_tid, TIMER_DELAYABLE)) {
		bulk_cid_t bulk, sbulk;
		for (bulk = 0; bulk < bulk_nr_chans; bulk++) {
			sbulk = bulk_save_channel(bulk);
			bulk_channel_poll();
			bulk_restore_channel(sbulk);
		}
		timer_schedule_shot(bulk_tid, BULK_POLL_INTERVAL);
	}
}

static void bulk_handler(uint8_t event)
{
	switch (event) {
	case STATE_EVENT_WAKE:
		bulk_async_handler();
		break;
	case STATE_EVENT_SHOT:
		bulk_timer_handler();
		break;
	default:
		BUG();
		break;
	}
}

void bulk_init(void)
{
	bulk_sid = state_register(bulk_handler);
	bulk_tid = timer_register(bulk_sid, TIMER_DELAYABLE);
	timer_schedule_shot(bulk_tid, BULK_POLL_INTERVAL);
}
