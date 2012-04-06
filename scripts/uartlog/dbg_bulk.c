#include <host/uartdbg.h>

static void bulk_dump_set_flag(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x01:
		dbg_dumper(ctx, cmd, "flag+=SOFT");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "flag+=HART");
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "flag+=BACK");
		break;
	case 0x08:
		dbg_dumper(ctx, cmd, "flag+=SYNC");
		break;
	case 0x80:
		dbg_dumper(ctx, cmd, "flag+=HALT");
		break;
	default:
		dbg_dumper(ctx, cmd, "flag+=%02x", data);
		break;
	}
}

static void bulk_dump_clear_flag(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x01:
		dbg_dumper(ctx, cmd, "flag-=SOFT");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "flag-=HART");
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "flag-=BACK");
		break;
	case 0x08:
		dbg_dumper(ctx, cmd, "flag-=SYNC");
		break;
	case 0x80:
		dbg_dumper(ctx, cmd, "flag+=HALT");
		break;
	default:
		dbg_dumper(ctx, cmd, "flag-=%02x", data);
		break;
	}
}

static void bulk_dump_flow(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "flow=OPEN");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "flow=CLOSE");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "flow=START");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "flow=STOP");
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "flow=HALT");
		break;
	case 0x05:
		dbg_dumper(ctx, cmd, "flow=UNHALT");
		break;
	default:
		dbg_dumper(ctx, cmd, "flow=%02x", data);
		break;
	}
}

struct dbg_parser dbg_bulk_events[NR_BULK_EVENTS] = {
	{ "FLAG+", 0, bulk_dump_set_flag, },
	{ "FLAG-", 0, bulk_dump_clear_flag, },
	{ "FLOW-", 0, bulk_dump_flow, },
};

struct dbg_source dbg_bulk_state = {
	"bulk",
	dbg_bulk_events,
	NR_BULK_EVENTS,
};

void dbg_bulk_init(void)
{
	dbg_register_source(DBG_SRC_BULK, &dbg_bulk_state);
}
