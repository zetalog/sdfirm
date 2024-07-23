#include <target/espi.h>
#include <target/panic.h>

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

void espi_init(void)
{
	espi_hw_ctrl_init();
}
