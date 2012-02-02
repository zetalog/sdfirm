#include <host/uartdbg.h>

static void dfu_dump_control(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "req=DETACH");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "req=DNLOAD");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "req=UPLOAD");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "req=GETSTATUS");
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "req=CLRSTATUS");
		break;
	case 0x05:
		dbg_dumper(ctx, cmd, "req=GETSTATE");
		break;
	case 0x06:
		dbg_dumper(ctx, cmd, "req=ABORT");
		break;
	default:
		dbg_dumper(ctx, cmd, "req=%02x", data);
		break;
	}
}

static void dfu_dump_state(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "req=appIDLE");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "req=appDETACH");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "req=dfuIDLE");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "req=dfuDNLOAD_SYNC");
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "req=dfuDNBUSY");
		break;
	case 0x05:
		dbg_dumper(ctx, cmd, "req=dfuDNLOAD_IDLE");
		break;
	case 0x06:
		dbg_dumper(ctx, cmd, "req=dfuMANIFEST_SYNC");
		break;
	case 0x07:
		dbg_dumper(ctx, cmd, "req=dfuMANIFEST");
		break;
	case 0x08:
		dbg_dumper(ctx, cmd, "req=dfuMANIFEST_WAIT_RESET");
		break;
	case 0x09:
		dbg_dumper(ctx, cmd, "req=dfuUPLOAD_IDLE");
		break;
	case 0x0A:
		dbg_dumper(ctx, cmd, "req=dfuERROR");
		break;
	default:
		dbg_dumper(ctx, cmd, "state=%02x", data);
		break;
	}
}

static void dfu_dump_status(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "status=OK");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "status=errTARGET");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "status=errFILE");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "status=errWRITE");
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "status=errERASE");
		break;
	case 0x05:
		dbg_dumper(ctx, cmd, "status=errCHECK_ERASED");
		break;
	case 0x06:
		dbg_dumper(ctx, cmd, "status=errPROG");
		break;
	case 0x07:
		dbg_dumper(ctx, cmd, "status=errVERIFY");
		break;
	case 0x08:
		dbg_dumper(ctx, cmd, "status=errADDRESS");
		break;
	case 0x09:
		dbg_dumper(ctx, cmd, "status=errNOTDONE");
		break;
	case 0x0A:
		dbg_dumper(ctx, cmd, "status=errFIRMWARE");
		break;
	case 0x0B:
		dbg_dumper(ctx, cmd, "status=errVENDOR");
		break;
	case 0x0C:
		dbg_dumper(ctx, cmd, "status=errUSER");
		break;
	case 0x0D:
		dbg_dumper(ctx, cmd, "status=errPOR");
		break;
	case 0x0E:
		dbg_dumper(ctx, cmd, "status=errUNKNOWN");
		break;
	case 0x0F:
		dbg_dumper(ctx, cmd, "status=errSTALLEDPKT");
		break;
	default:
		dbg_dumper(ctx, cmd, "status=%02x", data);
		break;
	}
}

struct dbg_parser dbg_dfu_events[NR_DFU_EVENTS] = {
	{ "CTRL", 0, dfu_dump_control },
	{ "STATE", 0, dfu_dump_state },
	{ "STATUS", 0, dfu_dump_status },
};

struct dbg_source dbg_dfu_source = {
	"dfu",
	dbg_dfu_events,
	NR_DFU_EVENTS,
};

void dbg_dfu_init(void)
{
	dbg_register_source(DBG_SRC_DFU, &dbg_dfu_source);
}
