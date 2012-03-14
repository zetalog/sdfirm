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
#define BULK_FLAG_HALT		0x08
#define BULK_FLAG_SYNC		0x10	/* synchronous TX/RXAVAL events occured */

	size_t req_all;
	size_t req_cur;
	bulk_size_t xfr_all;
	bulk_size_t xfr_cur;
	size_t iter;
};

#define bulk_request_set_soft()			\
	raise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SOFT)
#define bulk_request_clear_soft()		\
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SOFT)
#define bulk_request_pending()			\
	bits_raised(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SOFT)
#define bulk_request_set_hard()			\
	raise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HARD)
#define bulk_request_clear_hard()		\
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HARD)
#define bulk_request_running()			\
	bits_raised(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HARD)
#define __bulk_request_set_async()		\
	raise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_ASYNC)
#define __bulk_request_clear_async()		\
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_ASYNC)
#define bulk_request_asyncing()			\
	bits_raised(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_ASYNC)
#define __bulk_request_set_sync()		\
	raise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SYNC)
#define __bulk_request_clear_sync()		\
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SYNC)
#define __bulk_request_syncing()		\
	bits_raised(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_SYNC)
#define __bulk_channel_halting(bulk)		\
	bits_raised(bulk_chan_ctrls[bulk].flags, BULK_FLAG_HALT)
#define __bulk_request_set_halt()		\
	raise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HALT)
#define __bulk_request_clear_halt()		\
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_HALT)

#define __bulk_request_handled()		\
	(bulk_chan_ctrls[bulk_cid].req_cur)
#define __bulk_request_unhandled(cid)		\
	(bulk_chan_ctrls[cid].req_all -		\
	 bulk_chan_ctrls[cid].req_cur)
#define __bulk_transfer_handled()		\
	(bulk_chan_ctrls[bulk_cid].xfr_cur)
#define __bulk_transfer_unhandled()		\
	(bulk_chan_ctrls[bulk_cid].xfr_all -	\
	 __bulk_transfer_handled())

struct bulk bulk_chan_ctrls[NR_BULK_CHANS];
DECLARE_BITMAP(bulk_chan_regs, NR_BULK_CHANS);

bulk_user_t *bulk_users[NR_BULK_CHANS];
bulk_channel_t *bulk_channels[NR_BULK_CHANS];
bulk_cid_t bulk_nr_chans = 0;
bulk_cid_t bulk_cid;

sid_t bulk_sid = INVALID_SID;
tid_t bulk_tid = INVALID_TID;

static void bulk_iter_reset(void);
static void bulk_iter_accel(void);

static uint8_t bulk_channel_size(void);
static uint8_t bulk_channel_dir(void);

static void bulk_request_poll(void);
static void bulk_request_reap(void);
static void bulk_request_open(void);
static void bulk_request_close(void);
static void __bulk_request_init(void);
static void __bulk_request_exit(void);
#ifdef CONFIG_BULK_ASYNC_WRITE
static void bulk_request_begin(void);
#else
#define bulk_request_begin()
#endif
static void bulk_request_end(void);
static void bulk_request_reset(void);
static void bulk_request_limit(size_t bytes);
static void bulk_transfer_reset(void);
static void bulk_transfer_limit(void);

static void bulk_request_set_async(void);
static void bulk_request_clear_async(void);

static void bulk_hw_channel_open(void);
static void bulk_hw_channel_close(void);
static void bulk_hw_transmit_start(void);
static void bulk_hw_transmit_stop(void);
static boolean bulk_hw_poll_ready(void);
static void bulk_hw_transmit_byte(uint8_t *c);
static uint8_t bulk_hw_read_byte(void);
static void bulk_hw_write_byte(uint8_t c);
static void bulk_hw_channel_halt(void);
static void bulk_hw_channel_unhalt(void);

static void bulk_handle_request_done(void);
static void bulk_handle_request_iocb(void);
static void bulk_handle_request_poll(void);

size_t bulk_request_handled(void)
{
	return __bulk_request_handled();
}

size_t bulk_request_unhandled(bulk_cid_t bulk)
{
	return __bulk_request_unhandled(bulk);
}

bulk_size_t bulk_transfer_handled(void)
{
	return __bulk_transfer_handled();
}

bulk_size_t bulk_transfer_unhandled(void)
{
	return __bulk_transfer_unhandled();
}

void bulk_request_set_sync(void)
{
	__bulk_request_set_sync();
}

