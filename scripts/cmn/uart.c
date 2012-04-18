#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/poll.h>
#include <host/uart.h>
#ifdef WIN32
#include <windows.h>
#include <io.h>
#endif
#include <host/bitops.h>

#ifdef WIN32
#define UART_MAX_BUF	16
#define UART_MAX_PORTS	1

struct uart {
	int id;
	HANDLE hdl;
	int state;
#define UART_OPENED	0x00
#define UART_CLOSED	0x01
#define UART_HALTED	0x02
	HANDLE rx_evt;
	int rx_pend;
	OVERLAPPED rx_olp;
	DWORD rx_cb;
	HANDLE tx_evt;
	int tx_pend;
	OVERLAPPED tx_olp;
	DWORD tx_cb;
	unsigned char rx_buf[UART_MAX_BUF];
	unsigned char tx_buf[UART_MAX_BUF];
};

struct uart uart_ports[UART_MAX_PORTS];
DECLARE_BITMAP(uart_port_regs, UART_MAX_PORTS);
int uart_nr_used = 0;

uart_t __uart_open(uart_t uart, int id)
{
	char port[MAX_PATH];
	HANDLE hdl = INVALID_HANDLE_VALUE;
	HANDLE rxevt = NULL, txevt = NULL;

	if (id > 9) {
		sprintf(port, "\\\\.\\COM%d", id+1);
	} else {
		sprintf(port, "COM%d", id+1);
	}
	hdl = CreateFile(port, GENERIC_READ | GENERIC_WRITE,
			 0, 0, OPEN_EXISTING,
			 0/*FILE_FLAG_OVERLAPPED*/, 0);
	rxevt = CreateEvent(NULL, FALSE, TRUE, NULL);
	txevt = CreateEvent(NULL, FALSE, TRUE, NULL);
	if ((hdl == INVALID_HANDLE_VALUE) || !rxevt || !txevt)
		goto failure;

	uart_ports[uart].hdl = hdl;
	uart_ports[uart].rx_evt = rxevt;
	uart_ports[uart].tx_evt = txevt;
	uart_ports[uart].rx_pend = FALSE;
	uart_ports[uart].tx_pend = FALSE;
	uart_ports[uart].rx_olp.hEvent = uart_ports[uart].rx_evt;
	uart_ports[uart].tx_olp.hEvent = uart_ports[uart].tx_evt;

	return uart;
failure:
	if (hdl != INVALID_HANDLE_VALUE)
		CloseHandle(hdl);
	if (rxevt)
		CloseHandle(rxevt);
	if (txevt)
		CloseHandle(txevt);
	return -1;
}

void __uart_close(uart_t uart)
{
	if (uart_ports[uart].hdl) {
		CloseHandle(uart_ports[uart].hdl);
		uart_ports[uart].hdl = NULL;
	}
	if (uart_ports[uart].rx_evt) {
		CloseHandle(uart_ports[uart].rx_evt);
		uart_ports[uart].rx_evt = NULL;
	}
	if (uart_ports[uart].tx_evt) {
		CloseHandle(uart_ports[uart].tx_evt);
		uart_ports[uart].tx_evt = NULL;
	}
}

int uart_write(uart_t uart, void *buf, size_t len)
{
	BOOL res;
	DWORD cb;
	HANDLE hdl = uart_ports[uart].hdl;
#if 0
	DWORD ret;
	DWORD err;
	int bytes = 0;
	DWORD pending_bytes;

	ret = WaitForSingleObject(uart_ports[uart].tx_evt, 0);
	if (uart_ports[uart].tx_pend && ret != WAIT_OBJECT_0) {
		/* no event, go slow path */
		return bytes;
	}
	if (uart_ports[uart].tx_pend) {
		res = GetOverlappedResult(hdl,
					  &uart_ports[uart].tx_olp,
					  &cb, FALSE);
		if (!res)
			goto failure;
		if (cb < uart_ports[uart].tx_cb) {
			memmove(uart_ports[uart].tx_buf,
				uart_ports[uart].tx_buf+cb,
				cb);
		}
		uart_ports[uart].tx_cb -= cb;
		uart_ports[uart].tx_pend = FALSE;
	}
	pending_bytes = UART_MAX_BUF - uart_ports[uart].tx_cb;
	if (pending_bytes > 0) {
		DWORD min_cb;
		min_cb = min(len, pending_bytes);
		memcpy(uart_ports[uart].tx_buf + uart_ports[uart].tx_cb,
		       buf, min_cb);
		uart_ports[uart].tx_cb += min_cb;
		len -= min_cb;
		bytes += min_cb;
	}
again:
	res = WriteFile(hdl,
			uart_ports[uart].tx_buf,
			uart_ports[uart].tx_cb,
			&cb,
			&uart_ports[uart].tx_olp);
	if (res) {
		if (cb < uart_ports[uart].tx_cb) {
			memmove(uart_ports[uart].tx_buf,
				uart_ports[uart].tx_buf+cb,
				cb);
		}
		uart_ports[uart].tx_cb -= cb;
		if (uart_ports[uart].tx_cb)
			return bytes;
	}
failure:
	err = GetLastError();
	if (err == ERROR_IO_PENDING) {
		uart_ports[uart].tx_pend = TRUE;
		return bytes;
	}
	if (err == ERROR_IO_INCOMPLETE) {
	//	CancelIo(uart_ports[uart].hdl);
	//	fprintf(stderr, "CancelIo write\r\n");
		uart_ports[uart].tx_pend = FALSE;
		goto again;
	}
	if (err == ERROR_OPERATION_ABORTED) {
	//	fprintf(stderr, "Aborting write\r\n");
		uart_ports[uart].tx_pend = FALSE;
		return bytes;
	}
	return -EINVAL;
#else
	res = WriteFile(hdl, buf, len, &cb, NULL);
	if (res) {
		return cb;
	}
	return -EINVAL;
#endif
}

