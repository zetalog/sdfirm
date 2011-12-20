#include "mach.h"
#include <host/uartdbg.h>

static void sim_uart_dbg_last(void *ctx,
			      const unsigned char *newbuf,
			      size_t newlen)
{
#if 0
	size_t index;
	FILE *fp = (FILE *)ctx;

	for (index = 0; index < newlen; index++) {
		fprintf(fp, "%02x ", newbuf[index]);
	}
#endif
}

static void sim_uart_dbg_dump(void *ctx, dbg_cmd_t cmd,
			      const char *fmt, ...)
{
	FILE *fp = (FILE *)ctx;
	va_list args;
	unsigned char buf[1024];
	
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	fprintf(fp, "<%s> %s: ",
		dbg_state_name(cmd), dbg_event_name(cmd));
	fprintf(fp, "%s", buf);
	fprintf(fp, "\r\n");
}

void sim_uart_write_byte(uint8_t byte)
{
	dbg_process_log(stdout, byte);
}

void sim_uart_init(void)
{
	dbg_init();
	dbg_register_output(sim_uart_dbg_dump, sim_uart_dbg_last);
}
