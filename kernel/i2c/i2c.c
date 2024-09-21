#include <target/i2c.h>
#include <target/panic.h>
#include <target/bh.h>

#ifdef SYS_REALTIME
#define i2c_poll_init()		__i2c_poll_init()
#define i2c_irq_init()		do { } while (0)
#else /* SYS_REALTIME */
#define i2c_poll_init()		do { } while (0)
#define i2c_irq_init()		__i2c_irq_init()
#endif /* SYS_REALTIME */

bh_t i2c_bh;

#if NR_I2C_MASTERS > 1
struct i2c_master i2c_masters[NR_I2C_MASTERS];
i2c_t i2c_mid;

void i2c_master_select(i2c_t i2c)
{
	i2c_hw_master_select(i2c);
	i2c_mid = i2c;
}

i2c_t i2c_master_save(i2c_t i2c)
{
	i2c_t i2cs = i2c_mid;
	i2c_mid = i2c;
	return i2cs;
}
#else
/* slave address addressed by the i2c controller */
i2c_addr_t i2c_target;
/* address used by the i2c controller when it acts as a slave device */
i2c_addr_t i2c_address;
i2c_addr_t i2c_abrt_slave;
uint16_t i2c_freq;
uint8_t i2c_mode;
/* transfer variables */
i2c_len_t i2c_limit;
i2c_len_t i2c_current;
i2c_len_t i2c_commit;
i2c_len_t i2c_rxsubmit;
i2c_len_t i2c_txsubmit;
uint8_t i2c_status;
uint8_t i2c_state;
i2c_event_t i2c_event;
i2c_device_t *i2c_device = NULL;
#endif

const char *i2c_status_names[] = {
	"IDLE",
	"START",
	"ACK",
	"NACK",
	"ARBI",
	"STOP",
};

const char *i2c_state_names[] = {
	"IDLE",
	"WAIT",
	"READ",
	"WRITE",
};

const char *i2c_event_names[] = {
	"IDLE",
	"START",
	"PAUSE (ACK)",
	"ABORT (NACK)",
	"ARBITRATION",
	"STOP",
};

const char *i2c_status_name(uint8_t status)
{
	if (status >= ARRAY_SIZE(i2c_status_names))
		return "UNKNOWN";
	return i2c_status_names[status];
}

const char *i2c_state_name(uint8_t state)
{
	if (state >= ARRAY_SIZE(i2c_state_names))
		return "UNKNOWN";
	return i2c_state_names[state];
}

const char *i2c_event_name(uint8_t event)
{
	int i, nr_evts = 0;

	for (i = 0; i < ARRAY_SIZE(i2c_event_names); i++) {
		if (_BV(i) & event) {
			nr_evts++;
			return i2c_event_names[i];
		}
	}
	return "NONE";
}

void i2c_raise_event(uint8_t event)
{
	con_dbg("i2c: event = %s\n", i2c_event_name(event));
	i2c_event |= event;
	bh_resume(i2c_bh);
}

#ifdef CONFIG_I2C_10BIT_ADDRESS
static void i2c_write_address(void)
{
}
#else
static void i2c_write_address(void)
{
	if (i2c_txsubmit <= I2C_ADDR_LEN)
		i2c_write_byte(i2c_addr_mode(i2c_target, I2C_MODE_RX));
	else
		i2c_write_byte(i2c_addr_mode(i2c_target, I2C_MODE_TX));
}
#endif

void i2c_enter_state(uint8_t state)
{
	con_dbg("i2c: state = %s\n", i2c_state_name(state));
	i2c_state = state;
	if (state == I2C_STATE_IDLE) {
		if (i2c_rxsubmit == 0)
			i2c_hw_stop_condition();
	} else if (state == I2C_STATE_WAIT)
		i2c_hw_start_condition(false);
	else if (state == I2C_STATE_WRITE) {
		if (i2c_current < I2C_ADDR_LEN)
			i2c_write_address();
		else if (i2c_current < i2c_limit)
			i2c_device->iocb(1);
	} else if (state == I2C_STATE_READ) {
		if (i2c_current < i2c_limit)
			i2c_device->iocb(1);
	}
}

