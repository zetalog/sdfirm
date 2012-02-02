#include <target/config.h>
#include <target/generic.h>
#include <target/usb.h>
#include <driver/pn53x.h>

#define PN53X_ENDP_INTERVAL_IN		0x01
#define PN53X_ENDP_INTERVAL_OUT		0x01

#define NR_PN53X_ENDPS			2
#define PN53X_ENDP_BULK_IN		0
#define PN53X_ENDP_BULK_OUT		1
#define PN53X_ADDR_OUT			usb_pn53x_addr[PN53X_ENDP_BULK_OUT]
#define PN53X_ADDR_IN			usb_pn53x_addr[PN53X_ENDP_BULK_IN]

#define USB_INTERFACE_SUBCLASS_VENDOR	0xff
#define USB_INTERFACE_PROTOCOL_VENDOR	0xff

#define USB_PN53X_STRING_FIRST		0xf0
#define USB_PN53X_STRING_INTERFACE	USB_PN53X_STRING_FIRST+0
#define USB_PN53X_STRING_LAST		USB_PN53X_STRING_INTERFACE

__near__ uint8_t usb_pn53x_addr[NR_PN53X_ENDPS];

struct usb_pn53x_ctrl {
	/* communication state */
	uint8_t state;
#define USB_PN53X_STATE_IDLE	0x00	/* wait for CMD/ACK/NAK */
#define USB_PN53X_STATE_CMD	0x01
#define USB_PN53X_STATE_RESP	0x02
	uint8_t flags;
#define USB_PN53X_FLAG_RESP	0x01
#define USB_PN53X_FLAG_POLL	0x02
	scs_size_t in_length;
	scs_size_t out_length;
};

struct usb_pn53x_ctrl usb_pn53x_ctrl;
uint8_t usb_pn53x_cmd[PN53X_BUF_SIZE];
uint8_t usb_pn53x_resp[PN53X_BUF_SIZE];

static void usb_pn53x_discard_response(void)
{
	usbd_request_discard_addr(PN53X_ADDR_IN);
	usb_pn53x_ctrl.in_length = 0;
}

static void usb_pn53x_write_cmpl(scs_size_t length)
{
	pn53x_write_cmpl(length);
	usb_pn53x_ctrl.out_length = length;
}

static void usb_pn53x_read_cmpl(scs_size_t length)
{
	pn53x_read_cmpl(length);
	usb_pn53x_ctrl.in_length = length;
}

static scs_size_t usb_pn53x_cmd_total(void)
{
	if (PN53X_NORMAL(usb_pn53x_cmd))
		return PN53X_NORMAL_SIZE(usb_pn53x_cmd);
	else
		return PN53X_HEAD_SIZE;
}

static scs_size_t usb_pn53x_resp_total(void)
{
	if (PN53X_NORMAL(usb_pn53x_resp))
		return PN53X_NORMAL_SIZE(usb_pn53x_resp);
	else
		return PN53X_HEAD_SIZE;
}

static void usb_pn53x_clear_flags(flags)
{
	unraise_bits(usb_pn53x_ctrl.flags, flags);
	pn53x_debug(PN53X_DEBUG_USB_FLAGS, usb_pn53x_ctrl.flags);
}

static void usb_pn53x_set_flags(flags)
{
	raise_bits(usb_pn53x_ctrl.flags, flags);
	pn53x_debug(PN53X_DEBUG_USB_FLAGS, usb_pn53x_ctrl.flags);
}

static boolean usb_pn53x_test_flags(flags)
{
	return bits_raised(usb_pn53x_ctrl.flags, flags);
}

static void usb_pn53x_set_state(uint8_t state)
{
	pn53x_debug(PN53X_DEBUG_USB_STATE, state);

	usb_pn53x_discard_response();

	usb_pn53x_ctrl.state = state;
	switch (state) {
	case USB_PN53X_STATE_IDLE:
		usb_pn53x_clear_flags(USB_PN53X_FLAG_RESP |
				      USB_PN53X_FLAG_POLL);
		break;
	case USB_PN53X_STATE_CMD:
	case USB_PN53X_STATE_RESP:
		usb_pn53x_set_flags(USB_PN53X_FLAG_RESP |
				    USB_PN53X_FLAG_POLL);
		break;
	}
}