void bulk_request_clear_sync(void)
{
	__bulk_request_clear_sync();
}

void bulk_request_discard(void)
{
	if (bulk_request_pending())
		bulk_request_commit(bulk_chan_ctrls[bulk_cid].req_cur);
}

boolean bulk_channel_halting(bulk_cid_t bulk)
{
	return __bulk_channel_halting(bulk);
}

void bulk_channel_halt(bulk_cid_t bulk)
{
	bulk_cid_t sbulk;

	sbulk = bulk_save_channel(bulk);
	__bulk_request_set_halt();
	bulk_hw_channel_halt();
	if (bulk_request_pending())
		bulk_request_discard();
	bulk_reset_fifo(bulk_cid);
	bulk_restore_channel(sbulk);
}

void bulk_channel_unhalt(bulk_cid_t bulk, bulk_size_t resync_bytes)
{
	if (__bulk_channel_halting(bulk)) {
		ASSIGN_CIRCBF16_REF(buffer, circbf,
				    &bulk_chan_ctrls[bulk].buffer);
		bulk_size_t length = bulk_chan_ctrls[bulk].length;
		bulk_cid_t sbulk;

		sbulk = bulk_save_channel(bulk);
			if (bulk_channel_dir() == O_RDONLY)
				circbf_write(circbf, length, resync_bytes);
			bulk_hw_channel_unhalt();
			__bulk_request_clear_halt();
			if (bulk_request_pending())
				bulk_request_set_async();
		bulk_restore_channel(sbulk);
	}
}

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
	bulk_request_reset();

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

boolean bulk_request_syncing(void)
{
	return __bulk_request_syncing();
}

static void bulk_request_set_async(void)
{
	__bulk_request_set_async();
	bulk_hw_channel_close();
	state_wakeup(bulk_sid);
}

static void bulk_request_clear_async(void)
{
	__bulk_request_clear_async();
	bulk_hw_channel_open();
}

static void bulk_hw_channel_halt(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->halt);
	chan->halt();
}

static void bulk_hw_channel_unhalt(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->unhalt);
	chan->unhalt();
}

static void bulk_hw_transmit_start(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->start);
	chan->start();
}

static void bulk_hw_transmit_stop(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->stop);
	chan->stop();
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

static boolean bulk_hw_poll_ready(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->testpoll);

	return chan->testpoll();
}

static void bulk_hw_transmit_byte(uint8_t *c)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk_cid];
	BUG_ON(!chan || !chan->xmitbyte && !bulk_hw_poll_ready());
	chan->xmitbyte(c);
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

static void bulk_handle_request_poll(void)
{
	bulk_user_t *user;
	
	user = bulk_users[bulk_cid];
	BUG_ON(!user || !user->poll);
	user->poll();
}

static void bulk_handle_request_iocb(void)
{
	bulk_user_t *user;
	
	user = bulk_users[bulk_cid];
	BUG_ON(!user || !user->iocb);
	user->iocb();
}

static void bulk_handle_request_done(void)
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
	return __bulk_request_unhandled(bulk_cid) == 0 ||
	       __bulk_channel_halting(bulk_cid);
}

void bulk_transfer_submit(bulk_cid_t bulk, bulk_size_t bytes)
{
	bulk_chan_ctrls[bulk].xfr_all = bytes;
}

static void bulk_request_reap(void)
{
	uint8_t reap = 0;

	if (bulk_channel_dir() != O_RDONLY)
		return;
	while (__bulk_transfer_unhandled() > 0) {
		BULK_READB(reap);
	}
}

static void bulk_transfer_reset(void)
{
	bulk_chan_ctrls[bulk_cid].xfr_cur = 0;
	bulk_transfer_limit();
	bulk_hw_transmit_start();
}

static void bulk_request_reset(void)
{
	unraise_bits(bulk_chan_ctrls[bulk_cid].flags, BULK_FLAG_MASK);
	bulk_chan_ctrls[bulk_cid].req_all = 0;
	bulk_chan_ctrls[bulk_cid].req_cur = 0;
	bulk_chan_ctrls[bulk_cid].xfr_all = 0;
	bulk_chan_ctrls[bulk_cid].xfr_cur = 0;
	bulk_request_poll();
}

