#include <target/mem.h>
#include <target/readline.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/i2c.h>
#include <target/spd.h>

#define foreach_cmd(cmdp)		\
	for (cmdp = __cmd_start; cmdp < __cmd_end; cmdp++)
#define MAXARGS				10

#define I2C_TOOL_DATA_BUF_SIZE 8
#define I2C_CLK_FREQ_MAX_KHZ (5 * 1000)	// 5.0 MHz

static int do_i2cmem(int argc, char *argv[])
{
	unsigned int master_num;
	uint16_t freq_khz = 300;
	uint8_t slave_addr = 0x10;
	unsigned int mem_addr = 0xA8;
	int mem_addr_size = 1;
	uint8_t data_buf_tx[I2C_TOOL_DATA_BUF_SIZE];
	uint8_t data_buf_rx[I2C_TOOL_DATA_BUF_SIZE];
	int data_size = 2;
	int i;
	int err_cnt = 0;
	int ret;
	
	for (i = 0; i < I2C_TOOL_DATA_BUF_SIZE; i++) {
		data_buf_tx[i] = 0xD0 + i;
		data_buf_rx[i] = 0;
	}

	dw_i2c_init();

	master_num = 0;
	printf("i2cmem master num = %u, freq = %ukHz, slave_addr = 0x%x, mem_addr = 0x%x, data_size = %d\n",
			master_num, freq_khz, slave_addr, mem_addr, data_size);

	i2c_master_select(master_num);
	i2c_master_init();
	i2c_set_frequency(freq_khz);
	ret = dw_i2c_write_mem(slave_addr, mem_addr, mem_addr_size,
			       data_buf_tx, data_size);
	if (ret != 0) {
		return -1;
	}
	ret = dw_i2c_read_mem(slave_addr, mem_addr, mem_addr_size,
			      data_buf_rx, data_size);
	if (ret != 0) {
		return -1;
	}

	for (i = 0; i < data_size; i++) {
		if (data_buf_rx[i] != data_buf_tx[i])
			err_cnt++;
	}
	if (err_cnt > 0) {
		printf("i2cmem Failed. err_cnt = %d\n", err_cnt);
		return err_cnt;
	} else {
		printf("i2cmem Success\n");
		return 0;
	}
}

DEFINE_COMMAND(i2cmem, do_i2cmem, "I2C Tool for memory Slave",
	"i2cmem usage\n"
);

static int do_i2cvip(int argc, char *argv[])
{
	unsigned int master_num;
	uint16_t freq_khz = 100;
	uint8_t slave_addr = 0x10;
	static unsigned int mem_addr = 0x01;
	uint8_t data_buf_tx[I2C_TOOL_DATA_BUF_SIZE];
	uint8_t data_buf_rx[I2C_TOOL_DATA_BUF_SIZE];
	int data_size = 4;
	int i;
	int ret;

	if (argc >= 3) {
		master_num = (unsigned int)strtoul(argv[1], NULL, 10);
		if (master_num >= CONFIG_I2C_MAX_MASTERS) {
			printf("Invalid I2C Master number %d. Valid: 0 - %d\n",
			       master_num, CONFIG_I2C_MAX_MASTERS);
			return -1;
		}
		freq_khz = (uint16_t)strtoul(argv[2], NULL, 10);
		if (freq_khz <= 0 || freq_khz > I2C_CLK_FREQ_MAX_KHZ) {
			printf("Invalid I2C clock frequency %u KHz. Max: %d\n",
			       freq_khz, I2C_CLK_FREQ_MAX_KHZ);
			return -1;
		}
	}
	for (i = 0; i < I2C_TOOL_DATA_BUF_SIZE; i++) {
		data_buf_tx[i] = 0xD0 + i;
		data_buf_rx[i] = 0;
	}

	dw_i2c_init();

	master_num = 0;
	printf("i2cvip master num = %u, freq = %ukHz, slave_addr = 0x%x, mem_addr = 0x%x, data_size = %d\n",
	       master_num, freq_khz, slave_addr, mem_addr, data_size);

	i2c_master_select(master_num);
	i2c_master_init();
	i2c_set_frequency(freq_khz);

	/* Write test */
	ret = dw_i2c_write_vip(slave_addr, mem_addr,
			       data_buf_tx, data_size);
	if (ret != 0)
		return -1;

	/* This write to mem_addr-1 sets the memory address to the
	 * starting postion the previous write started and gets ready for
	 * the following read.
	 */
	ret = dw_i2c_write_vip(slave_addr, mem_addr-1,
			       data_buf_tx, 1);
	if (ret != 0)
		return -1;

	/* Read test */
	ret = dw_i2c_read_vip(slave_addr, data_buf_rx, data_size);
	if (ret != 0)
		return -1;

	printf("Test result:\n");
	for (i = 0; i < data_size; i++) {
		printf("[%d] w %02x r %02x\n", i,
		       data_buf_tx[i], data_buf_rx[i]);
	}
	printf("i2cvip End\n");
	return 0;
}

