#include <host/uartdbg.h>

static unsigned char bare_num_map[] = {
	0x1B,	0x30,	0x0D,	'+',
	0x31,	0x32,	0x33,	'-',
	0x34,	0x35,	0x36,	'x',
	0x37,	0x38,	0x39,	'%',
};

static void keyboard_dump_down(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	if (data < (sizeof (bare_num_map) / sizeof (unsigned char)))
		dbg_dumper(ctx, cmd, "scan=%02x, ascii=%c", data, bare_num_map[data]);
	else
		dbg_dumper(ctx, cmd, "scan=%c", data);
}

static void keyboard_dump_up(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	if (data < (sizeof (bare_num_map) / sizeof (unsigned char)))
		dbg_dumper(ctx, cmd, "scan=%02x, ascii=%c", data, bare_num_map[data]);
	else
		dbg_dumper(ctx, cmd, "scan=%c", data);
}

static void keyboard_dump_wait(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "wait=%s", data ? "true" : "false");
}

struct dbg_parser dbg_io_events[NR_IO_EVENTS] = {
	{ "KEY_DOWN", 0, keyboard_dump_down, },
	{ "KEY_UP", 0, keyboard_dump_up, },
	{ "WAIT_KEY", 0, keyboard_dump_wait, },
};

struct dbg_source dbg_io_source = {
	"io",
	dbg_io_events,
	NR_IO_EVENTS,
};

void dbg_io_init(void)
{
	dbg_register_source(DBG_SRC_IO, &dbg_io_source);
}