static void bulk_transfer_limit(void)
{
	if (bulk_channel_dir() == O_WRONLY) {
		bulk_chan_ctrls[bulk_cid].xfr_all = bulk_channel_size();
		if ((size_t)(bulk_chan_ctrls[bulk_cid].xfr_all) >
		    (size_t)(__bulk_request_unhandled(bulk_cid) -
			     __bulk_transfer_handled())) {
			bulk_chan_ctrls[bulk_cid].xfr_all =
				(bulk_size_t)(__bulk_request_unhandled(bulk_cid) -
					      __bulk_transfer_handled());
		}
	}
}

static void bulk_request_limit(size_t bytes)
{
	bulk_chan_ctrls[bulk_cid].req_all = bytes;
	bulk_transfer_limit();
}

static void bulk_request_open(void)
{
	bulk_hw_channel_open();
	bulk_request_begin();
}

static void bulk_request_close(void)
{
	if (bulk_request_pending()) {
		bulk_handle_request_done();
		bulk_request_clear_soft();
	}
}

#ifdef CONFIG_BULK_ASYNC_WRITE
static void bulk_request_begin(void)
{
	bulk_channel_t *chan;
	
	chan = bulk_channels[bulk];
	if (bulk_channel_dir() == O_WRONLY) {
		bulk_request_set_async();
	}
}
#endif

static void bulk_request_end(void)
{
	if (__bulk_channel_halting(bulk_cid)) {
		bulk_request_set_async();
		return;
	}
	if (bulk_request_syncing())
		bulk_transfer_reset();
	if (!bulk_request_asyncing()) {
		bulk_hw_transmit_stop();
	}
	if (bulk_transfer_last()) {
		if (bulk_channel_dir() == O_RDONLY ||
		    bulk_request_syncing()) {
			bulk_request_set_async();
		}
	}
}

static void __bulk_request_init(void)
{
	/* setup the request */
	bulk_request_set_hard();
}

static void __bulk_request_exit(void)
{
	/* teardown the request */
	BUG_ON(__bulk_channel_halting(bulk_cid));
	if (bulk_request_running())
		bulk_hw_channel_close();
	bulk_request_clear_hard();
	bulk_request_close();
	bulk_request_reset();
}

static void __bulk_transfer_data(void)
{
	if (__bulk_channel_halting(bulk_cid)) {
		return;
	}
	bulk_iter_reset();
	bulk_transfer_reset();
	BUG_ON(bulk_chan_ctrls[bulk_cid].req_cur >
	       bulk_chan_ctrls[bulk_cid].req_all);
	bulk_handle_request_iocb();
	bulk_request_reap();
	bulk_request_end();
}

void bulk_transfer_iocb(void)
{
	if (!bulk_request_running()) {
		__bulk_request_init();
		__bulk_transfer_data();
	} else if (bulk_transfer_last()) {
		__bulk_request_exit();
	} else {
		__bulk_transfer_data();
	}
}

void bulk_writeb_sync(uint8_t val)
{
	if (__bulk_transfer_handled() == bulk_channel_size())
		bulk_transfer_reset();
	if (__bulk_request_unhandled(bulk_cid) > 0 &&
	    bulk_chan_ctrls[bulk_cid].req_cur ==
	    bulk_chan_ctrls[bulk_cid].iter) {
		bulk_hw_write_byte(val);
		bulk_chan_ctrls[bulk_cid].xfr_cur++;
		bulk_chan_ctrls[bulk_cid].req_cur++;
		if (__bulk_transfer_handled() == bulk_channel_size()) {
			bulk_hw_transmit_stop();
		}
	}
	bulk_chan_ctrls[bulk_cid].iter++;
}

uint8_t bulk_readb_sync(void)
{
	uint8_t val = 0;

	if (__bulk_transfer_handled() == bulk_channel_size())
		bulk_transfer_reset();
	if (__bulk_request_unhandled(bulk_cid) > 0 &&
	    bulk_chan_ctrls[bulk_cid].req_cur ==
	    bulk_chan_ctrls[bulk_cid].iter) {
		val = bulk_hw_read_byte();
		bulk_chan_ctrls[bulk_cid].xfr_cur++;
		bulk_chan_ctrls[bulk_cid].req_cur++;
		if (__bulk_transfer_handled() == bulk_channel_size()) {
			bulk_hw_transmit_stop();
		}
	}
	bulk_chan_ctrls[bulk_cid].iter++;

	return val;
}