int uart_read(uart_t uart, void *buf, size_t len)
{
	BOOL res;
	DWORD cb;
	HANDLE hdl = uart_ports[uart].hdl;
#if 0
	DWORD err;
	DWORD ret;
	int bytes = 0;
	DWORD pending_bytes;

success:
	ret = WaitForSingleObject(uart_ports[uart].rx_evt, 0);
	if (uart_ports[uart].rx_pend && ret != WAIT_OBJECT_0) {
		/* no event, go slow path */
		return bytes;
	}
	if (uart_ports[uart].rx_pend) {
		res = GetOverlappedResult(hdl,
					  &uart_ports[uart].rx_olp,
					  &cb, FALSE);
		if (!res)
			goto failure;
		uart_ports[uart].rx_cb += cb;
		uart_ports[uart].rx_pend = FALSE;
	}
	pending_bytes = uart_ports[uart].rx_cb;
	if (pending_bytes > 0) {
		DWORD min_cb;
		min_cb = min(len, pending_bytes);
		memcpy(((unsigned char *)buf)+bytes,
		       uart_ports[uart].rx_buf, min_cb);
		if (min_cb < uart_ports[uart].rx_cb) {
			memmove(uart_ports[uart].rx_buf,
				uart_ports[uart].rx_buf+min_cb,
				uart_ports[uart].rx_cb-min_cb);
		}
		uart_ports[uart].rx_cb -= min_cb;
		len -= min_cb;
		bytes += min_cb;
		if (0 == len)
			return bytes;
	}
again:
	res = ReadFile(hdl,
		       uart_ports[uart].rx_buf,
		       UART_MAX_BUF-uart_ports[uart].rx_cb,
		       &cb,
		       &uart_ports[uart].rx_olp);
	if (res) {
		uart_ports[uart].rx_cb += cb;
		goto success;
	}
failure:
	err = GetLastError();
	if (err == ERROR_IO_PENDING) {
		uart_ports[uart].rx_pend = TRUE;
		return bytes;
	}
	if (err == ERROR_IO_INCOMPLETE) {
		CancelIo(uart_ports[uart].hdl);
		uart_ports[uart].tx_pend = FALSE;
	//	printf("CancelIo read\r\n");
		goto again;
	}
	if (err == ERROR_OPERATION_ABORTED) {
	//	printf("Aborting read\r\n");
		uart_ports[uart].rx_pend = FALSE;
		return bytes;
	}
	return -EINVAL;
#else
	res = ReadFile(hdl, buf, len, &cb, NULL);
	if (res) {
		return cb;
	}
	return -EINVAL;
#endif
}

uart_t uart_open(int id)
{
	uart_t uart = uart_nr_used;

	if (uart >= UART_MAX_PORTS)
		return -1;
	uart = __uart_open(uart, id);
	if (uart >= 0) {
		uart_nr_used++;
		uart_ports[uart].id = id;
		uart_ports[uart].state = UART_OPENED;
		uart_ports[uart].rx_cb = uart_ports[uart].tx_cb = 0;
	}
	return uart;
}

void uart_close(uart_t uart)
{
	__uart_close(uart);
	uart_nr_used--;
	uart_ports[uart].state = UART_CLOSED;
}

