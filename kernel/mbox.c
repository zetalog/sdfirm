#include <target/mbox.h>
#include <target/panic.h>
#include <target/atomic.h>

#ifdef SYS_REALTIME
#define mbox_poll_init()	__mbox_poll_init()
#define mbox_irq_init()		do { } while (0)
#else /* SYS_REALTIME */
#define mbox_poll_init()	do { } while (0)
#define mbox_irq_init()		__mbox_irq_init()
#endif /* SYS_REALTIME */

bh_t mbox_bh;

#if NR_MAILBOXES > 1
struct mbox_controller mbox_controllers[NR_MAILBOXES];
mbox_bid_t mbox_bid;

void mbox_controller_select(mbox_bid_t bid)
{
	mbox_hw_ctrl_select(bid);
	mbox_bid = bid;
}

mbox_bid_t mbox_controller_save(mbox_bid_t bid)
{
	mbox_bid_t sbid = mbox_bid;
	mbox_controller_select(bid);
	return sbid;
}
#endif

static LIST_HEAD(mbox_list);

struct mbox_controller *mbox_controller_find(mbox_bid_t bid)
{
	struct list_head *pos;

	list_for_each(pos, &mbox_list) {
		struct mbox_controller *mbox = to_mbox_controller(pos);

		if (mbox->bid == bid)
			return mbox;
	}
	return NULL;
}

void mbox_controller_add(struct mbox_controller *mbox)
{
	BUG_ON(!mbox || !mbox->max_xfer_len);

	mbox->bid = mbox_bid;
	INIT_LIST_HEAD(&mbox->node);
	INIT_ATOMIC(&mbox->xfer_next_seq, 0);
	INIT_LIST_HEAD(&mbox->chan_list);
	list_add(&mbox->node, &mbox_list);
}

void mbox_controller_remove(struct mbox_controller *mbox)
{
	struct mbox_chan *chan;

	if (!mbox)
		return;

	while (!list_empty(&mbox->chan_list)) {
		chan = list_first_entry(&mbox->chan_list,
					struct mbox_chan, node);
		mbox_controller_free_chan(chan);
	}
	list_del(&mbox->node);
}

struct mbox_chan *mbox_controller_request_chan(struct mbox_controller *mbox,
					       uint32_t *chan_args)
{
	struct mbox_chan *ret;
	struct list_head *pos;

	if (!chan_args || !mbox)
		return NULL;

	list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		if (!memory_cmp((caddr_t)ret->chan_args,
				(caddr_t)chan_args,
				sizeof(ret->chan_args)))
			return ret;
	}

	ret = mbox_hw_request_channel(chan_args);
	if (!ret)
		return NULL;

	INIT_LIST_HEAD(&ret->node);
	ret->mbox = mbox;
	memory_copy((caddr_t)ret->chan_args,
		    (caddr_t)chan_args, sizeof(ret->chan_args));
	list_add(&ret->node, &mbox->chan_list);
	return ret;
}

void mbox_controller_free_chan(struct mbox_chan *chan)
{
	list_del(&chan->node);
	mbox_hw_free_channel(chan);
}

int mbox_chan_xfer(struct mbox_chan *chan, struct mbox_xfer *xfer)
{
	if (!xfer || !chan || !chan->mbox)
		return -EINVAL;

	if (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		return -EINVAL;

	if (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		return -EINVAL;

	if (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(1, &chan->mbox->xfer_next_seq));

	return mbox_hw_chan_xfer(chan, xfer);
}

void mbox_chan_get(struct mbox_chan *chan, int attr_id, void *out_value)
{
	if (!chan || !chan->mbox || !out_value)
		return;
	if (!chan->mbox->get)
		return;
	chan->mbox->get(chan, attr_id, out_value);
}

void mbox_chan_set(struct mbox_chan *chan, int attr_id, void *new_value)
{
	if (!chan || !chan->mbox || !new_value)
		return;
	if (!chan->mbox->set)
		return;
	chan->mbox->set(chan, attr_id, new_value);
}

static void mbox_handle_irq(void)
{
	mbox_bid_t bid;
	__unused mbox_bid_t sbid;

	for (bid = 0; bid < NR_MAILBOXES; bid++) {
		sbid = mbox_controller_save(bid);
		mbox_hw_handle_irq();
		mbox_controller_restore(sbid);
	}
}

static void mbox_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ)
		mbox_handle_irq();
}

#ifdef SYS_REALTIME
static void __mbox_poll_init(void)
{
	irq_register_poller(mbox_bh);
}
#else
static void __mbox_irq_init(void)
{
	mbox_hw_irq_init();
}
#endif

void mbox_init(void)
{
	mbox_bid_t bid;
	__unused mbox_bid_t sbid;

	mbox_bh = bh_register_handler(mbox_bh_handler);
	for (bid = 0; bid < NR_MAILBOXES; bid++) {
		sbid = mbox_controller_save(bid);
		mbox_hw_ctrl_init();
		mbox_irq_init();
		mbox_controller_restore(sbid);
	}
	mbox_poll_init();
}
