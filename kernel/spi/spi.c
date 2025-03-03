#include <target/panic.h>
#include <target/spi.h>

#ifdef SYS_REALTIME
#define spi_poll_init()		__spi_poll_init()
#define spi_irq_init()		do { } while (0)
#else
#define spi_poll_init()		do { } while (0)
#define spi_irq_init()		__spi_irq_init()
#endif

static bh_t spi_bh;
spi_device_t *spi_devices[NR_SPI_DEVICES];
spi_t spi_last_id = 0;
uint8_t spi_last_mode = INVALID_SPI_MODE;
uint8_t spi_target;
spi_addr_t spi_address;
uint32_t spi_last_freq = 0;
spi_len_t spi_limit;
spi_len_t spi_current;
spi_len_t spi_commit;
spi_len_t spi_rxsubmit;
spi_len_t spi_txsubmit;
uint8_t spi_status;
uint8_t spi_state;
uint16_t spi_event;
uint8_t spi_mode;
spi_device_t *spi_device = NULL;

#ifdef CONFIG_SPI_DEBUG
#define spi_dbg(...)	con_dbg(__VA_ARGS__)
#else
#define spi_dbg(...)	do { } while (0)
#endif

const char *spi_status_names[] = {
	"IDLE",
	"START",
	"ACK",
	"NACK",
	"ARBI",
	"STOP",
};

const char *spi_state_names[] = {
	"IDLE",
	"WAIT",
	"READ",
	"WRITE",
};

const char *spi_event_names[] = {
	"IDLE",
	"START",
	"PAUSE (ACK)",
	"ABORT (NACK)",
	"ARBITRATION",
	"STOP",
};

uint8_t spi_dir_mode(void)
{
	return SPI_DIR(spi_mode);
}

bool spi_last_byte(void)
{
	return spi_current == spi_limit;
}

bool spi_prev_byte(void)
{
	return (spi_current + 1) == spi_limit;
}

bool spi_first_byte(void)
{
	if (spi_dir_mode() == SPI_MODE_RX)
		return spi_current == 0;
	else
		return spi_current == SPI_ADDR_LEN;
}

void spi_write_byte(uint8_t byte)
{
	spi_hw_write_byte(byte);
}

uint8_t spi_read_byte(void)
{
	return spi_hw_read_byte();
}

uint8_t spi_txrx(uint8_t byte)
{
	spi_tx(byte);
	return spi_rx();
}
void spi_slave_commit(spi_len_t len)
{
	BUG_ON(spi_current + spi_commit > spi_limit);
	spi_commit = len;
}

void spi_slave_submit(uint8_t slave, spi_len_t txlen, spi_len_t rxlen)
{
	spi_target = slave;

	/* IDLE means start, STOP means repeated start */
	if (spi_state == SPI_STATE_IDLE) {
		spi_current = 0;
		spi_txsubmit = txlen + SPI_ADDR_LEN;
		spi_rxsubmit = rxlen;
		if (spi_txsubmit > 0) {
			spi_limit = spi_txsubmit;
			spi_config_mode(SPI_MODE_MASTER_TX, true);
		} else if (spi_rxsubmit > 0) {
			spi_limit = spi_rxsubmit;
			spi_config_mode(SPI_MODE_MASTER_RX, true);
		}
	} else if (spi_state == SPI_STATE_WRITE) {
		spi_txsubmit += txlen;
		spi_limit += txlen;
		spi_rxsubmit += rxlen;
	} else if (spi_state == SPI_STATE_READ) {
		spi_rxsubmit += rxlen;
		spi_limit += rxlen;
	}

	if (spi_state == SPI_STATE_IDLE)
		spi_enter_state(SPI_STATE_WAIT);

	while (spi_state != SPI_STATE_IDLE)
		bh_sync();
}

