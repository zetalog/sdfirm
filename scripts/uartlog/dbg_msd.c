#include <host/uartdbg.h>

static void msd_dump_control(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "req=ADSC (CBI)");
		break;
	case 0xFC:
		dbg_dumper(ctx, cmd, "req=GET_REQUESTS (LSD)");
		break;
	case 0xFD:
		dbg_dumper(ctx, cmd, "req=SET_REQUESTS (LSD)");
		break;
	case 0xFE:
		dbg_dumper(ctx, cmd, "req=GET_MAX_LUNS (BBB)");
		break;
	case 0xFF:
		dbg_dumper(ctx, cmd, "req=BULK_ONLY_MASS_STORAGE_RESET (BBB)");
		break;
	default:
		dbg_dumper(ctx, cmd, "req=%02x", data);
		break;
	}
}

static void msd_dump_state(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "state=CBW");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "state=DATA");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "state=CSW");
		break;
	default:
		dbg_dumper(ctx, cmd, "state=%02x", data);
		break;
	}
}

static void msd_dump_halt(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "halt=%s", data ? "true" : "false");
}

struct dbg_parser dbg_msd_events[NR_MSD_EVENTS] = {
	{ "CTRL", 0, msd_dump_control, },
	{ "STATE", 0, msd_dump_state, },
	{ "HALT", 0, msd_dump_halt, },
};

struct dbg_source dbg_msd_source = {
	"msd",
	dbg_msd_events,
	NR_MSD_EVENTS,
};

void dbg_msd_init(void)
{
	dbg_register_source(DBG_SRC_MSD, &dbg_msd_source);
}
