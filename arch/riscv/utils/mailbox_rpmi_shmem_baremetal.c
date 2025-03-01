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

#include <asm/fdt.h>
#include <target/fdt.h>
#include <sbi/sbi_heap.h>
#include <asm/io.h>
#include <sbi_utils/mailbox/mailbox.h>
#include <sbi_utils/mailbox/fdt_mailbox.h>
#include <sbi_utils/mailbox/rpmi_mailbox.h>

#ifndef DOORBELL_BASE
#define DOORBELL_BASE ULL(0)
#endif

#ifndef SHMEM_REG_BASE
#define SHMEM_REG_BASE ULL(0)
#endif

#ifndef SHMEM_REG_SIZE
#define SHMEM_REG_SIZE ULL(0x100)
#endif

#ifndef QUEUE_SIZE
#define QUEUE_SIZE ULL(0x1000)
#endif

#ifndef SLOT_SIZE
#define SLOT_SIZE ULL(0x40)
#endif

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

/** Mailbox registers */
struct rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	volatile le32_t db_reg;
} __packed;

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
	struct rpmi_mb_regs *mb_regs;
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
		return SBI_EINVAL;
	if ((sizeof(u32) * args->rx_endian_words) > xfer->rx_len)
		return SBI_EINVAL;

	/* There should be some message in the queue */
	if (__smq_queue_empty(qctx))
		return SBI_ENOENT;

	/* Get the head/read index and tail/write index */
	headidx = le32_to_cpu(*qctx->headptr);
	tailidx = le32_to_cpu(*qctx->tailptr);

	/*
	 * Compute msgidn expected in the incoming message
	 * NOTE: DOORBELL bit is not expected to be set.
	 */
	msgidn = MAKE_MESSAGE_ID(service_group_id, args->service_id, args->type);

	/* Find the Rx message with matching token */
	pos = headidx;
	while (pos != tailidx) {
		src = (void *)qctx->buffer + (pos * slot_size);
		if ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_MSG_TOKEN_MASK)))
			break;
		pos = (pos + 1) % qctx->num_slots;
	}
	if (pos == tailidx)
		return SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	if (pos != headidx) {
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
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->num_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	return SBI_OK;
}

static int __smq_tx(struct smq_queue_ctx *qctx, struct rpmi_mb_regs *mb_regs,
		    u32 slot_size, u32 service_group_id, struct mbox_xfer *xfer)
{
	u32 i, tailidx;
	void *dst, *src;
	struct rpmi_message_header header = { 0 };
	struct rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	if ((sizeof(u32) * args->tx_endian_words) >
	    (slot_size - sizeof(struct rpmi_message_header)))
		return SBI_EINVAL;
	if ((sizeof(u32) * args->tx_endian_words) > xfer->tx_len)
		return SBI_EINVAL;

	/* There should be some room in the queue */
	if (__smq_queue_full(qctx))
		return SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

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

	/* Ring the RPMI doorbell if present */
	if (mb_regs)
		writel(cpu_to_le32(1), &mb_regs->db_reg);

	return SBI_OK;
}

static int smq_rx(struct rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, struct mbox_xfer *xfer)
{
	int ret, rxretry = 0;
	struct smq_queue_ctx *qctx;

	if (mctl->queue_count < queue_id) {
		printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		return SBI_EINVAL;
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

	return SBI_ETIMEDOUT;
}

static int smq_tx(struct rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, struct mbox_xfer *xfer)
{
	int ret, txretry = 0;
	struct smq_queue_ctx *qctx;

	if (mctl->queue_count < queue_id) {
		printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		return SBI_EINVAL;
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
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->slot_size,
				service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		if (!ret)
			return 0;

		mdelay(1);
		txretry += 1;
	} while (txretry < xfer->tx_timeout);

	return SBI_ETIMEDOUT;
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
		return SBI_EINVAL;

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
			return SBI_EINVAL;
		if (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} else {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		break;
	case RPMI_MSG_ACKNOWLDGEMENT:
		if (do_tx && do_rx)
			return SBI_EINVAL;
		if (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} else {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		break;
	default:
		return SBI_ENOTSUPP;
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

extern struct fdt_mailbox fdt_mailbox_rpmi_shmem;

int rpmi_shmem_transport_init(struct rpmi_shmem_mbox_controller *mctl)
{
	int count, qid;
	uint64_t reg_addr, reg_size;
	struct smq_queue_ctx *qctx;

	mctl->slot_size = SLOT_SIZE;
	if (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		printf("%s: slot_size < mimnum required message size\n",
			   __func__);
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
		return SBI_EINVAL;

	mctl->queue_count = count - RPMI_REG_IDX_MAX_COUNT;

	/* parse all queues and populate queues context structure */

	for (qid = 0; qid < mctl->queue_count; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		reg_addr = SHMEM_REG_BASE + qid * QUEUE_SIZE;
		reg_size = SHMEM_REG_SIZE;

		memset((void *)reg_addr, 0, reg_size * 100);

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

	return SBI_SUCCESS;
}

struct rpmi_shmem_mbox_controller g_mctl;
struct mbox_controller *g_mbox_ptr = &g_mctl.controller;
struct mbox_chan g_chan;
struct rpmi_mb_regs g_mb_regs = {
	.db_reg = DOORBELL_BASE
};

void rpmi_shmem_init(void)
{
	g_mctl.mb_regs = &g_mb_regs;
	g_mbox_ptr->xfer = rpmi_shmem_mbox_xfer;
	g_mbox_ptr->max_xfer_len = mctl->slot_size - sizeof(struct rpmi_message_header);
	g_chan.mbox = g_mbox_ptr;

	rpmi_shmem_transport_init(&g_mctl);
}
