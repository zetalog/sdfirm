#include <target/espi.h>
#include <target/panic.h>
#include <target/bitops.h>
#include <target/cmdline.h>

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
uint16_t espi_rsp;
uint16_t espi_addr;
uint32_t espi_gen_cfg;
uint32_t espi_peri_cfg;
uint32_t espi_vwire_cfg;
uint32_t espi_oob_cfg;
uint32_t espi_flash_cfg;

#ifdef CONFIG_ESPI_DEBUG
const char *espi_op_names[] = {
	"NONE",
	"PROBE",
};

const char *espi_op_name(uint32_t val)
{
	if (val >= ARRAY_SIZE(espi_op_names))
		return "UNKNOWN";
	return espi_op_names[val];
}

const char *espi_state_names[] = {
	"INIT",
	"RESET",
	"GET_GEN",
	"SET_GEN",
	"GET_VWIRE",
	"SET_VWIRE",
	"GET_OOB",
	"SET_OOB",
	"GET_FLASH",
	"SET_FLASH",
	"ASSERT_PLTRST",
	"DEASSERT_PLTRST",
	"GET_PERI",
	"SET_PERI",
	"VALID",
	"INVALID",
};

const char *espi_state_name(uint32_t val)
{
	if (val >= ARRAY_SIZE(espi_state_names))
		return "UNKNOWN";
	return espi_state_names[val];
}

const char *espi_event_names[] = {
	"INIT",
	"ACCEPT",
	"DEFER",
	"WAIT_STATE",
	"REJECT",
	"RESPONSE",
	"NO_RESPONSE",
	"PROBE",
};

const char *espi_event_name(espi_event_t event)
{
	int i, nr_evts = 0;

	for (i = 0; i < ARRAY_SIZE(espi_event_names); i++) {
		if (_BV(i) & event) {
			nr_evts++;
			return espi_event_names[i];
		}
	}
	return "NONE";
}

const char *espi_reg_names[] = {
	"NONE",
	"SLAVE_DEV_ID",
	"SLAVE_GEN_CFG",
	"NONE",
	"SLAVE_PERI_CFG",
	"NONE",
	"NONE",
	"NONE",
	"SLAVE_VWIRE_CFG",
	"NONE",
	"NONE",
	"NONE",
	"SLAVE_OOB_CFG",
	"NONE",
	"NONE",
	"NONE",
	"SLAVE_FLASH_CFG",
};

const char *espi_reg_name(uint16_t address)
{
	int val = address / 4;

	if (val >= ARRAY_SIZE(espi_reg_names))
		return "UNKNOWN";
	return espi_reg_names[val];
}

void espi_debug(uint8_t tag, uint32_t val)
{
	switch (tag) {
	case ESPI_DEBUG_STATE:
		con_dbg("espi: state %s\n", espi_state_name(val));
		break;
	case ESPI_DEBUG_EVENT:
		con_dbg("espi: event %s\n", espi_event_name(val));
		break;
	case ESPI_DEBUG_OP:
		con_dbg("espi: op %s\n", espi_op_name(val));
		break;
	case ESPI_DEBUG_REG:
		con_dbg("espi: reg %s\n", espi_reg_name(val));
		break;
	default:
		BUG();
		break;
	}
}
#endif

uint8_t espi_state_get(void)
{
	return espi_state;
}

void espi_state_set(uint8_t state)
{
	espi_debug_state(state);
	espi_state = state;
	espi_seq_handler();
}

void espi_enter_state(uint8_t state)
{
	espi_state_set(state);
}

