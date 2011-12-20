#include <host/uartdbg.h>

#define DBG_RAW_LINE_BYTES	32

dbg_dump_cb dbg_dumper = NULL;
dbg_last_cb dbg_logger = NULL;

unsigned char dbg_dump_buffer[DBG_RAW_LINE_BYTES];
size_t dbg_dump_size = 0;
size_t dbg_dump_stored = 0;

#define DBG_EVENT_SIZE		3
BYTE dbg_event_buffer[DBG_EVENT_SIZE];
size_t dbg_event_stored;

struct dbg_source *dbg_sources[NR_DBG_UI_SRCS];

static bool dbg_dump_log(void *ctx, const unsigned char *log);
static void dbg_dump_raw(void *ctx);

dbg_storage_cb dbg_store_call = dbg_store_raw;

void dbg_restore_storage(dbg_storage_cb cb)
{
	dbg_store_call = cb;
}

dbg_storage_cb dbg_save_storage(dbg_storage_cb cb)
{
	dbg_storage_cb ocb = dbg_store_call;
	dbg_restore_storage(cb);
	return ocb;
}

static void dbg_dump_raw(void *ctx)
{
	unsigned char buf[DBG_RAW_LINE_BYTES * 3 + 1];
	unsigned int iter = 0;

	assert(dbg_dump_stored <= DBG_RAW_LINE_BYTES);
	memset(buf, 0, sizeof (buf));

	for (iter = 0; iter < min(dbg_dump_stored, DBG_RAW_LINE_BYTES); iter++) {
		sprintf(buf + (iter * 3), "%02x ", dbg_dump_buffer[iter]);
	}
	dbg_dump_stored = 0;
	dbg_dumper(ctx, (DBG_SRC_RAW<<4 | RAW_EVENT_DATA), "%s", buf);
}

void dbg_store_raw(void *ctx, unsigned char log)
{
	assert(dbg_dump_stored <= DBG_RAW_LINE_BYTES);

	dbg_dump_buffer[dbg_dump_stored] = log;
	dbg_dump_stored++;

	if (dbg_dump_stored == DBG_RAW_LINE_BYTES) {
		dbg_dump_raw(ctx);
	}
}

bool dbg_dump_log(void *ctx, const unsigned char *log)
{
	dbg_cmd_t cmd;
	dbg_cmd_t state, event;
	struct dbg_parser *desc;

	cmd = log[0];

	state = DBG_STATE_ID(cmd);
	event = DBG_EVENT_ID(cmd);

	if (state >= NR_DBG_UI_SRCS || !dbg_sources[state] ||
	    event >= dbg_sources[state]->nr_events)
		return FALSE;
	desc = &dbg_sources[state]->events[event];
	if (!desc || !desc->func)
		return FALSE;

	if (dbg_dump_stored)
		dbg_dump_raw(ctx);
	desc->func(ctx, cmd, log[1]);
	return TRUE;
}

void dbg_process_log(void *ctx, uint8_t byte)
{
	size_t iter;
	uint8_t raw;

	if (dbg_logger)
		dbg_logger(ctx, &byte, 1);

	assert(dbg_event_stored < DBG_EVENT_SIZE);

	dbg_event_buffer[dbg_event_stored] = byte;
	dbg_event_stored++;

	if (dbg_event_stored == DBG_EVENT_SIZE) {
		if (dbg_event_buffer[0] == dbg_event_buffer[2]) {
			if (dbg_dump_log(ctx, dbg_event_buffer)) {
				dbg_event_stored = 0;
				return;
			}
		}
		raw = dbg_event_buffer[0];
		dbg_event_stored--;
		for (iter = 0; iter < dbg_event_stored; iter++) {
			dbg_event_buffer[iter] = dbg_event_buffer[iter+1];
		}
		
		dbg_store_call(ctx, raw);
	}
}

const char *dbg_state_name(dbg_cmd_t cmd)
{
	dbg_cmd_t state = DBG_STATE_ID(cmd);

	if (state >= NR_DBG_UI_SRCS || !dbg_sources[state] ||
	    !dbg_sources[state]->name)
		return "dbg";
	return dbg_sources[state]->name;
}