static void i2c_unraise_event(uint8_t event)
{
	i2c_event &= ~event;
}

uint8_t i2c_bus_dir_mode(void)
{
	return I2C_BUS_DIR(i2c_mode);
}

uint8_t i2c_bus_mode(void)
{
	return I2C_BUS(i2c_mode);
}

uint8_t i2c_dir_mode(void)
{
	return I2C_DIR(i2c_mode);
}

static void i2c_transfer_reset(void)
{
	i2c_set_status(I2C_STATUS_IDLE);
	i2c_hw_transfer_reset();
}

bool i2c_last_byte(void)
{
	return i2c_current == i2c_limit;
}

bool i2c_prev_byte(void)
{
	return (i2c_current + 1) == i2c_limit;
}

bool i2c_first_byte(void)
{
	if (i2c_dir_mode() == I2C_MODE_RX)
		return i2c_current == 0;
	else
		return i2c_current == I2C_ADDR_LEN;
}

uint8_t i2c_read_byte(void)
{
	uint8_t byte;

	BUG_ON(!i2c_device);
	if (i2c_state != I2C_STATE_READ)
		return 0;

	byte = i2c_hw_read_byte();
	return byte;
}

void i2c_write_byte(uint8_t byte)
{
	BUG_ON(!i2c_device);
	if (i2c_state != I2C_STATE_WRITE)
		return;

	i2c_hw_write_byte(byte);
}

void i2c_apply_frequency(void)
{
	i2c_hw_set_frequency(I2C_FREQ);
}

void i2c_master_abort(i2c_addr_t slave)
{
	i2c_abrt_slave = slave;
	i2c_raise_event(I2C_EVENT_ABORT);
}

void i2c_master_start(void)
{
	i2c_raise_event(I2C_EVENT_START);
}

void i2c_master_stop(void)
{
	i2c_raise_event(I2C_EVENT_STOP);
}

void i2c_master_commit(i2c_len_t len)
{
	BUG_ON(i2c_current + i2c_commit > i2c_limit);
	i2c_commit = len;
}

void i2c_master_submit(i2c_addr_t slave, i2c_len_t txlen, i2c_len_t rxlen)
{
	i2c_target = slave;

	/* IDLE means start, STOP means repeated start */
	if (i2c_state == I2C_STATE_IDLE) {
		i2c_current = 0;
		i2c_txsubmit = txlen + I2C_ADDR_LEN;
		i2c_rxsubmit = rxlen;
		if (i2c_txsubmit > 0) {
			i2c_limit = i2c_txsubmit;
			i2c_config_mode(I2C_MODE_MASTER_TX, true);
		} else if (i2c_rxsubmit > 0) {
			i2c_limit = i2c_rxsubmit;
			i2c_config_mode(I2C_MODE_MASTER_RX, true);
		}
	} else if (i2c_state == I2C_STATE_WRITE) {
		i2c_txsubmit += txlen;
		i2c_limit += txlen;
		i2c_rxsubmit += rxlen;
	} else if (i2c_state == I2C_STATE_READ) {
		i2c_rxsubmit += rxlen;
		i2c_limit += rxlen;
	}

	if (i2c_state == I2C_STATE_IDLE)
		i2c_enter_state(I2C_STATE_WAIT);

	while (i2c_state != I2C_STATE_IDLE)
		bh_sync();
}

uint8_t i2c_master_write(i2c_addr_t slave, i2c_len_t txlen)
{
	i2c_master_submit(slave, txlen, 0);
	return i2c_current;
}

uint8_t i2c_master_read(i2c_addr_t slave, i2c_len_t rxlen)
{
	i2c_master_submit(slave, 0, rxlen);
	while (i2c_state != I2C_STATE_IDLE)
		bh_sync();
	return i2c_current;
}

void i2c_master_release(void)
{
	i2c_hw_stop_condition();
	i2c_transfer_reset();
}

