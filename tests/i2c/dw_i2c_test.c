#include <target/i2c.h>
#include <target/console.h>

void i2c_slave_init(caddr_t base, uint8_t addr);
/* Test configuration */
#define I2C_DEV_NUM_MASTER 0
#define I2C_DEV_NUM_SLAVE 1
#define I2C_SLAVE_ADDR 0x18
#define I2C_SPEED 100

/* From DW_apb_i2c configuration */
#define IC_TX_BUFFER_DEPTH 8
//#define IC_RX_BUFFER_DEPTH 8

#define DATA_BUF_SIZE (IC_TX_BUFFER_DEPTH)
static uint8_t data_buf_master_rx[DATA_BUF_SIZE];
static uint8_t data_buf_master_tx[DATA_BUF_SIZE];
static uint8_t data_buf_slave_rx[DATA_BUF_SIZE];
static uint8_t data_buf_slave_tx[DATA_BUF_SIZE];

static void print_data_buf(uint8_t *buf, int size)
{
	if (buf == NULL) return;
	if (size <= 0) return;

	for (int i = 0; i < size; i++) {
		con_printf("%X ", buf[i]);
	}
	con_printf("\n");
	return;
}

int i2c_test_init(void)
{
	int i;
	caddr_t slave_base = DW_I2C_BASE(I2C_DEV_NUM_SLAVE);

	con_printf("I2C Test: Initiate data bufers\n");
	for (i = 0; i < DATA_BUF_SIZE; i++) {
		data_buf_master_tx[i] = 0xA0 + i;
		data_buf_slave_tx[i] = 0x80 + i;
		data_buf_master_rx[i] = 0xFF;
		data_buf_slave_rx[i] = 0xFF;
	}

	con_printf("I2C Test: Prepare Slave\n");
	i2c_slave_init(slave_base, I2C_SLAVE_ADDR);

	con_printf("I2C Test: Prepare Master\n");
	/* TODO Select Master */
	i2c_apply_frequency();

	return 0;
}

int i2c_test_start(void)
{
	int i;
	int check_error;
	uint32_t val;
	caddr_t slave_base = DW_I2C_BASE(I2C_DEV_NUM_SLAVE);

	/*
	 * Transfer Test
	 */	
	con_printf("I2C Test: Transfer from Slave to Master\n");
	/* Prepare bulk data for slave */
	for (i = 0; i < DATA_BUF_SIZE; i++) {
		__raw_writel((uint32_t)data_buf_slave_tx[i], slave_base + IC_DATA_CMD);
	}
	/* Master read 1 byte by 1 byte */
	for (i = 0; i < DATA_BUF_SIZE; i++) {
		data_buf_master_rx[i] = i2c_master_read(I2C_SLAVE_ADDR, 1);
	}
	/* Check result */
	check_error = 0;
	for (i = 0; i < DATA_BUF_SIZE; i++) {
		if (data_buf_master_rx[i] != data_buf_slave_tx[i]) {
			check_error++;
			con_printf("I2C Test Error: data[%d] 0x %X != %X\n", i, 
					data_buf_master_rx[i], data_buf_slave_rx[i]);
		}
	}
	if (check_error == 0) {
		con_printf("I2C Test: Transfer from Slave to Master: OK\n");
	} else {
		con_printf("I2C Test: Transfer from Slave to Master: %d Error\n", check_error);
	}
	con_printf("Data at Slave\n");
	print_data_buf(data_buf_slave_tx, DATA_BUF_SIZE);
	con_printf("Data at Master\n");
	print_data_buf(data_buf_master_rx, DATA_BUF_SIZE);

	/*
	 * Get device ID of slave
	 */	
	con_printf("I2C Test: Get device ID of Slave\n");
	val = i2c_probe_devid(I2C_SLAVE_ADDR);
	con_printf("I2C Test: Get device ID of Slave = 0x%x\n", val);

	/* TODO General Call */

	return 0;
}

/*
 * Slave device operations
 */

void i2c_slave_init(caddr_t base, uint8_t addr)
{
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	__raw_writel(0, base + IC_ENABLE);
	__raw_writel(0 /* IC_SLAVE_DISABLE */
				 | IC_CON_RE /* IC_RESTART_EN */
				 /* IC_10BITADDR_MASTER = 0 */
				 /* IC_10BITADDR_SLAVE = 0 */
				 | IC_CON_SPD_SS /* SEEP = Stand */
				 | 0 /* IC_MASTER_MODE */
				 , base + IC_CON);
	__raw_writel(0, base + IC_RX_TL); /* 0 sets the threshold for 1 entry */
	__raw_writel(8, base + IC_TX_TL); /* 8 sets the threshold for 8 entry */
	__raw_writel(IC_INTR_ALL, base + IC_INTR_MASK);
	__raw_writel((uint32_t)addr, base + IC_SAR);
	__raw_writel(1, base + IC_ENABLE);

	return;
}
