#include <target/bulk.h>

struct bulk_block_writer {
	bulk_size_t min_block;
	bulk_size_t max_block;
	bulk_size_t adjust;
	bulk_xmit_cb write;
	bulk_space_cb space;
};

struct bulk_block_reader {
	bulk_size_t min_block;
	bulk_size_t max_block;
	bulk_size_t adjust;
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

#define BULK_ADJUST_RCAPS	0x04
#define BULK_ADJUST_RTUNED	0x08
#define BULK_ADJUST_WCAPS	0x40
#define BULK_ADJUST_WTUNED	0x80
#define BULK_ADJUST_WMASK	0xC0
#define BULK_ADJUST_RMASK	0x0C
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
bulk_cid_t bulk_last_chan = 0;

#define bulk_is_type(bulk, type)					\
	((bulk_channels[bulk].flags & BULK_TYPE_MASK) == type)

/* Writer is offloadable & reader is adjustable */
#define bulk_is_adjust_read(bulk)					\
	(bulk_channels[bulk].w.b.space &&				\
	 (bulk_channels[bulk].flags & BULK_ADJUST_RCAPS))
#define bulk_is_tuning_read(bulk)					\
	(bulk_channels[bulk].w.b.space &&				\
	 (bulk_channels[bulk].flags & BULK_ADJUST_RMASK) == BULK_ADJUST_RCAPS)
/* Reader is offloadable & writer is adjustable */
#define bulk_is_adjust_write(bulk)					\
	(bulk_channels[bulk].r.b.space &&				\
	 (bulk_channels[bulk].flags & BULK_ADJUST_WCAPS))
#define bulk_is_tuning_write(bulk)					\
	(bulk_channels[bulk].r.b.space &&				\
	 (bulk_channels[bulk].flags & BULK_ADJUST_WMASK) == BULK_ADJUST_WCAPS)

#define bulk_set_tuned_read(bulk, radjust)				\
	do {								\
		bulk_channels[bulk].flags |= BULK_ADJUST_RTUNED;	\
		bulk_channels[bulk].r.b.adjust = radjust;		\
	} while (0)
#define bulk_clear_tuned_read(bulk)					\
	do {								\
		bulk_channels[bulk].flags &= ~BULK_ADJUST_RTUNED;	\
	} while (0)
#define bulk_set_tuned_write(bulk, wadjust)				\
	do {								\
		bulk_channels[bulk].flags |= BULK_ADJUST_WTUNED;	\
		bulk_channels[bulk].w.b.adjust = wadjust;		\
	} while (0)
#define bulk_clear_tuned_write(bulk)					\
	do {								\
		bulk_channels[bulk].flags &= ~BULK_ADJUST_WTUNED;	\
	} while (0)

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

bulk_cid_t bulk_init(uint8_t *buffer, bulk_size_t length)
{
	bulk_cid_t bulk;
	BUG_ON(bulk_last_chan >= NR_BULK_CHANS);
	bulk = bulk_last_chan;
	bulk_last_chan++;
	bulk_channels[bulk].length = length;
	INIT_CIRCBF_ASSIGN(&(bulk_channels[bulk].buffer), buffer);
	return bulk;
}

void bulk_reset(bulk_cid_t bulk, uint8_t type)
{
	INIT_CIRCBF_DECLARE(&(bulk_channels[bulk].buffer));
	bulk_channels[bulk].flags = type;
}

void bulk_dma_read_cpu(bulk_cid_t bulk,
		       bulk_size_t min_block, bulk_size_t max_block)
{
	BUG_ON(!bulk_is_type(bulk, BULK_TYPE_DMA));
	BUG_ON(bulk_channels[bulk].r.b.space != bulk_def_space);
	/* Block size 'min_block' must be a power of two. */
	BUG_ON((min_block & ((1<<__fls16(min_block))-1)) != 0);
	bulk_channels[bulk].r.b.min_block = min_block;
	bulk_channels[bulk].r.b.max_block = min(max_block, bulk_channels[bulk].length>>1);
	bulk_channels[bulk].flags |= BULK_ADJUST_RCAPS;
	if (min_block == max_block)
		bulk_set_tuned_read(bulk, min_block);
	else
		bulk_clear_tuned_read(bulk);
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
	if (!space)
		bulk_dma_read_cpu(bulk, flush, flush);
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

void bulk_dma_write_cpu(bulk_cid_t bulk,
			bulk_size_t min_block, bulk_size_t max_block)
{
	BUG_ON(!bulk_is_type(bulk, BULK_TYPE_DMA));
	BUG_ON(bulk_channels[bulk].w.b.space != bulk_def_space);
	/* Block size 'min_block' must be a power of two. */
	BUG_ON((min_block & ((1<<__fls16(min_block))-1)) != 0);
	bulk_channels[bulk].w.b.min_block = min_block;
	bulk_channels[bulk].w.b.max_block = min(max_block, bulk_channels[bulk].length>>1);
	bulk_channels[bulk].flags |= BULK_ADJUST_WCAPS;
	if (min_block == max_block)
		bulk_set_tuned_write(bulk, min_block);
	else
		bulk_clear_tuned_write(bulk);
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
	if (!space)
		bulk_dma_write_cpu(bulk, flush, flush);
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

#define BULK_TUNING_NONE	0x00
#define BULK_TUNING_NEEDED	0x01
#define BULK_TUNING_KICKED	0x02

void bulk_dma_execute(bulk_cid_t bulk, size_t size)
{
	ASSIGN_CIRCBF16_REF(buffer, circbf, &bulk_channels[bulk].buffer);
	bulk_size_t length = bulk_channels[bulk].length;
	bulk_size_t rflush = bulk_channels[bulk].rflush;
	bulk_size_t wflush = bulk_channels[bulk].wflush;
	bulk_size_t radjust = bulk_channels[bulk].r.b.adjust;
	bulk_size_t wadjust = bulk_channels[bulk].w.b.adjust;
	boolean rtuning = bulk_is_tuning_read(bulk) ?
			  BULK_TUNING_NEEDED : BULK_TUNING_NONE;
	boolean wtuning = bulk_is_tuning_write(bulk) ?
			  BULK_TUNING_NEEDED : BULK_TUNING_NONE;
	size_t wsize = 0, rsize = 0;
	bulk_size_t wwait = 0, rwait = 0;

	BUG_ON(!bulk_is_type(bulk, BULK_TYPE_DMA));

	/* Initial tuning block size. */
	if (wtuning & BULK_TUNING_NEEDED)
		wadjust = bulk_channels[bulk].w.b.min_block;
	if (rtuning & BULK_TUNING_NEEDED)
		radjust = bulk_channels[bulk].r.b.min_block;
	if (bulk_is_adjust_read(bulk)) rflush = radjust;
	if (bulk_is_adjust_write(bulk)) wflush = wadjust;

	while ((wsize < size) || (rsize < size)) {
		if ((wsize < size) && bulk_channels[bulk].w.b.space()) {
			if (wwait) {
				/* XXX: Power of Tow Ensurance
				 * This can help to ensure wsize is a
				 * power of tow during tuning.
				 */
				if ((wtuning & BULK_TUNING_NEEDED) && wsize) {
					wadjust += wflush;
					if (wadjust >= bulk_channels[bulk].w.b.max_block) {
						wadjust = bulk_channels[bulk].w.b.max_block;
						bulk_set_tuned_write(bulk, wadjust);
						wtuning = BULK_TUNING_NONE;
					}
					wflush = wadjust;
				}
				if (rtuning & BULK_TUNING_KICKED) {
					bulk_set_tuned_read(bulk, radjust);
					rflush = radjust;
					rtuning = BULK_TUNING_NONE;
				}
				circbf_write(circbf, length, wwait);
				wsize += wwait;
				wwait = 0;
			}
			if (circbf_space(circbf, length) >= wflush) {
				wwait = (bulk_size_t)min((size_t)wflush, size-wsize);
				bulk_channels[bulk].w.b.write(circbf_wpos(circbf), wwait);
				if (wtuning & BULK_TUNING_NEEDED)
					wtuning |= BULK_TUNING_KICKED;
			} else {
				wtuning &= ~BULK_TUNING_KICKED;
			}
		}
		if ((rsize < size) && bulk_channels[bulk].r.b.space()) {
			if (rwait) {
				/* XXX: Power of Tow Ensurance
				 * This can help to ensure wsize is a
				 * power of tow during tuning.
				 */
				if ((rtuning & BULK_TUNING_NEEDED) && rsize) {
					radjust += rflush;
					if (radjust >= bulk_channels[bulk].r.b.max_block) {
						radjust = bulk_channels[bulk].r.b.max_block;
						bulk_set_tuned_read(bulk, radjust);
						rtuning = BULK_TUNING_NONE;
					}
					rflush = radjust;
				}
				if (wtuning & BULK_TUNING_KICKED) {
					bulk_set_tuned_write(bulk, wadjust);
					wflush = wadjust;
					wtuning = BULK_TUNING_NONE;
				}
				circbf_read(circbf, length, rwait);
				rsize += rwait;
				rwait = 0;
			}
			if (circbf_count(circbf, length) >= rflush) {
				rwait = (bulk_size_t)min((size_t)rflush, size-rsize);
				bulk_channels[bulk].r.b.read(circbf_rpos(circbf), rwait);
				if (rtuning & BULK_TUNING_NEEDED)
					rtuning |= BULK_TUNING_KICKED;
			} else {
				rtuning &= ~BULK_TUNING_KICKED;
			}
		}
	}

	/* Wait for the completion of both sides. */
	while (!bulk_channels[bulk].w.b.space() || !bulk_channels[bulk].r.b.space());
}

bulk_size_t bulk_dma_read_size(bulk_cid_t bulk)
{
	if (bulk_is_adjust_read(bulk))
		return bulk_channels[bulk].r.b.adjust;
	return bulk_channels[bulk].rflush;
}

bulk_size_t bulk_dma_write_size(bulk_cid_t bulk)
{
	if (bulk_is_adjust_write(bulk))
		return bulk_channels[bulk].w.b.adjust;
	return bulk_channels[bulk].wflush;
}
