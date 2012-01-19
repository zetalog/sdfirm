#include "ut_priv.h"
#include <getopt.h>

static struct option g_long_options[] = {
	{"log", 1, 0, 'l'},
	{"help", 0, 0, 'h'},
	{0, 0, 0, 0}
};

static void usage(void)
{
	fprintf(stderr, "Usage %s [options] cmd\n", "ut_main");
	fprintf(stderr, "options:\n");
	fprintf(stderr, "  -l, --log=<level>              specify logging level\n");
	fprintf(stderr, "    logging levels can be:\n");
	fprintf(stderr, "    error: errornous messages\n");
	fprintf(stderr, "    info: informative messages\n");
	fprintf(stderr, "    debug: debugging messages\n");
	fprintf(stderr, "    others: turning logging off\n");
	fprintf(stderr, "  -h, --help                     print this message\n");
	exit(1);
}

static void parse_argv(int argc, char *argv[])
{
	int option_index;
	int c;

	while (1) {
		option_index = 0;

		c = getopt_long(argc, argv, "l:h", g_long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'l':
			log_set_level(log_name2level(optarg));
			break;
		case '?':
			fprintf(stderr, "invalid usage %c.\r\n", c);
		case 'h':
		default:
			usage();
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	parse_argv(argc, argv);

	if (log_init() || usb_init() || ccid_init() || pcsc_init())
		return -1;

	ccid_run_suite();

	pcsc_exit();
	ccid_exit();
	usb_exit();
	log_exit();
	return 0;
}