void espi_raise_event(espi_event_t event)
{
	espi_debug_event(event);
	espi_event |= event;
	bh_resume(espi_bh);
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

uint32_t espi_config_alert_pin(uint32_t cfgs)
{
	switch (ESPI_ALERT_PIN) {
	case ESPI_GEN_ALERT_MODE_IO1:
		return ESPI_GEN_ALERT_MODE_IO1;
	case ESPI_GEN_ALERT_MODE_PIN:
	default:
		if (ESPI_ALERT_TYPE == ESPI_GEN_ALERT_TYPE_OD) {
			if (cfgs & ESPI_GEN_OPEN_DRAIN_ALERT_SUP) {
				return ESPI_GEN_ALERT_MODE_PIN |
				       ESPI_GEN_OPEN_DRAIN_ALERT_SEL;
			}
			con_log("espi: open drain alert PIN not supported, falls to push pull.");
		}
		return ESPI_GEN_ALERT_MODE_PIN;
	}
}

uint32_t espi_config_io_mode(uint32_t cfgs)
{
	switch (ESPI_IO_MODE) {
	case ESPI_GEN_IO_MODE_QUAD:
		if (espi_slave_io_mode_sup_quad(cfgs))
			return ESPI_GEN_IO_MODE_SEL(ESPI_GEN_IO_MODE_QUAD);
		con_log("espi: quad I/O not supported, falls to dual I/O.\n");
	case ESPI_GEN_IO_MODE_DUAL:
		if (espi_slave_io_mode_sup_dual(cfgs))
			return ESPI_GEN_IO_MODE_SEL(ESPI_GEN_IO_MODE_DUAL);
		con_log("espi: dual I/O not supported, falls to single I/O.\n");
	case ESPI_GEN_IO_MODE_SINGLE:
	default:
		/* Single I/O mode is always supported. */
		return ESPI_GEN_IO_MODE_SEL(ESPI_GEN_IO_MODE_SINGLE);
	}
}

uint32_t espi_config_op_freq(uint32_t cfgs)
{
	uint32_t fmax = espi_slave_op_freq_sup_max(cfgs);

	switch (ESPI_OP_FREQ) {
	case ESPI_GEN_OP_FREQ_66MHZ:
		if (fmax >= 66)
			return ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_66MHZ);
	case ESPI_GEN_OP_FREQ_50MHZ:
		if (fmax >= 50)
			return ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_50MHZ);
	case ESPI_GEN_OP_FREQ_33MHZ:
		if (fmax >= 33)
			return ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_33MHZ);
	case ESPI_GEN_OP_FREQ_25MHZ:
		if (fmax > 0)
			return ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_25MHZ);
	case ESPI_GEN_OP_FREQ_20MHZ:
	default:
		return ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_20MHZ);
	}
}

void espi_nego_config(uint16_t address, uint32_t cfgs)
{
	uint32_t hwcfgs;

	switch (address) {
	case ESPI_SLAVE_GEN_CFG:
		hwcfgs = cfgs & ESPI_GEN_CAP_MASK;
		hwcfgs |= ESPI_CRC_CHECKING | ESPI_RSP_MODIFIER;
		hwcfgs |= espi_config_alert_pin(cfgs);
		hwcfgs |= espi_config_op_freq(cfgs);
		hwcfgs |= espi_config_io_mode(cfgs);
		espi_gen_cfg = hwcfgs;
		break;
	case ESPI_SLAVE_PERI_CFG:
		break;
	case ESPI_SLAVE_VWIRE_CFG:
		break;
	case ESPI_SLAVE_OOB_CFG:
		break;
	case ESPI_SLAVE_FLASH_CFG:
		break;
	}
}

void espi_inband_reset(void)
{
	espi_write_cmd_async(ESPI_CMD_RESET, 0, NULL, 0, NULL);
}

void espi_get_configuration(uint16_t address)
{
	uint8_t hbuf[3];

	espi_addr = address;
	hbuf[0] = 0x00;
	hbuf[1] = HIBYTE(address);
	hbuf[2] = LOBYTE(address);

	espi_write_cmd_async(ESPI_CMD_GET_CONFIGURATION,
			     3, hbuf, 0, NULL);
}

void espi_set_configuration(uint16_t address, uint32_t config)
{
	uint8_t hbuf[7];

	espi_addr = address;
	hbuf[0] = 0x00;
	hbuf[1] = HIBYTE(address);
	hbuf[2] = LOBYTE(address);
	hbuf[3] = HIBYTE(HIWORD(config));
	hbuf[4] = LOBYTE(HIWORD(config));
	hbuf[5] = HIBYTE(LOWORD(config));
	hbuf[6] = LOBYTE(LOWORD(config));

	espi_write_cmd_async(ESPI_CMD_SET_CONFIGURATION, 
			     7, hbuf, 0, NULL);
}

int espi_write_cmd(uint8_t opcode,
		   uint8_t hlen, uint8_t *hbuf,
		   uint8_t dlen, uint8_t *dbuf)
{
	espi_raise_event(ESPI_EVENT_WAIT_STATE);
	espi_write_cmd_async(opcode, hlen, hbuf, dlen, dbuf);
	espi_sync();
	return 0;
}

