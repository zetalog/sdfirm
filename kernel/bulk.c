#include <target/bulk.h>

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

	uint8_t flags;

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

struct bulk bulk_channels[NR_BULK_CHANS];
DECLARE_BITMAP(bulk_chan_regs, NR_BULK_CHANS);

#define bulk_is_type(bulk, type)					\
	((bulk_channels[bulk].flags & BULK_TYPE_MASK) == type)

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

bulk_cid_t bulk_set_buffer(uint8_t *buffer, bulk_size_t length)
{
	bulk_cid_t bulk;

	bulk = find_first_clear_bit(bulk_chan_regs, NR_BULK_CHANS);
	/* Wrong firmware configuration */
	BUG_ON(bulk >= NR_BULK_CHANS);
	bulk_channels[bulk].length = length;
	INIT_CIRCBF_ASSIGN(&(bulk_channels[bulk].buffer), buffer);
	set_bit(bulk, bulk_chan_regs);

	return bulk;
}

void bulk_flush_buffer(bulk_cid_t bulk, uint8_t type)
{
	INIT_CIRCBF_DECLARE(&(bulk_channels[bulk].buffer));
	bulk_channels[bulk].flags = type;
}

void bulk_clear_buffer(bulk_cid_t bulk)
{
	BUG_ON(bulk >= NR_BULK_CHANS);
	bulk_channels[bulk].length = 0;
	INIT_CIRCBF_ASSIGN(&(bulk_channels[bulk].buffer), NULL);
	clear_bit(bulk, bulk_chan_regs);
}

bulk_size_t bulk_write_byte(bulk_cid_t bulk, uint8_t c)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf, &bulk_channels[bulk].buffer);
	bulk_size_t length = bulk_channels[bulk].length;
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
	ASSIGN_CIRCBF16_REF(buffer, circbf, &bulk_channels[bulk].buffer);
	bulk_size_t length = bulk_channels[bulk].length;
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

void bulk_dma_read(bulk_cid_t bulk, bulk_size_t flush,
		   bulk_xmit_cb read, bulk_space_cb space)
{
	BUG_ON(!bulk_is_type(bulk, BULK_TYPE_DMA));
	BUG_ON(flush > (bulk_channels[bulk].length>>1));
	/* Block size 'flush' must be a power of two. */
	BUG_ON((flush & ((1<<__fls16(flush))-1)) != 0);
	bulk_channels[bulk].rflush = flush;
	bulk_channels[bulk].r.b.read = read;
	bulk_channels[bulk].r.b.space = space ? space : bulk_def_space;
}

void bulk_cpu_read(bulk_cid_t bulk, bulk_size_t flush,
		   bulk_open_cb open,
		   bulk_read_cb read,
		   bulk_close_cb close)
{
	BUG_ON(!bulk_is_type(bulk, BULK_TYPE_CPU));
	bulk_channels[bulk].rflush = flush;
	bulk_channels[bulk].r.c.read = read;
	bulk_channels[bulk].r.c.open = open ? open : bulk_def_open;
	bulk_channels[bulk].r.c.close = close ? close : bulk_def_close;
}

void bulk_dma_write(bulk_cid_t bulk, bulk_size_t flush,
		    bulk_xmit_cb write, bulk_space_cb space)
{
	BUG_ON(!bulk_is_type(bulk, BULK_TYPE_DMA));
	BUG_ON(flush > (bulk_channels[bulk].length>>1));
	/* Block size 'flush' must be a power of two. */
	BUG_ON((flush & ((1<<__fls16(flush))-1)) != 0);
	bulk_channels[bulk].wflush = flush;
	bulk_channels[bulk].w.b.write = write;
	bulk_channels[bulk].w.b.space = space ? space : bulk_def_space;
}

void bulk_cpu_write(bulk_cid_t bulk, bulk_size_t flush,
		    bulk_open_cb open,
		    bulk_write_cb write,
		    bulk_close_cb close)
{
	BUG_ON(!bulk_is_type(bulk, BULK_TYPE_CPU));
	bulk_channels[bulk].wflush = flush;
	bulk_channels[bulk].w.c.write = write;
	bulk_channels[bulk].w.c.open = open ? open : bulk_def_open;
	bulk_channels[bulk].w.c.close = close ? close : bulk_def_close;
}

void bulk_cpu_execute(bulk_cid_t bulk, size_t size)
{
	bulk_read_cb read_byte = bulk_channels[bulk].r.c.read;
	bulk_write_cb write_byte = bulk_channels[bulk].w.c.write;
	bulk_size_t rflush = bulk_channels[bulk].rflush;
	bulk_size_t wflush = bulk_channels[bulk].wflush;
	bulk_size_t witer, riter;
	size_t iter = 0;
	bulk_size_t wchunk, rchunk;

	BUG_ON(!bulk_is_type(bulk, BULK_TYPE_CPU));

	while (size-iter) {
		if (rflush < wflush) {
			wchunk = (bulk_size_t)min((size_t)wflush, size-iter);
			bulk_channels[bulk].w.c.open(iter);
			for (witer = 0; witer < wchunk; witer++) {
				rchunk = (bulk_size_t)min((size_t)rflush, size-iter);
				bulk_channels[bulk].r.c.open(iter);
				for (riter = 0; riter < rchunk; riter++) {
					write_byte(read_byte());
				}
				bulk_channels[bulk].r.c.close(riter);
				iter += rchunk;
			}
			bulk_channels[bulk].w.c.close(witer);
		} else {
			bulk_channels[bulk].r.c.open(iter);
			rchunk = (bulk_size_t)min((size_t)rflush, size-iter);
			for (riter = 0; riter < rchunk; riter++) {
				wchunk = (bulk_size_t)min((size_t)wflush, size-iter);
				bulk_channels[bulk].w.c.open(iter);
				for (witer = 0; witer < wchunk; witer++) {
					write_byte(read_byte());
				}
				bulk_channels[bulk].w.c.close(witer);
				iter += wchunk;
			}
			bulk_channels[bulk].r.c.close(riter);
		}
	}
}

void bulk_dma_execute(bulk_cid_t bulk, size_t size)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf, &bulk_channels[bulk].buffer);
	bulk_size_t length = bulk_channels[bulk].length;
	bulk_size_t rflush = bulk_channels[bulk].rflush;
	bulk_size_t wflush = bulk_channels[bulk].wflush;
	size_t wsize = 0, rsize = 0;
	bulk_size_t wwait = 0, rwait = 0;

	BUG_ON(!bulk_is_type(bulk, BULK_TYPE_DMA));

	/* Initial tuning block size. */
	while ((wsize < size) || (rsize < size)) {
		if ((wsize < size) && bulk_channels[bulk].w.b.space()) {
			if (wwait) {
				circbf_write(circbf, length, wwait);
				wsize += wwait;
				wwait = 0;
			}
			if (circbf_space(circbf, length) >= wflush) {
				wwait = (bulk_size_t)min((size_t)wflush, size-wsize);
				bulk_channels[bulk].w.b.write(circbf_wpos(circbf), wwait);
			}
		}
		if ((rsize < size) && bulk_channels[bulk].r.b.space()) {
			if (rwait) {
				circbf_read(circbf, length, rwait);
				rsize += rwait;
				rwait = 0;
			}
			if (circbf_count(circbf, length) >= rflush) {
				rwait = (bulk_size_t)min((size_t)rflush, size-rsize);
				bulk_channels[bulk].r.b.read(circbf_rpos(circbf), rwait);
			}
		}
	}

	/* Wait for the completion of both sides. */
	while (!bulk_channels[bulk].w.b.space() || !bulk_channels[bulk].r.b.space());
}
