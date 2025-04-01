#include <target/panic.h>
#include <target/spi.h>
#include <target/console.h>
#include <target/cmdline.h>

#ifdef CONFIG_SPI_DEBUG
#define spi_dbg(...)	con_dbg(__VA_ARGS__)
#else
#define spi_dbg(...)	do { } while (0)
#endif

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
uint8_t spi_buf[64];
uint16_t spi_txlen;
uint16_t spi_rxlen;
spi_addr_t spi_address;
spi_addr_t spi_abrt_slave;
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

const char *spi_status_name(uint8_t status)
{
	if (status >= ARRAY_SIZE(spi_status_names))
		return "UNKNOWN";
	return spi_status_names[status];
}

const char *spi_state_name(uint8_t state)
{
	if (state >= ARRAY_SIZE(spi_state_names))
		return "UNKNOWN";
	return spi_state_names[state];
}

const char *spi_event_name(uint8_t event)
{
	int i, nr_evts = 0;

	for (i = 0; i < ARRAY_SIZE(spi_event_names); i++) {
		if (_BV(i) & event) {
			nr_evts++;
			return spi_event_names[i];
		}
	}
	return "NONE";
}

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
		else if (spi_current < spi_limit)
			spi_device->iocb(1);
	} else if (state == SPI_STATE_READ) {
		if (spi_current < spi_limit)
			spi_device->iocb(1);
	}
}

static void spi_unraise_event(uint8_t event)
{
	spi_event &= ~event;
}

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

void spi_master_abort(spi_addr_t slave)
{
	spi_abrt_slave = slave;
	spi_raise_event(SPI_EVENT_ABORT);
}

void spi_master_start(void)
{
	spi_raise_event(SPI_EVENT_START);
}

void spi_master_stop(void)
{
	spi_raise_event(SPI_EVENT_STOP);
}

void spi_master_commit(spi_len_t len)
{
	BUG_ON(spi_current + spi_commit > spi_limit);
	spi_commit = len;
}

void spi_master_submit(spi_addr_t slave, spi_len_t txlen, spi_len_t rxlen)
{
	spi_target = slave;

	/* IDLE means start, STOP means repeated start */
	if (spi_state == SPI_STATE_IDLE) {
		spi_current = 0;
		spi_txsubmit = txlen + SPI_ADDR_LEN;
		spi_rxsubmit = rxlen;
		if (spi_txsubmit > 0) {
			spi_limit = spi_txsubmit;
			spi_config_mode(SPI_MODE_MASTER_TX);
		} else if (spi_rxsubmit > 0) {
			spi_limit = spi_rxsubmit;
			spi_config_mode(SPI_MODE_MASTER_RX);
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

uint8_t spi_master_write(spi_addr_t slave, spi_len_t txlen)
{
	spi_master_submit(slave, txlen, 0);
	return spi_current;
}

uint8_t spi_master_read(spi_addr_t slave, spi_len_t rxlen)
{
	spi_master_submit(slave, 0, rxlen);
	while (spi_state != SPI_STATE_IDLE)
		bh_sync();
	return spi_current;
}

void spi_master_release(void)
{
	spi_hw_stop_condition();
}


void spi_config_mode(uint8_t mode)
{
	spi_mode = mode;
}

void spi_set_status(uint8_t status)
{
	spi_dbg("spi: status = %s\n", spi_status_name(status));
	spi_status = status;
	if (status == SPI_STATUS_IDLE)
		spi_enter_state(SPI_STATE_IDLE);
	if (status == SPI_STATUS_START)
		spi_raise_event(SPI_EVENT_START);
	if (status == SPI_STATUS_STOP)
		spi_raise_event(SPI_EVENT_STOP);
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

static void spi_handle_irq(void)
{
	spi_t spi;
	__unused spi_t sspi;

	for (spi = 0; spi < NR_SPI_DEVICES; spi++) {
		sspi = spi_master_save(spi);
		spi_hw_handle_irq();
		spi_master_restore(sspi);
	}
}

static void spi_handle_xfr(void)
{
	uint8_t event = spi_event;

	if (!event)
		return;

	spi_unraise_event(event);
	switch (spi_state) {
	case SPI_STATE_READ:
		if (event & SPI_EVENT_PAUSE)
			spi_enter_state(SPI_STATE_READ);
		if (event & SPI_EVENT_ABORT)
			spi_enter_state(SPI_STATE_IDLE);
		if (event & SPI_EVENT_STOP)
			spi_enter_state(SPI_STATE_IDLE);
		break;
	case SPI_STATE_WRITE:
		if (event & SPI_EVENT_PAUSE)
			spi_enter_state(SPI_STATE_WRITE);
		if (event & SPI_EVENT_ABORT)
			spi_enter_state(SPI_STATE_IDLE);
		if (event & SPI_EVENT_STOP) {
			if (spi_rxsubmit > 0) {
				spi_current = 0;
				spi_limit = spi_rxsubmit;
				spi_config_mode(SPI_MODE_MASTER_RX);
				spi_enter_state(SPI_STATE_READ);
			} else
				spi_enter_state(SPI_STATE_IDLE);
		}
		break;
	case SPI_STATE_WAIT:
		if (event & SPI_EVENT_START) {
			if (spi_dir_mode() == SPI_MODE_TX)
				spi_enter_state(SPI_STATE_WRITE);
			else
				spi_enter_state(SPI_STATE_READ);
		}
	case SPI_STATE_IDLE:
		break;
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
#endif

void spi_sync_status(void)
{
	while (spi_event)
		bh_sync();
}

static void spi_handle_bh(void)
{
	spi_t spi;
	__unused spi_t sspi;

	for (spi = 0; spi < NR_SPI_DEVICES; spi++) {
		sspi = spi_master_save(spi);
		spi_handle_xfr();
		spi_master_restore(sspi);
	}
}

static void spi_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ)
		spi_handle_irq();
	else
		spi_handle_bh();
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
	spi_t spi;
	__unused spi_t sspi;

	spi_bh = bh_register_handler(spi_bh_handler);
	for (spi = 0; spi < NR_SPI_DEVICES; spi++) {
		sspi = spi_master_save(spi);
		spi_set_status(SPI_STATUS_IDLE);
		spi_hw_ctrl_init();
		spi_irq_init();
		spi_master_restore(sspi);
	}
	spi_poll_init();
}

int do_spi(int argc, char *argv[])
{
	uint8_t spiread, spiwrite;
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "read") == 0) {
		spiread = spi_read_byte();
		printf("spi_read: 0x%x\n", spiread);
		return 0;
	}
	if (strcmp(argv[1], "write") == 0) {
		spiwrite = (uint8_t)strtoull(argv[2], 0, 0);
		spi_write_byte(spiwrite);
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(spi, do_spi, "SPI Commands",
	"spi read\n"
	"       -spi read data\n"
	"spi write <data>\n"
	"       -spi write data\n"
);