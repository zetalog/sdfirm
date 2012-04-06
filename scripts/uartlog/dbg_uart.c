#include <host/uartdbg.h>

static void uart_dump_irq(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0:
		dbg_dumper(ctx, cmd, "irq=TX");
		break;
	case 1:
		dbg_dumper(ctx, cmd, "irq=RX");
		break;
	default:
		dbg_dumper(ctx, cmd, "irq=%02x", data);
		break;
	}
}

static void uart_dump_port(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "pid=%02x", data);
}

static void uart_dump_oob_plus(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "%02x", data);
}

static void uart_dump_oob_minus(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	dbg_dumper(ctx, cmd, "%02x", data);
}

struct dbg_parser dbg_uart_events[NR_UART_EVENTS] = {
	{ "IRQ", 0, uart_dump_irq, },
	{ "PORT", 0, uart_dump_port, },
	{ "OOB+", 0, uart_dump_oob_plus, },
	{ "OOB-", 0, uart_dump_oob_minus, },
};

struct dbg_source dbg_uart_state = {
	"uart",
	dbg_uart_events,
	NR_UART_EVENTS,
};

void dbg_uart_init(void)
{
	dbg_register_source(DBG_SRC_UART, &dbg_uart_state);
}
