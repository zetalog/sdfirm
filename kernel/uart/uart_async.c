#include <target/uart.h>

uart_port_t *uart_ports[NR_UART_PORTS];
uart_pid_t uart_nr_ports;
struct uart_state uart_states[NR_UART_PORTS];
DECLARE_BITMAP(uart_port_regs, NR_UART_PORTS);

uart_pid_t uart_startup(uint8_t params, uint32_t baudrate,
			bulk_user_t *txusr, uint8_t *txbuf, int txlen,
			bulk_user_t *rxusr, uint8_t *rxbuf, int rxlen)
{
	uart_port_t *port;
	uart_pid_t pid;

	for (pid = 0; pid < NR_UART_PORTS; pid++) {
		if (!test_bit(pid, uart_port_regs)) {
			port = uart_ports[pid];
			BUG_ON(!port || !port->startup || !port->config);
			port->config(params, baudrate);
			port->startup();
			bulk_open_channel(uart_states[pid].bulk_tx,
					  txusr, txbuf, txlen, 1);
			bulk_open_channel(uart_states[pid].bulk_rx,
					  rxusr, rxbuf, rxlen, 1);
			set_bit(pid, uart_port_regs);
			break;
		}
	}

	return pid;
}

void uart_cleanup(uart_pid_t pid)
{
	uart_port_t *port;

	bulk_close_channel(uart_states[pid].bulk_tx);
	bulk_close_channel(uart_states[pid].bulk_rx);
	port = uart_ports[pid];
	BUG_ON(!port || !port->cleanup);
	port->cleanup();
	clear_bit(pid, uart_port_regs);
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
