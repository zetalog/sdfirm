#include <target/i3c.h>
#include <target/panic.h>
#include <target/bh.h>

#ifdef CONFIG_I3C_DEBUG
#define i3c_dbg(...)		con_dbg(__VA_ARGS__)
#else
#define i3c_dbg(...)		do { } while (0)
#endif

#ifdef SYS_REALTIME
#define i3c_poll_init()		__i3c_poll_init()
#define i3c_irq_init()		do { } while (0)
#else /* SYS_REALTIME */
#define i3c_poll_init()		do { } while (0)
#define i3c_irq_init()		__i3c_irq_init()
#endif /* SYS_REALTIME */

bh_t i3c_bh;

#if NR_I3C_MASTERS > 1
struct i3c_master i3c_masters[NR_I3C_MASTERS];
i3c_t i3c_mid;

void i3c_master_select(i3c_t i3c)
{
	i3c_hw_master_select(i3c);
	i3c_mid = i3c;
}

i3c_t i3c_master_save(i3c_t i3c)
{
	i3c_t i3cs = i3c_mid;
	i3c_mid = i3c;
	return i3cs;
}
#else
uint8_t i3c_state;
uint8_t i3c_mode;
i3c_addr_t i3c_dev_addr;
i3c_len_t i3c_txsubmit;
i3c_len_t i3c_rxsubmit;
i3c_len_t i3c_limit;
i3c_len_t i3c_current;
i3c_len_t i3c_commit;
uint8_t i3c_status;
uint8_t i3c_state;
i3c_event_t i3c_event;
i3c_device_t *i3c_device = NULL;
DECLARE_BITMAP(i3c_addr_slot, I3C_NR_ADDRS);
DECLARE_BITMAP(i3c_addr_i2c, I3C_NR_ADDRS);
#endif

const char *i3c_state_names[] = {
	"IDLE",
	"WAIT",
	"READ",
	"WRITE",
	"START",
	"STOP",
};

const char *i3c_event_names[] = {
	"IDLE",
	"START",
	"ACK",
	"NACK",
	"ABORT",
};

const char *i3c_state_name(uint8_t state)
{
	if (state >= ARRAY_SIZE(i3c_state_names))
		return "UNKNOWN";
	return i3c_state_names[state];
}

const char *i3c_event_name(uint8_t event)
{
	int i, nr_evts = 0;

	for (i = 0; i < ARRAY_SIZE(i3c_event_names); i++) {
		if (_BV(i) & event) {
			nr_evts++;
			return i3c_event_names[i];
		}
	}
	return "NONE";
}

void i3c_raise_event(uint8_t event)
{
	i3c_dbg("i3c: event = %s\n", i3c_event_name(event));
	i3c_event |= event;
	bh_resume(i3c_bh);
}

void i3c_enter_state(uint8_t state)
{
	i3c_dbg("i3c: state = %s\n", i3c_state_name(state));
	i3c_state = state;
	if (state == I3C_STATE_IDLE) {
		if (i3c_rxsubmit == 0)
			i3c_hw_stop_condition();
	} else if (state == I3C_STATE_WAIT)
		i3c_hw_start_condition(false);
	else if (state == I3C_STATE_WRITE) {
		if (i3c_current < I3C_ADDR_LEN)
			i3c_write_address();
		else if (i3c_current < i3c_limit)
			i3c_device->iocb(1);
	} else if (state == I3C_STATE_READ) {
		if (i3c_current < i3c_limit)
			i3c_device->iocb(1);
	}
}

static void i3c_unraise_event(uint8_t event)
{
	i3c_event &= ~event;
}

uint8_t i3c_bus_dir_mode(void)
{
	return I3C_BUS_DIR(i3c_mode);
}

uint8_t i3c_bus_mode(void)
{
	return I3C_BUS(i3c_mode);
}

uint8_t i3c_dir_mode(void)
{
	return I3C_DIR(i3c_mode);
}

static void i3c_transfer_reset(void)
{
	i3c_set_status(I3C_STATUS_IDLE);
	i3c_hw_transfer_reset();
}

