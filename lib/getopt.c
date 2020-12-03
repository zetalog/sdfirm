#include <getopt.h>
#include <stdio.h>

char *optarg = 0;
/* XXX 1003.2 says this must be 1 before any call.  */
int optind = 0;
static char *nextchar;
int opterr = 1;
int optopt = '?';

static enum {
    REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER
} ordering;

static int first_nonopt;
static int last_nonopt;

static void exchange(char **argv)
{
	int bottom = first_nonopt;
	int middle = last_nonopt;
	int top = optind;
	char *tem;

	while (top > middle && middle > bottom) {
		if (top - middle > middle - bottom) {
			/* Bottom segment is the short one.  */
			int len = middle - bottom;
			register int i;

			/* Swap it with the top part of the top segment.  */
			for (i = 0; i < len; i++) {
				tem = argv[bottom + i];
				argv[bottom + i] =
					argv[top - (middle - bottom) + i];
				argv[top - (middle - bottom) + i] = tem;
			}
			/* Exclude the moved bottom segment from further
			 * swapping.
			 */
			top -= len;
		} else {
			/* Top segment is the short one.  */
			int len = top - middle;
			register int i;

			/* Swap it with the bottom part of the bottom
			 * segment.
			 */
			for (i = 0; i < len; i++) {
				tem = argv[bottom + i];
				argv[bottom + i] = argv[middle + i];
				argv[middle + i] = tem;
			}
			/* Exclude the moved top segment from further
			 * swapping.
			 */
			bottom += len;
		}
	}

	/* Update records for the slots the non-options now occupy.  */
	first_nonopt += (optind - last_nonopt);
	last_nonopt = optind;
}

void getopt_reset(void)
{
	optind = 0;
}

int getopt(int argc, char *const *argv, const char *optstring)
{
	optarg = 0;

	if (optind == 0) {
		first_nonopt = last_nonopt = optind = 1;

		nextchar = NULL;

		/* Determine how to handle the ordering of options and
		 * nonoptions.
		 */
		if (optstring[0] == '-') {
			ordering = RETURN_IN_ORDER;
			++optstring;
		} else if (optstring[0] == '+') {
			ordering = REQUIRE_ORDER;
			++optstring;
		} else
			ordering = PERMUTE;
	}

	if (nextchar == NULL || *nextchar == '\0') {
		if (ordering == PERMUTE) {
			if (first_nonopt != last_nonopt &&
			    last_nonopt != optind)
				exchange((char **) argv);
			else if (last_nonopt != optind)
				first_nonopt = optind;

			while (optind < argc &&
			       (argv[optind][0] != '-' ||
				argv[optind][1] == '\0'))
				optind++;
			last_nonopt = optind;
		}

		if (optind != argc && !strcmp(argv[optind], "--")) {
			optind++;

			if (first_nonopt != last_nonopt &&
			    last_nonopt != optind)
				exchange((char **) argv);
			else if (first_nonopt == last_nonopt)
				first_nonopt = optind;
			last_nonopt = argc;
			optind = argc;
		}

		if (optind == argc) {
			if (first_nonopt != last_nonopt)
				optind = first_nonopt;
			return EOF;
		}

		if ((argv[optind][0] != '-' || argv[optind][1] == '\0')) {
			if (ordering == REQUIRE_ORDER)
				return EOF;
			optarg = argv[optind++];
			return 1;
		}

		nextchar = argv[optind] + 1;
	}

	/* Look at and handle the next option-character.  */
	{
		char c = *nextchar++;
		char *temp = strchr(optstring, c);

		/* Increment `optind' when we start to process its last
		 * character.
		 */
		if (*nextchar == '\0')
			++optind;

		if (temp == NULL || c == ':') {
			if (opterr) {
				/* 1003.2 specifies the format of this
				 * message.
				 */
				printf("%s: illegal option -- %c\n",
				       argv[0], c);
			}
			optopt = c;
			return '?';
		}
		if (temp[1] == ':') {
			if (temp[2] == ':') {
				/* This is an option that accepts an
				 * argument optionally.
				 */
				if (*nextchar != '\0') {
					optarg = nextchar;
					optind++;
				} else
					optarg = 0;
				nextchar = NULL;
			} else {
				/* This is an option that requires an
				 * argument.
				 */
				if (*nextchar != '\0') {
					optarg = nextchar;
  					optind++;
				} else if (optind == argc) {
					if (opterr) {
						/* 1003.2 specifies the
						 * format of this message.
					       	 */
						printf("%s: option requires an argument -- %c\n",
						       argv[0], c);
					}
					optopt = c;
					if (optstring[0] == ':')
						c = ':';
					else
						c = '?';
				} else
					optarg = argv[optind++];
				nextchar = NULL;
			}
		}
		return c;
	}
}
