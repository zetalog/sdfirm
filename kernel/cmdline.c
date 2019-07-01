#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <target/types.h>
#include <target/readline.h>
#include <target/cmdline.h>
#include <target/timer.h>
#include <target/console.h>

#define SIZE_1M		0x100000

extern cmd_tbl __cmd_start[0];
extern cmd_tbl __cmd_end[0];

#define foreach_cmd(cmdp)		\
	for (cmdp = __cmd_start; cmdp < __cmd_end; cmdp++)
#define MAXARGS				10
#define MAX_LINE_LENGTH_BYTES		64
#define DEFAULT_LINE_LENGTH_BYTES	16
#define DISP_LINE_LEN			16

static int parse_line(char *line, char *argv[])
{
	int nargs = 0;

	while (nargs < MAXARGS) {
		/* skip any white space */
		while (isblank(*line))
			++line;

		if (*line == '\0') {
			/* end of line, no more args */
			argv[nargs] = NULL;
			return nargs;
		}

		/* begin of argument string */
		argv[nargs++] = line;
		/* find end of string */
		while (*line && !isblank(*line))
			++line;

		if (*line == '\0') {
			/* end of line, no more args */
			argv[nargs] = NULL;
			return nargs;
		}
		/* terminate current arg */
		*line++ = '\0';
	}

	printf("** Too many args (max. %d) **\n", MAXARGS);
	return nargs;
}

static cmd_tbl *find_cmd(const char *cmd)
{
	cmd_tbl *cmdp;

	foreach_cmd(cmdp) {
		if (strcmp(cmdp->name, cmd))
			continue;
		return cmdp;
	}
	return NULL;
}

int cmd_help(char *cmd)
{
	cmd_tbl *cmdp;

	if (!cmd) {
		foreach_cmd(cmdp) {
			printf("%-*s- %s\n", 8,
			       cmdp->name, cmdp->help);
		}
	} else {
		cmdp = find_cmd(cmd);
		if (!cmdp) {
			printf("Unknown command '%s'\n\n", cmd);
			return -EINVAL;
		}
		printf("Usage of %s:\n%s\n",
		       cmdp->name, cmdp->usage);
	}
	return 0;
}

static int do_help(int argc, char *argv[])
{
	int i, tmp, ret = 0;

	if (argc == 1)
		return cmd_help(NULL);
	for (i = 1; i < argc; ++i) {
		tmp = cmd_help(argv[i]);
		if (tmp)
			ret = tmp;
	}
	if (ret)
		cmd_help(NULL);
	return ret;
}

int cmd_get_data_size(char* arg, int default_size)
{
	switch (arg[0]) {
	case 'b':
		return 1;
	case 'w':
		return 2;
	case 'l':
		return 4;
	case 'q':
		return 8;
	default:
		return -1;
	}
	return default_size;
}

int print_buffer(unsigned long addr, const void *data,
		 unsigned int width, unsigned int count,
		 unsigned int linelen)
{
	/* linebuf as a union causes proper alignment */
	union linebuf {
		uint64_t uq[MAX_LINE_LENGTH_BYTES/sizeof(uint64_t) + 1];
		uint32_t ui[MAX_LINE_LENGTH_BYTES/sizeof(uint32_t) + 1];
		uint16_t us[MAX_LINE_LENGTH_BYTES/sizeof(uint16_t) + 1];
		uint8_t  uc[MAX_LINE_LENGTH_BYTES/sizeof(uint8_t) + 1];
	} lb;
	int i;
	uint64_t x;

	if (linelen*width > MAX_LINE_LENGTH_BYTES)
		linelen = MAX_LINE_LENGTH_BYTES / width;
	if (linelen < 1)
        linelen = DEFAULT_LINE_LENGTH_BYTES / width;

	while (count) {
		unsigned int thislinelen = linelen;
		printf("%08lx:", addr);

		/* check for overflow condition */
		if (count < thislinelen)
			thislinelen = count;

		/* Copy from memory into linebuf and print hex values */
		for (i = 0; i < thislinelen; i++) {
			if (width == 4)
				x = lb.ui[i] = *(volatile uint32_t *)data;
			else if (width == 8)
				x = lb.uq[i] = *(volatile uint64_t *)data;
			else if (width == 2)
				x = lb.us[i] = *(volatile uint16_t *)data;
			else
				x = lb.uc[i] = *(volatile uint8_t *)data;
			printf(" %0*llx", width * 2, (long long)x);
			data += width;
		}

		while (thislinelen < linelen) {
			/* fill line with whitespace for nice ASCII print */
			for (i = 0; i < width*2+1; i++)
				printf(" ");
			linelen--;
		}

		/* Print data in ASCII characters */
		for (i = 0; i < thislinelen * width; i++) {
			if (!isprint(lb.uc[i]) || lb.uc[i] >= 0x80)
				lb.uc[i] = '.';
		}
		lb.uc[i] = '\0';
		printf("    %s\n", lb.uc);

		/* update references */
		addr += thislinelen * width;
		count -= thislinelen;
	}
	return 0;
}

/* Memory Display
 *
 * Syntax:
 *  md{b, w, l, q} {addr} {len}
 */
static int do_mem_display(int argc, char * argv[])
{
	int size;
	unsigned long addr = 0;
	unsigned long length = 0;
	void *buf = NULL;

	if (argc < 2)
		return -EINVAL;

	if ((size = cmd_get_data_size(argv[1], 4)) < 0)
		return -EINVAL;

	addr = strtoul(argv[2], 0, 0);
	if (argc > 2)
		length = strtoul(argv[3], NULL, 0);

	buf = (void *)(unsigned long)addr;
	return print_buffer(addr, buf, size, length, DISP_LINE_LEN / size);
}

DEFINE_COMMAND(mem, do_mem_display, "Display memory contents",
	"    -display mem\n"
	"help command ...\n"
	"     mem b|w|l|q addr [len]"
	"\n"
);

DEFINE_COMMAND(help, do_help, "Print command description/usage",
	"    - print brief description of all commands\n"
	"help command ...\n"
	"    - print detailed usage of 'command'"
	"\n"
);

static int process_cmd(int argc, char * argv[])
{
	cmd_tbl *cmdp;

	cmdp = find_cmd(argv[0]);
	if (cmdp == NULL)
		return -1;
	return cmdp->cmd(argc, argv);
}

int cmd_console_handler(char *buf, int len)
{
	char *argv[MAXARGS + 1];
	int argc;

	if (len < 0) {
		printf("<INTERRUPT>\n");
		return -EINTR;
	}

	argc = parse_line(buf, argv);
	if (argc > 0)
		process_cmd(argc, argv);
	puts("sdfirm> ");
	return 0;
}

int cmd_init(void)
{
	console_late_init();
	readline_register_handler(cmd_console_handler);
	puts("sdfirm> ");
	return 0;
}
