#include <target/bulk.h>
#include <target/state.h>

struct bulk_block_writer {
	bulk_xmit_cb write;
	bulk_space_cb space;
};

struct bulk_block_reader {
	bulk_xmit_cb read;
	bulk_space_cb space;
};

struct bulk_byte_writer {
	bulk_open_cb open;
	bulk_write_cb write;
	bulk_close_cb close;
};

struct bulk_byte_reader {
	bulk_open_cb open;
	bulk_read_cb read;
	bulk_close_cb close;
};

struct bulk {
	bulk_size_t length;
	ASSIGN_CIRCBF16_MEMBER(buffer);

	bulk_size_t rflush;
	bulk_size_t wflush;

	union bulk_reader {
		struct bulk_block_reader b;
		struct bulk_byte_reader c;
	} r;
	union bulk_writer {
		struct bulk_block_writer b;
		struct bulk_byte_writer c;
	} w;
};

struct bulk bulk_chan_ctrls[NR_BULK_CHANS];
bulk_channel_t *bulk_channels[NR_BULK_CHANS];
bulk_cid_t bulk_nr_chans = 0;
DECLARE_BITMAP(bulk_chan_users, NR_BULK_CHANS);
sid_t bulk_sid = INVALID_SID;

static void bulk_def_open(size_t size)
{
}

static void bulk_def_close(bulk_size_t size)
{
}

static boolean bulk_def_space(void)
{
	return true;
}

bulk_cid_t bulk_alloc_fifo(uint8_t *buffer, bulk_size_t length)
{
	bulk_cid_t bulk;

	bulk = find_first_clear_bit(bulk_chan_users, NR_BULK_CHANS);
	/* Wrong firmware configuration */
	BUG_ON(bulk >= NR_BULK_CHANS);
	bulk_chan_ctrls[bulk].length = length;
	INIT_CIRCBF_ASSIGN(&(bulk_chan_ctrls[bulk].buffer), buffer);
	set_bit(bulk, bulk_chan_users);

	return bulk;
}

void bulk_reset_fifo(bulk_cid_t bulk)
{
	INIT_CIRCBF_DECLARE(&(bulk_chan_ctrls[bulk].buffer));
}

void bulk_free_fifo(bulk_cid_t bulk)
{
	BUG_ON(bulk >= NR_BULK_CHANS);
	bulk_chan_ctrls[bulk].length = 0;
	INIT_CIRCBF_ASSIGN(&(bulk_chan_ctrls[bulk].buffer), NULL);
	clear_bit(bulk, bulk_chan_users);
}

bulk_size_t bulk_write_byte(bulk_cid_t bulk, uint8_t c)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf, &bulk_chan_ctrls[bulk].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk].length;
	bulk_size_t ret = 0;

	if (circbf_space(circbf, length) != 0) {
		*(circbf_wpos(circbf)) = c;
		circbf_write(circbf, length, 1);
		ret = 1;
	}

	return ret;
}

bulk_size_t bulk_write_buffer(bulk_cid_t bulk,
			      const uint8_t *buf, bulk_size_t len)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf, &bulk_chan_ctrls[bulk].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk].length;
	bulk_size_t ret = 0, c;

	while (1) {
		c = circbf_space_end(circbf, length);
		if (len < c)
			c = len;
		if (c <= 0)
			break;
		memory_copy((caddr_t)(circbf_wpos(circbf)),
			    (const caddr_t)buf, c);
		circbf_write(circbf, length, c);
		buf += c;
		len -= c;
		ret += c;
	}

	return ret;
}

void bulk_config_read_byte(bulk_cid_t bulk, bulk_size_t flush,
			   bulk_open_cb open,
			   bulk_read_cb read,
			   bulk_close_cb close)
{
	bulk_chan_ctrls[bulk].rflush = flush;
	bulk_chan_ctrls[bulk].r.c.read = read;
	bulk_chan_ctrls[bulk].r.c.open = open ? open : bulk_def_open;
	bulk_chan_ctrls[bulk].r.c.close = close ? close : bulk_def_close;
}

void bulk_config_write_byte(bulk_cid_t bulk, bulk_size_t flush,
			    bulk_open_cb open,
			    bulk_write_cb write,
			    bulk_close_cb close)
{
	bulk_chan_ctrls[bulk].wflush = flush;
	bulk_chan_ctrls[bulk].w.c.write = write;
	bulk_chan_ctrls[bulk].w.c.open = open ? open : bulk_def_open;
	bulk_chan_ctrls[bulk].w.c.close = close ? close : bulk_def_close;
}

