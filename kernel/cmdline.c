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
