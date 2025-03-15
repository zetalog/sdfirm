#include <stdio.h>
#include <stdlib.h>
#include <target/dma.h>
#include <target/cmdline.h>
#include <target/console.h>

typedef struct {
	char opt[30];
	int (*parse)(char *option);
} dmatest_opt_t;

static caddr_t cpu_src_addr;
static caddr_t cpu_dst_addr;
static dma_addr_t dma_src_addr;
static dma_addr_t dma_dst_addr;
static dma_t dmatest_chan = 0;
static size_t transfer_size = PAGE_SIZE;
static bool dmatest_verify = true;

static void dmatest(void)
{
	uint32_t failed_cnt = 0;

	printf("dmatest use channel[%d] transfer size: %ld\n", dmatest_chan, transfer_size);
	dma_request_channel(dmatest_chan, DMA_FROM_DEVICE, NULL);

	cpu_src_addr = dma_alloc_coherent(dmatest_chan, PAGE_SIZE, &dma_src_addr);
	cpu_dst_addr = dma_alloc_coherent(dmatest_chan, PAGE_SIZE, &dma_dst_addr);

	if (!cpu_src_addr || !cpu_dst_addr) {
		printf("dmatest alloc failed size: 0x%lx\n", transfer_size);
		goto failed;
	}

	uint8_t *p_src = (uint8_t *)cpu_src_addr;
	uint8_t *p_dst = (uint8_t *)cpu_dst_addr;
	for(int i = 0; i < PAGE_SIZE; i++) {
		p_src[i] = i;
	}

	dma_memcpy_sync(dmatest_chan, dma_dst_addr, dma_src_addr, PAGE_SIZE, 0);

	if (dmatest_verify) {
		size_t cnt = 32;
		for(int i = 0; i < PAGE_SIZE; i++) {
			if (p_src[i] != p_dst[i]) {
				if (failed_cnt < cnt) {
					printf("[%d]except: 0x%02x get:0x%02x\n", 
					i ,
					p_src[i],
					p_dst[i]);
				}
				failed_cnt ++;
			}
		}
	}

	printf("dmatest transfer completed %s\n", failed_cnt ? "failed" : "successfully");
failed:
	if (cpu_src_addr) dma_free_coherent(dmatest_chan, PAGE_SIZE, cpu_src_addr, dma_src_addr);
	if (cpu_dst_addr) dma_free_coherent(dmatest_chan, PAGE_SIZE, cpu_dst_addr, dma_dst_addr);
	dma_release_channel(dmatest_chan);
}

static int channel_parse(char *option)
{
	uint32_t val = (uint32_t)strtoull(option, 0, 0);
	if (val >= NR_DMAS) {
		printf("channel id must be < %d\n", NR_DMAS);
		return -1;
	}
	dmatest_chan = val;

	return 0;
}

static int verify_parse(char *option)
{
	uint32_t val = (uint32_t)strtoull(option, 0, 0);
	dmatest_verify = (val > 0 ? true : false);

	return 0;
}

static int transfer_size_parse(char *option)
{
	transfer_size = (uint32_t)strtoull(option, 0, 0);
	return 0;
}

static dmatest_opt_t long_options[] = {
	{"chan", channel_parse},
	{"verify", verify_parse},
	{"len", transfer_size_parse}
};

static int __parseopt(int argc, char *argv[])
{
	int i,j;
	for (i = 0; i < argc; i++) {
		for (j = 0; j < ARRAY_SIZE(long_options); j++) {
			if (strcmp(argv[i]+2, long_options[j].opt) == 0) {
				if (i + 1 < argc) {
					long_options[j].parse(argv[i + 1]);
					i ++;
				}
			}
		}
	}

	return 0;
}

static int dmatest_help(void)
{
	printf("dmatest --chan <chan id> --len <transfer size> --verify <1/0>\n"
		"	chan: channel id (default: 0)\n"
		"	len: transfer size (default: PAGE_SIZE)\n"
		"	verify: enable/disable data verification (default: 1)\n");
	return 0;
}
static int do_dmatest(int argc, char *argv[])
{
	if (argc == 2 && (strcmp(argv[1], "--help") == 0)) {
		dmatest_help();
	} else {
		__parseopt(argc, argv);
		dmatest();
	}
	return 0;
}

DEFINE_COMMAND(dmatest, do_dmatest, "DMA test global commands",
	"dmatest\n"
);
