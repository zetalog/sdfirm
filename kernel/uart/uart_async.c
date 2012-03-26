#include <target/uart.h>
#include <target/state.h>

uart_port_t *uart_ports[NR_UART_PORTS];
uart_pid_t uart_nr_ports;
struct uart_state uart_states[NR_UART_PORTS];
DECLARE_BITMAP(uart_port_regs, NR_UART_PORTS);
sid_t uart_sid;
uart_pid_t uart_pid;

void uart_hw_port_startup(uart_pid_t pid,
			  uint8_t params, uint32_t baudrate)
{
	uart_port_t *port;

	port = uart_ports[pid];
	BUG_ON(!port || !port->startup || !port->config);
	port->config(params, baudrate);
	port->startup();
}

void uart_hw_port_cleanup(uart_pid_t pid)
{
	uart_port_t *port;

	port = uart_ports[pid];
	BUG_ON(!port || !port->cleanup);
	port->cleanup();
}

static void uart_hw_port_read(uint8_t *c)
{
	uart_port_t *port;

	port = uart_ports[uart_pid];
	BUG_ON(!port || !port->rxxmit);
	port->rxxmit(c);
}

static boolean uart_hw_port_poll(void)
{
	uart_port_t *port;

	port = uart_ports[uart_pid];
	BUG_ON(!port || !port->rxpoll);
	return port->rxpoll();
}

static void uart_hw_port_start(void)
{
	uart_port_t *port;

	port = uart_ports[uart_pid];
	BUG_ON(!port || !port->rxstart);
	port->rxstart();
}

static void uart_hw_port_stop(void)
{
	uart_port_t *port;

	port = uart_ports[uart_pid];
	BUG_ON(!port || !port->rxstop);
	port->rxstop();
}

static void uart_oob_init(uart_pid_t pid, uart_user_t *user)
{
	uart_states[pid].sync_buf = user->sync_buf;
	uart_states[pid].sync_func = user->sync_func;
	uart_states[pid].sync_size = user->sync_size;
	uart_states[pid].sync_len = 0;
}

static void uart_oob_exit(uart_pid_t pid)
{
	uart_states[pid].sync_len = 0;
	uart_states[pid].sync_size = 0;
	uart_states[pid].sync_func = NULL;
	uart_states[pid].sync_buf = NULL;
}

static void uart_oob_sync(uart_pid_t pid)
{
	uint8_t c;
	uart_sync_cb sync = uart_states[pid].sync_func;
	bulk_size_t len = uart_states[pid].sync_len;
	bulk_size_t size = uart_states[pid].sync_size;
	uint8_t *buf = uart_states[pid].sync_buf;

	BUG_ON(len >= size || !sync);

	uart_hw_port_read(&c);
	buf[len++] = c;
	if (len == uart_states[pid].sync_size) {
		if (sync(buf)) {
			uart_states[pid].sync_len = len;
			bulk_channel_unhalt(uart_bulk_rx(pid));
			state_wakeup(uart_sid);
		} else {
			len--;
			if (len) {
				memory_copy((caddr_t)buf,
					    (caddr_t)(buf+1), len);
			}
		}
	}
	uart_states[pid].sync_len = len;
}

void uart_async_halt(void)
{
	uart_states[uart_pid].sync_len = 0;
}

void uart_async_start(void)
{
	if (uart_states[uart_pid].sync_len) {
		BUG_ON(bulk_channel_halting());
		bulk_transfer_submit(1);
	} else {
		uart_hw_port_start();
	}
}

void uart_async_stop(void)
{
	if (uart_states[uart_pid].sync_len) {
		BUG_ON(bulk_channel_halting());
		bulk_transfer_submit(1);
	} else {
		uart_hw_port_stop();
	}
}

boolean uart_async_poll(void)
{
	if (uart_states[uart_pid].sync_len) {
		return true;
	} else {
		return uart_hw_port_poll();
	}
}

void uart_async_read(uint8_t *byte)
{
	if (uart_states[uart_pid].sync_len) {
		uint8_t *buf = uart_states[uart_pid].sync_buf;
		bulk_size_t len = uart_states[uart_pid].sync_len;

		*byte = buf[0];
		len--;
		if (len) {
			memory_copy((caddr_t)buf,
				    (caddr_t)(buf+1), len);
		}
		uart_states[uart_pid].sync_len = len;
	} else {
		uart_hw_port_read(byte);
	}
}

void uart_async_select(void)
{
	uart_pid_t pid;

	for (pid = 0; pid < uart_nr_ports; pid++) {
		if (uart_bulk_rx(pid) == bulk_cid ||
		    uart_bulk_tx(pid) == bulk_cid)
			uart_pid = pid;
	}
}

void uart_startup(uart_pid_t pid, uart_user_t *user)
{
	BUG_ON(!user->rxbuf || !user->rxlen);

	if (test_bit(pid, uart_port_regs)) {
		uart_hw_port_startup(pid,
				     user->params,
				     user->baudrate);
		bulk_open_channel(uart_bulk_tx(pid),
				  user->txusr,
				  user->txbuf,
				  user->txlen, 1);
		bulk_open_channel(uart_bulk_rx(pid),
				  user->rxusr,
				  user->rxbuf,
				  user->rxlen, 1);
		uart_oob_init(pid, user);
	}
}

void uart_cleanup(uart_pid_t pid)
{
	uart_oob_exit(pid);
	bulk_close_channel(uart_bulk_rx(pid));
	bulk_close_channel(uart_bulk_tx(pid));
	uart_hw_port_cleanup(pid);
	clear_bit(pid, uart_port_regs);
}

void uart_read_submit(uart_pid_t pid, bulk_size_t size)
{
	uart_read_select(pid);
	if (!bulk_channel_halting()) {
		bulk_transfer_submit(size);
	}
}

void uart_read_byte(uart_pid_t pid)
{
	uart_read_select(pid);
	if (bulk_channel_halting()) {
		uart_oob_sync(pid);
	} else {
		bulk_transfer_read();
	}
}

void uart_write_byte(uart_pid_t pid)
{
	uart_write_select(pid);
	if (bulk_request_unhandled() > 0)
		bulk_transfer_write();
}

#ifdef CONFIG_UART_WAIT
void uart_wait_start(uart_pid_t pid, timeout_t tout_ms,
		     io_cb cb_func, void *cb_data)
{
	uart_states[pid].cb_func = cb_func;
	uart_states[pid].cb_data = cb_data;
}

void uart_wait_stop(uart_pid_t pid)
{
	uart_states[pid].cb_func = NULL;
	uart_states[pid].cb_data = NULL;
}

void uart_wait_timeout(uart_pid_t pid)
{
}
#endif

uart_pid_t uart_register_port(uart_port_t *port)
{
	uart_pid_t pid = uart_nr_ports;

	BUG_ON(pid >= NR_UART_PORTS);
	uart_ports[pid] = port;
	uart_bulk_tx(pid) = bulk_register_channel(port->tx);
	uart_bulk_rx(pid) = bulk_register_channel(port->rx);
	uart_nr_ports++;

	return pid;
}

void uart_async_handler(uint8_t event)
{
	uart_pid_t pid;

	if (event == STATE_EVENT_WAKE) {
		for (pid = 0; pid < uart_nr_ports; pid++) {
			uart_read_select(pid);
			if (!bulk_channel_halting() &&
			    uart_states[pid].sync_len) {
				bulk_transfer_read();
			}
		}
	}
}

void uart_async_init(void)
{
	uart_hw_async_init();
	uart_sid = state_register(uart_async_handler);
}
