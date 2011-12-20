#include <host/uartdbg.h>

static void usb_dump_call(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "call=POLL");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "call=DATA");
		break;
	case 2:
		dbg_dumper(ctx, cmd, "call=DONE");
		break;
	default:
		dbg_dumper(ctx, cmd, "call=%02x", data);
		break;
	}
}

static void usb_dump_irq(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "irq=DEV[RESET]");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "irq=DEV[RESUME]");
		break;
	case 2:
		dbg_dumper(ctx, cmd, "irq=DEV[SUSPEND]");
		break;
	case 3:
		dbg_dumper(ctx, cmd, "irq=DEV[SOF]");
		break;
	case 4:
		dbg_dumper(ctx, cmd, "irq=DEV[DETACH]");
		break;
	case 5:
		dbg_dumper(ctx, cmd, "irq=ENDP[SETUP]");
		break;
	case 6:
		dbg_dumper(ctx, cmd, "irq=ENDP[OUT]");
		break;
	case 7:
		dbg_dumper(ctx, cmd, "irq=ENDP[IN]");
		break;
	case 8:
		dbg_dumper(ctx, cmd, "irq=ENDP[STALL]");
		break;
	case 9:
		dbg_dumper(ctx, cmd, "irq=ENDP[NAK(OUT)]");
		break;
	case 10:
		dbg_dumper(ctx, cmd, "irq=ENDP[NAK(IN)]");
		break;
	case 11:
		dbg_dumper(ctx, cmd, "irq=OTG[VBUS]");
		break;
	case 12:
		dbg_dumper(ctx, cmd, "irq=OTG[ID]");
		break;
	default:
		dbg_dumper(ctx, cmd, "irq=%02x", data);
		break;
	}
}

static void usb_dump_device_state(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "state=ATTACHED");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "state=POWERED");
		break;
	case 2:
		dbg_dumper(ctx, cmd, "state=SUSPENDED");
		break;
	case 3:
		dbg_dumper(ctx, cmd, "state=DEFAULT");
		break;
	case 4:
		dbg_dumper(ctx, cmd, "state=ADDRESS");
		break;
	case 5:
		dbg_dumper(ctx, cmd, "state=CONFIGURED");
		break;
	default:
		dbg_dumper(ctx, cmd, "state=%02x", data);
		break;
	}
}

static void usb_dump_control_stage(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "stage=SETUP");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "stage=DATA");
		break;
	case 2:
		dbg_dumper(ctx, cmd, "stage=STATUS");
		break;
	default:
		dbg_dumper(ctx, cmd, "stage=%02x", data);
		break;
	}
}

static void usb_dump_control_setup(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "req=GET_STATUS");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "req=CLEAR_FEATURE");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "req=SET_FEATURE");
		break;
	case 0x05:
		dbg_dumper(ctx, cmd, "req=SET_ADDRESS");
		break;
	case 0x06:
		dbg_dumper(ctx, cmd, "req=GET_DESCRIPTOR");
		break;
	case 0x07:
		dbg_dumper(ctx, cmd, "req=SET_DESCRIPTOR");
		break;
	case 0x08:
		dbg_dumper(ctx, cmd, "req=GET_CONFIGURATION");
		break;
	case 0x09:
		dbg_dumper(ctx, cmd, "req=SET_CONFIGURATION");
		break;
	case 0x0A:
		dbg_dumper(ctx, cmd, "req=GET_INTERFACE");
		break;
	case 0x0B:
		dbg_dumper(ctx, cmd, "req=SET_INTERFACE");
		break;
	case 0x0C:
		dbg_dumper(ctx, cmd, "req=SYNCH_FRAME");
		break;
	default:
		dbg_dumper(ctx, cmd, "req=%02x", data);
		break;
	}
}

void usb_dump_descriptor(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x01:
		dbg_dumper(ctx, cmd, "desc=DEVICE");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "desc=CONFIGURATION");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "desc=STRING");
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "desc=INTERFACE");
		break;
	case 0x05:
		dbg_dumper(ctx, cmd, "desc=ENDPOINT");
		break;
	case 0x06:
		dbg_dumper(ctx, cmd, "desc=DEVICE_QUALIFIER");
		break;
	case 0x07:
		dbg_dumper(ctx, cmd, "desc=OTHER_SPEED_CONFIGURATION");
		break;
	case 0x08:
		dbg_dumper(ctx, cmd, "desc=INTERFACE_POWER");
		break;
	default:
		dbg_dumper(ctx, cmd, "desc=%02x", data);
		break;
	}
}

static void usb_dump_endpoint(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "eid=%02x", data);
}

static void usb_dump_interface(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "iid=%02x", data);
}

struct dbg_parser dbg_usb_events[NR_USB_EVENTS] = {
	{ "IRQ", 0, usb_dump_irq, },
	{ "STATE", 0, usb_dump_device_state, },
	{ "STAGE", 0, usb_dump_control_stage, },
	{ "SETUP", 0, usb_dump_control_setup, },
	{ "DESC", 0, usb_dump_descriptor, },
	{ "ENDP", 0, usb_dump_endpoint, },
	{ "INTF", 0, usb_dump_interface, },
	{ "CALL", 0, usb_dump_call, },
};

struct dbg_source dbg_usb_state = {
	"usb",
	dbg_usb_events,
	NR_USB_EVENTS,
};

void dbg_usb_init(void)
{
	dbg_register_source(DBG_SRC_USB, &dbg_usb_state);
}
