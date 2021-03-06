#include <target/uart.h>

uart_port_t *uart_ports[NR_UART_PORTS];
uart_pid_t uart_nr_ports;
struct uart_state uart_states[NR_UART_PORTS];
DECLARE_BITMAP(uart_port_regs, NR_UART_PORTS);
uart_pid_t uart_pid;

void uart_hw_oob_open(uart_pid_t pid)
{
	uart_port_t *port;

	port = uart_ports[pid];
	BUG_ON(!port || !port->rx || !port->rx->open);
	port->rx->open();
}

void uart_hw_oob_close(uart_pid_t pid)
{
	uart_port_t *port;

	port = uart_ports[pid];
	BUG_ON(!port || !port->rx || !port->rx->close);
	port->rx->close();
}

void uart_hw_port_startup(uart_pid_t pid,
			  uint8_t params, uint32_t baudrate)
{
	uart_port_t *port;

	port = uart_ports[pid];
	BUG_ON(!port || !port->config ||
	       !port->startup || !port->cleanup);
	port->cleanup();
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
	if (user->sync_size > 0)
		bulk_channel_halt(uart_bulk_rx(pid));
}

static void uart_oob_exit(uart_pid_t pid)
{
	uart_states[pid].sync_len = 0;
	uart_states[pid].sync_size = 0;
	uart_states[pid].sync_func = NULL;
	uart_states[pid].sync_buf = NULL;
}

static boolean uart_oob_sync(uart_pid_t pid)
{
	uint8_t c;
	uart_sync_cb sync = uart_states[pid].sync_func;
	bulk_size_t len = uart_states[pid].sync_len;
	bulk_size_t size = uart_states[pid].sync_size;
	uint8_t *buf = uart_states[pid].sync_buf;
	boolean synced = false;

	BUG_ON(len >= size || !sync);

	while (uart_hw_port_poll() && !synced) {
		uart_hw_port_read(&c);
		uart_debug(UART_DEBUG_OOB, c);
		buf[len++] = c;
		if (len == uart_states[pid].sync_size) {
			if (sync(buf)) {
				uart_states[pid].sync_len = len;
				bulk_channel_unhalt(uart_bulk_rx(pid));
				return true;
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
	return false;
}

void uart_async_halt(void)
{
	uart_states[uart_pid].sync_len = 0;
	uart_hw_oob_open(uart_pid);
}

void uart_async_unhalt(void)
{
	uart_hw_oob_close(uart_pid);
}

boolean uart_read_interrupting(uart_pid_t pid)
{
	bulk_cid_t cid = uart_bulk_rx(pid);
	return bulk_request_interrupting(cid) ||
	       __bulk_channel_halting(cid);
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
		uint8_t c;
		uint8_t *buf = uart_states[uart_pid].sync_buf;
		bulk_size_t len = uart_states[uart_pid].sync_len;

		c = buf[0];
		uart_debug(UART_DEBUG_UNOOB, c);
		*byte = c;
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

void uart_select_port(uart_pid_t pid)
{
	uart_pid = pid;
}

void uart_async_select(void)
{
	uart_pid_t pid;

	for (pid = 0; pid < uart_nr_ports; pid++) {
		if (uart_bulk_rx(pid) == bulk_cid ||
		    uart_bulk_tx(pid) == bulk_cid) {
			uart_select_port(pid);
			break;
		}
	}
}

void uart_async_bh(void)
{
	if (bulk_cid == uart_bulk_rx(uart_pid)) {
		uart_read_byte(uart_pid);
	}
	if (bulk_cid == uart_bulk_tx(uart_pid)) {
		uart_write_byte(uart_pid);
	}
}

void uart_startup(uart_pid_t pid, uart_user_t *user)
{
	BUG_ON(test_bit(pid, uart_port_regs));

	bulk_select_channel(uart_bulk_rx(pid));
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

void uart_cleanup(uart_pid_t pid)
{
	bulk_select_channel(uart_bulk_rx(pid));
	uart_oob_exit(pid);
	bulk_close_channel(uart_bulk_rx(pid));
	bulk_close_channel(uart_bulk_tx(pid));
	uart_hw_port_cleanup(pid);
	clear_bit(pid, uart_port_regs);
}

void uart_read_submit(bulk_size_t size)
{
	if (!bulk_channel_halting()) {
		bulk_transfer_submit(size);
	}
}

void uart_read_byte(uart_pid_t pid)
{
	bulk_cid_t cid = uart_bulk_rx(pid);

	uart_debug_irq(UART_IRQ_RX);
	uart_debug_pid(pid);

	if (__bulk_channel_halting(cid)) {
sync:
		if (uart_oob_sync(pid))
			goto bulk;
	} else {
bulk:
		bulk_transfer_read(cid);
		if (__bulk_channel_halting(cid)) {
			/* XXX: Bulk Channel Halt
			 *
			 * Note that bulk may disable IRQ after a halted
			 * bulk_transfer_iocb, in case of UART, we need to
			 * reenable the IRQ for UART OOB synchronization.
			 */
			uart_async_halt();
			goto sync;
		}
	}
}

void uart_write_byte(uart_pid_t pid)
{
	uart_debug_irq(UART_IRQ_TX);
	uart_debug_pid(pid);
	bulk_transfer_write(uart_bulk_tx(pid));
}

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

void uart_init(void)
{
	uart_hw_ctrl_init();
}