DEFINE_COMMAND(i2cvip, do_i2cvip, "I2C Tool for VIP Slave",
	"i2ctool usage\n"
);

#ifdef CONFIG_DPU_DDR_SPD_COUNT
#define SPD_CNT CONFIG_DPU_DDR_SPD_COUNT
#else
#define SPD_CNT 1
#endif

/* Write test not enabled at present */
//#define SPD_TEST_WRITE

#if 0
#define SPD_DATA_SIZE 512
#else
#define SPD_DATA_SIZE 256
#endif
static int do_i2cspd(int argc, char *argv[])
{
#ifdef SPD_TEST_WRITE
	uint8_t data_buf_tx[SPD_DATA_SIZE];
#endif
	uint8_t data_buf_rx[SPD_DATA_SIZE] = {0};
	int len;
	int ret;
	int i;
	uint8_t spd_num = 0;

#ifdef SPD_TEST_WRITE
	for (i = 0; i < SPD_DATA_SIZE; i++) {
		data_buf_tx[i] = (uint8_t)(0xD0 + i);
	}
#endif

	printf("SPD initiate\n");
	ret = spd_hw_init();
	if (ret != 0) {
		printf("Error: Failed spd_hw_init. ret = %d\n", ret);
		return 0;
	}

	for (spd_num = 0; spd_num < SPD_CNT; spd_num++) {
		len = 8;
		printf("SPD[%d] read a few bytes\n", spd_num);
		ret = spd_hw_read_bytes(spd_num, 0, data_buf_rx, len);
		for (i = 0; i < len; i++) {
			printf("%d-%02x ", i, data_buf_rx[i]);
		}
		printf("\n");
	}

#ifdef SPD_TEST_WRITE
	for (spd_num = 0; spd_num < SPD_CNT; spd_num++) {
		printf("SPD[%d] write a few bytes\n", spd_num);
		ret = spd_hw_write_bytes(spd_num, 0, data_buf_tx, len);

		printf("SPD[%d] read a few bytes again\n", spd_num);
		ret = spd_hw_read_bytes(spd_num, 0, data_buf_rx, len);
		for (i = 0; i < len; i++) {
			printf("%d-%02x ", i, data_buf_rx[i]);
		}
		printf("\n");

		len = SPD_DATA_SIZE;
		printf("SPD[%d] write full SDP\n", spd_num);
		ret = spd_hw_write_bytes(spd_num, 0, data_buf_tx, len);
		printf("SPD[%d] read full SDP\n", spd_num);
		ret = spd_hw_read_bytes(spd_num, 0, data_buf_rx, len);
		for (i = 0; i < len; i++) {
			if (i % 37 != 0) break; /* Sample print */
			printf("%d-%02x", i, data_buf_rx[i]);
		}
		printf("\n");
	}
#endif
	printf("i2cspd End\n");
	return 0;
}

DEFINE_COMMAND(i2cspd, do_i2cspd, "I2C Tool for SPD Slave",
	"i2cspd usage\n"
);
