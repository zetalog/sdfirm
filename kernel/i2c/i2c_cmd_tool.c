#include <target/mem.h>
#include <target/readline.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/i2c.h>

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

	i2c_hw_init();

	master_num = 0;
	printf("i2cmem master num = %u, freq = %ukHz, slave_addr = 0x%x, mem_addr = 0x%x, data_size = %d\n",
			master_num, freq_khz, slave_addr, mem_addr, data_size);

	ret = i2c_hw_master_select_by_num(master_num);
	if (ret < 0) {
		printf("Error: Invalid I2C master num = %d\n", master_num);
		return -1;
	}
	i2c_hw_ctrl_init();
	i2c_hw_set_frequency(freq_khz);
	ret = i2c_hw_write_mem(slave_addr, mem_addr, mem_addr_size, data_buf_tx, data_size);
	if (ret != 0) {
		return -1;
	}
	ret = i2c_hw_read_mem(slave_addr, mem_addr, mem_addr_size, data_buf_rx, data_size);
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
			printf("Invalid I2C Master number %d. Valid: 0 - %d\n", master_num, CONFIG_I2C_MAX_MASTERS);
			return -1;
		}
		freq_khz = (uint16_t)strtoul(argv[2], NULL, 10);
		if (freq_khz <= 0 || freq_khz > I2C_CLK_FREQ_MAX_KHZ) {
			printf("Invalid I2C clock frequency %u KHz. Max: %d\n", freq_khz, I2C_CLK_FREQ_MAX_KHZ);
			return -1;
		}
	}
	for (i = 0; i < I2C_TOOL_DATA_BUF_SIZE; i++) {
		data_buf_tx[i] = 0xD0 + i;
		data_buf_rx[i] = 0;
	}

	i2c_hw_init();

	master_num = 0;
	printf("i2cvip master num = %u, freq = %ukHz, slave_addr = 0x%x, mem_addr = 0x%x, data_size = %d\n",
			master_num, freq_khz, slave_addr, mem_addr, data_size);

	ret = i2c_hw_master_select_by_num(master_num);
	if (ret < 0) {
		printf("Error: Invalid I2C master num = %d\n", master_num);
		return -1;
	}
	i2c_hw_ctrl_init();

	//i2c_hw_set_frequency(freq_khz);

	/* Write test */
	ret = i2c_hw_write_vip(slave_addr, mem_addr, data_buf_tx, data_size);
	if (ret != 0) {
		return -1;
	}

	/* This write to mem_addr-1 sets the memory address to the starting postion the previous write started
	 * and gets ready for the following read. */
	ret = i2c_hw_write_vip(slave_addr, mem_addr-1, data_buf_tx, 1);
	if (ret != 0) {
		return -1;
	}

	/* Read test */
	ret = i2c_hw_read_vip(slave_addr, data_buf_rx, data_size);
	if (ret != 0) {
		return -1;
	}

	printf("Test result:\n");
	for (i = 0; i < data_size; i++) {
		printf("[%d] w %02x r %02x\n", i, data_buf_tx[i], data_buf_rx[i]);
	}
	printf("i2cvip End\n");
	return 0;
}

DEFINE_COMMAND(i2cvip, do_i2cvip, "I2C Tool for VIP Slave",
	"i2ctool usage\n"
);
