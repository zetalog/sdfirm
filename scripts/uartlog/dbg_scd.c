#include <host/uartdbg.h>

static void scd_dump_control(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x01:
		dbg_dumper(ctx, cmd, "req=ABORT");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "req=GET_CLOCK_FREQUENCIES");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "req=GET_DATA_RATES");
		break;
	default:
		dbg_dumper(ctx, cmd, "req=%02x", data);
		break;
	}
}

static void scd_dump_slot(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "sid=%02x", data);
}

static void scd_dump_interrupt(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "intr=RUNNING_SET");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "intr=RUNNING_UNSET");
		break;
	case 2:
		dbg_dumper(ctx, cmd, "intr=PENDING_SET");
		break;
	case 3:
		dbg_dumper(ctx, cmd, "intr=PENDING_UNSET");
		break;
	case 4:
		dbg_dumper(ctx, cmd, "intr=ICC_PRESENT");
		break;
	case 5:
		dbg_dumper(ctx, cmd, "intr=ICC_NOTPRESENT");
		break;
	default:
		dbg_dumper(ctx, cmd, "intr=%02x", data);
		break;
	}
}

static void scd_dump_digit(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "digit=%02x", data);
}

static void scd_dump_abort(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "abort=CTRL_BEGIN", data);
		break;
	case 1:
		dbg_dumper(ctx, cmd, "abort=BULK_BEGIN", data);
		break;
	case 2:
		dbg_dumper(ctx, cmd, "abort=CTRL_END", data);
		break;
	case 3:
		dbg_dumper(ctx, cmd, "abort=BULK_END", data);
		break;
	case 4:
		dbg_dumper(ctx, cmd, "abort=KBD", data);
		break;
	case 5:
		dbg_dumper(ctx, cmd, "abort=XFR", data);
		break;
	default:
		dbg_dumper(ctx, cmd, "abort=XFR", data);
		break;
	}
}

static void scd_dump_ins(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "ins=%02x", data);
}

static void scd_dump_state(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "state=PC2RDR", data);
		break;
	case 1:
		dbg_dumper(ctx, cmd, "state=ISO7816", data);
		break;
	case 2:
		dbg_dumper(ctx, cmd, "state=RDR2PC", data);
		break;
	case 3:
		dbg_dumper(ctx, cmd, "state=SANITY", data);
		break;
	default:
		dbg_dumper(ctx, cmd, "state=%02x", data);
		break;
	}
}

static void scd_dump_command(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x61:
		dbg_dumper(ctx, cmd, "cmd=SetParameters");
		break;
	case 0x62:
		dbg_dumper(ctx, cmd, "cmd=IccPowerOn");
		break;
	case 0x63:
		dbg_dumper(ctx, cmd, "cmd=IccPowerOff");
		break;
	case 0x65:
		dbg_dumper(ctx, cmd, "cmd=GetSlotStatus");
		break;
	case 0x69:
		dbg_dumper(ctx, cmd, "cmd=Secure");
		break;
	case 0x6A:
		dbg_dumper(ctx, cmd, "cmd=T0APDU");
		break;
	case 0x6B:
		dbg_dumper(ctx, cmd, "cmd=Escape");
		break;
	case 0x6C:
		dbg_dumper(ctx, cmd, "cmd=GetParameters");
		break;
	case 0x6D:
		dbg_dumper(ctx, cmd, "cmd=ResetParameters");
		break;
	case 0x6E:
		dbg_dumper(ctx, cmd, "cmd=IccClock");
		break;
	case 0x6F:
		dbg_dumper(ctx, cmd, "cmd=XfrBlock");
		break;
	case 0x71:
		dbg_dumper(ctx, cmd, "cmd=Mechanical");
		break;
	case 0x72:
		dbg_dumper(ctx, cmd, "cmd=Abort");
		break;
	case 0x73:
		dbg_dumper(ctx, cmd, "cmd=SetDataRateAndClock");
		break;
	default:
		dbg_dumper(ctx, cmd, "cmd=%02x", data);
		break;
	}
}

static void scd_dump_response(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
}

struct dbg_parser dbg_scd_events[NR_SCD_EVENTS] = {
	{ "CTRL", 0, scd_dump_control,},
	{ "PC2RDR", 0, scd_dump_command,},
	{ "RDR2PC", 0, scd_dump_response,},
	{ "SLOT", 0, scd_dump_slot,},
	{ "INTR", 0, scd_dump_interrupt,},
	{ "STATE", 0, scd_dump_state,},
	{ "DIGIT", 0, scd_dump_digit,},
	{ "INS", 0, scd_dump_ins,},
	{ "ABORT", 0, scd_dump_abort,},
};

struct dbg_source dbg_scd_source = {
	"scd",
	dbg_scd_events,
	NR_SCD_EVENTS,
};

void dbg_scd_init(void)
{
	dbg_register_source(DBG_SRC_SCD, &dbg_scd_source);
}