void spi_set_status(uint8_t status)
{
	spi_dbg("spi: status = %s\n", spi_status_name(status));
	spi_status = status;
#if 0
	if (status == SPI_STATUS_IDLE)
		spi_enter_state(SPI_STATE_IDLE);
#endif
	if (status == SPI_STATUS_START)
		spi_raise_event(SPI_EVENT_START);
	if (status == SPI_STATUS_STOP)
		spi_raise_event(SPI_EVENT_STOP);
	if (status == SPI_STATUS_ARBI)
		spi_raise_event(SPI_EVENT_ABORT);
	if (status == SPI_STATUS_NACK)
		spi_raise_event(SPI_EVENT_ABORT);
	if (status == SPI_STATUS_ACK) {
		spi_current++;
		if (spi_current == spi_limit)
			spi_raise_event(SPI_EVENT_STOP);
		else
			spi_raise_event(SPI_EVENT_PAUSE);
	}
}

#ifdef CONFIG_SPI_MASTER
spi_t spi_register_device(spi_device_t *dev)
{
	spi_t spi;

	BUG_ON(spi_last_id == INVALID_SPI_DID || !dev);

	spi = spi_last_id;
	spi_devices[spi] = dev;
	spi_last_id++;
	return spi;
}

void __spi_config_master(uint32_t khz, uint8_t mode)
{
	spi_last_mode = mode;
	spi_last_freq = khz;
	spi_hw_ctrl_stop();
	spi_hw_config_mode(mode);
	spi_hw_config_freq(min(khz, SPI_MAX_FREQ));
	spi_hw_ctrl_start();
}

static inline void spi_config_master(uint32_t khz, uint8_t mode)
{
	if ((spi_last_mode != mode) || (spi_last_freq != khz)) {
		__spi_config_master(khz, mode);
	}
}

void spi_select_device(spi_t spi)
{
	BUG_ON(spi >= spi_last_id);
	spi_config_master(spi_devices[spi]->max_freq_khz,
			  spi_devices[spi]->mode);
	spi_hw_chip_select(spi_devices[spi]->chip);
}

void spi_apply_frequency(void)
{
	spi_hw_set_frequency(SPI_FREQ);
}
#endif

void spi_raise_event(uint8_t event)
{
	spi_dbg("spi: event = %s\n", spi_event_name(event));
	spi_event |= event;
	bh_resume(spi_bh);
}

static void spi_write_address(void)
{
	if (spi_txsubmit <= SPI_ADDR_LEN)
		spi_write_byte(spi_addr_mode(spi_target, SPI_MODE_RX));
	else
		spi_write_byte(spi_addr_mode(spi_target, SPI_MODE_TX));
}

void spi_enter_state(uint8_t state)
{
	spi_dbg("spi: state = %s\n", spi_state_name(state));
	spi_state = state;
	if (state == SPI_STATE_IDLE) {
		if (spi_rxsubmit == 0)
			spi_hw_stop_condition();
	} else if (state == SPI_STATE_WAIT)
		spi_hw_start_condition(false);
	else if (state == SPI_STATE_WRITE) {
		if (spi_current < SPI_ADDR_LEN)
			spi_write_address();
		// else if (spi_current < spi_limit)
		// 	spi_device->iocb(1);
	} else if (state == SPI_STATE_READ) {
		// if (spi_current < spi_limit)
		// 	spi_device->iocb(1);
	}
}

void spi_config_mode(uint8_t mode, bool freq)
{
	spi_mode = mode;

	if (SPI_BUS(spi_mode) == SPI_MODE_MASTER) {
		if (freq)
			spi_apply_frequency();
	}
}

static void spi_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		spi_hw_handle_irq();
		return;
	}
}

#ifdef SYS_REALTIME
void __spi_poll_init(void)
{
	irq_register_poller(spi_bh);
}
#else
void __spi_irq_init(void)
{
	spi_hw_irq_init();
}
#endif


void spi_init(void)
{
	spi_bh = bh_register_handler(spi_bh_handler);
	spi_irq_init();
	spi_poll_init();
	spi_hw_ctrl_init();
}
