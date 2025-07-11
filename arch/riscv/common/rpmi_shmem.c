#include <target/generic.h>
#include <target/mbox.h>
#include <target/delay.h>
#include <target/irq.h>
#include <target/console.h>
#include <target/spinlock.h>
#include <target/panic.h>

#define to_srvgrp_chan(mbox_chan)	\
	container_of(mbox_chan, struct rpmi_srvgrp_chan, chan);

struct rpmi_shmem_mbox_controller rpmi_shmem_boxes[RPMI_SHMEM_MAX_CONTROLLERS];

/** Minimum Base group version required */
#define RPMI_BASE_VERSION_MIN		RPMI_VERSION(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#define GET_SERVICEGROUP_ID(msg)		\
({						\
	struct rpmi_message *mbuf = msg;	\
	le16_to_cpu(mbuf->header.servicegroup_id);\
})

#define GET_SERVICE_ID(msg)			\
({						\
	struct rpmi_message *mbuf = msg;	\
	mbuf->header.service_id;		\
})

#define GET_FLAGS(msg)				\
({						\
	struct rpmi_message *mbuf = msg;	\
	mbuf->header.flags;			\
})

#define GET_MESSAGE_ID(msg)			\
({						\
	struct rpmi_message *mbuf = msg;	\
	((u32)mbuf->header.flags << (RPMI_MSG_FLAGS_OFFSET * 8)) | \
	((u32)mbuf->header.service_id << (RPMI_MSG_SERVICE_ID_OFFSET * 8)) | \
	((u32)le16_to_cpu(mbuf->header.servicegroup_id)); \
})

#define MAKE_MESSAGE_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_MSG_SERVICEGROUP_ID_OFFSET * 8); \
	__ret |= (u32)(__service_id) << (RPMI_MSG_SERVICE_ID_OFFSET * 8); \
	__ret |= (u32)(__flags) << (RPMI_MSG_FLAGS_OFFSET * 8); \
	__ret;					\
})

#define GET_DLEN(msg)				\
({						\
	struct rpmi_message *mbuf = msg;	\
	le16_to_cpu(mbuf->header.datalen);	\
})

#define GET_TOKEN(msg)				\
({						\
	struct rpmi_message *mbuf = msg;	\
	le16_to_cpu(mbuf->header.token);	\
})

#define GET_MESSAGE_TYPE(msg)						\
({									\
	uint8_t flags = *((uint8_t *)msg + RPMI_MSG_FLAGS_OFFSET);	\
	((flags & RPMI_MSG_FLAGS_TYPE) >> RPMI_MSG_FLAGS_TYPE_POS));	\
})

enum rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_COUNT,
};

#if RPMI_SHMEM_MAX_CONTROLLERS > 1
mbox_bid_t rpmi_shmem_bid;

void rpmi_shmem_select(mbox_bid_t bid)
{
	rpmi_shmem_bid = bid - RPMI_SHMEM_BID_BASE;
}
#endif

static bool __smq_queue_full(struct smq_queue_ctx *qctx)
{
	return ((le32_to_cpu(*qctx->tailptr) + 1) % qctx->num_slots ==
			le32_to_cpu(*qctx->headptr)) ? true : false;
}

static bool __smq_queue_empty(struct smq_queue_ctx *qctx)
{
	return (le32_to_cpu(*qctx->headptr) ==
		le32_to_cpu(*qctx->tailptr)) ? true : false;
}

