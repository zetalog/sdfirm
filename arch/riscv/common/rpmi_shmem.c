/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <target/rpmi.h>
#include <target/spinlock.h>
#include <target/types.h>
#include <target/clk.h>
#include <target/delay.h>
#include <asm/mach/reg.h>
#include <target/irq.h>

#ifndef SHMEM_REG_BASE
#define SHMEM_REG_BASE (__RMU_SRAM_MAILBOX_NS_BASE)
#endif

#ifndef SHMEM_REG_SIZE
#define SHMEM_REG_SIZE ULL(16 * 1024) // 16KB
#endif

#ifndef SHMEM_QUEUE_SIZE
#define SHMEM_QUEUE_SIZE ULL(0x1000)
#endif

#ifndef SHMEM_SLOT_SIZE
#define SHMEM_SLOT_SIZE ULL(0x40) // 64 bytes
#endif

#define RPMI_IRQ_COUNT	8
static const unsigned int rpmi_irqs[RPMI_IRQ_COUNT] = {
	IRQ_RPMI_A2P_REQ_S,
	IRQ_RPMI_A2P_ACK_S,
	IRQ_RPMI_A2P_REQ_NS,
	IRQ_RPMI_A2P_ACK_NS,
	IRQ_RPMI_P2A_REQ_S,
	IRQ_RPMI_P2A_ACK_S,
	IRQ_RPMI_P2A_REQ_NS,
	IRQ_RPMI_P2A_ACK_NS
};
bh_t	rpmi_bh;

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

enum rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

enum rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_COUNT,
};

enum rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_COUNT,
};

struct rpmi_mb_regs {
	volatile le32_t *a2p_req;
	volatile le32_t *a2p_ack;
	volatile le32_t *p2a_req;
	volatile le32_t *p2a_ack;
};

/** Single Queue Context Structure */
struct smq_queue_ctx {
	u32 queue_id;
	u32 num_slots;
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	enum rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	volatile le32_t *headptr;
	volatile le32_t *tailptr;
	volatile uint8_t *buffer;
	/* Name of the queue */
	char name[RPMI_NAME_CHARS_MAX];
};

struct rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_version;
	struct mbox_chan chan;
};

#define to_srvgrp_chan(mbox_chan)	\
		container_of(mbox_chan, struct rpmi_srvgrp_chan, chan);

struct rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_size;
	u32 queue_count;
	struct rpmi_mb_regs mb_regs;
	struct smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_COUNT];
	/* Mailbox framework related members */
	struct mbox_controller controller;
	struct mbox_chan *base_chan;
	u32 impl_version;
	u32 impl_id;
	u32 spec_version;
	u32 plat_info_len;
	char *plat_info;
	struct {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
		bool f0_msi_en;
	} base_flags;
};

#define RPMI_MAX_CHANNELS	16

static struct rpmi_shmem_mbox_controller g_mctl;
static struct rpmi_srvgrp_chan g_srvgrp_chans[RPMI_MAX_CHANNELS];
static uint32_t g_chan_used[RPMI_MAX_CHANNELS];