static void usb_pn53x_submit_command(void)
{
	usbd_request_submit(PN53X_ADDR_OUT, PN53X_HEAD_SIZE);
}

static void usb_pn53x_handle_command(void)
{
	scs_off_t i;

	usbd_dump_on(PN53X_DUMP_USBD);
	for (i = 0; i < PN53X_HEAD_SIZE; i++)
		USBD_OUTB(usb_pn53x_cmd[i]);

	if (usbd_request_handled() < PN53X_HEAD_SIZE)
		return;

	if (PN53X_NORMAL(usb_pn53x_cmd)) {
		if (usbd_request_handled() == PN53X_HEAD_SIZE)
			usbd_request_commit(PN53X_NORMAL_SIZE(usb_pn53x_cmd));

		for (i = PN53X_HEAD_SIZE;
		     i < PN53X_NORMAL_SIZE(usb_pn53x_cmd); i++) {
			USBD_OUTB(usb_pn53x_cmd[i]);
		}
	}
	usbd_dump_off();
}

static void usb_pn53x_complete_command(void)
{
	scs_off_t i;
	scs_size_t total = usb_pn53x_cmd_total();

	if (usbd_request_handled() != total)
		return;

	/* Validate LCS or DCS. */

	for (i = 0; i < total; i++)
		pn53x_xchg_write(i, usb_pn53x_cmd[i]);
	usb_pn53x_write_cmpl(i);

	switch (pn53x_type(usb_pn53x_cmd)) {
	case PN53X_ACK:
		usb_pn53x_set_state(USB_PN53X_STATE_IDLE);
		break;
	case PN53X_NAK:
		usb_pn53x_set_state(USB_PN53X_STATE_RESP);
		break;
	default:
		usb_pn53x_set_state(USB_PN53X_STATE_CMD);
		break;
	}
}

static void usb_pn53x_poll_completion(void)
{
	scs_off_t i;
	scs_size_t total;

	BUG_ON(usb_pn53x_ctrl.state == USB_PN53X_STATE_IDLE ||
	       !usb_pn53x_test_flags(USB_PN53X_FLAG_POLL));

	for (i = 0; i < PN53X_HEAD_SIZE; i++)
		usb_pn53x_resp[i] = pn53x_xchg_read(i);
	total = usb_pn53x_resp_total();
	for (; i < total; i++) {
		usb_pn53x_resp[i] = pn53x_xchg_read(i);
	}

	usb_pn53x_read_cmpl(i);
	usb_pn53x_clear_flags(USB_PN53X_FLAG_POLL);
}

static void usb_pn53x_submit_response(void)
{
	if (!usb_pn53x_test_flags(USB_PN53X_FLAG_RESP))
		return;
	if (usb_pn53x_test_flags(USB_PN53X_FLAG_POLL)) {
		if (pn53x_poll_ready()) {
			usb_pn53x_poll_completion();
		}
	}
	if (!usb_pn53x_test_flags(USB_PN53X_FLAG_POLL)) {
		usbd_request_submit(PN53X_ADDR_IN,
				    usb_pn53x_ctrl.in_length);
	}
}

static void usb_pn53x_handle_response(void)
{
	scs_size_t i;

	usbd_dump_on(PN53X_DUMP_USBD);
	for (i = 0; i < usb_pn53x_ctrl.in_length; i++)
		USBD_INB(usb_pn53x_resp[i]);
	usbd_dump_off();
}

