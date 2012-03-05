/* *************************************
 * EZIO RS232 LCD Control Sample Program
 * *************************************
 * *************************************************************************
 * Company: Portwell Inc.
 * Date: 4/16/2003
 * Program: 02A.c
 * Version: 1.02
 * Compile: Linux GNU C
 * Purpose: Direct access to EZIO LCD, the program will display
 * messages according to the control button. The current
 * version only has the following function:
 *
 * 1: display welcome message
 * 2: display UP message if "scroll up" button is pressed
 * 3: display ENTER message if "ENTER" button is pressed
 * 4: display ESC message if "ESC" button is pressed
 * 5: display DOWN message if "scroll down" button is pressed
 *
 * Program Overview:
 *
 * - Parameters:
 * uart : a file name for open() method, here represents the com port
 * Cmd : command prefix
 * cls : clear command
 * init : initialize command
 * blank : display blank screen
 * stopsend : stop input/output
 * home : move cursor to initial position
 * readkey : set to read from EZIO
 * hide : hide cursor & display blanked characters
 * movel : move cursor one character left
 * mover : move cursor one character right
 * turn : turn on blinking-block cursor
 * show : turn on underline cursor
 * scl : scroll cursor one character left
 * scr : scroll cursor one character right
 * setdis : set character-generator address
 *
 * - Procedure:
 * 1. The program sets up the environment, i.e. com port settings.
 * 2. The main function MUST call init() twice to initialize EZIO
 * before any communication.
 * 3. For executing any command, the command prefix, Cmd, MUST be
 * called be command. So all command contains two parts, eg.
 * to initialize the sequence of HEX number is 0xFE, 0x25.
 * 4. After clear screen and display welcome message, ReadKey()
 * method must be call to advise EZIO for reading data.
 * 5. A pooling method is implemented to get input from EZIO while
 * any button is pressed.
 *
 * - NOTE: This program is a sample program provided " AS IS" with NO
 * warranty.
 *
 * Copyright (c) Portwell, Inc. All Rights Reserved.
 *
 * ************************************************************************/

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

#define EZIO_WRITE		0xFE
#define EZIO_READ		0xFD

#define EZIO_CMD_StartOfHEX	0x28
#define EZIO_CMD_EndOfHex	0x37
#define EZIO_CMD_ClearScreen	0x01
#define EZIO_CMD_HomeCursor	0x02
#define EZIO_CMD_ReadKey	0x06
#define EZIO_CMD_BlankDisplay	0x08
#define EZIO_CMD_HideCursor	0x0C
#define EZIO_CMD_TurnOn		0x0D
#define EZIO_CMD_ShowCursor	0x0E
#define EZIO_CMD_MoveLeft	0x10
#define EZIO_CMD_MoveRight	0x14
#define EZIO_CMD_ScrollLeft	0x18
#define EZIO_CMD_ScrollRight	0x1C
#define EZIO_CMD_SetDispAddr	0x80
#define EZIO_CMD_SetCharAddr	0x40

#define EZIO_KEY_Escape		0x4E
#define EZIO_KEY_UpArrow	0x4D
#define EZIO_KEY_Enter		0x4B
#define EZIO_KEY_DownArrow	0x47

static uart_t uart;

void ezio_write(int cmd)
{
	int ezi = EZIO_WRITE;

	uart_write(uart, &ezi, 1);
	uart_write(uart, &cmd, 1);
}

#define StartHex()	ezio_write(EZIO_CMD_StartOfHEX)
#define StopHex()	ezio_write(EZIO_CMD_EndOfHex)
#define Cls()		ezio_write(EZIO_CMD_ClearScreen)
#define Home()		ezio_write(EZIO_CMD_HomeCursor)
#define ReadKey()	ezio_write(EZIO_CMD_ReadKey)
#define Blank()		ezio_write(EZIO_CMD_BlankDisplay)
#define TurnOn()	ezio_write(EZIO_CMD_TurnOn)
#define Hide()		ezio_write(EZIO_CMD_HideCursor)
#define Show()		ezio_write(EZIO_CMD_ShowCursor)
#define MoveL()		ezio_write(EZIO_CMD_MoveLeft)
#define MoveR()		ezio_write(EZIO_CMD_MoveRight)
#define ScrollL()	ezio_write(EZIO_CMD_ScrollLeft)
#define ScrollR()	ezio_write(EZIO_CMD_ScrollRight)
#define SetDis()	ezio_write(EZIO_CMD_SetDispAddr)

void ShowMessage(char *str2)
{
	char mes1[] = "Portwell EZIO";
	char mes2[] = "*************";
	char nul[] = " ";
	int a, b;

	if (str2 == NULL)
		str2 = mes2;

	a = strlen(mes1);
	b = 40 - a;
	uart_write(uart, mes1, a);
	uart_write(uart, nul, b);
	uart_write(uart, str2, strlen(str2));
}

int main(int argc, char **argv)
{
	if (argc < 3)
		return -1;

	uart = uart_open(atoi(argv[1]));
	uart_config(uart, atoi(argv[2]));

	/* initialize EZIO twice */
	StartHex();
	StartHex();
	/* clear screen */
	Cls();
	ShowMessage(NULL);

	while (1) {
		int res;
		char buf[255];

		SetDis();
		ReadKey();
		res = uart_read(uart, buf, 255);

		/* switch the pressed key */
		switch (buf[1]) {
		case EZIO_KEY_UpArrow:
			Cls();
			ShowMessage("UP");
			break;
		case EZIO_KEY_DownArrow:
			Cls();
			ShowMessage("DOWN");
			break;
		case EZIO_KEY_Enter:
			Cls();
			ShowMessage("ENTER");
			break;
		case EZIO_KEY_Escape:
			Cls();
			ShowMessage("ESC");
			break;
		}
	}

	uart_close(uart);
}