/**************** Shared Memory Queues Helpers **************/

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
			u32 slot_size, u32 service_group_id, struct mbox_xfer *xfer)
{
	u32 i, tailidx;
	void *dst, *src;
	struct rpmi_message_header header = { 0 };
	struct rpmi_message_args *args = xfer->args;
	volatile le32_t *doorbell = NULL;

	/* Tx sanity checks */
	if ((sizeof(u32) * args->tx_endian_words) >
		(slot_size - sizeof(struct rpmi_message_header)))
		return -EINVAL;
	if ((sizeof(u32) * args->tx_endian_words) > xfer->tx_len)
		return -EINVAL;

	/* There should be some room in the queue */
	if (__smq_queue_full(qctx)) {
		printf("RPMI TX: queue full\n");
		return -ENOMEM;
	}

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);
	printf("RPMI TX: queue[%d] tail=%d\n", qctx->queue_id, tailidx);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

	printf("RPMI TX: sending message service_id=0x%x len=%ld\n",
		args->service_id, xfer->tx_len);

	/* Write header into the slot */
	dst = (char *)qctx->buffer + (tailidx * slot_size);
	memcpy(dst, &header, sizeof(header));
	dst += sizeof(header);

	/* Write data into the slot */
	if (xfer->tx) {
		src = xfer->tx;
		for (i = 0; i < args->tx_endian_words; i++)
			((u32 *)dst)[i] = cpu_to_le32(((u32 *)src)[i]);
		dst += sizeof(u32) * args->tx_endian_words;
		src += sizeof(u32) * args->tx_endian_words;
		memcpy(dst, src,
			xfer->tx_len - (sizeof(u32) * args->tx_endian_words));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->num_slots;
	printf("RPMI TX: updated tail to %d / %d\n",
		le32_to_cpu(*qctx->tailptr), qctx->num_slots);

	/* Ring the RPMI doorbell if present */
	if (mb_regs) {
		switch (qctx->queue_id) {
			case RPMI_QUEUE_IDX_A2P_REQ:
				doorbell = mb_regs->a2p_req;
				break;
			case RPMI_QUEUE_IDX_A2P_ACK:
				doorbell = mb_regs->a2p_ack;
				break;
			case RPMI_QUEUE_IDX_P2A_REQ:
				doorbell = mb_regs->p2a_req;
				break;
			case RPMI_QUEUE_IDX_P2A_ACK:
				doorbell = mb_regs->p2a_ack;
				break;
			default:
				break;
		}

		if (doorbell)
			writel(cpu_to_le32(1), doorbell);
	}

	return 0;
}

static int smq_rx(struct rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, struct mbox_xfer *xfer)
{
	int ret, rxretry = 0;
	struct smq_queue_ctx *qctx;

	if (mctl->queue_count < queue_id) {
		printf("RPMI: invalid queue_id or service_group_id\n");
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
		if (!ret) {
			printf("RPMI RX: receive successful\n");
			return 0;
		}

		mdelay(1);
		rxretry += 1;
		printf("RPMI RX: retry %d/%ld\n", rxretry, xfer->rx_timeout);
	} while (rxretry < xfer->rx_timeout);

	printf("RPMI RX: timeout after %d retries\n", rxretry);
	return -ETIMEDOUT;
}

