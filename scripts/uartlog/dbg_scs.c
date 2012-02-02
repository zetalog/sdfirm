#include <host/uartdbg.h>

static void scs_dump_convention(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x3B:
		dbg_dumper(ctx, cmd, "conv=direct");
		break;
	case 0x3F:
		dbg_dumper(ctx, cmd, "conv=inverse");
		break;
	default:
		dbg_dumper(ctx, cmd, "conv=unknown");
		break;
	}
}

static void scs_dump_class(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "class=auto");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "class=A (5V)");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "class=B (3V)");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "class=C (1.8V)");
		break;
	default:
		break;
	}
}

static void scs_dump_etu(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "%02x", data);
}

static void scs_dump_wt(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "%02x", data);
}

static void scs_dump_gt(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "%02x", data);
}


static void scs_dump_slot_event(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "event=ICC_IN");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "event=ICC_OUT");
		break;
	case 2:
		dbg_dumper(ctx, cmd, "event=CLS_CHG");
		break;
	case 3:
		dbg_dumper(ctx, cmd, "event=HW_ERR");
		break;
	case 4:
		dbg_dumper(ctx, cmd, "event=PWR_OFF");
		break;
	case 5:
		dbg_dumper(ctx, cmd, "event=PWR_OK");
		break;
	case 6:
		dbg_dumper(ctx, cmd, "event=ATR_OK");
		break;
	case 7:
		dbg_dumper(ctx, cmd, "event=PPS_OK");
		break;
	case 8:
		dbg_dumper(ctx, cmd, "event=WARM_RST");
		break;
	case 15:
		dbg_dumper(ctx, cmd, "event=XFR_SYNC");
		break;
	default:
		dbg_dumper(ctx, cmd, "event=bug");
		break;
	}
}

static void scs_dump_slot_state(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "state=NOTPRESENT");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "state=PRESENT");
		break;
	case 2:
		dbg_dumper(ctx, cmd, "state=HWERROR");
		break;
	case 3:
		dbg_dumper(ctx, cmd, "state=ATR_READY");
		break;
	case 4:
		dbg_dumper(ctx, cmd, "state=PPS_READY");
		break;
	case 5:
		dbg_dumper(ctx, cmd, "state=SELECTED");
		break;
	case 6:
		dbg_dumper(ctx, cmd, "state=ACTIVATED");
		break;
	default:
		dbg_dumper(ctx, cmd, "state=BUG");
		break;
	}
}

static void scs_dump_error(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "error=SUCCESS");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "error=HW_ERROR");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "error=NOTPRESENT");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "error=TIMEOUT");
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "error=BUSY_AUTO");
		break;
	case 0x05:
		dbg_dumper(ctx, cmd, "error=BUSY_SLOT");
		break;
	case 0x06:
		dbg_dumper(ctx, cmd, "error=BAD_CLASS");
		break;
	case 0x07:
		dbg_dumper(ctx, cmd, "error=BAD_T0_PB");
		break;
	case 0x08:
		dbg_dumper(ctx, cmd, "error=BAD_TS");
		break;
	case 0x09:
		dbg_dumper(ctx, cmd, "error=BAD_TCK");
		break;
	case 0x0A:
		dbg_dumper(ctx, cmd, "error=BAD_PROTO");
		break;
	case 0x0B:
		dbg_dumper(ctx, cmd, "error=BAD_FD");
		break;
	case 0x0C:
		dbg_dumper(ctx, cmd, "error=BAD_PPSS");
		break;
	case 0x0D:
		dbg_dumper(ctx, cmd, "error=BAD_PPSR");
		break;
	case 0x0E:
		dbg_dumper(ctx, cmd, "error=BAD_PCK");
		break;
	case 0x0F:
		dbg_dumper(ctx, cmd, "error=OVERRUN");
		break;
	case 0x10:
		dbg_dumper(ctx, cmd, "error=PARITY_ERR");
		break;
	case 0x11:
		dbg_dumper(ctx, cmd, "error=UNSUPPORT");
		break;
	case 0x12:
		dbg_dumper(ctx, cmd, "error=PROGRESS");
		break;
	default:
		dbg_dumper(ctx, cmd, "error=BUG");
	}
}

static void scs_dump_xchg_state(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "state=IDLE");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "state=BUSY");
		break;
	case 2:
		dbg_dumper(ctx, cmd, "state=ERROR");
		break;
	default:
		dbg_dumper(ctx, cmd, "state=BUG");
		break;
	}
}

static void scs_dump_xchg_event(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 1<<0x00:
		dbg_dumper(ctx, cmd, "event=WT_EXPIRE");
		break;
	case 1<<0x01:
		dbg_dumper(ctx, cmd, "event=XFR_CMPL");
		break;
	case 1<<0x02:
		dbg_dumper(ctx, cmd, "event=ICC_OUT");
		break;
	case 1<<0x03:
		dbg_dumper(ctx, cmd, "event=PARITY_ERR");
		break;
	case 1<<0x04:
		dbg_dumper(ctx, cmd, "event=POWER_ERR");
		break;
	case 1<<0x05:
		dbg_dumper(ctx, cmd, "event=CONV_ERR");
		break;
	case 1<<0x06:
		dbg_dumper(ctx, cmd, "event=XFR_OVER");
		break;
	case 1<<0x07:
		dbg_dumper(ctx, cmd, "event=XFR_MARK");
		break;
	default:
		dbg_dumper(ctx, cmd, "event=BUG");
		break;
	}
}

static void scs_dump_sequence(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "sequence=AUTO");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "sequence=POWER_ON");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "sequence=POWER_OFF");
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "sequence=SET_PARAM");
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "sequence=RST_PARAM");
		break;
	case 0x05:
		dbg_dumper(ctx, cmd, "sequence=INTERCHANGE");
		break;
	case 0xFF:
		dbg_dumper(ctx, cmd, "sequence=IDLE");
		break;
	default:
		dbg_dumper(ctx, cmd, "sequence=BUG");
		break;
	}

}

static void scs_dump_slot(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "sid=%02x", data);
}

struct dbg_parser dbg_scs_events[NR_SCS_EVENTS] = {
	{ "SLOT", 0, scs_dump_slot, },
	{ "SLEVENT", 0, scs_dump_slot_event, },
	{ "SLSTATE", 0, scs_dump_slot_state, },
	{ "XGSTATE", 0, scs_dump_xchg_state, },
	{ "XGEVENT", 0, scs_dump_xchg_event, },
	{ "SEQ", 0, scs_dump_sequence, },
	{ "ERROR", 0, scs_dump_error, },
	{ "CLASS", 0, scs_dump_class, },
	{ "CONV", 0, scs_dump_convention, },
	{ "CONF_ETU", 0, scs_dump_etu, },
	{ "CONF_WT", 0, scs_dump_wt, },
	{ "CONF_GT", 0, scs_dump_gt, },
};

struct dbg_source dbg_scs_source = {
	"scs",
	dbg_scs_events,
	NR_SCS_EVENTS,
};

void dbg_scs_init(void)
{
	dbg_register_source(DBG_SRC_SCS, &dbg_scs_source);
}