static int __smq_rx(struct smq_queue_ctx *qctx, u32 slot_size,
			u32 service_group_id, struct mbox_xfer *xfer)
{
	void *dst, *src;
	struct rpmi_message *msg;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	struct rpmi_message_args *args = xfer->args;
	bool no_rx_token = (args->flags & RPMI_MSG_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	if ((sizeof(u32) * args->rx_endian_words) >
		(slot_size - sizeof(struct rpmi_message_header)))
		return -EINVAL;
	if ((sizeof(u32) * args->rx_endian_words) > xfer->rx_len)
		return -EINVAL;

	/* There should be some message in the queue */
	if (__smq_queue_empty(qctx)) {
		printf("RPMI RX: queue empty\n");
		return -ENOENT;
	}

	/* Get the head/read index and tail/write index */
	headidx = le32_to_cpu(*qctx->headptr);
	tailidx = le32_to_cpu(*qctx->tailptr);
	printf("RPMI RX: queue[%d] head=%d tail=%d\n",
		qctx->queue_id, headidx, tailidx);

	/*
	 * Compute msgidn expected in the incoming message
	 * NOTE: DOORBELL bit is not expected to be set.
	 */
	printf("RPMI RX: MAKE_MESSAGE_ID inputs: group=0x%x, service=0x%x, type=0x%x\n",
		service_group_id, args->service_id, args->type);
	msgidn = MAKE_MESSAGE_ID(service_group_id, args->service_id, args->type);
	printf("RPMI RX: expected msgid=0x%x\n", msgidn);

	/* Find the Rx message with matching token */
	pos = headidx;
	while (pos != tailidx) {
		src = (void *)qctx->buffer + (pos * slot_size);
		if ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
			(GET_TOKEN(src) == (xfer->seq & RPMI_MSG_TOKEN_MASK)))
			break;
		pos = (pos + 1) % qctx->num_slots;
	}
	if (pos == tailidx) {
		printf("RPMI RX: no matching message found\n");
		return -ENOENT;
	}

	/* If Rx message is not first message then make it first message */
	if (pos != headidx) {
		printf("RPMI RX: reordering message from pos %d to head\n", pos);
		src = (void *)qctx->buffer + (pos * slot_size);
		dst = (void *)qctx->buffer + (headidx * slot_size);
		for (i = 0; i < slot_size / sizeof(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	msg = (void *)qctx->buffer + (headidx * slot_size);
	if (no_rx_token)
		args->rx_token = GET_TOKEN(msg);

	/* Extract data from the first message */
	if (xfer->rx) {
		args->rx_data_len = dlen = GET_DLEN(msg);
		if (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (void *)msg + sizeof(struct rpmi_message_header);
		dst = xfer->rx;
		for (i = 0; i < args->rx_endian_words; i++)
			((u32 *)dst)[i] = le32_to_cpu(((u32 *)src)[i]);
		dst += sizeof(u32) * args->rx_endian_words;
		src += sizeof(u32) * args->rx_endian_words;
		memcpy(dst, src,
			xfer->rx_len - (sizeof(u32) * args->rx_endian_words));

		printf("RPMI RX: received message service_id=0x%x len=%d\n",
			GET_SERVICE_ID(msg), dlen);
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->num_slots;
	printf("RPMI RX: updated head to %d\n",
		le32_to_cpu(*qctx->headptr));

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	return 0;
}

static int __smq_tx(struct smq_queue_ctx *qctx, struct rpmi_mb_regs *mb_regs,
		    uint32_t doorbell_value,
		    uint32_t slot_size, uint32_t service_group_id,
		    struct mbox_xfer *xfer)
{
	uint32_t i, tailidx;
	void *dst, *src;
	struct rpmi_message_header header = { 0 };
	struct rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	if ((sizeof(uint32_t) * args->tx_endian_words) >
	    (slot_size - sizeof(struct rpmi_message_header)))
		return -EINVAL;
	if ((sizeof(uint32_t) * args->tx_endian_words) > xfer->tx_len)
		return -EINVAL;

	/* There should be some room in the queue */
	if (__smq_queue_full(qctx))
		return -ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((uint16_t)xfer->tx_len);
	header.token = cpu_to_le16((uint16_t)xfer->seq);

	/* Write header into the slot */
	dst = (char *)qctx->buffer + (tailidx * slot_size);
	memcpy(dst, &header, sizeof(header));
	dst += sizeof(header);

	/* Write data into the slot */
	if (xfer->tx) {
		src = xfer->tx;
		for (i = 0; i < args->tx_endian_words; i++)
			((uint32_t *)dst)[i] = cpu_to_le32(((uint32_t *)src)[i]);
		dst += sizeof(uint32_t) * args->tx_endian_words;
		src += sizeof(uint32_t) * args->tx_endian_words;
		memcpy(dst, src, xfer->tx_len -
				 (sizeof(uint32_t) * args->tx_endian_words));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->num_slots;

	/* Ring the RPMI doorbell if present */
	if (mb_regs)
		__raw_setl(doorbell_value, (caddr_t)&mb_regs->db_reg);

	return 0;
}

static int smq_rx(struct rpmi_shmem_mbox_controller *mctl,
		  uint32_t queue_id, uint32_t service_group_id,
		  struct mbox_xfer *xfer)
{
	int ret, rxretry = 0;
	struct smq_queue_ctx *qctx;

	if (mctl->queue_count < queue_id) {
		con_err("rpmi_shmem(%d): invalid queue_id or service_group_id\n",
			mctl->controller.bid);
		return -EINVAL;
	}
	qctx = &mctl->queue_ctx_tbl[queue_id];

	/*
	 * Once the timeout happens and call this function is returned
	 * to the client then there is no way to deliver the response
	 * message after that if it comes later.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before recv retry. udelay or mdelay
	 */
	do {
		spin_lock(&qctx->queue_lock);
		ret = __smq_rx(qctx, mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		if (!ret)
			return 0;

		mdelay(1);
		rxretry += 1;
	} while (rxretry < xfer->rx_timeout);

	return -ETIMEDOUT;
}

static int smq_tx(struct rpmi_shmem_mbox_controller *mctl,
		  uint32_t queue_id, uint32_t service_group_id,
		  struct mbox_xfer *xfer)
{
	int ret, txretry = 0;
	struct smq_queue_ctx *qctx;

	if (mctl->queue_count < queue_id) {
		con_err("rpmi_shmem(%d): invalid queue_id or service_group_id\n",
			mctl->controller.bid);
		return -EINVAL;
	}
	qctx = &mctl->queue_ctx_tbl[queue_id];

	/*
	 * Ignoring the tx timeout since in RPMI has no mechanism
	 * with which other side can let know about the reception of
	 * message which marks as tx complete. For RPMI tx complete is
	 * marked as done when message in successfully copied in queue.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before send retry. udelay or mdelay
	 */
	do {
		spin_lock(&qctx->queue_lock);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->doorbell_value,
				mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		if (!ret)
			return 0;

		mdelay(1);
		txretry += 1;
	} while (txretry < xfer->tx_timeout);

	return -ETIMEDOUT;
}

#if 0
static int smq_base_get_two_u32(struct rpmi_shmem_mbox_controller *mctl,
				uint32_t service_id, uint32_t *inarg, uint32_t *outvals)
{
	return rpmi_normal_request_with_status(
			mctl->base_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}
#endif

/**************** Mailbox Controller Functions **************/

int rpmi_shmem_transfer(struct mbox_chan *chan, struct mbox_xfer *xfer)
{
	int ret;
	uint32_t tx_qid = 0, rx_qid = 0;
	struct rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
					 struct rpmi_shmem_mbox_controller,
					 controller);
	struct rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	struct rpmi_message_args *args = xfer->args;
	bool do_tx = (args->flags & RPMI_MSG_FLAGS_NO_TX) ? false : true;
	bool do_rx = (args->flags & RPMI_MSG_FLAGS_NO_RX) ? false : true;

	if (!do_tx && !do_rx)
		return -EINVAL;

	switch (args->type) {
	case RPMI_MSG_NORMAL_REQUEST:
		if (do_tx && do_rx) {
			tx_qid = mctl->type == RPMI_MB_TYPE_AP ?
					       RPMI_QUEUE_IDX_A2P_REQ :
					       RPMI_QUEUE_IDX_P2A_REQ;
			rx_qid = mctl->type == RPMI_MB_TYPE_AP ?
					       RPMI_QUEUE_IDX_P2A_ACK :
					       RPMI_QUEUE_IDX_A2P_ACK;
		} else if (do_tx) {
			tx_qid = mctl->type == RPMI_MB_TYPE_AP ?
					       RPMI_QUEUE_IDX_A2P_REQ :
					       RPMI_QUEUE_IDX_P2A_REQ;
		} else if (do_rx) {
			rx_qid = mctl->type == RPMI_MB_TYPE_AP ?
					       RPMI_QUEUE_IDX_P2A_REQ :
					       RPMI_QUEUE_IDX_A2P_REQ;
		}
		break;
	case RPMI_MSG_POSTED_REQUEST:
		if (do_tx && do_rx)
			return -EINVAL;
		if (do_tx) {
			tx_qid = mctl->type == RPMI_MB_TYPE_AP ?
					       RPMI_QUEUE_IDX_A2P_REQ :
					       RPMI_QUEUE_IDX_P2A_REQ;
		} else {
			rx_qid = mctl->type == RPMI_MB_TYPE_AP ?
					       RPMI_QUEUE_IDX_P2A_REQ :
					       RPMI_QUEUE_IDX_A2P_REQ;
		}
		break;
	case RPMI_MSG_ACKNOWLDGEMENT:
		if (do_tx && do_rx)
			return -EINVAL;
		if (do_tx) {
			tx_qid = mctl->type == RPMI_MB_TYPE_AP ?
					       RPMI_QUEUE_IDX_A2P_ACK :
					       RPMI_QUEUE_IDX_P2A_ACK;
		} else {
			rx_qid = mctl->type == RPMI_MB_TYPE_AP ?
					       RPMI_QUEUE_IDX_P2A_ACK :
					       RPMI_QUEUE_IDX_A2P_ACK;
		}
		break;
	default:
		return -ENOTSUP;
	}

	if (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		if (ret)
			return ret;
	}

	if (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		if (ret)
			return ret;
	}

	return 0;
}

int rpmi_shmem_transport_init(struct rpmi_shmem_mbox_controller *mctl,
			      struct rpmi_shmem *config)
{
	int qid;
	uint64_t reg_addr, reg_size;
	struct smq_queue_ctx *qctx;

	mctl->type = config->mb_type;
	mctl->slot_size = config->slot_size;
	if (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		con_log("rpmi_shmem(%d): slot_size < mimnum required message size\n",
			mctl->controller.bid);
		mctl->slot_size = RPMI_SLOT_SIZE_MIN;
	}

	mctl->doorbell_value = config->db_tx_val;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	mctl->queue_count = config->queue_count;

	/* parse all queues and populate queues context structure */
	for (qid = 0; qid < mctl->queue_count; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size */
		reg_addr = (uint64_t)config->queue_base[qid];
		reg_size = (uint64_t)config->queue_size[qid];
		if (!reg_addr || !reg_size)
			return -ENOENT;

		/* TODO: memregion configuration of queue */

		/* calculate number of slots in each queue */
		qctx->num_slots =
			(reg_size - (mctl->slot_size * RPMI_QUEUE_HEADER_SLOTS)) / mctl->slot_size;

		/* setup queue pointers */
		qctx->headptr = ((void *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEAD_SLOT * mctl->slot_size;
		qctx->tailptr = ((void *)(unsigned long)reg_addr) +
				RPMI_QUEUE_TAIL_SLOT * mctl->slot_size;
		qctx->buffer = ((void *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEADER_SLOTS * mctl->slot_size;

		/* store the index as queue_id */
		qctx->queue_id = qid;

		/* SPIN_LOCK_INIT(qctx->queue_lock); */
	}

	/* get the doorbell property name */
	mctl->mb_regs = (void *)(unsigned long)config->db_tx_reg;

	/* TODO: memregion configuration of db_reg */

	mctl->irq = config->db_irq;
	mctl->regs = (void *)(unsigned long)config->db_rx_reg;
	mctl->val = config->db_rx_val;
	return 0;
}

void rpmi_shmem_free_slot(int sid)
{
	if (sid < RPMI_MAX_CHANNELS)
		rpmi_shmem_chan_used[sid] = 0;
}

int rpmi_shmem_alloc_slot(struct mbox_controller *mbox)
{
	int sid;

	/* Find an empty channel */
	for (sid = 0; sid < RPMI_MAX_CHANNELS; sid++) {
		if (!rpmi_shmem_chan_used[sid]) {
			rpmi_shmem_chan_used[sid] = 1;
			rpmi_shmem_srvgrp_chans[sid].chan.mbox = mbox;
			return sid;
		}
	}
	return -1;
}

static struct mbox_chan *rpmi_shmem_mbox_request_chan(
						struct mbox_controller *mbox,
						uint32_t *chan_args)
{
	struct rpmi_srvgrp_chan *srvgrp_chan;
	__unused struct rpmi_shmem_mbox_controller *mctl =
			container_of(mbox,
				     struct rpmi_shmem_mbox_controller,
				     controller);
	int sid;

	/* Service group id not defined or in reserved range is invalid */
        if (chan_args[0] >= RPMI_SRVGRP_ID_MAX_COUNT &&
	    chan_args[0] <= RPMI_SRVGRP_RESERVE_END)
		return NULL;

#if 0
	/* Base serivce group is always present so probe other groups */
	if (chan_args[0] != RPMI_SRVGRP_BASE) {
		int ret;

		/* Probe service group */
		ret = smq_base_get_two_u32(mctl,
					   RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
					   chan_args, tval);
		if (ret || !tval[1])
			return NULL;
	}
#endif

	sid = rpmi_shmem_alloc_slot(mbox);
	if (sid < 0)
		return NULL;

	srvgrp_chan = &rpmi_shmem_srvgrp_chans[sid];
	srvgrp_chan->sid = sid;
	srvgrp_chan->servicegroup_id = chan_args[0];
	srvgrp_chan->servicegroup_version = chan_args[1];

	return &srvgrp_chan->chan;
}

struct mbox_chan *rpmi_shmem_request_channel(uint32_t *args)
{
	struct rpmi_shmem_mbox_controller *mctl = &rpmi_shmem_boxes[rpmi_shmem_bid];
	struct mbox_chan *chan;

	chan = rpmi_shmem_mbox_request_chan(&mctl->controller, args);
	if (!chan) {
		con_err("rpmi_shmem(%d): failed to request channel %d\n",
			rpmi_shmem_bid, args[0]);
		BUG();
	}
	return chan;
}

static void rpmi_shmem_free_chan(struct mbox_controller *mbox,
				struct mbox_chan *chan)
{
	struct rpmi_srvgrp_chan *srvgrp_chan;
	mbox_sid_t sid;

	if (!mbox || !chan)
		return;

	srvgrp_chan = to_srvgrp_chan(chan);
	for (sid = 0; sid < RPMI_MAX_CHANNELS; sid++) {
		if (srvgrp_chan == &rpmi_shmem_srvgrp_chans[sid]) {
			rpmi_shmem_free_slot(sid);
			break;
		}
	}
}

void rpmi_shmem_free_channel(struct mbox_chan *chan)
{
	rpmi_shmem_free_chan(chan->mbox, chan);
}

void rpmi_shmem_handle_irq(void)
{
	struct rpmi_shmem_mbox_controller *mctl = &rpmi_shmem_boxes[rpmi_shmem_bid];
	enum rpmi_queue_idx queue_ids[2];
	uint32_t doorbell_value = mctl->val;
	struct rpmi_mb_regs *mb_regs = mctl->regs;
	struct smq_queue_ctx *qctx;
	int ret;
	int i, j;
	struct rpmi_message_args args;
	struct mbox_xfer xfer;
	bool handler_found = false;
	uint32_t service_id;

	if (mctl->type == RPMI_MB_TYPE_AP) {
		queue_ids[0] = RPMI_QUEUE_IDX_P2A_REQ;
		queue_ids[1] = RPMI_QUEUE_IDX_P2A_ACK;
	}
	if (mctl->type == RPMI_MB_TYPE_PuC) {
		queue_ids[0] = RPMI_QUEUE_IDX_A2P_REQ;
		queue_ids[1] = RPMI_QUEUE_IDX_A2P_ACK;
	}
	if (mb_regs)
		__raw_clearl(doorbell_value, (caddr_t)&mb_regs->db_reg);

	for (i = 0; i < 2; i++) {
		con_dbg("rpmi_shmem(%d): handling %s_%s...\n",
			rpmi_shmem_bid,
			mctl->type == RPMI_MB_TYPE_AP ? "P2A" : "A2P",
			i == 0 ? "REQ" : "ACK");
		qctx = &rpmi_shmem_queue_ctx_tbl[queue_ids[i]];
		while (!__smq_queue_empty(qctx)) {
			memset(&args, 0, sizeof(args));
			args.type = RPMI_MSG_NORMAL_REQUEST;
			args.rx_endian_words = 2;

			memset(&xfer, 0, sizeof(xfer));
			xfer.args = &args;
			xfer.rx = NULL;
			xfer.rx_len = mctl->slot_size;
			xfer.rx_timeout = RPMI_DEF_RX_TIMEOUT;

			ret = __smq_rx(qctx, mctl->slot_size, 0, &xfer);
			if (ret) {
				con_err("rpmi_shmem(%d): RX failed",
					rpmi_shmem_bid);
				break;
			}
			service_id = GET_SERVICE_ID(xfer.rx);
#if 0
			for (j = 0; j < num_handlers; j++) {
				if (msg_handlers[j].service_id == service_id) {
					msg_handlers[j].handler(NULL, &xfer);
					handler_found = true;
					break;
				}
			}
#endif
			if (!handler_found)
				con_err("rpmi_shmem(%d): no handler found (service=0x%x)",
					rpmi_shmem_bid, service_id);
		}
	}

}

void rpmi_shmem_irq_handler(irq_t irq)
{
	mbox_bid_t rpmi_bid;
	__unused mbox_bid_t sbid;
	__unused struct rpmi_shmem_mbox_controller *mctl;

	for (rpmi_bid = 0; rpmi_bid < RPMI_SHMEM_MAX_CONTROLLERS; rpmi_bid++) {
		mctl = &rpmi_shmem_boxes[rpmi_shmem_bid];
		if (rpmi_shmem_boxes[rpmi_bid].irq == irq) {
			sbid = mbox_controller_save(mctl->controller.bid);
			rpmi_shmem_handle_irq();
			mbox_controller_restore(sbid);
		}
	}
}

void rpmi_shmem_irq_init(void)
{
	irqc_configure_irq(rpmi_shmem_irq, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(rpmi_shmem_irq, rpmi_shmem_irq_handler);
	irqc_enable_irq(rpmi_shmem_irq);
}

void rpmi_shmem_init(struct rpmi_shmem *config)
{
	int ret;
	struct rpmi_shmem_mbox_controller *mctl;

	mctl = &rpmi_shmem_boxes[rpmi_shmem_bid];
	/* initialize transport layer */
	ret = rpmi_shmem_transport_init(mctl, config);
	if (ret) {
		con_err("rpmi_shmem(%d): failed to register.\n",
			rpmi_shmem_bid);
		BUG();
		return;
	}

	mctl->controller.bid = mbox_bid;
	mctl->controller.max_xfer_len = mctl->slot_size -
					sizeof(struct rpmi_message_header);
	/* register mailbox controller */
	mbox_controller_add(&mctl->controller);

	con_log("rpmi_shmem(%d): registered.\n", rpmi_shmem_bid);
}

struct mbox_controller *rpmi_shmem_get_controller(void)
{
	return &rpmi_shmem_boxes[0].controller;
}
