#include <target/mbox.h>

#define MAX_MSG_HANDLERS 16
struct rpmi_msg_handler msg_handlers[MAX_MSG_HANDLERS];
int num_handlers = 0;

int rpmi_normal_request_with_status(struct mbox_chan *chan,
					uint32_t service_id,
			void *req, u32 req_words, u32 req_endian_words,
			void *resp, u32 resp_words, u32 resp_endian_words)
{
	int ret;
	struct mbox_xfer xfer;
	struct rpmi_message_args args = { 0 };

	args.type = RPMI_MSG_NORMAL_REQUEST;
	args.service_id = service_id;
	args.tx_endian_words = req_endian_words;
	args.rx_endian_words = resp_endian_words;
	mbox_xfer_init_txrx(&xfer, &args,
			req, sizeof(u32) * req_words, RPMI_DEF_TX_TIMEOUT,
			resp, sizeof(u32) * resp_words, RPMI_DEF_RX_TIMEOUT);

	ret = mbox_chan_xfer(chan, &xfer);
	if (ret)
		return ret;

	return ((u32 *)resp)[0];
}

int rpmi_posted_request(
		struct mbox_chan *chan, u32 service_id,
		void *req, u32 req_words, u32 req_endian_words)
{
	struct mbox_xfer xfer;
	struct rpmi_message_args args = { 0 };

	args.type = RPMI_MSG_POSTED_REQUEST;
	args.flags = RPMI_MSG_FLAGS_NO_RX;
	args.service_id = service_id;
	args.tx_endian_words = req_endian_words;
	mbox_xfer_init_tx(&xfer, &args,
			  req, sizeof(u32) * req_words, RPMI_DEF_TX_TIMEOUT);

	return mbox_chan_xfer(chan, &xfer);
}

struct mbox_controller *rpmi_get_controller(void)
{
	return rpmi_shmem_get_controller();
}

int rpmi_register_handler(uint32_t service_id,
	void (*handler)(struct mbox_chan *chan, struct mbox_xfer *xfer))
{
	if (num_handlers >= MAX_MSG_HANDLERS) {
		return -ENOMEM;
	}

	msg_handlers[num_handlers].service_id = service_id;
	msg_handlers[num_handlers].handler = handler;
	num_handlers++;

	return 0;
}