void bulk_transfer_sync(bulk_cid_t bulk, size_t size)
{
	bulk_read_cb read_byte = bulk_chan_ctrls[bulk].r.c.read;
	bulk_write_cb write_byte = bulk_chan_ctrls[bulk].w.c.write;
	bulk_size_t rflush = bulk_chan_ctrls[bulk].rflush;
	bulk_size_t wflush = bulk_chan_ctrls[bulk].wflush;
	bulk_size_t witer, riter;
	size_t iter = 0;
	bulk_size_t wchunk, rchunk;

	while (size-iter) {
		if (rflush < wflush) {
			wchunk = (bulk_size_t)min((size_t)wflush, size-iter);
			bulk_chan_ctrls[bulk].w.c.open(iter);
			for (witer = 0; witer < wchunk; witer++) {
				rchunk = (bulk_size_t)min((size_t)rflush, size-iter);
				bulk_chan_ctrls[bulk].r.c.open(iter);
				for (riter = 0; riter < rchunk; riter++) {
					write_byte(read_byte());
				}
				bulk_chan_ctrls[bulk].r.c.close(riter);
				iter += rchunk;
			}
			bulk_chan_ctrls[bulk].w.c.close(witer);
		} else {
			bulk_chan_ctrls[bulk].r.c.open(iter);
			rchunk = (bulk_size_t)min((size_t)rflush, size-iter);
			for (riter = 0; riter < rchunk; riter++) {
				wchunk = (bulk_size_t)min((size_t)wflush, size-iter);
				bulk_chan_ctrls[bulk].w.c.open(iter);
				for (witer = 0; witer < wchunk; witer++) {
					write_byte(read_byte());
				}
				bulk_chan_ctrls[bulk].w.c.close(witer);
				iter += wchunk;
			}
			bulk_chan_ctrls[bulk].r.c.close(riter);
		}
	}
}

bulk_cid_t bulk_register_channel(bulk_channel_t *chan)
{
	bulk_cid_t bulk = bulk_nr_chans;

	BUG_ON(bulk_nr_chans >= NR_BULK_CHANS);
	bulk_channels[bulk] = chan;
	bulk_nr_chans++;

	return bulk;
}

void bulk_handle_write_byte(bulk_cid_t bulk,
			    bulk_write_cb write,
			    bulk_size_t count)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf, &bulk_chan_ctrls[bulk].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk].length;
	bulk_size_t wflush = bulk_chan_ctrls[bulk].wflush;

	if (count == 0)
		count = wflush;
#if 0
	bulk_chan_ctrls[bulk].write_cmpl();
	while ((circbf_space(circbf, length) >= wflush) &&
	       count > 0) {
		write(bulk_write_byte(circbf_wpos(circbf)));
		count--;
	}
	if (circbf_space(circbf, length) > 0)
		state_wakeup(bulk_sid);
/*		bulk_chan_ctrls[bulk].write_aval(); */
#endif
}

void bulk_handle_read_byte(bulk_cid_t bulk,
			   bulk_read_cb read,
			   bulk_size_t count)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf, &bulk_chan_ctrls[bulk].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk].length;
	bulk_size_t rflush = bulk_chan_ctrls[bulk].rflush;

	if (count == 0)
		count = rflush;
}

void bulk_handle_write_buffer(bulk_cid_t bulk,
			      bulk_xmit_cb write,
			      bulk_size_t count)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf, &bulk_chan_ctrls[bulk].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk].length;
	bulk_size_t wflush = bulk_chan_ctrls[bulk].wflush;

	if (count == 0)
		count = wflush;
}

void bulk_handle_read_buffer(bulk_cid_t bulk,
			     bulk_xmit_cb read,
			     bulk_size_t count)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf, &bulk_chan_ctrls[bulk].buffer);
	bulk_size_t length = bulk_chan_ctrls[bulk].length;
	bulk_size_t rflush = bulk_chan_ctrls[bulk].wflush;

	if (count == 0)
		count = rflush;
}

static void bulk_handler(uint8_t event)
{
	switch (event) {
	case STATE_EVENT_SHOT:
		break;
	case STATE_EVENT_WAKE:
		break;
	default:
		BUG();
		break;
	}
}

void bulk_init(void)
{
	bulk_sid = state_register(bulk_handler);
}
