#include <target/generic.h>
#include <target/perf.h>

#define bmu_irq2unit(irq)	0

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
	bmu_ctrl_start(n, true);
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