/* other parameters are 8N1 */
int uart_config(uart_t uart, int baudr)
{
	int spd = -1;
	DCB dcb;
	HANDLE hdl = uart_ports[uart].hdl;
	
	ZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(hdl, &dcb)) {
		return -1;
	}

	switch (baudr / 100) {
	case 0:		spd = 0;	break;
	case 3:		spd = CBR_300;	break;
	case 6:		spd = CBR_600;	break;
	case 12:	spd = CBR_1200;	break;
	case 24:	spd = CBR_2400;	break;
	case 48:	spd = CBR_4800;	break;
	case 96:	spd = CBR_9600;	break;
	case 192:	spd = CBR_19200;	break;
	case 384:	spd = CBR_38400;	break;
	case 576:	spd = CBR_57600;	break;
	case 1152:	spd = CBR_115200;	break;
	}
	
	dcb.BaudRate = spd;
	dcb.ByteSize = 8;
	dcb.Parity = 0;
	dcb.StopBits = 0;
	if (!SetCommState(hdl, &dcb)) {
		return -1;
	}

	return 0;
}

#else
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define uart_write(uart, buf, len)	write(uart, buf, len)
#define uart_read(uart, buf, len)	read(uart, buf, len)

uart_t uart_open(int id)
{
	char port[MAX_PATH];

	sprintf(port, "dev/ttyS%d", id);
	return open(port, O_RDWR | O_NOCTTY, S_IREAD | S_IWRITE);
}

void uart_close(uart_t uart)
{
	close(uart);
}

void uart_set_hwflow(uart_t uart, int on)
{
	struct termios tty;
	
	tcgetattr(uart, &tty);
	if (on)
		tty.c_cflag |= CRTSCTS;
	else
		tty.c_cflag &= ~CRTSCTS;
	tcsetattr(uart, TCSANOW, &tty);
}

/* other parameters are 8N1 */
int uart_config(uart_t uart, int baudr)
{
	int spd = -1;
	int newbaud;
	int bit = 8;
	struct termios tty;

	tcgetattr(uart, &tty);
	switch (baudr / 100) {
	case 0:
#ifdef B0
			spd = B0;	break;
#else
			spd = 0;	break;
#endif
	case 3:		spd = B300;	break;
	case 6:		spd = B600;	break;
	case 12:	spd = B1200;	break;
	case 24:	spd = B2400;	break;
	case 48:	spd = B4800;	break;
	case 96:	spd = B9600;	break;
#ifdef B19200
	case 192:	spd = B19200;	break;
#else /* B19200 */
#  ifdef EXTA
	case 192:	spd = EXTA;	break;
#   else /* EXTA */
	case 192:	spd = B9600;	break;
#   endif /* EXTA */
#endif	 /* B19200 */
#ifdef B38400
	case 384:	spd = B38400;	break;
#else /* B38400 */
#  ifdef EXTB
	case 384:	spd = EXTB;	break;
#   else /* EXTB */
	case 384:	spd = B9600;	break;
#   endif /* EXTB */
#endif	 /* B38400 */
#ifdef B57600
	case 576:	spd = B57600;	break;
#endif
#ifdef B115200
	case 1152:	spd = B115200;	break;
#endif
#ifdef B230400
	case 2304:	spd = B230400;	break;
#endif
	}
#if defined (_BSD43) && !defined (POSIX_TERMIOS)
	if (spd != -1) tty.sg_ispeed = tty.sg_ospeed = spd;
	/* number of bits is ignored */
	tty.sg_flags = RAW | TANDEM;
	tty.sg_flags |= PASS8 | ANYP;
	ioctl(uart, TIOCSETP, &tty);
#ifdef TIOCSDTR
	/* FIXME: huh? - MvS */
	ioctl(uart, TIOCSDTR, 0);
#endif
#endif /* _BSD43 && !POSIX_TERMIOS */
#if defined (_V7) && !defined (POSIX_TERMIOS)
	if (spd != -1) tty.sg_ispeed = tty.sg_ospeed = spd;
	tty.sg_flags = RAW;
	ioctl(uart, TIOCSETP, &tty);
#endif /* _V7 && !POSIX */
#ifdef POSIX_TERMIOS
	if (spd != -1) {
		cfsetospeed(&tty, (speed_t)spd);
		cfsetispeed(&tty, (speed_t)spd);
	}
	switch (bit) {
	case '5':
		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5;
		break;
	case '6':
		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6;
		break;
	case '7':
		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;
		break;
	case '8':
	default:
		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
		break;
	}		
	/* set into raw, no echo mode */
	tty.c_iflag =  IGNBRK;
	tty.c_lflag = 0;
	tty.c_oflag = 0;
	tty.c_cflag |= CLOCAL | CREAD;
#ifdef _DCDFLOW
	tty.c_cflag &= ~CRTSCTS;
#endif
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 5;
	tty.c_iflag &= ~(IXON|IXOFF|IXANY);
	tty.c_cflag &= ~(PARENB | PARODD);
	tty.c_cflag &= ~CSTOPB;
	tcsetattr(uart, TCSANOW, &tty);
	set_rts(uart);
#endif /* POSIX_TERMIOS */
#ifndef _DCDFLOW
	uart_set_hwflow(uart, 0);
#endif

	return 0;
}
#endif
