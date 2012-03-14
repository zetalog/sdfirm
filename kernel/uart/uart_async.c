#include <target/uart.h>

uart_port_t *uart_ports[NR_UART_PORTS];
uart_pid_t uart_nr_ports;
struct uart_state uart_states[NR_UART_PORTS];
DECLARE_BITMAP(uart_port_regs, NR_UART_PORTS);

static void __uart_sync_init(uart_pid_t pid, uart_user_t *user)
{
	uart_states[pid].sync_buf = user->rxbuf;
	uart_states[pid].sync_func = user->sync_func;
	uart_states[pid].sync_size = user->sync_size;
	uart_states[pid].sync_len = 0;
}

static void __uart_sync_exit(uart_pid_t pid)
{
	uart_states[pid].sync_len = 0;
	uart_states[pid].sync_size = 0;
	uart_states[pid].sync_func = NULL;
	uart_states[pid].sync_buf = NULL;
}

static void __uart_sync(uart_pid_t pid, uint8_t c)
{
	uart_sync_cb sync = uart_states[pid].sync_func;
	bulk_size_t len = uart_states[pid].sync_len;
	bulk_size_t size = uart_states[pid].sync_size;
	uint8_t *buf = uart_states[pid].sync_buf;

	BUG_ON(len >= size || !sync);

	buf[len++] = c;
	if (len == uart_states[pid].sync_size) {
		if (sync(buf)) {
			uart_states[pid].sync_len = len;
			bulk_channel_unhalt(uart_states[pid].bulk_rx, len);
		} else {
			memory_copy(buf, buf+1, len-1);
		}
	} else {
		uart_states[pid].sync_len = len;
	}
}

void uart_hw_port_startup(uart_pid_t pid,
			  uint8_t params, uint32_t baudrate)
{
	uart_port_t *port;

	port = uart_ports[pid];
	BUG_ON(!port || !port->startup || !port->config);
	port->config(params, baudrate);
	port->startup();
}

void uart_startup(uart_pid_t pid, uart_user_t *user)
{
	BUG_ON(!user->rxbuf || !user->rxlen ||
	       (user->sync_size < user_rxlen));

	if (test_bit(pid, uart_port_regs)) {
		uart_hw_port_startup(pid,
				     user->params,
				     user->baudrate);
		bulk_open_channel(uart_states[pid].bulk_tx,
				  user->txusr,
				  user->txbuf,
				  user->txlen, 1);
		bulk_open_channel(uart_states[pid].bulk_rx,
				  user->rxusr,
				  user->rxbuf,
				  user->rxlen, 1);
		__uart_sync_init(pid, user);
	}
}

void uart_cleanup(uart_pid_t pid)
{
	uart_port_t *port;

	__uart_sync_exit(pid);
	bulk_close_channel(uart_states[pid].bulk_rx);
	bulk_close_channel(uart_states[pid].bulk_tx);
	port = uart_ports[pid];
	BUG_ON(!port || !port->cleanup);
	port->cleanup();
	clear_bit(pid, uart_port_regs);
}

void uart_read_sync(uart_pid_t pid)
{
	uart_states[pid].sync_len = 0;
}

void uart_read_submit(uart_pid_t pid, bulk_size_t size)
{
	bulk_cid_t bulk = uart_states[pid].bulk_rx;

	if (!bulk_channel_halting(bulk))
		bulk_transfer_submit(bulk, size);
}

void uart_read_byte(uart_pid_t pid, uint8_t c)
{
	bulk_cid_t bulk = uart_states[pid].bulk_rx;

	if (bulk_channel_halting(bulk)) {
		__uart_sync(pid, c);
	} else {
		bulk_transfer_read(bulk);
	}
}

void uart_write_byte(uart_pid_t pid)
{
	bulk_cid_t bulk = uart_states[pid].bulk_tx;

	if (bulk_request_unhandled(bulk) > 0)
		bulk_transfer_write(bulk);
}

uart_pid_t uart_register_port(uart_port_t *port)
{
	uart_pid_t pid = uart_nr_ports;

	BUG_ON(pid >= NR_UART_PORTS);
	uart_ports[pid] = port;
	uart_states[pid].bulk_tx = bulk_register_channel(port->tx);
	uart_states[pid].bulk_rx = bulk_register_channel(port->rx);
	uart_nr_ports++;

	return pid;
}

void uart_async_init(void)
{
	uart_hw_async_init();
}
