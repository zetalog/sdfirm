#include <target/usb_pn53x.h>
#include <driver/pn53x.h>

#define PN53X_ENDP_INTERVAL_IN	0x01
#define PN53X_ENDP_INTERVAL_OUT	0x01

#define NR_PN53X_ENDPS		2
#define PN53X_ENDP_BULK_IN	0
#define PN53X_ENDP_BULK_OUT	1
#define PN53X_ADDR_OUT		usb_pn53x_addr[PN53X_ENDP_BULK_OUT]
#define PN53X_ADDR_IN		usb_pn53x_addr[PN53X_ENDP_BULK_IN]

#define USB_INTERFACE_SUBCLASS_VENDOR	0xff
#define USB_INTERFACE_PROTOCOL_VENDOR	0xff

#define PN53X_STRING_FIRST	0xf0
#define PN53X_STRING_INTERFACE	PN53X_STRING_FIRST+0
#define PN53X_STRING_LAST	PN53X_STRING_INTERFACE

__near__ uint8_t usb_pn53x_addr[NR_PN53X_ENDPS];

struct usb_pn53x_ctrl {
	/* communication state */
	uint8_t state;
#define PN53X_STATE_CMD		0x00
#define PN53X_STATE_RESP	0x01
#define PN53X_STATE_ACK		0x02
#define PN53X_ERR		0x01ff
#define PN53X_NORMAL(type)	((type) != PN53X_ACK && (type) != PN53X_NAK)
#define PN53X_ERROR()		(PN53X_RESP_LEN == 0x01 && \
				 PN53X_RESP_TFI != 0xD5)
	boolean responding;
	boolean polling;
	scs_size_t in_length;
	scs_size_t out_length;
};

#define PN53X_CMD_LEN		(usb_pn53x_cmd[3])
#define PN53X_RESP_LEN		(usb_pn53x_resp[3])
#define PN53X_RESP_TFI		(usb_pn53x_resp[5])
#define PN53X_CMD_NORMAL_SIZE	((PN53X_HEAD_SIZE -1) + \
				 PN53X_CMD_LEN + PN53X_TAIL_SIZE)
#define PN53X_RESP_NORMAL_SIZE	((PN53X_HEAD_SIZE -1) + \
				 PN53X_RESP_LEN + PN53X_TAIL_SIZE)

struct usb_pn53x_ctrl usb_pn53x_ctrl;
uint8_t usb_pn53x_cmd[PN53X_BUF_SIZE];
uint8_t usb_pn53x_resp[PN53X_BUF_SIZE];

static void pn53x_discard_response(void)
{
	usbd_request_discard_addr(PN53X_ADDR_IN);
	usb_pn53x_ctrl.in_length = 0;
}

static void pn53x_set_state(uint8_t state, scs_size_t length)
{
	pn53x_discard_response();

	usb_pn53x_ctrl.state = state;
	usb_pn53x_ctrl.out_length = length;

	switch (state) {
	case PN53X_STATE_CMD:
		usb_pn53x_ctrl.responding = false;
		usb_pn53x_ctrl.polling = false;
		break;
	case PN53X_STATE_RESP:
	case PN53X_STATE_ACK:
		usb_pn53x_ctrl.responding = true;
		usb_pn53x_ctrl.polling = true;
		break;
	}
}

static void pn53x_poll_completion(void)
{
	scs_off_t i;

	BUG_ON(usb_pn53x_ctrl.state == PN53X_STATE_CMD ||
	       !usb_pn53x_ctrl.polling);

	for (i = 0; i < PN53X_HEAD_SIZE; i++)
		usb_pn53x_resp[i] = pn53x_xchg_read(i);

	switch (pn53x_type(usb_pn53x_resp)) {
	case PN53X_ACK:
		usb_pn53x_ctrl.in_length = PN53X_HEAD_SIZE;
		break;
	default:
		BUG_ON(i != PN53X_HEAD_SIZE);
		for (; i < PN53X_RESP_NORMAL_SIZE; i++) {
			usb_pn53x_resp[i] = pn53x_xchg_read(i);
		}
		usb_pn53x_ctrl.in_length = PN53X_RESP_NORMAL_SIZE;
		break;
	}
	pn53x_read_cmpl(i);
	usb_pn53x_ctrl.polling = false;
}

static void pn53x_submit_response(void)
{
	if (!usb_pn53x_ctrl.responding)
		return;

	if (usb_pn53x_ctrl.polling) {
		if (pn53x_poll_ready()) {
			pn53x_poll_completion();
		}
	}
	if (!usb_pn53x_ctrl.polling) {
		usbd_request_submit(PN53X_ADDR_IN,
				    usb_pn53x_ctrl.in_length);
	}
}

static void pn53x_handle_response(void)
{
	scs_size_t i;

	usbd_dump_on(PN53X_DUMP_USBD);
	for (i = 0; i < usb_pn53x_ctrl.in_length; i++)
		USBD_INB(usb_pn53x_resp[i]);
	usbd_dump_off();
}

static void pn53x_complete_response(void)
{
	if (usb_pn53x_ctrl.polling || !usb_pn53x_ctrl.responding) {
		/* A discarded URB? */
		return;
	}
	if (usb_pn53x_ctrl.state == PN53X_STATE_ACK) {
		pn53x_set_state(PN53X_STATE_RESP, 0);
	}
}