static void usb_pn53x_complete_response(void)
{
	if (usb_pn53x_test_flags(USB_PN53X_FLAG_POLL) ||
	    !usb_pn53x_test_flags(USB_PN53X_FLAG_RESP)) {
		/* No state transitions on a discarded URB. */
		return;
	}
	switch (pn53x_type(usb_pn53x_resp)) {
	case PN53X_ACK:
		usb_pn53x_set_state(USB_PN53X_STATE_RESP);
		break;
	default:
		usb_pn53x_set_state(USB_PN53X_STATE_IDLE);
		break;
	}
}

usbd_endpoint_t usb_pn53x_endpoint_in = {
	USBD_ENDP_BULK_IN,
	PN53X_ENDP_INTERVAL_IN,
	usb_pn53x_submit_response,
	usb_pn53x_handle_response,
	usb_pn53x_complete_response,
};

usbd_endpoint_t usb_pn53x_endpoint_out = {
	USBD_ENDP_BULK_OUT,
	PN53X_ENDP_INTERVAL_OUT,
	usb_pn53x_submit_command,
	usb_pn53x_handle_command,
	usb_pn53x_complete_command,
};

static void usb_pn53x_get_config_desc(void)
{
	usbd_input_interface_desc(USB_INTERFACE_CLASS_VENDOR,
			  USB_INTERFACE_SUBCLASS_VENDOR,
			  USB_INTERFACE_PROTOCOL_VENDOR,
			  USB_PN53X_STRING_INTERFACE);

	usbd_input_endpoint_desc(PN53X_ADDR_IN);
	usbd_input_endpoint_desc(PN53X_ADDR_OUT);
}

static void usb_pn53x_get_string_desc(void)
{
	uint8_t id = LOBYTE(usbd_control_request_value());

	switch (id) {
	case USB_PN53X_STRING_INTERFACE:
		usbd_input_device_name();
		break;
	default:
		USBD_INB(0x00);
		break;
	}
}

static void usb_pn53x_get_descriptor(void)
{
	uint8_t desc;

	desc = HIBYTE(usbd_control_request_value());

	switch (desc) {
	case USB_DT_CONFIG:
		usb_pn53x_get_config_desc();
		break;
	case USB_DT_STRING:
		usb_pn53x_get_string_desc();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

#define usb_pn53x_set_descriptor()	usbd_endpoint_halt()

static void usb_pn53x_standard_control(void)
{
	uint8_t req = usbd_control_request_type();

	switch (req) {
	case USB_REQ_GET_DESCRIPTOR:
		usb_pn53x_get_descriptor();
		break;
	case USB_REQ_SET_DESCRIPTOR:
		usb_pn53x_set_descriptor();
		break;
	default:
		usbd_endpoint_halt();
	}
}

static void usb_pn53x_control_iocb(void)
{
	uint8_t type = usbd_control_setup_type();
	uint8_t recp = usbd_control_setup_recp();

	switch(recp) {
	case USB_RECP_DEVICE:
		switch (type) {
		case USB_TYPE_STANDARD:
			usb_pn53x_standard_control();
			return;
		}
		break;
	case USB_RECP_INTERFACE:
		switch (type) {
		case USB_TYPE_STANDARD:
			usb_pn53x_standard_control();
			return;
		}
		break;
	}
	usbd_endpoint_halt();
}

static uint16_t usb_pn53x_config_length(void)
{
	return 0;
}

usbd_interface_t usb_pn53x_interface = {
	USB_PN53X_STRING_FIRST,
	USB_PN53X_STRING_LAST,
	NR_PN53X_ENDPS,
	usb_pn53x_config_length,
	usb_pn53x_control_iocb,
};

void usb_pn53x_init(void)
{
	usbd_declare_interface(100, &usb_pn53x_interface);
	PN53X_ADDR_IN = usbd_claim_endpoint(true, &usb_pn53x_endpoint_in);
	PN53X_ADDR_OUT = usbd_claim_endpoint(true, &usb_pn53x_endpoint_out);
	pn53x_ctrl_init();
	usb_pn53x_set_state(USB_PN53X_STATE_IDLE);
}
