#include <host/uartdbg.h>

static void hid_dump_control(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x01:
		dbg_dumper(ctx, cmd, "req=GET_REPORT");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "req=GET_IDLE");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "req=GET_PROTOCOL");
		break;
	case 0x09:
		dbg_dumper(ctx, cmd, "req=SET_REPORT");
		break;
	case 0x0A:
		dbg_dumper(ctx, cmd, "req=SET_IDLE");
		break;
	case 0x0B:
		dbg_dumper(ctx, cmd, "req=SET_PROTOCOL");
		break;
	default:
		dbg_dumper(ctx, cmd, "req=%02x", data);
		break;
	}
}

static void hid_dump_descriptor(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x21:
		dbg_dumper(ctx, cmd, "desc=HID");
		break;
	case 0x22:
		dbg_dumper(ctx, cmd, "desc=REPORT");
		break;
	case 0x23:
		dbg_dumper(ctx, cmd, "desc=PHYSICAL");
		break;
	default:
		usb_dump_descriptor(ctx, cmd, data);
		break;
	}
}

static void hid_dump_report(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "rid=%02x", data);
}

static void hid_dump_interrupt(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "intr=PENDING_SET");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "intr=PENDING_UNSET");
		break;
	case 2:
		dbg_dumper(ctx, cmd, "intr=RUNNING_SET");
		break;
	case 3:
		dbg_dumper(ctx, cmd, "intr=RUNNING_UNSET");
		break;
	default:
		dbg_dumper(ctx, cmd, "intr=%02x", data);
		break;
	}
}

struct dbg_parser dbg_hid_events[NR_HID_EVENTS] = {
	{ "CTRL", 0, hid_dump_control },
	{ "DESC", 0, hid_dump_descriptor },
	{ "REPORT", 0, hid_dump_report },
	{ "INTR", 0, hid_dump_interrupt },
};

struct dbg_source dbg_hid_source = {
	"hid",
	dbg_hid_events,
	NR_HID_EVENTS,
};

void dbg_hid_init(void)
{
	dbg_register_source(DBG_SRC_HID, &dbg_hid_source);
}
