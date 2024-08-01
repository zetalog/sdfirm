#include <target/espi.h>
#include <target/panic.h>
#include <target/bitops.h>

#ifdef SYS_REALTIME
#define espi_poll_init()	__espi_poll_init()
#define espi_irq_init()		do { } while (0)
#else
#define espi_poll_init()	do { } while (0)
#define espi_irq_init()		__espi_irq_init()
#endif

static bh_t espi_bh;
uint8_t espi_state;
espi_event_t espi_event;
espi_op_t espi_op;
espi_cmpl_cb espi_op_cb;
uint16_t espi_cmd;

void espi_enter_state(uint8_t state)
{
	espi_state = state;
	espi_seq_handler();
}

void espi_raise_event(espi_event_t event)
{
	espi_event |= event;
	bh_resume(espi_bh);
}

void espi_clear_event(espi_event_t event)
{
	espi_event &= ~event;
}

espi_event_t espi_event_save(void)
{
	espi_event_t events;
	events = espi_event;
	espi_event = 0;
	return events;
}

void espi_event_restore(espi_event_t event)
{
	espi_event |= event;
}

void espi_sync(void)
{
	do {
		bh_sync();
		if (espi_event)
			bh_resume(espi_bh);
	} while (espi_event);
}

void espi_config_alert_pin(uint32_t slave_caps, uint32_t *slave_config, uint32_t *ctrlr_config)
{
	switch (ESPI_ALERT_PIN) {
	case ESPI_GEN_ALERT_MODE_IO1:
		*slave_config &= ~ESPI_GEN_ALERT_MODE_PIN;
		break;
	case ESPI_GEN_ALERT_MODE_PIN:
		*ctrlr_config |= ESPI_ALERT_MODE;
		*slave_config |= ESPI_GEN_ALERT_MODE_PIN;
		if (ESPI_ALERT_TYPE == ESPI_GEN_ALERT_TYPE_OD) {
			if (slave_caps & ESPI_GEN_OPEN_DRAIN_ALERT_SUP) {
				*slave_config |= ESPI_GEN_OPEN_DRAIN_ALERT_SEL;
				break;
			}
			con_log("espi: open drain alert PIN not supported, falls to push pull.");
		}
		*slave_config &= ~ESPI_GEN_OPEN_DRAIN_ALERT_SEL;
		break;
	default:
		BUG();
	}
}

void espi_config_io_mode(uint32_t slave_caps, uint32_t *slave_config,
			 uint32_t *ctrlr_config)
{
	switch (ESPI_IO_MODE) {
	case ESPI_GEN_IO_MODE_QUAD:
		if (espi_slave_io_mode_sup_quad(slave_caps)) {
			*slave_config |= ESPI_GEN_IO_MODE_SEL(ESPI_GEN_IO_MODE_QUAD);
			*ctrlr_config |= ESPI_GEN_IO_MODE_QUAD;
			break;
		}
		con_log("espi: quad I/O not supported, falls to dual I/O.\n");
	case ESPI_GEN_IO_MODE_DUAL:
		if (espi_slave_io_mode_sup_dual(slave_caps)) {
			*slave_config |= ESPI_GEN_IO_MODE_SEL(ESPI_GEN_IO_MODE_DUAL);
			*ctrlr_config |= ESPI_GEN_IO_MODE_DUAL;
			break;
		}
		con_log("espi: dual I/O not supported, falls to single I/O.\n");
	case ESPI_GEN_IO_MODE_SINGLE:
		/* Single I/O mode is always supported. */
		*slave_config |= ESPI_GEN_IO_MODE_SEL(ESPI_GEN_IO_MODE_SINGLE);
		*ctrlr_config |= ESPI_GEN_IO_MODE_SINGLE;
		break;
	default:
		BUG();
		break;
	}
}

void espi_config_op_freq(uint32_t slave_caps, uint32_t *slave_config, uint32_t *ctrlr_config)
{
	int slave_max_speed_mhz = espi_slave_op_freq_sup_max(slave_caps);

	switch (ESPI_OP_FREQ) {
	case ESPI_GEN_OP_FREQ_66MHZ:
		if (slave_max_speed_mhz >= 66) {
			*slave_config |= ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_66MHZ);
			*ctrlr_config |= ESPI_GEN_OP_FREQ_66MHZ;
			break;
		}
	case ESPI_GEN_OP_FREQ_50MHZ:
		if (slave_max_speed_mhz >= 50) {
			*slave_config |= ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_50MHZ);
			*ctrlr_config |= ESPI_GEN_OP_FREQ_50MHZ;
			break;
		}
	case ESPI_GEN_OP_FREQ_33MHZ:
		if (slave_max_speed_mhz >= 33) {
			*slave_config |= ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_33MHZ);
			*ctrlr_config |= ESPI_GEN_OP_FREQ_33MHZ;
			break;
		}
	case ESPI_GEN_OP_FREQ_25MHZ:
		if (slave_max_speed_mhz > 0) {
			*slave_config |= ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_25MHZ);
			*ctrlr_config |= ESPI_GEN_OP_FREQ_25MHZ;
			break;
		}
	case ESPI_GEN_OP_FREQ_20MHZ:
		if (slave_max_speed_mhz > 0) {
			*slave_config |= ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_20MHZ);
			*ctrlr_config |= ESPI_GEN_OP_FREQ_20MHZ;
			break;
		}
	default:
		BUG();
	}
}

int espi_start_op(espi_op_t op, espi_cmpl_cb cb)
{
	if (espi_op_busy())
		return -EBUSY;

	espi_op = op;
	espi_op_cb = cb;
	espi_seq_handler();
	return 0;
}

void espi_cmd_complete(uint8_t status)
{
	espi_clear_event(ESPI_EVENT_WAIT_CMD);
}

void espi_handle_setup_slave(bool is_op)
{
	if (espi_state == ESPI_STATE_INIT) {
		espi_inband_reset();
	}
}

void espi_seq_handler(void)
{
	if (espi_op_is(ESPI_OP_SETUP_SLAVE))
		espi_handle_setup_slave(true);
}

static void espi_async_handler(void)
{
	espi_event_t flags;

	flags = espi_event_save();
	if (flags & ESPI_EVENT_SETUP) {
		espi_setup_slave();
		unraise_bits(flags, ESPI_EVENT_SETUP);
	}
	espi_event_restore(flags);
}

static void espi_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		espi_hw_handle_irq();
		return;
	} else {
		switch (events) {
		case BH_WAKEUP:
			espi_async_handler();
			break;
		default:
			BUG();
		}
	}
}

#ifdef SYS_REALTIME
void __espi_poll_init(void)
{
	irq_register_poller(espi_bh);
}
#else
void __espi_irq_init(void)
{
	espi_hw_irq_init();
}
#endif

void espi_write_cmd(uint8_t opcode, uint8_t hlen, uint8_t *hbuf,
		    uint8_t dlen, uint8_t *dbuf)
{
	espi_cmd = opcode;
	espi_hw_write_cmd(opcode, hlen, hbuf, dlen, dbuf);
}

void espi_init(void)
{
	espi_bh = bh_register_handler(espi_bh_handler);
	espi_irq_init();
	espi_poll_init();

	espi_state = ESPI_STATE_INIT;
	espi_event = 0;
	espi_op = ESPI_OP_NONE;
	espi_cmd = ESPI_CMD_NONE;

	espi_hw_ctrl_init();
	espi_raise_event(ESPI_EVENT_SETUP);
}