void bulk_writeb_async(uint8_t val)
{
	if (__bulk_transfer_unhandled() > 0 &&
	    bulk_chan_ctrls[bulk_cid].req_cur ==
	    bulk_chan_ctrls[bulk_cid].iter) {
		bulk_hw_write_byte(val);
		bulk_chan_ctrls[bulk_cid].xfr_cur++;
		bulk_chan_ctrls[bulk_cid].req_cur++;
		if (__bulk_transfer_handled() == bulk_channel_size()) {
			bulk_hw_transmit_stop();
		}
	}
	bulk_chan_ctrls[bulk_cid].iter++;
}

uint8_t bulk_readb_async(uint8_t val)
{
	uint8_t newval = val;

	if (__bulk_transfer_unhandled() > 0 &&
	    bulk_chan_ctrls[bulk_cid].req_cur ==
	    bulk_chan_ctrls[bulk_cid].iter) {
		newval = bulk_hw_read_byte();
		bulk_chan_ctrls[bulk_cid].xfr_cur++;
		bulk_chan_ctrls[bulk_cid].req_cur++;
		if (__bulk_transfer_handled() == bulk_channel_size()) {
			bulk_hw_transmit_stop();
		}
	}
	bulk_chan_ctrls[bulk_cid].iter++;

	return newval;
}

void bulk_writeb(uint8_t byte)
{
	if (bulk_request_syncing()) {
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
	if (bulk_request_syncing()) {
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

boolean bulk_request_submit(bulk_cid_t bulk, size_t length)
{
	uint8_t sbulk;
	boolean result = false;

	sbulk = bulk_save_channel(bulk);
	if (bulk_request_pending() || __bulk_channel_halting(bulk_cid))
		goto end;

	bulk_request_limit(length);
	bulk_request_set_soft();
	bulk_request_open();
	result = true;

end:
	bulk_restore_channel(sbulk);
	return result;
}

void bulk_request_commit(size_t length)
{
	BUG_ON(!bulk_request_pending());
	if (!__bulk_channel_halting(bulk_cid))
		bulk_request_limit(length);
}

static void bulk_hw_write_byte(uint8_t c)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf,
			    &bulk_chan_ctrls[bulk_cid].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk_cid].length;

	if (!circbf->buffer) {
		/* direct IO */
		BUG_ON(!bulk_hw_poll_ready());
		bulk_hw_transmit_byte(&c);
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
	uint8_t c;

	if (!circbf->buffer) {
		/* direct IO */
		BUG_ON(!bulk_hw_poll_ready());
		bulk_hw_transmit_byte(&c);
	} else {
		/* buffer IO */
		BUG_ON(circbf_count(circbf, length) == 1);
		c = *(circbf_rpos(circbf));
		circbf_read(circbf, length, 1);
	}

	return c;
}

static int bulk_putchar(uint8_t *c)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf,
			    &bulk_chan_ctrls[bulk_cid].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk_cid].length;
	int ret = 0;

	if (!circbf->buffer) {
		/* direct IO */
		if (bulk_hw_poll_ready()) {
			bulk_hw_transmit_byte(c);
			ret = 1;
		}
	} else {
		/* buffer IO */
		if (circbf_space(circbf, length) == 0) {
			*(circbf_wpos(circbf)) = *c;
			circbf_write(circbf, length, 1);
			ret = 1;
		}
	}

	return ret;
}

static int bulk_getchar(uint8_t *c)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf,
			    &bulk_chan_ctrls[bulk_cid].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk_cid].length;
	int ret = 0;

	if (!circbf->buffer) {
		/* direct IO */
		if (bulk_hw_poll_ready()) {
			bulk_hw_transmit_byte(c);
			ret = 1;
		}
	} else {
		/* buffer IO */
		if (circbf_count(circbf, length) > 0) {
			*c = *(circbf_rpos(circbf));
			circbf_read(circbf, length, 1);
			ret = 1;
		}
	}

	return ret;
}

int bulk_write(uint8_t *buf, size_t len)
{
}

int bulk_read(uint8_t *buf, size_t len)
{
}

void bulk_async_iocb(void)
{
	if (bulk_request_asyncing()) {
		bulk_request_clear_async();
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

static void bulk_request_poll(void)
{
	if (bulk_users[bulk_cid] &&
	    !bulk_request_pending()) {
		bulk_handle_request_poll();
	}
}

static void bulk_timer_handler(void)
{
	if (timer_timeout_raised(bulk_tid, TIMER_DELAYABLE)) {
		bulk_cid_t bulk, sbulk;
		for (bulk = 0; bulk < bulk_nr_chans; bulk++) {
			sbulk = bulk_save_channel(bulk);
			bulk_request_poll();
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
