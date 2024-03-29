#include <target/generic.h>
#include <target/perf.h>
#include <target/cmdline.h>

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
	return 0;
}

static int do_bmu_filter(int n, int argc, char *argv[])
{
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

DEFINE_COMMAND(bmu, do_bmu, "K1Matrix bus monitor unit commands",
	"bmu config\n"
	"    -config bmu conter\n"
	"bmu filter\n"
	"    -config bmu counter filter\n"
	"bmu match unit data d0 d1 d2 d3 mask\n"
	"    -match bus data\n"
	"bmu match unit addr addr mask\n"
	"    -match bus address\n"
	"bmu once unit\n"
	"    -toggle once/multiple times\n"
);
