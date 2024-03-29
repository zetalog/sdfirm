#include <target/generic.h>
#include <target/perf.h>
#include <target/cmdline.h>
#include <asm/mach/bmu.h>
#define bmu_irq2unit(irq)	0

static bool bmu_once = false;

void bmu_dump_rd(int n)
{
	uint32_t addr, size;

	bmu_vector_size(n, RD, size);
	while (size) {
		bmu_vector_next(n, RD, addr);
		printf("RD Duration: %08lx\n", (unsigned long)addr);
		size--;
	}
}

void bmu_dump_wr(int n)
{
	uint32_t addr, size;

	bmu_vector_size(n, WR, size);
	while (size) {
		bmu_vector_next(n, WR, addr);
		printf("WR Duration: %08lx\n", (unsigned long)addr);
		size--;
	}
}

void bmu_dump_cnt(int n)
{
	printf("RD_TRAN_CNT: %ld\n",
	       (unsigned long)bmu_read_counter(n, BMU_RD_TRAN_CNT));
	printf("RD_DATA_CNT: %ld\n",
	       (unsigned long)bmu_read_counter(n, BMU_RD_DATA_CNT));
	printf("RD_DURATION_CNT: %ld\n",
	       (unsigned long)bmu_read_counter(n, BMU_RD_DURATION_CNT));
	printf("RD_DRT_OVTHR_CNT: %ld\n",
	       (unsigned long)bmu_read_counter(n, BMU_RD_DRT_OVTHR_CNT));
	printf("WR_TRAN_CNT: %ld\n",
	       (unsigned long)bmu_read_counter(n, BMU_WR_TRAN_CNT));
	printf("WR_DATA_CNT: %ld\n",
	       (unsigned long)bmu_read_counter(n, BMU_WR_DATA_CNT));
	printf("WR_DURATION_CNT: %ld\n",
	       (unsigned long)bmu_read_counter(n, BMU_WR_DURATION_CNT));
	printf("WR_DRT_OVTHR_CNT: %ld\n",
	       (unsigned long)bmu_read_counter(n, BMU_WR_DRT_OVTHR_CNT));
	printf("FILT_RD_TRAN_CNT: %ld\n",
	       (unsigned long)bmu_read_counter(n, BMU_FILT_RD_TRAN_CNT));
	printf("FILT_WR_TRAN_CNT: %ld\n",
	       (unsigned long)bmu_read_counter(n, BMU_FILT_WR_TRAN_CNT));
}

void bmu_handle_irq(int irq)
{
	int n = bmu_irq2unit(irq);
	uint32_t irqs = BMU_INTR_STAT(n);

	if (irqs & bmu_wr_res_time_out_int) {
		printf("WR RES timeout\n");
		bmu_dump_wr(n);
	}
	if (irqs & bmu_rd_res_time_out_int) {
		printf("WR RES timeout\n");
		bmu_dump_rd(n);
	}
	if (irqs & bmu_awready_tmo_int)
		printf("AWREADY timeout\n");
	if (irqs & bmu_arready_tmo_int)
		printf("ARREADY timeout\n");
	if (irqs & bmu_wr_error_res_int)
		printf("WR ERROR RRESP: %s\n",
		       bmu_error_res(n, WR) == AXI_RESP_DECERR ?
		       "DECERR" : "SLVERR");
	if (irqs & bmu_rd_error_res_int)
		printf("RD ERROR RRESP: %s\n",
		       bmu_error_res(n, RD) == AXI_RESP_DECERR ?
		       "DECERR" : "SLVERR");
	if (irqs & bmu_wr_data_hit_int)
		printf("WR DATA hit\n");
	if (irqs & bmu_rd_data_hit_int)
		printf("RD DATA hit\n");
	if (irqs & bmu_wr_addr_hit_int)
		printf("WR ADDR hit\n");
	if (irqs & bmu_rd_addr_hit_int)
		printf("RD ADDR hit\n");
	if (irqs & bmu_task_done_int)
		bmu_dump_cnt(n);
}

