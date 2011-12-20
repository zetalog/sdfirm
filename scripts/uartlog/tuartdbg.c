#include <host/uartdbg.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <getopt.h>

int g_hComFile = -1;
DWORD g_dwLastError = 0;

#define DEF_PATH	"/dev/ttyS0"
#define DEF_BITS	"8"
#define DEF_RATE	"115200"
#define DEF_STOP	"1"
#define DEF_PAR		"N"

const char *g_prog_name = NULL;
char *g_com_path = NULL;
char *g_bits = NULL;
char *g_rates = NULL;
char *g_parity = NULL;
char *g_stopb = NULL;
char **g_cmd_argv = NULL;
		
static struct option g_long_options[] = {
	{"compath", 1, 0, 'c'},
	{"bits", 1, 0, 'b'},
	{"baudrate", 1, 0, 'r'},
	{"parity", 1, 0, 'p'},
	{"stopb", 1, 0, 's'},
	{"help", 0, 0, 'h'},
	{0, 0, 0, 0}
};

/* print help message to stderr */
static void usage(void)
{
	fprintf(stderr, "Usage %s [options] cmd\n", g_prog_name?g_prog_name:"uartdbg");
	fprintf(stderr, "options:\n");
	fprintf(stderr, "        -c, --compath=<path>           specify serial file full path\n");
	fprintf(stderr, "        -b, --bits=<bits>              specify serial param <bit> \n");
	fprintf(stderr, "        -r, --baudrate=<rate>          specify serial param <rate> \n");
	fprintf(stderr, "        -p, --parity=<parity>          specify seaial param <parity> \n");
	fprintf(stderr, "        -s, --stopb=<stopb>            specify seaial param <stopb> \n");
	fprintf(stderr, "        -h, --help                     print this message\n");
	exit(1);
}

static void parse_argv(int argc, char *argv[])
{
	int option_index;
	int c;

	while (1) {
		option_index = 0;

		c = getopt_long(argc, argv, "c:hb:r:p:s:", g_long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'c':
			g_com_path = optarg;
			break;
		case 'b':
			g_bits = optarg;
			break;
		case 'r':
			g_rates = optarg;
			break;
		case 'p':
			g_parity = optarg;
			break;
		case 's':
			g_stopb = optarg;
			break;
		case '?':
			fprintf(stderr, "invalid usage %c.\r\n", c);
		case 'h':
		default:
			usage();
			break;
		}
	}
	if (!g_com_path)
		g_com_path = DEF_PATH;
	if (!g_bits)
		g_bits = DEF_BITS;
	if (!g_rates)
		g_rates = DEF_RATE;
	if (!g_parity)
		g_parity = DEF_PAR;
	if (!g_stopb)
		g_stopb = DEF_STOP;
}

/* set hardware flow control. */
void set_hwflow(int fd, int on)
{
	struct termios tty;
	
	tcgetattr(fd, &tty);
	if (on)
		tty.c_cflag |= CRTSCTS;
	else
		tty.c_cflag &= ~CRTSCTS;
	tcsetattr(fd, TCSANOW, &tty);
}

void set_params(int fd, char *baudr, char *par, char *bits,
		char *stopb, int hwf, int swf)
{
	int spd = -1;
	int newbaud;
	int bit = bits[0];
	
	struct termios tty;
	
	tcgetattr(fd, &tty);
	
	/* we generate mark and space parity ourself. */
	if (bit == '7' && (par[0] == 'M' || par[0] == 'S'))
		bit = '8';
	
	/* check if 'baudr' is really a number */
	if ((newbaud = (atol(baudr) / 100)) == 0 && baudr[0] != '0') newbaud = -1;
	
	switch (newbaud) {
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
	if (par[0] == 'E')
		tty.sg_flags |= EVENP;
	else if (par[0] == 'O')
		tty.sg_flags |= ODDP;
	else
		tty.sg_flags |= PASS8 | ANYP;
	
	ioctl(fd, TIOCSETP, &tty);
	
#  ifdef TIOCSDTR
	/* FIXME: huh? - MvS */
	ioctl(fd, TIOCSDTR, 0);
#  endif
#endif /* _BSD43 && !POSIX_TERMIOS */
	
#if defined (_V7) && !defined (POSIX_TERMIOS)
	if (spd != -1) tty.sg_ispeed = tty.sg_ospeed = spd;
	tty.sg_flags = RAW;
	if (par[0] == 'E')
		tty.sg_flags |= EVENP;
	else if (par[0] == 'O')
		tty.sg_flags |= ODDP;
	
	ioctl(fd, TIOCSETP, &tty);
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
	
	if (swf)
		tty.c_iflag |= IXON | IXOFF;
	else
		tty.c_iflag &= ~(IXON|IXOFF|IXANY);
	
	tty.c_cflag &= ~(PARENB | PARODD);
	if (par[0] == 'E')
		tty.c_cflag |= PARENB;
	else if (par[0] == 'O')
		tty.c_cflag |= (PARENB | PARODD);
	
	if (stopb[0] == '2')
		tty.c_cflag |= CSTOPB;
	else
		tty.c_cflag &= ~CSTOPB;
	
	tcsetattr(fd, TCSANOW, &tty);
	
	set_rts(fd);
#endif /* POSIX_TERMIOS */
	
#ifndef _DCDFLOW
	set_hwflow(fd, hwf);
#endif
}

static void DumpError(void)
{
	printf("<raw> ERROR: ");
	printf("%d, %s", errno, strerror(errno));
	printf("\r\n");
}

void cleanup(void)
{
	dbg_exit();
	if (g_hComFile >= 0)
		close(g_hComFile);
}

static void ConsoleDump(void *ctx, dbg_cmd_t bCmd, const char *szFormat, ...)
{
	FILE *pFile = (FILE *)ctx;
	va_list argList;
	unsigned char szBuf[1024];
	
	va_start(argList, szFormat );
	vsprintf(szBuf, szFormat, argList);
	va_end(argList);

	fprintf(pFile, "<%s> %s: ",
		dbg_state_name(bCmd), dbg_event_name(bCmd));
	fprintf(pFile, "%s", szBuf);
	fprintf(pFile, "\r\n");
}

int main(int argc, char **argv)
{
	atexit(cleanup);
	dbg_init();
	dbg_register_output(ConsoleDump, ConsoleLast);

	if (argc < 2) {
		usage();
	}

	parse_argv(argc, argv);
opencom:
	while (1) {
		g_hComFile = open(g_com_path, O_RDWR | O_NOCTTY, S_IREAD | S_IWRITE);
		if (g_hComFile < 0) {
			DumpError();
			sleep(1);
		} else {
			set_params(g_hComFile, g_rates, g_parity, g_bits, g_stopb, 0, 0);
			//Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
			//Opt.c_oflag &= ~OPOST; /*Output*/
			printf("open ok\r\n");
			goto readcom;
		}
	}
readcom:
	while (1) {
		int res;
		unsigned char byte;

		if ((res = read(g_hComFile, byte, 1)) < 0) {
			DumpError();
			sleep(1);
			CloseHandle(g_hComFile);
			goto opencom;
		}
		dbg_process_log(stdout, byte);
	}
	return 0;
}