uint8_t espi_read_rsp(uint8_t opcode,
		      uint8_t hlen, uint8_t *hbuf,
		      uint8_t dlen, uint8_t *dbuf)
{
	espi_hw_read_rsp(opcode, hlen, hbuf, dlen, dbuf);
	return espi_rsp;
}

void espi_op_complete(bool result)
{
	uint8_t op = espi_op;

	espi_cmpl_cb cb = espi_op_cb;

	espi_op = ESPI_OP_NONE;
	espi_op_cb = NULL;
	espi_cmd = ESPI_CMD_NONE;
	espi_rsp = ESPI_RSP_NONE;

	if (cb)
		cb(0, op, result);
}

int espi_start_op(espi_op_t op, espi_cmpl_cb cb)
{
	if (espi_op_busy())
		return -EBUSY;

	espi_debug_op(op);
	espi_op = op;
	espi_op_cb = cb;
	espi_seq_handler();
	return 0;
}

void espi_get_config(uint16_t address)
{
	uint32_t cfgs;
	uint8_t hbuf[4];

	(void)espi_read_rsp(ESPI_CMD_GET_CONFIGURATION,
			    4, hbuf, 0, NULL);
	cfgs = MAKELONG(MAKEWORD(hbuf[0], hbuf[1]),
			MAKEWORD(hbuf[2], hbuf[3]));
	espi_debug_reg(address);
	con_dbg("espi: %04x=%08x\n", address, cfgs);
	espi_nego_config(address, cfgs);
}

void espi_set_config(uint16_t address)
{
	switch (address) {
	case ESPI_SLAVE_GEN_CFG:
		espi_hw_set_cfg(address, espi_gen_cfg);
		break;
	case ESPI_SLAVE_PERI_CFG:
		espi_hw_set_cfg(address, espi_peri_cfg);
		break;
	case ESPI_SLAVE_VWIRE_CFG:
		espi_hw_set_cfg(address, espi_vwire_cfg);
		break;
	case ESPI_SLAVE_OOB_CFG:
		espi_hw_set_cfg(address, espi_oob_cfg);
		break;
	case ESPI_SLAVE_FLASH_CFG:
		espi_hw_set_cfg(address, espi_flash_cfg);
		break;
	}
}

void espi_rsp_available(void)
{
	if (espi_cmd_is_get(ESPI_SLAVE_GEN_CFG))
		espi_get_gen();
	else if (espi_cmd_is_set(ESPI_SLAVE_GEN_CFG))
		espi_set_gen();
	else if (espi_cmd_is_get(ESPI_SLAVE_PERI_CFG))
		espi_get_peri();
	else if (espi_cmd_is_set(ESPI_SLAVE_PERI_CFG))
		espi_set_peri();
	else if (espi_cmd_is_get(ESPI_SLAVE_VWIRE_CFG))
		espi_get_vwire();
	else if (espi_cmd_is_set(ESPI_SLAVE_VWIRE_CFG))
		espi_set_vwire();
	else if (espi_cmd_is_get(ESPI_SLAVE_OOB_CFG))
		espi_get_oob();
	else if (espi_cmd_is_set(ESPI_SLAVE_OOB_CFG))
		espi_set_oob();
	else if (espi_cmd_is_get(ESPI_SLAVE_FLASH_CFG))
		espi_get_flash();
	else if (espi_cmd_is_set(ESPI_SLAVE_FLASH_CFG))
		espi_set_flash();
}

void espi_cmd_complete(uint8_t rsp)
{
	espi_rsp = rsp;
	switch (rsp) {
	case ESPI_RSP_NON_FATAL_ERROR:
	case ESPI_RSP_FATAL_ERROR:
		espi_raise_event(ESPI_EVENT_REJECT);
		break;
	case ESPI_RSP_ACCEPT(0):
		espi_rsp_available();
		espi_raise_event(ESPI_EVENT_ACCEPT);
		break;
	case ESPI_RSP_NO_RESPONSE:
		/* When performing an in-band reset the host controller
		 * and the peripheral can have mismatched IO configs.
		 *
		 * i.e., The eSPI peripheral can be in IO-4 mode while, the
		 * eSPI host will be in IO-1. This results in the
		 * peripheral getting invalid packets and thus not
		 * responding.
		 *
		 * If the peripheral is alerting when we perform an in-band
		 * reset, there is a race condition in
		 * spacemit_espi_write_cmd().
		 * 1) spacemit_espi_write_cmd() clears the interrupt
		 *    status.
		 * 2) eSPI host controller hardware notices the alert and
		 *    sends a GET_STATUS.
		 * 3) spacemit_espi_write_cmd() writes the in-band reset
		 *    command.
		 * 4) eSPI hardware enqueues the in-band reset until
		 *    GET_STATUS is complete.
		 * 5) GET_STATUS fails with NO_RSP_INT and sets the
		 *    interrupt status.
		 * 6) eSPI hardware performs in-band reset.
		 * 7) spacemit_espi_write_cmd() checks the status and sees
		 *    a NO_RSP_INT bit.
		 *
		 * As a workaround we allow the NO_RSP_INT status code
		 * when we perform an in-band reset.
		 */
		espi_raise_event(ESPI_EVENT_NO_RESPONSE);
		break;
	}
}

