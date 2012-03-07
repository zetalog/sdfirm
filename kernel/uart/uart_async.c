#include <target/uart.h>

const uart_port_t *uart_ports[NR_UART_PORTS];
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

bulk_cid_t uart_bulk_out(void)
{
	return uart_states[uart_pid].bulk_out;
}

bulk_cid_t uart_bulk_in(void)
{
	return uart_states[uart_pid].bulk_in;
}

uart_pid_t uart_startup(uint8_t *inbuf, int inlen,
			uint8_t *outbuf, int outlen)
{
	uart_port_t *port;
	uart_pid_t pid;

	for (pid = 0; pid < NR_UART_PORTS; pid++) {
		if (!uart_started(pid)) {
			uart_port_select(pid);
			port = uart_ports[uart_pid];
			uart_states[uart_pid].bulk_out =
				bulk_set_buffer(outbuf, outlen);
			uart_states[uart_pid].bulk_in =
				bulk_set_buffer(inbuf, inlen);
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
	bulk_clear_buffer(uart_states[uart_pid].bulk_out);
	bulk_clear_buffer(uart_states[uart_pid].bulk_in);
	clear_bit(pid, uart_port_regs);
}

int uart_put_char(uint8_t c)
{
	return bulk_write_byte(uart_states[uart_pid].bulk_out, c);
}

int uart_write(const uint8_t *buf, int count)
{
	uart_port_t *port = uart_ports[uart_pid];
	int ret = 0;

	ret = bulk_write_buffer(uart_states[uart_pid].bulk_out,
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

uart_pid_t uart_register_port(const uart_port_t *port)
{
	uart_pid_t pid = uart_nr_ports;

	BUG_ON(pid >= NR_UART_PORTS);
	uart_ports[pid] = port;
	uart_nr_ports++;

	return pid;
}

void uart_async_init(void)
{
}
