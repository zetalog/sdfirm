#include <target/mem.h>
#include <target/readline.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/perf.h>
#include <getopt.h>

#define foreach_cmd(cmdp)		\
	for (cmdp = __cmd_start; cmdp < __cmd_end; cmdp++)
#define MAXARGS				10

int cmd_parse(char *line, char *argv[])
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
		if (strcmp(cmdp->name, cmd) != 0)
			continue;
		return cmdp;
	}
	return NULL;
}

bool cmd_set_flags(const char *cmd, uint8_t flags)
{
	cmd_tbl *cmdp;

	cmdp = find_cmd(cmd);
	if (!cmdp)
		return false;
	cmdp->flags |= flags;
	return true;
}

bool cmd_clear_flags(const char *cmd, uint8_t flags)
{
	cmd_tbl *cmdp;

	cmdp = find_cmd(cmd);
	if (!cmdp)
		return false;
	cmdp->flags &= ~flags;
	return true;
}

void cmd_clear_all_flags(uint8_t flags)
{
	cmd_tbl *cmdp;

	foreach_cmd(cmdp) {
		cmdp->flags &= ~flags;
	}
}

int cmd_help(char *cmd)
{
	cmd_tbl *cmdp;

	if (!cmd) {
		foreach_cmd(cmdp) {
			printf("%c %-*s- %s\n", 8,
			       (cmdp->flags & CMD_FLAG_PERF) ? '*' : ' ',
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
	"help\n"
	"    - print brief description of all commands\n"
	"help command\n"
	"    - print detailed usage of 'command'\n"
);

int cmd_execute(int argc, char * argv[])
{
	cmd_tbl *cmdp;
	int ret;
	bool perf_started = false;

	cmdp = find_cmd(argv[0]);
	if (cmdp == NULL) {
		printf("No such command '%s'\n\n", argv[0]);
		return -1;
	}
	getopt_reset();
	if (cmdp->flags & CMD_FLAG_PERF) {
		perf_start();
		perf_started = true;
	}
	ret = cmdp->cmd(argc, argv);
	if (perf_started)
		perf_stop();
	if (ret < 0)
		printf("Command failure '%s - %d'\n\n", argv[0], ret);
	else
		printf("Command success '%s - %d'\n\n", argv[0], ret);
	return ret;
}

#ifdef CONFIG_CONSOLE_COMMAND
int cmd_console_handler(char *buf, int len)
{
	char *argv[MAXARGS + 1];
	int argc;

	if (len < 0) {
		printf("<INTERRUPT>\n");
		return -EINTR;
	}

	argc = cmd_parse(buf, argv);
	if (argc > 0)
		cmd_execute(argc, argv);
	puts("sdfirm> ");
	return 0;
}

void cmd_console(void)
{
	console_late_init();
	readline_register_handler(cmd_console_handler);
	con_dbg("cmd: Command section: %016llx - %016llx\n",
		(uint64_t)__cmd_start, (uint64_t)__cmd_end);
}
#else
#define cmd_console()		do { } while (0)
#endif

void cmd_dump_sect(void)
{
	hexdump((caddr_t)__cmd_start, (void *)__cmd_start, 8,
		(caddr_t)__cmd_end - (caddr_t)__cmd_start);
}

#ifdef CONFIG_COMMAND_BATCH
static char *parse_batch(char *line, int *args, char *argv[])
{
	int nargs = 0;

	while (nargs < MAXARGS) {
		/* skip any white space */
		while (isblank(*line) || *line == ';')
			++line;

		if (*line == '\0') {
			/* end of line, no more args */
			argv[nargs] = NULL;
			*args = nargs;
			return NULL;
		}

		/* begin of argument string */
		argv[nargs++] = line;
		/* find end of string */
		while (*line && !(isblank(*line) || *line == ';'))
			++line;

		if (*line == '\0') {
			/* end of line, no more args */
			argv[nargs] = NULL;
			*args = nargs;
			return NULL;
		}
		if (*line == ';') {
			argv[nargs] = NULL;
			*args = nargs;
			*line++ = '\0';
			return line;
		}
		/* terminate current arg */
		*line++ = '\0';
	}

	printf("** Too many args (max. %d) **\n", MAXARGS);
	return NULL;
}

char cmd_batch_orig[] = CONFIG_COMMAND_BATCH_COMMAND;
char *cmd_batch_cmds = cmd_batch_orig;

void cmd_batch(void)
{
	char *argv[MAXARGS + 1];
	int argc;

again:
	argc = 0;
	cmd_batch_cmds = parse_batch(cmd_batch_cmds, &argc, argv);
	if (argc > 0)
		cmd_execute(argc, argv);
	if (!cmd_batch_cmds) {
		puts("sdfirm> ");
		return;
	}
	goto again;
}
#else
void cmd_batch(void)
{
	puts("sdfirm> ");
}
#endif

int cmd_init(void)
{
	cmd_console();
	cmd_batch();
	return 0;
}
