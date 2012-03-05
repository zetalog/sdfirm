#include <target/uart.h>
#include <target/generic.h>

struct uart_port uart_ports[NR_UART_PORTS];
uart_pid_t uart_nr_ports;
uart_pid_t uart_pid;

void uart_write_wakeup(void)
{
	bulk_schedule_write(uart_ports[uart_pid].bulk);
}