#ifdef CONFIG_I2C_GENERAL_CALL
uint8_t i2c_general_call(uint8_t cbyte, i2c_len_t limit)
{
	uint8_t status;

	/* IDLE means start, STOP means repeated start */
	BUG_ON(!i2c_device || (cbyte == I2C_CALL_INVAL) ||
	       (i2c_status != I2C_STATUS_IDLE &&
	        i2c_status != I2C_STATUS_STOP));

	i2c_config_mode(I2C_MODE_MASTER_TX, true);
	i2c_target = cbyte;
	i2c_limit = limit + 2;
	i2c_current = 0;

	i2c_hw_start_condition(false);
	while (i2c_status == I2C_STATUS_IDLE ||
	       i2c_status == I2C_STATUS_STOP);

	i2c_write_byte(i2c_addr_mode(I2C_ADDR_CALL, I2C_MODE_TX));
	i2c_write_byte(i2c_target);
	i2c_device->iocb(limit);

	status = i2c_status;
	i2c_master_release();
	return status;
}
#endif

#ifdef CONFIG_I2C_DEVICE_ID
uint32_t i2c_probe_devid(i2c_addr_t slave)
{
	uint8_t i;
	i2c_devid_t devid = I2C_DEVID_INVAL;

	/* IDLE means start, STOP means repeated start */
	BUG_ON(!i2c_device ||
	       (i2c_status != I2C_STATUS_IDLE &&
	        i2c_status != I2C_STATUS_STOP));

	i2c_config_mode(I2C_MODE_MASTER_TX, true);
	i2c_target = slave;
	i2c_limit = 1 + I2C_ADDR_LEN;
	i2c_current = 0;

	i2c_hw_start_condition(false);
	while (i2c_status == I2C_STATUS_IDLE ||
	       i2c_status == I2C_STATUS_STOP);

	i2c_write_byte(i2c_addr_mode(I2C_ADDR_DEVID, I2C_MODE_TX));
	i2c_write_address();
	if (i2c_status != I2C_STATUS_STOP) {
		i2c_master_release();
		return I2C_DEVID_INVAL;
	}

	i2c_config_mode(I2C_MODE_MASTER_RX, false);
	i2c_target = slave;
	i2c_limit = I2C_DEVID_LEN + I2C_ADDR_LEN;
	i2c_current = 0;
	i2c_write_address();

	for (i = 0; i < 3; i++)
		devid |= (i2c_read_byte() << (3-i));
	if (i2c_status != I2C_STATUS_STOP) {
		i2c_master_release();
		return I2C_DEVID_INVAL;
	}

	i2c_master_release();
	return devid;
}
#endif

#ifdef CONFIG_I2C_SLAVE
#ifdef CONFIG_I2C_10BIT_ADDRESS
static void i2c_read_address(void)
{
}
#else
static void i2c_read_address(void)
{
	uint8_t byte;

	byte = i2c_hw_read_byte();
	I2C_SET_DIR(i2c_mode, byte & I2C_DIR_MASK);
	i2c_target = (byte & I2C_ADDR_MASK) >> I2C_ADDR_OFFSET;
}
#endif

void i2c_set_address(i2c_addr_t addr, boolean call)
{
	i2c_address = addr;

	if (call)
		i2c_mode |= I2C_MODE_CALL;
	else
		i2c_mode &= ~I2C_MODE_CALL;
}

void i2c_apply_address(void)
{
	if (i2c_device)
		i2c_hw_set_address(i2c_address, I2C_CALL(i2c_mode));
}
#endif

void i2c_config_mode(uint8_t mode, bool freq)
{
	i2c_mode = mode;

	if (I2C_BUS(i2c_mode) == I2C_MODE_MASTER) {
		if (freq)
			i2c_apply_frequency();
	} else {
		i2c_apply_address();
	}
}

void i2c_register_device(i2c_device_t *dev)
{
	i2c_device = dev;
}

