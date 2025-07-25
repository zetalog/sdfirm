/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ifndef __MAILBOX_H__
#define __MAILBOX_H__

//#include <sbi/sbi_types.h>
#include <asm/types.h>
#include <sbi/sbi_list.h>
//#include <sbi/riscv_atomic.h>
#include <target/atomic.h>
#include <asm/types.h>

/** Representation of a mailbox channel */

#define to_mbox_chan(__node)	\
	container_of((__node), struct mbox_chan, node)

/**
 * Representation of a mailbox data transfer
 *
 * NOTE: If both "tx" and "rx" are non-NULL then Tx is done before Rx.
 */

#define mbox_xfer_init_tx(__p, __a, __t, __t_len, __t_tim)		\
do {									\
	(__p)->flags = 0;						\
	(__p)->args = (__a);						\
	(__p)->tx = (__t);						\
	(__p)->tx_len = (__t_len);					\
	(__p)->tx_timeout = (__t_tim);					\
	(__p)->rx = NULL;						\
	(__p)->rx_len = 0;						\
	(__p)->rx_timeout = 0;						\
} while (0)

#define mbox_xfer_init_rx(__p, __a, __r, __r_len, __r_tim)		\
do {									\
	(__p)->flags = 0;						\
	(__p)->args = (__a);						\
	(__p)->tx = NULL;						\
	(__p)->tx_len = 0;						\
	(__p)->tx_timeout = 0;						\
	(__p)->rx = (__r);						\
	(__p)->rx_len = (__r_len);					\
	(__p)->rx_timeout = (__r_tim);					\
} while (0)

#define mbox_xfer_init_txrx(__p, __a, __t, __t_len, __t_tim, __r, __r_len, __r_tim)\
do {									\
	(__p)->flags = 0;						\
	(__p)->args = (__a);						\
	(__p)->tx = (__t);						\
	(__p)->tx_len = (__t_len);					\
	(__p)->tx_timeout = (__t_tim);					\
	(__p)->rx = (__r);						\
	(__p)->rx_len = (__r_len);					\
	(__p)->rx_timeout = (__r_tim);					\
} while (0)

#define mbox_xfer_set_sequence(__p, __seq)				\
do {									\
	(__p)->flags |= MBOX_XFER_SEQ;					\
	(__p)->seq = (__seq);						\
} while (0)

/** Representation of a mailbox controller */
struct mbox_controller {
	/** List head */
	struct sbi_dlist node;
	/** Next sequence atomic counter */
	atomic_t xfer_next_seq;
	/* List of mailbox channels */
	struct sbi_dlist chan_list;
	/** Unique ID of the mailbox controller assigned by the driver */
	unsigned int id;
	/** Maximum length of transfer supported by the mailbox controller */
	unsigned int max_xfer_len;
	/** Pointer to mailbox driver owning this mailbox controller */
	void *driver;
	/** Request a mailbox channel from the mailbox controller */
	struct mbox_chan *(*request_chan)(struct mbox_controller *mbox,
					  u32 *chan_args);
	/** Free a mailbox channel from the mailbox controller */
	void (*free_chan)(struct mbox_controller *mbox,
			  struct mbox_chan *chan);
	/** Transfer data over mailbox channel */
	int (*xfer)(struct mbox_chan *chan, struct mbox_xfer *xfer);
	/** Get an attribute of mailbox channel */
	int (*get_attribute)(struct mbox_chan *chan, int attr_id, void *out_value);
	/** Set an attribute of mailbox channel */
	int (*set_attribute)(struct mbox_chan *chan, int attr_id, void *new_value);
};

#define to_mbox_controller(__node)	\
	container_of((__node), struct mbox_controller, node)

/** Find a registered mailbox controller */
struct mbox_controller *mbox_controller_find(unsigned int id);

/** Register mailbox controller */
int mbox_controller_add(struct mbox_controller *mbox);

/** Un-register mailbox controller */
void mbox_controller_remove(struct mbox_controller *mbox);

/** Request a mailbox channel */
struct mbox_chan *mbox_controller_request_chan(struct mbox_controller *mbox,
					       u32 *chan_args);

/** Free a mailbox channel */
void mbox_controller_free_chan(struct mbox_chan *chan);

/** Data transfer over mailbox channel */
int mbox_chan_xfer(struct mbox_chan *chan, struct mbox_xfer *xfer);

/** Get an attribute of mailbox channel */
int mbox_chan_get_attribute(struct mbox_chan *chan, int attr_id, void *out_value);

/** Set an attribute of mailbox channel */
int mbox_chan_set_attribute(struct mbox_chan *chan, int attr_id, void *new_value);

#endif