void bmu_reinit(int n)
{
	uint32_t bmu_data[4] = {0, 0, 0, 0};
	uint64_t bmu_addr = 0, bmu_mask = 0xFFFFFFFFFFFFFFFF;
	uint64_t bmu_lower = 0, bmu_upper = 0;
	uint64_t bmu_len = 0;

	bmu_ctrl_stop(n);
	bmu_status_clr(n);
	bmu_config_data_match(n, bmu_data, 0xffffffff);
	bmu_config_addr_match(n, bmu_addr, bmu_mask);
	bmu_config_target_id(n, RD, 0, 0);
	bmu_config_target_id(n, WR, 0, 0);
	bmu_config_address_range(n, BMU_FILT_RD_TRAN_CNT,
				 bmu_lower, bmu_upper);
	bmu_filter_address_range(n, BMU_FILT_RD_TRAN_CNT);
	bmu_config_address_range(n, BMU_FILT_WR_TRAN_CNT,
				 bmu_lower, bmu_upper);
	bmu_filter_address_range(n, BMU_FILT_WR_TRAN_CNT);
	bmu_filter_address_align(n, BMU_FILT_RD_TRAN_CNT,
				 bmu_addr, bmu_mask);
	bmu_filter_address_align(n, BMU_FILT_WR_TRAN_CNT,
				 bmu_addr, bmu_mask);
	bmu_config_target_length(n, BMU_FILT_RD_TRAN_CNT, bmu_len);
	bmu_filter_target_length(n, BMU_FILT_RD_TRAN_CNT);
	bmu_config_target_length(n, BMU_FILT_WR_TRAN_CNT, bmu_len);
	bmu_filter_target_length(n, BMU_FILT_WR_TRAN_CNT);
	bmu_config_duration_threshold(n, RD, 0);
	bmu_config_duration_threshold(n, WR, 0);
	bmu_config_ovtime_threshold(n, 0);
	bmu_config_awready_threshold(n, 0);
	bmu_config_arready_threshold(n, 0);
	bmu_ctrl_start(n, bmu_once);
}

void bmu_match_data(int n, uint32_t data[], uint64_t mask)
{
	bmu_ctrl_stop(n);
	bmu_status_clr(n);
	bmu_config_data_match(n, data, mask);
	bmu_ctrl_start(n, bmu_once);
}

void bmu_match_addr(int n, uint64_t addr, uint64_t mask)
{
	bmu_ctrl_stop(n);
	bmu_status_clr(n);
	bmu_config_addr_match(n, addr, mask);
	bmu_ctrl_start(n, bmu_once);
}

static void bmu_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		bmu_handle_irq(BMU_IRQ);
		return;
	}
}