void espi_config_vwire(uint8_t vwire, bool state)
{
}

void espi_handle_probe(bool is_op)
{
	if (espi_cmd_is(ESPI_CMD_NONE)) {
		espi_inband_reset();
	} else if (espi_state == ESPI_STATE_RESET) {
		espi_get_configuration(ESPI_SLAVE_GEN_CFG);
	} else if (espi_state == ESPI_STATE_GET_GEN) {
		espi_set_configuration(ESPI_SLAVE_GEN_CFG, espi_gen_cfg);
	} else if (espi_state == ESPI_STATE_SET_GEN) {
		espi_get_configuration(ESPI_SLAVE_VWIRE_CFG);
	} else if (espi_state == ESPI_STATE_GET_VWIRE) {
		espi_set_configuration(ESPI_SLAVE_VWIRE_CFG, espi_vwire_cfg);
	} else if (espi_state == ESPI_STATE_SET_VWIRE) {
		espi_get_configuration(ESPI_SLAVE_OOB_CFG);
	} else if (espi_state == ESPI_STATE_GET_OOB) {
		espi_set_configuration(ESPI_SLAVE_OOB_CFG, espi_oob_cfg);
	} else if (espi_state == ESPI_STATE_SET_OOB) {
		espi_get_configuration(ESPI_SLAVE_FLASH_CFG);
	} else if (espi_state == ESPI_STATE_GET_FLASH) {
		espi_set_configuration(ESPI_SLAVE_FLASH_CFG, espi_flash_cfg);
	} else if (espi_state == ESPI_STATE_SET_FLASH) {
		espi_assert_vwire(ESPI_VWIRE_SYSTEM_PLTRST);
	} else if (espi_state == ESPI_STATE_ASSERT_PLTRST) {
		espi_deassert_vwire(ESPI_VWIRE_SYSTEM_PLTRST);
	} else if (espi_state == ESPI_STATE_DEASSERT_PLTRST) {
		espi_get_configuration(ESPI_SLAVE_PERI_CFG);
	} else if (espi_state == ESPI_STATE_GET_PERI) {
		espi_set_configuration(ESPI_SLAVE_PERI_CFG, espi_peri_cfg);
	} else if (espi_state == ESPI_STATE_SET_PERI) {
		espi_raise_event(ESPI_EVENT_PROBE);
	} else if (espi_state == ESPI_STATE_VALID) {
		if (is_op)
			espi_op_success();
	} else if (espi_state == ESPI_STATE_INVALID) {
		if (is_op)
			espi_op_failure();
	}
}

void espi_seq_handler(void)
{
	if (espi_op_is(ESPI_OP_PROBE))
		espi_handle_probe(true);
}

static void espi_async_handler(void)
{
	espi_event_t flags;

	flags = espi_event_save();
	if (flags & ESPI_EVENT_INIT) {
		unraise_bits(flags, ESPI_EVENT_INIT);
		espi_auto_probe();
	} else if (flags & ESPI_EVENT_ACCEPT) {
		unraise_bits(flags, ESPI_EVENT_ACCEPT);
		if (espi_cmd_is(ESPI_CMD_RESET))
			espi_enter_state(ESPI_STATE_RESET);
		else if (espi_cmd_is_get(ESPI_SLAVE_GEN_CFG))
			espi_enter_state(ESPI_STATE_GET_GEN);
		else if (espi_cmd_is_set(ESPI_SLAVE_GEN_CFG))
			espi_enter_state(ESPI_STATE_SET_GEN);
	} else if (flags & ESPI_EVENT_REJECT) {
		unraise_bits(flags, ESPI_EVENT_REJECT);
		espi_enter_state(ESPI_STATE_INVALID);
	} else if (flags & ESPI_EVENT_DEFER) {
		unraise_bits(flags, ESPI_EVENT_REJECT);
	} else if (flags & ESPI_EVENT_NO_RESPONSE) {
		if (espi_cmd_is(ESPI_CMD_RESET))
			espi_enter_state(ESPI_STATE_RESET);
		else
			espi_enter_state(ESPI_STATE_INVALID);
	}
	espi_event_restore(flags);
}

