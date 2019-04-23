#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <target/readline.h>
#include <target/cmdline.h>
#include <target/timer.h>
#include <target/cpus.h>
#include <target/arch.h>
#include <target/console.h>

#define MAX_DDR_SIZE 0x200000000
#define SIZE_1M  0x100000

#ifdef CONFIG_CONSOLE_COMMAND
static cmd_tbl cmd_start[0] __attribute__((unused,section(".init_array.cmd_list1")));
static cmd_tbl cmd_end[0] __attribute__((unused,section(".init_array.cmd_list3")));

#define foreach_cmd(cmdp) for (cmdp = cmd_start; cmdp < cmd_end; cmdp++)
#define MAXARGS  10
#define MAX_LINE_LENGTH_BYTES (64)
#define DEFAULT_LINE_LENGTH_BYTES (16)
#define DISP_LINE_LEN   16

static int parse_line(char *line, char *argv[])
{
    int nargs = 0;

    while (nargs < MAXARGS) {
        /* skip any white space */
        while (isblank(*line))
            ++line;

        if (*line == '\0') {    /* end of line, no more args    */
            argv[nargs] = NULL;
            return nargs;
        }

        argv[nargs++] = line;   /* begin of argument string */
        /* find end of string */
        while (*line && !isblank(*line))
            ++line;

        if (*line == '\0') {    /* end of line, no more args    */
            argv[nargs] = NULL;
            return nargs;
        }
        *line++ = '\0';     /* terminate current arg     */
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
    switch (arg[0]){
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
            for (i=0; i<width*2+1; i++)
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
    int ret = -1;
    int size;
    unsigned long addr = 0;
    unsigned long length = 0;
    void *buf = NULL;

    if (argc < 2)
        return ret;

    if ((size = cmd_get_data_size(argv[1], 4)) < 0)
        return 1;

    addr = strtoul(argv[2], 0, 0);

    if (argc > 2)
        length = strtoul(argv[3], NULL, 0);

    buf = (void *)(unsigned long)addr;
    print_buffer(addr, buf, size, length, DISP_LINE_LEN / size);
    return 1;
}

MK_CMD(mem, do_mem_display, "Display memory contents",
    "    -display mem\n"
    "help command ...\n"
    "     mem b|w|l|q addr [len]"
    "\n"
);

MK_CMD(help, do_help, "Print command description/usage",
    "    - print brief description of all commands\n"
    "help command ...\n"
    "    - print detailed usage of 'command'"
    "\n"
);

static int process_cmd(int argc, char * argv[])
{
	int ret;
	cmd_tbl *cmdp;

	cmdp = find_cmd(argv[0]);
	if (cmdp == NULL)
		return -1;
	ret = cmdp->cmd(argc, argv);
	return ret ? 0 : 0;
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
#endif

#if 0
MK_CMD(dog, do_wdt, "some watchdog features",
    "{reset}\n"
    "    - watchdog expired causes a debug reset\n"
    "dog {enable}\n"
    "    - enable the IMC watchdog\n"
    "dog {disable}\n"
    "    - disable the IMC watchdog\n"
    "dog {kick}\n"
    "    - kick the IMC watchdog\n"
);

int ddr_test(uint64_t star_addr, uint64_t end_addr, uint64_t length, uint64_t offset)
{
    uint64_t *p;
    uint64_t i = 0;
    uint64_t temp_star,temp_end;
    uint64_t test_size = 0;

    test_size = length / SIZE_1M;
    p = (uint64_t *)star_addr;
    temp_star = star_addr;
    temp_end = temp_star + length;
    printf("\nddr test begin \n");

    while(temp_star < end_addr) {
        i = 0;
        p = (uint64_t *)temp_star;

        printf("0x%010x - 0x%010x:\n", temp_star, temp_end);
        while ((uint64_t)p < temp_end) {
            *p++ = i++;
        }
        printf("write success \n");

        i = 0;
        p = (uint64_t *)temp_star;
        while ((uint64_t)p < temp_end) {
            if (*p++ != i++) {
                printf("ddr read failure %p(%d) on \n",p, i);
                return -1;
            }
        }
        printf("read success\n");
        printf("-----------------------------------%d M test completed\n",test_size);
        temp_star = temp_star + offset + length;
        temp_end =  temp_star + length;

        if(temp_end > end_addr)
            temp_end = end_addr;
        test_size += ((temp_end - temp_star)/SIZE_1M);
    }
    printf("===========ddr test complete ============\n");
    return 0;
}

static int do_ddr_test(int argc, char * argv[])
{
    uint64_t star_addr = 0;
    uint64_t end_addr = 0;
    uint64_t offset = 0;
    uint64_t length = 0;
    uint64_t size = 0;

    switch (argc) {
        case 3:
            length = SIZE_1M;
            offset = 0;
            break;
        case 5:
            length = strtoul(argv[3], NULL, 0) * SIZE_1M;
            offset = strtoul(argv[4], NULL, 0) * SIZE_1M;
            break;
        default:
            printf("The num of argument is not correct \n");
            return -1;
            break;
    }

    star_addr = strtoul(argv[1], NULL, 0);
    size = strtoul(argv[2], NULL, 0) * SIZE_1M;

    if(((star_addr < 0) || (star_addr > MAX_DDR_SIZE)) || (star_addr % 0x1000 != 0)) {
        printf("ERROR:The star_addr is invalid, check if it over the max ddr size or not aline with 4K \n");
        return -1;
    }

    if(size < 0) {
        printf("ERROR:The size is invalid \n");
        return -1;
    }

    if(star_addr + size > MAX_DDR_SIZE) {
        printf("WARNING:The test region is over the max ddr size,change the end point to the max ddr addr \n");
        end_addr = MAX_DDR_SIZE;
    } else
        end_addr = star_addr + size;

    if(length > size){
        printf("ERROR:the length is bigger than the size you want to test.\n");
        return -1;
    }
    if(offset < 0) {
        printf("ERROR:the offset is invalid \n");
        return -1;
    }

    ddr_test(star_addr, end_addr, length, offset);

    return 0;
}


MK_CMD(tsens, do_tsens, "configure controllers and sensors before get temperature",
    "\n"
    "tsens T {MIN|LOWER|CRITICAL|UPPER|MAX} <-50-150>\n"
    "  -set tsens {MIN|LOWER|CRITICAL|UPPER|MAX} Threshold from -50 to 150\n"
    "  -this command will work after tsens init finishs\n"
    "tsens init [1-11]\n"
    "  -configure [1-11] controller and sensors(default init all contollers)\n"
    "tsens <0-99>\n"
    "  -read temprature of [0-99] sensors\n"
    "tsens all\n"
    "  -read temprature of 99 sensors\n"
    "tsens max\n"
    "  -read max temprature in all alive sensors\n"
    "tsens fuse\n"
    "  -read tsens fuses\n"
);

MK_CMD(avsbus, do_avsbus, "avsbus initialization, set or get voltage, get temp or power",
    "avsbus init\n"
    "    - avsbus initialization\n"
    "avsbus APC voltage\n"
    "    - set APC rail voltage(mV)\n"
    "avsbus CBF voltage\n"
    "    - set CBF rail voltage(mV)\n"
    "avsbus temp\n"
    "    - read apc and cbf rail temp\n"
    "avsbus power\n"
    "    - read apc and cbf rail power\n"
);

MK_CMD(getfuse, do_get_fuse, "print command description/usage",
    " getfuse \n"
    "    -get the fuse data\n"
);

MK_CMD(ddr, do_ddr_enable, "raw_info",
    " ddr init \n"
    "    -ddr enable. \n"
);

MK_CMD(apm, do_apm_switch, "raw_info",
     " apm -a \n"
    "    -switch to apc rail \n"
     " apm -m  \n"
    "    -switch to mx rail \n"
);

MK_CMD(cpr, do_cpr_measurement, "get the cpr result",
    " get the cpr result \n"
    " cpr all : measure all domains \n"
    " ----get all domain cpr counter result \n"
    " cpr {apr0~7| cx | mx | vnd | vds | cbf} \n"
    " ----get the target domain cpr count result \n"
);

MK_CMD(spinor, do_spinor, "use spi master to read/write spinor flash",
    "\n"
    " spinor {init} \n"
    "    -Initialize the spinor flash  \n"
    " spinor {burn} \n"
    "    -Burn the sigleimage into the spinor flash \n"
);

MK_CMD(ddr_test, do_ddr_test, "test the ddr",
    " test the ddr\n"
    " ddr_test {star_addr} {size(M)} [length(M)] [step_offset(M)]\n"
    " test the target the ddr from the star_addr to star_addr + sizet ,test the length size and then interval a step_offset."
    " ddr_test {star_addr} {size(M)}\n"
    " Continuous testing of a complete range from star_addr to star_addr + sizet."
);
#endif