static scs_size_t pn53x_cmd_expected(void)
{
	if (PN53X_NORMAL(pn53x_type(usb_pn53x_cmd)))
		return PN53X_CMD_NORMAL_SIZE;
	else
		return PN53X_HEAD_SIZE;
}

static void pn53x_submit_command(void)
{
	usbd_request_submit(PN53X_ADDR_OUT, PN53X_HEAD_SIZE);
}

static void pn53x_handle_command(void)
{
	scs_off_t i;

	usbd_dump_on(PN53X_DUMP_USBD);
	for (i = 0; i < PN53X_HEAD_SIZE; i++)
		USBD_OUTB(usb_pn53x_cmd[i]);

	if (usbd_request_handled() < PN53X_HEAD_SIZE)
		return;

	if (PN53X_NORMAL(pn53x_type(usb_pn53x_cmd))) {
		if (usbd_request_handled() == PN53X_HEAD_SIZE)
			usbd_request_commit(PN53X_CMD_NORMAL_SIZE);

		for (i = PN53X_HEAD_SIZE; i < PN53X_CMD_NORMAL_SIZE; i++) {
			USBD_OUTB(usb_pn53x_cmd[i]);
		}
	}
	usbd_dump_off();
}

static void pn53x_complete_command(void)
{
	scs_off_t i;
	scs_size_t expected = pn53x_cmd_expected();

	if (usbd_request_handled() != expected)
		return;

	/* Validate LCS or DCS. */

	for (i = 0; i < expected; i++)
		pn53x_xchg_write(i, usb_pn53x_cmd[i]);
	pn53x_write_cmpl(i);

	switch (pn53x_type(usb_pn53x_cmd)) {
	case PN53X_ACK:
		pn53x_set_state(PN53X_STATE_CMD, expected);
		break;
	case PN53X_NAK:
		pn53x_set_state(PN53X_STATE_RESP, expected);
		break;
	default:
		pn53x_set_state(PN53X_STATE_ACK, expected);
		break;
	}
}

usbd_endpoint_t pn53x_endpoint_in = {
	USBD_ENDP_BULK_IN,
	PN53X_ENDP_INTERVAL_IN,
	pn53x_submit_response,
	pn53x_handle_response,
	pn53x_complete_response,
};

usbd_endpoint_t pn53x_endpoint_out = {
	USBD_ENDP_BULK_OUT,
	PN53X_ENDP_INTERVAL_OUT,
	pn53x_submit_command,
	pn53x_handle_command,
	pn53x_complete_command,
};

static void pn53x_get_config_desc(void)
{
	usbd_input_interface_desc(USB_INTERFACE_CLASS_VENDOR,
			  USB_INTERFACE_SUBCLASS_VENDOR,
			  USB_INTERFACE_PROTOCOL_VENDOR,
			  PN53X_STRING_INTERFACE);

	usbd_input_endpoint_desc(PN53X_ADDR_IN);
	usbd_input_endpoint_desc(PN53X_ADDR_OUT);
}

static void pn53x_get_string_desc(void)
{
	uint8_t id = LOBYTE(usbd_control_request_value());

	switch (id) {
	case PN53X_STRING_INTERFACE:
		usbd_input_device_name();
		break;
	default:
		USBD_INB(0x00);
		break;
	}
}

static void pn53x_get_descriptor(void)
{
	uint8_t desc;

	desc = HIBYTE(usbd_control_request_value());

	switch (desc) {
	case USB_DT_CONFIG:
		pn53x_get_config_desc();
		break;
	case USB_DT_STRING:
		pn53x_get_string_desc();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

#define pn53x_set_descriptor()	usbd_endpoint_halt()

static void pn53x_handle_standard_request(void)
{
	uint8_t req = usbd_control_request_type();

	switch (req) {
	case USB_REQ_GET_DESCRIPTOR:
		pn53x_get_descriptor();
		break;
	case USB_REQ_SET_DESCRIPTOR:
		pn53x_set_descriptor();
		break;
	default:
		usbd_endpoint_halt();
	}
}

static void pn53x_handle_ctrl_data(void)
{
	uint8_t type = usbd_control_setup_type();
	uint8_t recp = usbd_control_setup_recp();

	switch(recp) {
	case USB_RECP_DEVICE:
		switch (type) {
		case USB_TYPE_STANDARD:
			pn53x_handle_standard_request();
			return;
		}
		break;
	case USB_RECP_INTERFACE:
		switch (type) {
		case USB_TYPE_STANDARD:
			pn53x_handle_standard_request();
			return;
		}
		break;
	}
	usbd_endpoint_halt();
}

static uint16_t pn53x_config_length(void)
{
	return 0;
}

usbd_interface_t usb_pn53x_interface = {
	PN53X_STRING_FIRST,
	PN53X_STRING_LAST,
	NR_PN53X_ENDPS,
	pn53x_config_length,
	pn53x_handle_ctrl_data,
};

void usb_pn53x_init(void)
{
	usbd_declare_interface(100, &usb_pn53x_interface);
	PN53X_ADDR_IN = usbd_claim_endpoint(true, &pn53x_endpoint_in);
	PN53X_ADDR_OUT = usbd_claim_endpoint(true, &pn53x_endpoint_out);
	pn53x_ctrl_init();
}
