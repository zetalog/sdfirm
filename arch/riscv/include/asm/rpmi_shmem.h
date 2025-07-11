#ifndef __MBOX_RPMI_SHMEM_H_INCLUDE__
#define __MBOX_RPMI_SHMEM_H_INCLUDE__

#include <target/spinlock.h>
#include <target/rpmi.h>
#include <target/irq.h>

#define RPMI_SHMEM_MAX_CONTROLLERS	CONFIG_RPMI_SHMEM_MAX_CONTROLLERS
#define RPMI_MAX_CHANNELS		16

enum rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

enum rpmi_mb_type {
	RPMI_MB_TYPE_AP = 0,
	RPMI_MB_TYPE_PuC = 1,
};

enum rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_COUNT,
};

struct rpmi_mb_regs {
	volatile le32_t db_reg;
};

struct rpmi_shmem {
	enum rpmi_mb_type mb_type;
	uint8_t queue_count;
	size_t slot_size;
	volatile le32_t *queue_base[RPMI_QUEUE_IDX_MAX_COUNT];
	size_t queue_size[RPMI_QUEUE_IDX_MAX_COUNT];
	/* TX doorbell */
	volatile le32_t *db_tx_reg;
	uint32_t db_tx_val;
	/* RX doorbell */
	irq_t db_irq;
	volatile le32_t *db_rx_reg;
	uint32_t db_rx_val;
};

/** Single Queue Context Structure */
struct smq_queue_ctx {
	uint32_t queue_id;
	uint32_t num_slots;
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	enum rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	volatile le32_t *headptr;
	volatile le32_t *tailptr;
	volatile uint8_t *buffer;
};

struct rpmi_srvgrp_chan {
	int sid;
	/* Channel arguments */
	uint32_t servicegroup_id;
	uint32_t servicegroup_version;
	struct mbox_chan chan;
};

struct rpmi_shmem_base_flags {
	uint8_t f0_priv_level;
	bool f0_ev_notif_en;
};

struct rpmi_shmem_mbox_controller {
	enum rpmi_mb_type type;
	irq_t irq;
	struct rpmi_mb_regs *regs;
	uint32_t val;
	/* Driver specific members */
	size_t slot_size;
	uint32_t queue_count;
	uint32_t doorbell_value;
	struct rpmi_mb_regs *mb_regs;
	struct smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_COUNT];
	/* Mailbox framework related members */
	struct mbox_controller controller;
	struct mbox_chan *base_chan;
	uint32_t impl_version;
	uint32_t impl_id;
	uint32_t spec_version;
	uint32_t plat_info_len;
	char *plat_info;
	struct rpmi_shmem_base_flags base_flags;
	struct rpmi_srvgrp_chan srvgrp_chans[RPMI_MAX_CHANNELS];
	uint32_t chan_used[RPMI_MAX_CHANNELS];
};

/**************** Shared Memory Queues Helpers **************/

extern struct rpmi_shmem_mbox_controller rpmi_shmem_boxes[RPMI_SHMEM_MAX_CONTROLLERS];
#if RPMI_SHMEM_MAX_CONTROLLERS > 1
extern mbox_bid_t rpmi_shmem_bid;
void rpmi_shmem_select(mbox_bid_t bid);
#else
#define rpmi_shmem_bid			0
#define rpmi_shmem_select(bid)		do { } while (0)
#endif

#define rpmi_shmem_type			rpmi_shmem_boxes[rpmi_shmem_bid].type
#define rpmi_shmem_irq			rpmi_shmem_boxes[rpmi_shmem_bid].irq
#define rpmi_shmem_slot_size		rpmi_shmem_boxes[rpmi_shmem_bid].slot_size
#define rpmi_shmem_doorbell_value	rpmi_shmem_boxes[rpmi_shmem_bid].doorbell_value
#define rpmi_shmem_mb_regs		rpmi_shmem_boxes[rpmi_shmem_bid].mb_regs
#define rpmi_shmem_queue_count		rpmi_shmem_boxes[rpmi_shmem_bid].queue_count
#define rpmi_shmem_queue_ctx_tbl	rpmi_shmem_boxes[rpmi_shmem_bid].queue_ctx_tbl
#define rpmi_shmem_controller		rpmi_shmem_boxes[rpmi_shmem_bid].controller
#define rpmi_shmem_base_chan		rpmi_shmem_boxes[rpmi_shmem_bid].base_chan
#define rpmi_shmem_impl_version		rpmi_shmem_boxes[rpmi_shmem_bid].impl_version
#define rpmi_shmem_impl_id		rpmi_shmem_boxes[rpmi_shmem_bid].impl_id
#define rpmi_shmem_spec_version		rpmi_shmem_boxes[rpmi_shmem_bid].spec_version
#define rpmi_shmem_plat_info_len	rpmi_shmem_boxes[rpmi_shmem_bid].plat_info_len
#define rpmi_shmem_plat_info		rpmi_shmem_boxes[rpmi_shmem_bid].plat_info
#define rpmi_shmem_srvgrp_chans		rpmi_shmem_boxes[rpmi_shmem_bid].srvgrp_chans
#define rpmi_shmem_chan_used		rpmi_shmem_boxes[rpmi_shmem_bid].chan_used

void rpmi_shmem_init(struct rpmi_shmem *config);
void rpmi_shmem_irq_init(void);
void rpmi_shmem_handle_irq(void);
struct mbox_chan *rpmi_shmem_request_channel(uint32_t *args);
void rpmi_shmem_free_channel(struct mbox_chan *chan);
int rpmi_shmem_transfer(struct mbox_chan *chan, struct mbox_xfer *xfer);
void rpmi_shmem_irq_handler(irq_t irq);

#endif /* __MBOX_RPMI_SHMEM_H_INCLUDE__ */