void i2c_set_status(uint8_t status)
{
	con_dbg("i2c: status = %s\n", i2c_status_name(status));
	i2c_status = status;
#if 0
	if (status == I2C_STATUS_IDLE)
		i2c_enter_state(I2C_STATE_IDLE);
#endif
	if (status == I2C_STATUS_START)
		i2c_raise_event(I2C_EVENT_START);
	if (status == I2C_STATUS_STOP)
		i2c_raise_event(I2C_EVENT_STOP);
	if (status == I2C_STATUS_ARBI)
		i2c_raise_event(I2C_EVENT_ABORT);
	if (status == I2C_STATUS_NACK)
		i2c_raise_event(I2C_EVENT_ABORT);
	if (status == I2C_STATUS_ACK) {
		i2c_current++;
		if (i2c_current == i2c_limit)
			i2c_raise_event(I2C_EVENT_STOP);
		else
			i2c_raise_event(I2C_EVENT_PAUSE);
	}
}

void i2c_master_init(void)
{
	i2c_hw_ctrl_init();
}

static void i2c_handle_irq(void)
{
	i2c_t i2c;
	__unused i2c_t si2c;

	for (i2c = 0; i2c < NR_I2C_MASTERS; i2c++) {
		si2c = i2c_master_save(i2c);
		i2c_hw_handle_irq();
		i2c_master_restore(si2c);
	}
}

static void i2c_handle_xfr(void)
{
	uint8_t event = i2c_event;

	if (!event)
		return;

	i2c_unraise_event(event);
	switch (i2c_state) {
	case I2C_STATE_READ:
		if (event & I2C_EVENT_PAUSE)
			i2c_enter_state(I2C_STATE_READ);
		if (event & I2C_EVENT_ABORT)
			i2c_enter_state(I2C_STATE_IDLE);
		if (event & I2C_EVENT_STOP)
			i2c_enter_state(I2C_STATE_IDLE);
		break;
	case I2C_STATE_WRITE:
		if (event & I2C_EVENT_PAUSE)
			i2c_enter_state(I2C_STATE_WRITE);
		if (event & I2C_EVENT_ABORT)
			i2c_enter_state(I2C_STATE_IDLE);
		if (event & I2C_EVENT_STOP) {
			if (i2c_rxsubmit > 0) {
				i2c_current = 0;
				i2c_limit = i2c_rxsubmit;
				i2c_config_mode(I2C_MODE_MASTER_RX, false);
				i2c_enter_state(I2C_STATE_READ);
			} else
				i2c_enter_state(I2C_STATE_IDLE);
		}
		break;
	case I2C_STATE_WAIT:
		if (event & I2C_EVENT_START) {
			if (i2c_dir_mode() == I2C_MODE_TX)
				i2c_enter_state(I2C_STATE_WRITE);
			else
				i2c_enter_state(I2C_STATE_READ);
		}
	case I2C_STATE_IDLE:
		break;
	}
}

void i2c_sync_status(void)
{
	while (i2c_event)
		bh_sync();
}

static void i2c_handle_bh(void)
{
	i2c_t i2c;
	__unused i2c_t si2c;

	for (i2c = 0; i2c < NR_I2C_MASTERS; i2c++) {
		si2c = i2c_master_save(i2c);
		i2c_handle_xfr();
		i2c_master_restore(si2c);
	}
}

static void i2c_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ)
		i2c_handle_irq();
	else
		i2c_handle_bh();
}

#ifdef SYS_REALTIME
static void __i2c_poll_init(void)
{
	irq_register_poller(i2c_bh);
}
#else
static void __i2c_irq_init(void)
{
	i2c_hw_irq_init();
}
#endif

void i2c_init(void)
{
	i2c_t i2c;
	__unused i2c_t si2c;

	i2c_bh = bh_register_handler(i2c_bh_handler);
	for (i2c = 0; i2c < NR_I2C_MASTERS; i2c++) {
		si2c = i2c_master_save(i2c);
		i2c_set_status(I2C_STATUS_IDLE);
		i2c_master_init();
		i2c_irq_init();
		i2c_transfer_reset();
		i2c_master_restore(si2c);
	}
	i2c_poll_init();
}