static int smq_tx(struct rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, struct mbox_xfer *xfer)
{
	int ret, txretry = 0;
	struct smq_queue_ctx *qctx;

	if (mctl->queue_count < queue_id) {
		printf("RPMI: queue_id or service_group_id\n");
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
		ret = __smq_tx(qctx, &mctl->mb_regs, mctl->slot_size,
				service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		if (!ret) {
			printf("RPMI TX: send successful\n");
			return 0;
		}

		mdelay(1);
		txretry += 1;
		printf("RPMI TX: retry %d/%ld\n", txretry, xfer->tx_timeout);
	} while (txretry < xfer->tx_timeout);

	printf("RPMI TX: timeout after %d retries\n", txretry);
	return -ETIMEDOUT;
}

/**************** Mailbox Controller Functions **************/

int rpmi_shmem_mbox_xfer(struct mbox_chan *chan, struct mbox_xfer *xfer)
{
	int ret;
	u32 tx_qid = 0, rx_qid = 0;
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
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} else if (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} else if (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		break;
	case RPMI_MSG_POSTED_REQUEST:
		if (do_tx && do_rx)
			return -EINVAL;
		if (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} else {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		break;
	case RPMI_MSG_ACKNOWLDGEMENT:
		if (do_tx && do_rx)
			return -EINVAL;
		if (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} else {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
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

int rpmi_shmem_transport_init(struct rpmi_shmem_mbox_controller *mctl)
{
	int count, qid;
	uint64_t reg_addr, reg_size;
	struct smq_queue_ctx *qctx;

	if (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		printf("RPMI: slot_size < mimnum required message size\n");
		mctl->slot_size = RPMI_SLOT_SIZE_MIN;
	}

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	count = 3;
	if (count < 0 ||
		count > (RPMI_QUEUE_IDX_MAX_COUNT + RPMI_REG_IDX_MAX_COUNT))
		return -EINVAL;

	mctl->queue_count = count - RPMI_REG_IDX_MAX_COUNT;

	/* parse all queues and populate queues context structure */
	for (qid = 0; qid < mctl->queue_count; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];
		/* get each queue share-memory base address and size */
		reg_addr = SHMEM_REG_BASE + qid * SHMEM_QUEUE_SIZE;
		reg_size = SHMEM_REG_SIZE;

		spin_lock(&qctx->queue_lock);
		memset((void *)reg_addr, 0, reg_size);
		spin_unlock(&qctx->queue_lock);

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

	}

	return 0;
}

static struct mbox_chan *rpmi_shmem_request_chan(struct mbox_controller *mbox,
						uint32_t *chan_args)
{
	struct rpmi_srvgrp_chan *srvgrp_chan;
	int i;

	if (!mbox || !chan_args)
		return NULL;

	/* Find an empty channel */
	for (i = 0; i < RPMI_MAX_CHANNELS; i++) {
		if (!g_chan_used[i]) {
			srvgrp_chan = &g_srvgrp_chans[i];
			srvgrp_chan->servicegroup_id = chan_args[0];
			srvgrp_chan->servicegroup_version = chan_args[1];
			srvgrp_chan->chan.mbox = mbox;
			g_chan_used[i] = 1;
			return &srvgrp_chan->chan;
		}
	}

	return NULL;
}

static void rpmi_shmem_free_chan(struct mbox_controller *mbox,
				struct mbox_chan *chan)
{
	struct rpmi_srvgrp_chan *srvgrp_chan;
	int i;

	if (!mbox || !chan)
		return;

	srvgrp_chan = to_srvgrp_chan(chan);
	for (i = 0; i < RPMI_MAX_CHANNELS; i++) {
		if (srvgrp_chan == &g_srvgrp_chans[i]) {
			g_chan_used[i] = 0;
			break;
		}
	}
}

static void rpmi_handler_irq(irq_t irq)
{
	struct rpmi_shmem_mbox_controller *mctl = &g_mctl;
	struct smq_queue_ctx *qctx;
	struct mbox_xfer xfer;
	uint32_t doorbell_val;
	int queue_idx = -1;
	volatile le32_t *doorbell_reg = NULL;
	uint32_t service_id;
	bool handler_found = false;

	switch (irq) {
	case IRQ_RPMI_A2P_REQ_S:
	case IRQ_RPMI_A2P_REQ_NS:
		queue_idx = RPMI_QUEUE_IDX_A2P_REQ;
		doorbell_reg = mctl->mb_regs.a2p_req;
		printf("RPMI IRQ: A2P_REQ interrupt (irq=%d)\n", irq_ext(irq));
			break;
	case IRQ_RPMI_A2P_ACK_S:
	case IRQ_RPMI_A2P_ACK_NS:
		queue_idx = RPMI_QUEUE_IDX_A2P_ACK;
		doorbell_reg = mctl->mb_regs.a2p_ack;
		printf("RPMI IRQ: A2P_ACK interrupt (irq=%d)\n", irq_ext(irq));
			break;
	case IRQ_RPMI_P2A_REQ_S:
	case IRQ_RPMI_P2A_REQ_NS:
		queue_idx = RPMI_QUEUE_IDX_P2A_REQ;
		doorbell_reg = mctl->mb_regs.p2a_req;
		printf("RPMI IRQ: P2A_REQ interrupt (irq=%d)\n", irq_ext(irq));
			break;
	case IRQ_RPMI_P2A_ACK_S:
	case IRQ_RPMI_P2A_ACK_NS:
		queue_idx = RPMI_QUEUE_IDX_P2A_ACK;
		doorbell_reg = mctl->mb_regs.p2a_ack;
		printf("RPMI IRQ: P2A_ACK interrupt (irq=%d)\n", irq_ext(irq));
			break;
		default:
		printf("RPMI IRQ: unknown interrupt (irq=%d)\n", irq_ext(irq));
		return;
	}

	if (queue_idx < 0 || queue_idx >= RPMI_QUEUE_IDX_MAX_COUNT) {
		printf("RPMI IRQ: invalid queue index %d\n", queue_idx);
		return;
		}

	qctx = &mctl->queue_ctx_tbl[queue_idx];

	doorbell_val = readl(doorbell_reg);
	if (!doorbell_val) {
		printf("RPMI IRQ: no doorbell detected for queue %d\n", queue_idx);
		return;
	}

	printf("RPMI IRQ: doorbell detected on queue %d, value=0x%x\n", queue_idx, doorbell_val);

	/* handler message in queue */
	spin_lock(&qctx->queue_lock);
	while (!__smq_queue_empty(qctx)) {
		/* Get the head index to read the message */
		uint32_t headidx = le32_to_cpu(*qctx->headptr);
		uint32_t tailidx = le32_to_cpu(*qctx->tailptr);

		if (headidx == tailidx) {
			/* Queue is empty */
			break;
		}

		struct rpmi_message *msg = (void *)qctx->buffer + (headidx * mctl->slot_size);
		service_id = GET_SERVICE_ID(msg);


		handler_found = false;
		for (int j = 0; j < num_handlers; j++) {
			if (msg_handlers[j].service_id == service_id) {
				memset(&xfer, 0, sizeof(xfer));
				xfer.rx = msg;
				xfer.rx_len = mctl->slot_size;

				msg_handlers[j].handler(NULL, &xfer);
				handler_found = true;
				break;
			}
		}

		if (!handler_found)
			printf("RPMI: no message handler found (service_id=0x%x)\n", service_id);

		*qctx->headptr = cpu_to_le32((headidx + 1) % qctx->num_slots);

		smp_wmb();
	}
	spin_unlock(&qctx->queue_lock);

	/* clear doorbell */
	writel(0, doorbell_reg);
}

#ifdef SYS_REALTIME
static void rpmi_poll_init(void)
{
	irq_register_poller(rpmi_bh);
}
#define rpmi_irq_init()		do {} while (0)
#else
static void rpmi_irq_init(void)
{
	int i;

	for (i = 0; i < RPMI_IRQ_COUNT; i++) {
		irqc_configure_irq(rpmi_irqs[i], 0, IRQ_LEVEL_TRIGGERED);
		irq_register_vector(rpmi_irqs[i], rpmi_handler_irq);
		irqc_enable_irq(rpmi_irqs[i]);
	}
}
#define rpmi_poll_init()	do {} while (0)
#endif

static void rpmi_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		for (int i = 0; i < RPMI_IRQ_COUNT; i++)
			rpmi_handler_irq(rpmi_irqs[i]);

		return;
	}
}

int rpmi_shmem_init(void)
{
	int ret;

	clk_enable(rmu_mailbox_s_clk);
	clk_enable(rmu_mailbox_ns_clk);
	clk_enable(rmu_sram_mailbox_s_clk);
	clk_enable(rmu_sram_mailbox_ns_clk);

	g_mctl.mb_regs.a2p_req = (volatile le32_t *)MBOX_RPMI_A2P_REQ;
	g_mctl.mb_regs.a2p_ack = (volatile le32_t *)MBOX_RPMI_A2P_ACK;
	g_mctl.mb_regs.p2a_req = (volatile le32_t *)MBOX_RPMI_P2A_REQ;
	g_mctl.mb_regs.p2a_ack = (volatile le32_t *)MBOX_RPMI_P2A_ACK;
	g_mctl.slot_size = SHMEM_SLOT_SIZE;
	g_mctl.controller.xfer = rpmi_shmem_mbox_xfer;
	g_mctl.controller.request_chan = rpmi_shmem_request_chan;
	g_mctl.controller.free_chan = rpmi_shmem_free_chan;
	g_mctl.controller.max_xfer_len = g_mctl.slot_size -
		sizeof(struct rpmi_message_header);

	/* initialize transport layer */
	ret = rpmi_shmem_transport_init(&g_mctl);
	if (ret) {
		printf("RPMI: failed to initialize transport layer (ret=%d)\n", ret);
		return ret;
	}

	/* register mailbox controller */
	mbox_controller_add(&g_mctl.controller);

	/* register interrupt handler */
	rpmi_bh = bh_register_handler(rpmi_bh_handler);
	rpmi_irq_init();
	rpmi_poll_init();

	printf("RPMI: shared memory initialized (shmem_base=0x%llx, slot_size=%d, max_xfer_len=%d)\n",
		SHMEM_REG_BASE, g_mctl.slot_size, g_mctl.controller.max_xfer_len);

	return 0;
}

struct mbox_controller *rpmi_shmem_get_controller(void)
{
	return &g_mctl.controller;
}
