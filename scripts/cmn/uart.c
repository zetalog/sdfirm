#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/poll.h>
#include <host/uart.h>

#ifdef WIN32
#include <windows.h>
#include <io.h>

int uart_write(uart_t uart, void *buf, size_t len)
{
	DWORD res;
	HANDLE hdl = (HANDLE)uart;

	if (WriteFile(hdl, buf, len, &res, NULL))
		return res;
	return -EINVAL;
}

int uart_read(uart_t uart, void *buf, size_t len)
{
	DWORD res;
	HANDLE hdl = (HANDLE)uart;

	if (ReadFile(hdl, buf, len, &res, NULL)) {
		return res;
	}
	if (GetLastError() == ERROR_IO_PENDING) {
		errno = EAGAIN;
		return 0;
	}
	return -EINVAL;
}

uart_t uart_open(int id)
{
	char port[MAX_PATH];
	HANDLE hdl;

	if (id > 9) {
		sprintf(port, "\\\\.\\COM%d", id+1);
	} else {
		sprintf(port, "COM%d", id+1);
	}
	hdl = CreateFile(port, GENERIC_READ | GENERIC_WRITE,
			 0, 0, OPEN_EXISTING, 0, 0);

	return (uart_t)hdl;
}

void uart_close(uart_t uart)
{
	HANDLE hdl = (HANDLE)uart;

	CloseHandle(hdl);
}

/* other parameters are 8N1 */
void uart_config(uart_t uart, int baudr)
{
	int spd = -1;
	DCB dcb;
	HANDLE hdl = (HANDLE)uart;
	
	ZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(hdl, &dcb))
		return;

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
	if (!SetCommState(hdl, &dcb))
		return;
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
void uart_config(uart_t uart, int baudr)
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
}
#endif
