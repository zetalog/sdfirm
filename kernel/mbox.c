#include <target/mbox.h>
#include <target/panic.h>
#include <target/atomic.h>

static LIST_HEAD(mbox_list);

struct mbox_controller *mbox_controller_find(unsigned int id)
{
	struct list_head *pos;

	list_for_each(pos, &mbox_list) {
		struct mbox_controller *mbox = to_mbox_controller(pos);

		if (mbox->id == id)
			return mbox;
	}

	return NULL;
}

void mbox_controller_add(struct mbox_controller *mbox)
{
	BUG_ON(!mbox || !mbox->max_xfer_len);
	BUG_ON(mbox_controller_find(mbox->id));

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
		if (mbox->free_chan)
			mbox->free_chan(mbox, chan);
		list_del(&chan->node);
	}
	list_del(&mbox->node);
}

struct mbox_chan *mbox_controller_request_chan(struct mbox_controller *mbox,
					       uint32_t *chan_args)
{
	struct mbox_chan *ret;
	struct list_head *pos;

	if (!chan_args || !mbox || !mbox->request_chan)
		return NULL;

	list_for_each(pos, &mbox->chan_list) {
		ret = to_mbox_chan(pos);
		if (!memory_cmp((caddr_t)ret->chan_args,
				(caddr_t)chan_args,
				sizeof(ret->chan_args)))
			return ret;
	}

	ret = mbox->request_chan(mbox, chan_args);
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
	if (!chan || !chan->mbox)
		return;

	if (chan->mbox->free_chan)
		chan->mbox->free_chan(chan->mbox, chan);
	list_del(&chan->node);
}

int mbox_chan_xfer(struct mbox_chan *chan, struct mbox_xfer *xfer)
{
	if (!xfer || !chan || !chan->mbox || !chan->mbox->xfer)
		return -EINVAL;

	if (xfer->tx && (xfer->tx_len > chan->mbox->max_xfer_len))
		return -EINVAL;

	if (xfer->rx && (xfer->rx_len > chan->mbox->max_xfer_len))
		return -EINVAL;

	if (!(xfer->flags & MBOX_XFER_SEQ))
		mbox_xfer_set_sequence(xfer,
			atomic_add_return(1, &chan->mbox->xfer_next_seq));

	return chan->mbox->xfer(chan, xfer);
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
