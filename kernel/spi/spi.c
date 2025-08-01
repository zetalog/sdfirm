#include <target/panic.h>
#include <target/spi.h>
#include <target/console.h>
#include <target/cmdline.h>
#include <target/bh.h>
#include <target/irq.h>

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

#if NR_SPI_MASTERS > 1
struct spi_master *spi_masters[NR_SPI_MASTERS];
spi_t spi_mid;

void spi_master_select(spi_t spi)
{
	spi_hw_master_select(spi);
	spi_mid = spi;
}

spi_t spi_master_save(spi_t spi)
{
	spi_t spis = spi_mid;
	spi_master_select(spi);
	return spis;
}
#else
spi_device_t *spi_devices[NR_SPI_DEVICES];
spi_t spi_last_id = 0;
uint8_t spi_last_mode = INVALID_SPI_MODE;
uint8_t spi_chip;
uint32_t spi_max_freq_khz;
uint32_t spi_last_freq = 0;
uint8_t spi_mode;
spi_len_t spi_rxsubmit;
spi_len_t spi_txsubmit;
spi_len_t spi_limit;
spi_len_t spi_current;
spi_len_t spi_commit;
uint8_t spi_status;
uint8_t spi_state;
spi_event_t spi_event;
spi_device_t *spi_device = NULL;
#endif

const char *spi_status_names[] = {
	"IDLE",
	"READ",
	"WRITE",
};

const char *spi_state_names[] = {
	"IDLE",
	"READ",
	"WRITE",
};

const char *spi_event_names[] = {
	"IDLE",
	"READ",
	"WRITE",
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

void spi_enter_state(uint8_t state)
{
	spi_dbg("spi: state = %s\n", spi_state_name(state));
	spi_state = state;
	if (state == SPI_STATE_READ) {
		if (spi_current < spi_limit)
			spi_device->iocb(1);
	}
	else if (state == SPI_STATE_WRITE) {
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

void spi_write_byte(uint8_t byte)
{
	BUG_ON(!spi_device);
	// if (spi_state != SPI_STATE_WRITE)
	// 	return;

	spi_hw_write_byte(byte);
}

uint8_t spi_read_byte(void)
{
	BUG_ON(!spi_device);
	// if (spi_state != SPI_STATE_READ)
	// 	return 0;

	return spi_hw_read_byte();
}

uint8_t spi_txrx(uint8_t byte)
{
	spi_tx(byte);
	return spi_rx();
}

void spi_submit_write(spi_len_t txlen)
{
	if (spi_state == SPI_STATE_IDLE) {
		spi_current = 0;
		spi_txsubmit = txlen;
		spi_limit = spi_txsubmit;
		spi_config_mode(SPI_MODE_TX);
	} else if (spi_state == SPI_STATE_WRITE) {
		spi_txsubmit += txlen;
		spi_limit += txlen;
	}

	while (spi_state != SPI_STATE_IDLE)
		bh_sync();
}

void spi_submit_read(spi_len_t rxlen)
{
	if (spi_state == SPI_STATE_IDLE) {
		spi_current = 0;
		spi_rxsubmit = rxlen;
		spi_limit = spi_rxsubmit;
		spi_config_mode(SPI_MODE_RX);
	} else if (spi_state == SPI_STATE_READ) {
		spi_rxsubmit += rxlen;
		spi_limit += rxlen;
	}

	while (spi_state != SPI_STATE_IDLE)
		bh_sync();
}

void spi_config_mode(uint8_t mode)
{
	spi_mode = mode;
}

void spi_set_status(uint8_t status)
{
	spi_dbg("spi: status = %s\n", spi_status_name(status));
	spi_status = status;
	if (status == SPI_STATUS_READ) {
		spi_raise_event(SPI_EVENT_READ);
		spi_current++;
	}
	else if (status == SPI_STATUS_WRITE) {
		spi_raise_event(SPI_EVENT_WRITE);
		spi_current++;
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
		spi_enter_state(SPI_STATE_READ);
		break;
	case SPI_STATE_WRITE:
		spi_enter_state(SPI_STATE_WRITE);
		break;
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
#else /* CONFIG_SPI_SLAVE */
spi_t spi_register_device(spi_device_t *dev)
{
	spi_devices[0] = dev;
	return 0;
}
#endif /* CONFIG_SPI_SLAVE */

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
