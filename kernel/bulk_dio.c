#include <target/bulk.h>
#include <target/bitops.h>
#include <target/circbf.h>
#include <target/panic.h>
#include <target/muldiv.h>

struct bulk_block_writer {
	iobulk_cb write;
	iotest_cb space;
};

struct bulk_block_reader {
	iobulk_cb read;
	iotest_cb space;
};

struct bulk_byte_writer {
	bulk_open_cb open;
	iowr_cb write;
	bulk_close_cb close;
};

struct bulk_byte_reader {
	bulk_open_cb open;
	iord_cb read;
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
DECLARE_BITMAP(bulk_chan_regs, NR_BULK_CHANS);

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

bulk_cid_t bulk_alloc_fifo(uint8_t *buffer, bulk_size_t length)
{
	bulk_cid_t bulk;

	bulk = find_first_clear_bit(bulk_chan_regs, NR_BULK_CHANS);
	BUG_ON(bulk >= NR_BULK_CHANS);
	set_bit(bulk, bulk_chan_regs);
	__bulk_alloc_fifo(bulk, buffer, length);

	return bulk;
}

void bulk_reset_fifo(bulk_cid_t bulk)
{
	INIT_CIRCBF_DECLARE(&(bulk_chan_ctrls[bulk].buffer));
}

void bulk_free_fifo(bulk_cid_t bulk)
{
	BUG_ON(bulk >= NR_BULK_CHANS);
	__bulk_free_fifo(bulk);
	clear_bit(bulk, bulk_chan_regs);
}

static void bulk_def_open(size_t size)
{
}

static void bulk_def_close(bulk_size_t size)
{
}

void bulk_config_read_byte(bulk_cid_t bulk, bulk_size_t flush,
			   bulk_open_cb open,
			   iord_cb read,
			   bulk_close_cb close)
{
	bulk_chan_ctrls[bulk].rflush = flush;
	bulk_chan_ctrls[bulk].r.c.read = read;
	bulk_chan_ctrls[bulk].r.c.open = open ? open : bulk_def_open;
	bulk_chan_ctrls[bulk].r.c.close = close ? close : bulk_def_close;
}

void bulk_config_write_byte(bulk_cid_t bulk, bulk_size_t flush,
			    bulk_open_cb open,
			    iowr_cb write,
			    bulk_close_cb close)
{
	bulk_chan_ctrls[bulk].wflush = flush;
	bulk_chan_ctrls[bulk].w.c.write = write;
	bulk_chan_ctrls[bulk].w.c.open = open ? open : bulk_def_open;
	bulk_chan_ctrls[bulk].w.c.close = close ? close : bulk_def_close;
}

void bulk_transfer_sync(bulk_cid_t bulk, size_t size)
{
	iord_cb read_byte = bulk_chan_ctrls[bulk].r.c.read;
	iowr_cb write_byte = bulk_chan_ctrls[bulk].w.c.write;
	bulk_size_t rflush = bulk_chan_ctrls[bulk].rflush;
	bulk_size_t wflush = bulk_chan_ctrls[bulk].wflush;
	bulk_size_t witer, riter;
	size_t iter = 0;
	bulk_size_t wchunk, rchunk;

	while (size-iter) {
		if (rflush < wflush) {
			wchunk = (bulk_size_t)min((size_t)wflush,
						  size-iter);
			bulk_chan_ctrls[bulk].w.c.open(iter);
			for (witer = 0; witer < wchunk; witer++) {
				rchunk = (bulk_size_t)min((size_t)rflush,
							  size-iter);
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
			rchunk = (bulk_size_t)min((size_t)rflush,
						  size-iter);
			for (riter = 0; riter < rchunk; riter++) {
				wchunk = (bulk_size_t)min((size_t)wflush,
							  size-iter);
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

void bulk_init(void)
{
}