#ifdef SYS_REALTIME
static void bmu_poll_init(void)
{
	irq_register_poller(bmu_bh);
}
#else
static void bmu_irq_init(void)
{
	irqc_configure_irq(BMU_IRQ, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(BMU_IRQ, lpc_handle_irq);
	irqc_enable_irq(BMU_IRQ);
}
#endif

void bmu_init(void)
{
	int n;
	uint32_t bmu_data[4] = {0, 0, 0, 0};
	uint64_t bmu_addr = 0, bmu_mask = 0xFFFFFFFFFFFFFFFF;
	uint64_t bmu_lower = 0, bmu_upper = 0;
	uint64_t bmu_len = 0;

	for (n = 0; n < NR_BMUS; n++) {
		bmu_mask_all_irqs(n);
		bmu_config_data_match(n, bmu_data, 0xffffffff);
		bmu_config_addr_match(n, bmu_addr, bmu_mask);
		bmu_config_target_id(n, RD, 0, 0);
		bmu_config_target_id(n, WR, 0, 0);
		bmu_config_address_range(n, BMU_FILT_RD_TRAN_CNT,
					 bmu_lower, bmu_upper);
		bmu_filter_address_range(n, BMU_FILT_RD_TRAN_CNT);
		bmu_config_address_range(n, BMU_FILT_WR_TRAN_CNT,
					 bmu_lower, bmu_upper);
		bmu_filter_address_range(n, BMU_FILT_WR_TRAN_CNT);
		bmu_filter_address_align(n, BMU_FILT_RD_TRAN_CNT,
					 bmu_addr, bmu_mask);
		bmu_filter_address_align(n, BMU_FILT_WR_TRAN_CNT,
					 bmu_addr, bmu_mask);
		bmu_config_target_length(n, BMU_FILT_RD_TRAN_CNT, bmu_len);
		bmu_filter_target_length(n, BMU_FILT_RD_TRAN_CNT);
		bmu_config_target_length(n, BMU_FILT_WR_TRAN_CNT, bmu_len);
		bmu_filter_target_length(n, BMU_FILT_WR_TRAN_CNT);
		bmu_config_duration_threshold(n, RD, 0);
		bmu_config_duration_threshold(n, WR, 0);
		bmu_config_ovtime_threshold(n, 0);
		bmu_config_awready_threshold(n, 0);
		bmu_config_arready_threshold(n, 0);
		bmu_ctrl_enter_monitor(n);
		bmu_ctrl_start(n, true);
	}
	bmu_irq_init();
	bmu_poll_init();
}

void bmu_toggle_once(int n)
{
	bmu_once = !bmu_once;

	bmu_ctrl_stop(n);
	bmu_status_clr(n);
	bmu_ctrl_start(n, bmu_once);

	if (bmu_once)
		printf("Switched to once.\n");
	else
		printf("Switched from once.\n");
}

static int do_bmu_config(int n, int argc, char *argv[])
{
	uint64_t bmu_addr,bmu_addr_low,bmu_addr_high,bmu_mask,bmu_length,bmu_threshold;
	uint32_t bmu_data[4];
	int i;
	int bmu_id,bmu_counter;
	if (argc < 4)
		return -EINVAL;
	if (strcmp(argv[4], "id") == 0) {	
		bmu_id=(int)strtoul(argv[5],0,0);
		bmu_mask = (uint64_t)strtoull(argv[6], 0, 0);
		if (strcmp(argv[3], "RD") == 0) {
			bmu_config_target_id(n,RD,bmu_id,bmu_mask);
		}
		else if (strcmp(argv[3], "WR") == 0) {
			bmu_config_target_id(n,WR,bmu_id,bmu_mask);
		}
	}
	else if (strcmp(argv[4], "range") == 0){
		bmu_counter=(int)strtoul(argv[3],0,0);
		bmu_addr_low=(uint64_t)strtoul(argv[5],0,0);
		bmu_addr_high=(uint64_t)strtoul(argv[6],0,0);
		bmu_config_address_range(n,bmu_counter,bmu_addr_low,bmu_addr_high);
	}
	else if (strcmp(argv[4], "length") == 0){
		bmu_counter=(int)strtoul(argv[3],0,0);
		bmu_length=(uint64_t)strtoul(argv[5],0,0);
		bmu_config_target_length(n,bmu_counter,bmu_length);
	}
	else if (strcmp(argv[3], "duration") == 0){
		if (strcmp(argv[4], "ovtime") == 0){
			bmu_threshold=(uint64_t)strtoul(argv[5],0,0);
			bmu_config_ovtime_threshold(n,bmu_threshold);
		}
		else{
			bmu_threshold=(uint64_t)strtoul(argv[5],0,0);
			if (strcmp(argv[3], "RD") == 0) {
				bmu_config_duration_threshold(n,RD,bmu_threshold);
			}
			else if (strcmp(argv[3], "WR") == 0) {
				bmu_config_duration_threshold(n,WR,bmu_threshold);
			}
			
		}
	}
	else if (strcmp(argv[3], "awready") == 0){
			bmu_threshold=(uint64_t)strtoul(argv[4],0,0);
			bmu_config_awready_threshold(n,bmu_threshold);
	}
	else if (strcmp(argv[3], "arready") == 0){
			bmu_threshold=(uint64_t)strtoul(argv[4],0,0);
			bmu_config_arready_threshold(n,bmu_threshold);
	}
	else if (strcmp(argv[3], "data") == 0){
			bmu_mask = (uint64_t)strtoull(argv[5], 0, 0);
			for (i = 0; i < 4; i++)
				bmu_data[i] = (uint32_t)strtoul(argv[4 + i], 0, 0);
			bmu_config_data_match(n,bmu_data,bmu_mask);
	}
	else if (strcmp(argv[3], "addr") == 0){
			bmu_mask = (uint64_t)strtoull(argv[5], 0, 0);
			bmu_addr = (uint64_t)strtoull(argv[4], 0, 0);
			bmu_config_addr_match(n,bmu_addr,bmu_mask);
	}
	return 0;
}

static int do_bmu_filter(int n, int argc, char *argv[])
{
	uint64_t bmu_counter,bmu_addr_high,bmu_addr_low,bmu_length,bmu_addr,bmu_mask;
	if (argc < 4)
		return -EINVAL;
	if (strcmp(argv[4], "range") == 0) {
		bmu_counter=(int)strtoul(argv[3],0,0);	
		bmu_filter_address_range(n,bmu_counter);
	}	
	else if (strcmp(argv[4], "align") == 0){
		bmu_counter=(int)strtoul(argv[3],0,0);
		bmu_addr=(uint64_t)strtoul(argv[5],0,0);
		bmu_mask=(uint64_t)strtoul(argv[6],0,0);
		bmu_filter_address_align(n,bmu_counter,bmu_addr,bmu_mask);
	}
	else if (strcmp(argv[4], "length") == 0){
		bmu_counter=(int)strtoul(argv[3],0,0);
		bmu_filter_target_length(n,bmu_counter);
	}
	return 0;
}

static int do_bmu_match(int n, int argc, char *argv[])
{
	uint32_t bmu_data[4];
	uint64_t bmu_addr, bmu_mask;
	int i;

	if (argc < 4)
		return -EINVAL;
	if (strcmp(argv[3], "data") == 0) {
		if (argc < 9)
			return -EINVAL;
		for (i = 0; i < 4; i++)
			bmu_data[i] = (uint32_t)strtoul(argv[4 + i], 0, 0);
		bmu_mask = (uint64_t)strtoull(argv[8], 0, 0);
		bmu_match_data(n, bmu_data, bmu_mask);
		return 0;
	}
	if (strcmp(argv[3], "addr") == 0) {
		if (argc < 6)
			return -EINVAL;
		bmu_addr = (uint64_t)strtoull(argv[4], 0, 0);
		bmu_mask = (uint64_t)strtoull(argv[5], 0, 0);
		bmu_match_addr(n, bmu_addr, bmu_mask);
		return 0;
	}
	return -EINVAL;
}

static int do_bmu(int argc, char *argv[])
{
	int bmu;

	if (argc < 3)
		return -EINVAL;

	bmu = (int)strtoull(argv[2], 0, 0);
	if (strcmp(argv[1], "config") == 0)
		return do_bmu_config(bmu, argc, argv);
	if (strcmp(argv[1], "filter") == 0)
		return do_bmu_filter(bmu, argc, argv);
	if (strcmp(argv[1], "match") == 0)
		return do_bmu_match(bmu, argc, argv);
	if (strcmp(argv[1], "once") == 0) {
		bmu_toggle_once(bmu);
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(bmu, do_bmu, "K1MXLite bus monitor unit commands",
	"bmu config <n> data <data> <mask>"
	"	-config bmu data match"
	"bmu config <n> addr <addr> <mask>"
	"	-config bmu addr match"
	"bmu config <n> <counter> id <id> <mask>\n"
	"    -config bmu counter target_id\n"
	"bmu config <n> <counter> range <low> <high> \n"
	"    -config bmu counter address range\n"
	"bmu config <n> <counter> length <length>\n"
    "    -config bmu counter burst length\n"
	"bmu config <n> <counter> duration <threshold> \n"
    "    -config bmu counter duration threshold\n"
	"bmu config <n> duration ovtime <threshold>\n"
    "    -config bmu counter duration threshold\n"
	"bmu config <n> awready <threshold>\n"
    "    -config bmu counter duration threshold\n"
	"bmu config <n> arready <threshold>\n"
    "    -config bmu counter duration threshold\n"
	"bmu filter <n> <counter> range "
	"    -config bmu counter filter address_range\n"
	"bmu filter <n> <counter> align <address> <mask>"
	"    -config bmu counter filter address_align\n"
	"bmu filter <n> <counter> length"
	"    -config bmu counter filter burst_length\n"
	"bmu match  data <data> <mask>\n"
	"    -match bus data\n"
	"bmu match  addr <addr> <mask>\n"
	"    -match bus addr\n"
);
