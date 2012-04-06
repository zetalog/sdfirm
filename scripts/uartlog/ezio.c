/* Direct access to EZIO LCD, the program will display messages according
 * to the control button.
 * 1: display welcome message
 * 2: display UP message if "UP" button is pressed
 * 3: display ENTER message if "ENTER" button is pressed
 * 4: display ESC message if "ESC" button is pressed
 * 5: display DOWN message if "DOWN" button is pressed
 *
 * Copyright (c) Soliton Shanghai. All Rights Reserved.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifdef WIN32
#include <windows.h>
#include <io.h>
typedef HANDLE uart_t;

int uart_write(uart_t uart, void *buf, size_t len)
{
	DWORD res;
	if (WriteFile(uart, buf, len, &res, NULL))
		return res;
	return -EINVAL;
}

int uart_read(uart_t uart, void *buf, size_t len)
{
	DWORD res;
	if (ReadFile(uart, buf, len, &res, NULL))
		return res;
	return -EINVAL;
}

uart_t uart_open(int id)
{
	char port[MAX_PATH];

	if (id > 9) {
		sprintf(port, "\\\\.\\COM%d", id+1);
	} else {
		sprintf(port, "COM%d", id+1);
	}
	return CreateFile(port, GENERIC_READ | GENERIC_WRITE,
			  0, 0, OPEN_EXISTING, 0, 0);
}

void uart_close(uart_t uart)
{
	CloseHandle(uart);
}

/* other parameters are 8N1 */
void uart_config(uart_t uart, int baudr)
{
	int spd = -1;
	DCB dcb;
	
	ZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(uart, &dcb))
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
	if (!SetCommState(uart, &dcb))
		return;
}

#else
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

typedef int uart_t;

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

#define EZIO_WRITE			0xFE
#define EZIO_READ			0xFD

#define EZIO_CMD_StartOfHEX		0x28
#define EZIO_CMD_EndOfHex		0x37
#define EZIO_CMD_ClearScreen		0x01
#define EZIO_CMD_HomeCursor		0x02
#define EZIO_CMD_ReadKey		0x06
#define EZIO_CMD_BlankDisplay		0x08
#define EZIO_CMD_HideCursor		0x0C
#define EZIO_CMD_TurnOn			0x0D
#define EZIO_CMD_ShowCursor		0x0E
#define EZIO_CMD_MoveLeft		0x10
#define EZIO_CMD_MoveRight		0x14
#define EZIO_CMD_ScrollLeft		0x18
#define EZIO_CMD_ScrollRight		0x1C
#define EZIO_CMD_SetDispAddr		0x80
#define EZIO_CMD_SetCharAddr		0x40

#define EZIO_SCAN_Escape		0x00
#define EZIO_SCAN_UpArrow		0x01
#define EZIO_SCAN_Enter			0x02
#define EZIO_SCAN_DownArrow		0x03
#define EZIO_MAX_SCAN			4
#define EZIO_KEY(scan)			(1<<(scan))
#define EZIO_KEY_VAL(key, scan)		(((key) & EZIO_KEY(scan)) >> (scan))
#define EZIO_KEY_DOWN(key, scan)	(EZIO_KEY_VAL(key, scan) == 1)

#define EZIO_MAX_LINE			16
#define EZIO_MAX_BUF			(2*EZIO_MAX_LINE)

static uart_t uart;

void ezio_write(int cmd)
{
	int ezi = EZIO_WRITE;

	uart_write(uart, &ezi, 1);
	uart_write(uart, &cmd, 1);
}

#define ezio_keypad_scan()	ezio_write(EZIO_CMD_ReadKey)
#define ezio_screen_clear()	ezio_write(EZIO_CMD_ClearScreen)
#define ezio_screen_blank()	ezio_write(EZIO_CMD_BlankDisplay)
#define ezio_data_start()	ezio_write(EZIO_CMD_StartOfHEX)
#define ezio_data_stop()	ezio_write(EZIO_CMD_EndOfHex)
#define ezio_data_left()	ezio_write(EZIO_CMD_ScrollLeft)
#define ezio_data_right()	ezio_write(EZIO_CMD_ScrollRight)
#define ezio_cursor_home()	ezio_write(EZIO_CMD_HomeCursor)
#define ezio_cursor_blink()	ezio_write(EZIO_CMD_TurnOn)
#define ezio_cursor_hide()	ezio_write(EZIO_CMD_HideCursor)
#define ezio_cursor_show()	ezio_write(EZIO_CMD_ShowCursor)
#define ezio_cursor_left()	ezio_write(EZIO_CMD_MoveLeft)
#define ezio_cursor_right()	ezio_write(EZIO_CMD_MoveRight)
#define ezio_cursor_locate()	ezio_write(EZIO_CMD_SetDispAddr)

void __ezio_display(unsigned char *msg)
{
	unsigned char nul[] = "                ";
	int a, b, i;

	a = strlen(msg);
	b = EZIO_MAX_LINE - a;
	uart_write(uart, msg, a);
	uart_write(uart, nul, b);

	fwrite(msg, 1, a, stdout);
	fwrite(nul, 1, b, stdout);
	fprintf(stdout, "\r\n");
	for (i = 0; i < a; i++)
		fprintf(stdout, "%02x ", msg[i]);
	for (i = 0; i < b; i++)
		fprintf(stdout, "%02x ", nul[i]);
	fprintf(stdout, "\r\n");
}

void ezio_display(unsigned char *str2)
{
	unsigned char str1[] = "Soliton EZIO LCD";
	unsigned char def2[] = "****************";

	if (str2 == NULL || str2[0] == 0)
		str2 = def2;

	ezio_screen_clear();
	ezio_data_start();
	__ezio_display(str1);
	__ezio_display(str2);
	ezio_data_stop();
}

const char *ezio_key_name(int i)
{
	switch (i) {
	case 0:
		return "Esc";
	case 1:
		return "U";
	case 2:
		return "Ent";
	case 3:
		return "D";
	}
	return "*";
}

int main(int argc, char **argv)
{
	int last_keys = 0x00, keys;
	int res, i, len;
	unsigned char buf[EZIO_MAX_BUF];

	if (argc < 3)
		return -1;

	uart = uart_open(atoi(argv[1]));
	uart_config(uart, atoi(argv[2]));

	/* initialize EZIO */
	ezio_screen_clear();
	/* display default message */
	ezio_display(NULL);

	len = 0;
	while (1) {
		ezio_keypad_scan();
		res = uart_read(uart, buf+len, EZIO_MAX_BUF-len);
		if (res <= 0)
			continue;

		len += res;
		while (len > 2) {
			if (buf[0] == EZIO_READ) {
				keys = (buf[1] & ~0xF0);
			} else {
				len--;
				memcpy(buf, buf+1, len);
			}
		}

		if (len <= 2)
			continue;

		buf[0] = 0;
		for (i = 0; i < EZIO_MAX_SCAN; i++) {
			char msg[6];

			if (EZIO_KEY_VAL(keys, i) !=
			    EZIO_KEY_VAL(last_keys, i)) {
				if (EZIO_KEY_DOWN(keys, i)) {
					sprintf(msg, "+%s", ezio_key_name(i));
				} else {
					sprintf(msg, "-%s", ezio_key_name(i));
				}
				strcat(buf, msg);
			}
		}

		ezio_display(buf);
		last_keys = keys;
		len -= 2;
	}

	uart_close(uart);
}
