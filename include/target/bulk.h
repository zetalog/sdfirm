#ifndef __BULK_H_INCLUDE__
#define __BULK_H_INCLUDE__

#include <target/generic.h>

#ifdef CONFIG_BULK_MAX_CHANS
#define NR_BULK_CHANS	CONFIG_BULK_MAX_CHANS
#else
#define NR_BULK_CHANS	1
#endif

/* bulk channel ID */
typedef uint8_t bulk_cid_t;
typedef uint16_t bulk_size_t;

#define BULK_TYPE_DMA		0x00
#define BULK_TYPE_CPU		0x01
#define BULK_TYPE_MASK		0x01

typedef void (*bulk_open_cb)(size_t);
typedef void (*bulk_close_cb)(bulk_size_t);
typedef uint8_t (*bulk_read_cb)(void);
typedef void (*bulk_write_cb)(uint8_t);
typedef void (*bulk_xmit_cb)(uint8_t *, bulk_size_t);
typedef boolean (*bulk_space_cb)(void);

bulk_cid_t bulk_set_buffer(uint8_t *buffer, bulk_size_t length);
void bulk_clear_buffer(bulk_cid_t bulk);
void bulk_flush_buffer(bulk_cid_t bulk, uint8_t type);

bulk_size_t bulk_write_buffer(bulk_cid_t bulk,
			      const uint8_t *buf, bulk_size_t len);
bulk_size_t bulk_write_byte(bulk_cid_t bulk, uint8_t c);

/* Both sides are CPU transfer */
void bulk_cpu_read(bulk_cid_t bulk, bulk_size_t flush,
		   bulk_open_cb open,
		   bulk_read_cb read,
		   bulk_close_cb close);
void bulk_cpu_write(bulk_cid_t bulk, bulk_size_t flush,
		    bulk_open_cb open,
		    bulk_write_cb write,
		    bulk_close_cb close);
void bulk_cpu_execute(uint8_t bulk, size_t size);

/* Either side is DMA transfer */
void bulk_dma_read(bulk_cid_t bulk, bulk_size_t flush,
		   bulk_xmit_cb read, bulk_space_cb space);
void bulk_dma_write(bulk_cid_t bulk, bulk_size_t flush,
		    bulk_xmit_cb write, bulk_space_cb space);
void bulk_dma_execute(bulk_cid_t bulk, size_t size);

#endif /* __BULK_H_INCLUDE__ */