static void espi_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		espi_hw_handle_irq();
		return;
	} else if (events == BH_WAKEUP) {
		espi_async_handler();
		return;
	} else {
		BUG();
		return;
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

void espi_write_cmd_async(uint8_t opcode,
			  uint8_t hlen, uint8_t *hbuf,
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
	espi_rsp = ESPI_RSP_NONE;

	espi_hw_ctrl_init();
	espi_raise_event(ESPI_EVENT_INIT);
}

#if 0
static int do_espi_read(int argc, char *argv[])
{
#if 0
	caddr_t addr;
	uint32_t val;

	if (argc < 3)
		return -EINVAL;

	if (strcmp(argv[2], "io") == 0) {
		if (argc < 4)
			return -EINVAL;
		addr = (caddr_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[3], "1") == 0) {
			val = espi_io_read8(addr);
			printf("IO: 0x%08lx=%02x\n", addr, (uint8_t)val);
		} else if (strcmp(argv[3], "2") == 0) {
			val = espi_io_read16(addr);
			printf("IO: 0x%08lx=%04x\n", addr, (uint16_t)val);
		} else if (strcmp(argv[3], "4") == 0) {
			val = espi_io_read32(addr);
			printf("IO: 0x%08lx=%08x\n", addr, val);
		} else
			return -EINVAL;
		return 0;
	} else {
		if (argc < 4)
			return -EINVAL;
		addr = (caddr_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[3], "1") == 0) {
			val = espi_mem_read8(addr);
			printf("Memory: 0x%08lx=%02x\n", addr, (uint8_t)val);
		} else if (strcmp(argv[3], "2") == 0) {
			val = espi_mem_read16(addr);
			printf("Memory: 0x%08lx=%04x\n", addr, (uint16_t)val);
		} else if (strcmp(argv[3], "4") == 0) {
			val = espi_mem_read32(addr);
			printf("Memory: 0x%08lx=%08x\n", addr, val);
		} else
			return -EINVAL;
		return 0;
	}
#endif
	return -EINVAL;
}

static int do_espi_write(int argc, char *argv[])
{
#if 0
	caddr_t addr;

	if (argc < 5)
		return -EINVAL;

	if (strcmp(argv[2], "io") == 0) {
		uint32_t v;
		int size;
		if (argc < 6)
			return -EINVAL;
		size = (uint32_t)strtoull(argv[3], 0, 0);
		v = (uint32_t)strtoull(argv[4], 0, 0);
		addr = (caddr_t)strtoull(argv[5], 0, 0);
		if (size == 1)
			espi_io_write8(v, addr);
		else if (size == 2)
			espi_io_write16(v, addr);
		else if (size == 4)
			espi_io_write32(v, addr);
		else
			return -EINVAL;
		return 0;
	} else {
		uint32_t v;
		int size;
		if (argc < 6)
			return -EINVAL;
		size = (uint32_t)strtoull(argv[3], 0, 0);
		v = (uint32_t)strtoull(argv[4], 0, 0);
		addr = (caddr_t)strtoull(argv[5], 0, 0);
		if (size == 1)
			espi_mem_write8(v, addr);
		else if (size == 2)
			espi_mem_write16(v, addr);
		else if (size == 4)
			espi_mem_write32(v, addr);
		else
			return -EINVAL;
		return 0;
	}
#endif
	return -EINVAL;
}

static int do_espi(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "read") == 0)
		return do_espi_read(argc, argv);
	if (strcmp(argv[1], "write") == 0)
		return do_espi_write(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(espi, do_espi, "SpacemiT enhanced SPI commands",
	"espi read io [1|2|4] <addr>\n"
	"espi read mem [1|2|4] <addr>\n"
	"    -eSPI read sequence\n"
	"espi write io [1|2|4] <value> <addr>\n"
	"espi write mem [1|2|4] <value> <addr>\n"
	"    -eSPI write sequence\n"
);

#endif