const char *dbg_event_name(dbg_cmd_t cmd)
{
	struct dbg_parser *desc;
	dbg_cmd_t state = DBG_STATE_ID(cmd);
	dbg_cmd_t event = DBG_EVENT_ID(cmd);

	if (state >= NR_DBG_UI_SRCS || !dbg_sources[state])
		return "UNDEF";
	if (event >= dbg_sources[state]->nr_events)
		return "UNDEF";
	desc = &(dbg_sources[state]->events[event]);
	if (!desc || !desc->name)
		return "UNDEF";
	return desc->name;
}

void dbg_register_output(dbg_dump_cb dump, dbg_last_cb last)
{
	dbg_dumper = dump;
	dbg_logger = last;
}

void dbg_register_source(dbg_cmd_t state, struct dbg_source *source)
{
	assert(state < NR_DBG_UI_SRCS);
	dbg_sources[state] = source;
}

static void main_dump_init(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "powered");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "module initialized");
		break;
	case 2:
		dbg_dumper(ctx, cmd, "IRQ enabled");
		break;
	default:
		dbg_dumper(ctx, cmd, "flavour");
		break;
	}
}

unsigned char panic_file[MAX_PATH];
int panic_offset;
unsigned char *panic_buffer = NULL;
dbg_storage_cb panic_storage = NULL;
unsigned char panic_line[2];

static void main_store_panic(void *ctx, unsigned char byte)
{
	panic_buffer[panic_offset] = byte;
	panic_offset++;
}

static void main_dump_panic(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	char *file = NULL;
	size_t line = 0;

	dbg_cmd_t p = (data & 0xF0) >> 4;
	switch (p) {
	case 0:
		dbg_dumper(ctx, cmd, "panic occurs at:");
		panic_offset = 0;
		panic_buffer = panic_file;
		panic_storage = dbg_save_storage(main_store_panic);
		break;
	case 1:
		if (panic_buffer) {
			panic_buffer[panic_offset] = '\0';
			dbg_dumper(ctx, cmd, "file: %s", file);
			dbg_restore_storage(panic_storage);
			panic_offset = 0;
			panic_buffer = panic_line;
			panic_storage = dbg_save_storage(main_store_panic);
		} else {
			dbg_store_raw(ctx, MAIN_DEBUG_PANIC);
			dbg_store_raw(ctx, 1);
			dbg_store_raw(ctx, MAIN_DEBUG_PANIC);
		}
		break;
	case 2:
		line = panic_line[0] + 256 * panic_line[1];
		dbg_dumper(ctx, cmd, "line: %d", line);
		dbg_restore_storage(panic_storage);
		break;
	default:
		dbg_dumper(ctx, cmd, "flavour");
		break;
	}
}

static void dbg_idle_pid(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "pid=%d", data);
}

static void dbg_idle_tid(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "tid=%d", data);
}

struct dbg_parser dbg_main_events[NR_MAIN_EVENTS] = {
	{ "INIT", 0, main_dump_init, },
	{ "PANIC", 0, main_dump_panic, },
};

struct dbg_source dbg_main_source = {
	"init",
	dbg_main_events,
	NR_MAIN_EVENTS,
};

struct dbg_parser dbg_raw_events[NR_RAW_EVENTS] = {
	{ "DATA", 0, NULL, },
	{ "ERROR", 0, NULL, },
};

struct dbg_source dbg_raw_source = {
	"raw",
	dbg_raw_events,
	NR_RAW_EVENTS,
};

struct dbg_parser dbg_idle_events[NR_IDLE_EVENTS] = {
	{ "PID", 0, dbg_idle_pid, },
	{ "TID", 0, dbg_idle_tid, },
};

struct dbg_source dbg_idle_source = {
	"idle",
	dbg_idle_events,
	NR_IDLE_EVENTS,
};

void dbg_init(void)
{
	int i;

	for (i = 0; i < NR_DBG_UI_SRCS; i++)
		dbg_sources[i] = NULL;
	dbg_event_stored = 0;

	dbg_register_source(DBG_SRC_MAIN, &dbg_main_source);
	dbg_register_source(DBG_SRC_IDLE, &dbg_idle_source);
	dbg_register_source(DBG_SRC_RAW, &dbg_raw_source);

	dbg_io_init();
	dbg_usb_init();
	dbg_scd_init();
	dbg_hid_init();
	dbg_scs_init();
	dbg_msd_init();
	dbg_dfu_init();
	//dbg_scsi_init();
}

void dbg_exit(void)
{
}
