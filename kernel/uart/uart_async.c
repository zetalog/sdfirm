#include <target/uart.h>

uart_port_t *uart_ports[NR_UART_PORTS];
uart_pid_t uart_nr_ports;
uart_pid_t uart_pid;

struct uart_state uart_states[NR_UART_PORTS];
DECLARE_BITMAP(uart_port_regs, NR_UART_PORTS);

void uart_port_restore(uart_pid_t pid)
{
	uart_pid = pid;
}

uart_pid_t uart_port_save(uart_pid_t pid)
{
	uart_pid_t spid = uart_pid;
	uart_port_restore(pid);
	return spid;
}

boolean uart_started(uart_pid_t pid)
{
	return test_bit(pid, uart_port_regs);
}

bulk_cid_t uart_bulk_tx(void)
{
	return uart_states[uart_pid].bulk_tx;
}

bulk_cid_t uart_bulk_rx(void)
{
	return uart_states[uart_pid].bulk_rx;
}

uart_pid_t uart_startup(bulk_user_t *txusr, uint8_t *txbuf, int txlen,
			bulk_user_t *rxusr, uint8_t *rxbuf, int rxlen)
{
	uart_port_t *port;
	uart_pid_t pid;

	for (pid = 0; pid < NR_UART_PORTS; pid++) {
		if (!uart_started(pid)) {
			uart_port_select(pid);
			port = uart_ports[uart_pid];
			bulk_open_channel(uart_states[uart_pid].bulk_tx,
					  txusr, txbuf, txlen, 1);
			bulk_open_channel(uart_states[uart_pid].bulk_rx,
					  rxusr, rxbuf, rxlen, 1);
			BUG_ON(!port || !port->startup);
			port->startup();
			set_bit(pid, uart_port_regs);
			break;
		}
	}

	return pid;
}

void uart_cleanup(uart_pid_t pid)
{
	uart_port_t *port;

	uart_port_select(pid);
	port = uart_ports[uart_pid];
	BUG_ON(!port || !port->cleanup);
	port->cleanup();
	bulk_close_channel(uart_states[uart_pid].bulk_tx);
	bulk_close_channel(uart_states[uart_pid].bulk_rx);
	clear_bit(pid, uart_port_regs);
}

int uart_put_char(uint8_t c)
{
	return bulk_write_byte(uart_states[uart_pid].bulk_tx, c);
}

int uart_write(const uint8_t *buf, int count)
{
	uart_port_t *port = uart_ports[uart_pid];
	int ret = 0;

	ret = bulk_write_buffer(uart_states[uart_pid].bulk_tx,
				buf, count);
	/* uart_start(); */

	return ret;
}

void uart_config_port(uint8_t params, uint32_t baudrate)
{
	uart_port_t *port = uart_ports[uart_pid];

	BUG_ON(!port || !port->config);
	port->config(params, baudrate);
}

bulk_channel_t uart_tx_channel = {
	O_WRONLY,
};

bulk_channel_t uart_rx_channel = {
	O_RDONLY,
};

uart_pid_t uart_register_port(uart_port_t *port)
{
	uart_pid_t pid = uart_nr_ports;

	BUG_ON(pid >= NR_UART_PORTS);
	uart_ports[pid] = port;
	uart_states[pid].bulk_tx = bulk_register_channel(&uart_tx_channel);
	uart_states[pid].bulk_rx = bulk_register_channel(&uart_rx_channel);
	uart_nr_ports++;

	return pid;
}

void uart_async_init(void)
{
}