void i3c_set_status(uint8_t status)
{
	i3c_dbg("i3c: status = %s\n", i3c_status_name(status));
	i3c_status = status;
#if 0
	if (status == I3C_STATUS_IDLE)
		i3c_enter_state(I3C_STATE_IDLE);
#endif
	if (status == I3C_STATUS_START)
		i3c_raise_event(I3C_EVENT_START);
	if (status == I3C_STATUS_STOP)
		i3c_raise_event(I3C_EVENT_STOP);
	if (status == I3C_STATUS_NACK)
		i3c_raise_event(I3C_EVENT_ABORT);
	if (status == I3C_STATUS_ACK) {
		i3c_current++;
		if (i3c_current == i3c_limit)
			i3c_raise_event(I3C_EVENT_STOP);
		else
			i3c_raise_event(I3C_EVENT_PAUSE);
	}
}

i3c_addr_t i3c_get_free_addr(i3c_addr_t start_addr)
{
	i3c_addr_t addr;

	addr = find_next_clear_bit(i3c_addr_slot, I3C_NR_ADDRS, start_addr);
	if (addr == I3C_NR_ADDRS)
		return INVALID_I3C_ADDR;
	set_bit(addr, i3c_addr_slot);
	return addr;
}

static void i3c_addr_slot_init(void)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
		set_bit(i, i3c_addr_slot);
	set_bit(I3C_ADDR_BROADCAST, i3c_addr_slot);
	for (i = 0; i < 7; i++)
		set_bit(I3C_ADDR_BROADCAST ^ _BV(i), i3c_addr_slot);
}

void i3c_master_init(void)
{
	i3c_addr_slot_init();
	i3c_hw_ctrl_init();
}

static void i3c_handle_irq(void)
{
	i3c_t i3c;
	__unused i3c_t si3c;

	for (i3c = 0; i3c < NR_I3C_MASTERS; i3c++) {
		si3c = i3c_master_save(i3c);
		i3c_hw_handle_irq();
		i3c_master_restore(si3c);
	}
}

static void i3c_handle_xfr(void)
{
	uint8_t event = i3c_event;

	if (!event)
		return;

	i3c_unraise_event(event);
	switch (i3c_state) {
	case I3C_STATE_READ:
		if (event & I3C_EVENT_PAUSE)
			i3c_enter_state(I3C_STATE_READ);
		if (event & I3C_EVENT_ABORT)
			i3c_enter_state(I3C_STATE_IDLE);
		if (event & I3C_EVENT_STOP)
			i3c_enter_state(I3C_STATE_IDLE);
		break;
	case I3C_STATE_WRITE:
		if (event & I3C_EVENT_PAUSE)
			i3c_enter_state(I3C_STATE_WRITE);
		if (event & I3C_EVENT_ABORT)
			i3c_enter_state(I3C_STATE_IDLE);
		if (event & I3C_EVENT_STOP) {
			if (i3c_rxsubmit > 0) {
				i3c_current = 0;
				i3c_limit = i3c_rxsubmit;
				i3c_config_mode(I3C_MODE_MASTER_RX, false);
				i3c_enter_state(I3C_STATE_READ);
			} else
				i3c_enter_state(I3C_STATE_IDLE);
		}
		break;
	case I3C_STATE_WAIT:
		if (event & I3C_EVENT_START) {
			if (i3c_dir_mode() == I3C_MODE_TX)
				i3c_enter_state(I3C_STATE_WRITE);
			else
				i3c_enter_state(I3C_STATE_READ);
		}
	case I3C_STATE_IDLE:
		break;
	}
}

static void i3c_handle_bh(void)
{
	i3c_t i3c;
	__unused i3c_t si3c;

	for (i3c = 0; i3c < NR_I3C_MASTERS; i3c++) {
		si3c = i3c_master_save(i3c);
		i3c_handle_xfr();
		i3c_master_restore(si3c);
	}
}

static void i3c_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ)
		i3c_handle_irq();
	else
		i3c_handle_bh();
}

#ifdef SYS_REALTIME
static void __i3c_poll_init(void)
{
	irq_register_poller(i3c_bh);
}
#else
static void __i3c_irq_init(void)
{
	i3c_hw_irq_init();
}
#endif

void i3c_init(void)
{
	i3c_t i3c;
	__unused i3c_t si3c;

	i3c_bh = bh_register_handler(i3c_bh_handler);
	for (i3c = 0; i3c < NR_I3C_MASTERS; i3c++) {
		si3c = i3c_master_save(i3c);
		//i3c_set_status(I3C_STATUS_IDLE);
		i3c_master_init();
		i3c_irq_init();
		i3c_transfer_reset();
		i3c_master_restore(si3c);
	}
	i3c_poll_init();
}
