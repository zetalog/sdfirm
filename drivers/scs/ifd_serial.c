#include <target/ifd.h>
#include <target/delay.h>

boolean ifd_wt_expired;
uint32_t ifd_wt_value;
uint16_t ifd_F_value;
uint8_t ifd_D_value;

boolean ifd_hw_get_contact(uint8_t pin)
{
	boolean ret = false;
	if (pin == ICC_CONTACT_IO) {
		ret = ifds_hw_read_pin(PORT_IO, PIN_IO) ? true : false;
	}
	if (pin == ICC_CONTACT_CLK) {
		ret = ifds_hw_read_pin(PORT_CLK, PIN_CLK) ? true : false;
	}
	return ret;
}

boolean ifd_hw_icc_present(void)
{
	return ifds_hw_read_pin(PORT_PRES, PIN_PRES);
}

#ifdef CONFIG_IFD_CLOCK_CONTROL
void ifd_hw_suspend_clock(uint8_t level)
{
	ifds_hw_ctrl_stop();
	ifds_hw_write_pin(PORT_CLK, PIN_CLK, level);
}

void ifd_hw_resume_clock(void)
{
	ifds_hw_ctrl_start();
}
#endif

void ifd_hw_config_cls(uint8_t cls)
{
	if (cls == IFD_CLASS_A) {
		ifds_hw_write_pin(PORT_VCCSEL, PIN_VCCSEL1, 0x01);
	} else {
		ifds_hw_write_pin(PORT_VCCSEL, PIN_VCCSEL1, 0x00);
		if (cls == IFD_CLASS_B) {
			ifds_hw_write_pin(PORT_VCCSEL, PIN_VCCSEL2, 0x01);
		} else {
			ifds_hw_write_pin(PORT_VCCSEL, PIN_VCCSEL2, 0x00);
		}
	}
}

void ifd_hw_config_etu(uint16_t F, uint8_t D)
{
	uint32_t br;
	
	ifd_F_value = F;
	ifd_D_value = D;
	br = mul32u(IFD_HW_FREQ_DEF, D);
	br = div32u(br, F);
	br = mul32u(br, 1000);
	ifds_hw_config_baudrate(br);
}

void ifd_hw_delay_gt(uint32_t gt, boolean blk)
{
	/* udelay(gt); */
}

static void __ifd_hw_wt_expire(void)
{
	ifd_wt_expired = true;
}

void ifd_hw_stop_wtc(void)
{
	/* timer_unregister(__ifd_hw_wt_expire); */
}

void ifd_hw_start_wtc(void)
{
	/* timer_register(__ifd_hw_wt_expire, ifd_wt_value, TIMER_INTERRUPT); */
}

void ifd_hw_delay_wt(uint32_t wt, boolean blk)
{
	ifd_hw_stop_wtc();
	ifd_wt_expired = false;
	ifd_wt_value = wt;
	ifd_hw_start_wtc();
}

void __ifd_hw_wait_flag(void)
{
	while (ifds_hw_read_empty()) {
		if (!ifd_hw_icc_present()) {
			ifd_xchg_stop(IFD_XCHG_EVENT_ICC_OUT);
			return;
		}
		if (ifd_wt_expired) {
			ifd_xchg_stop(IFD_XCHG_EVENT_WT_EXPIRE);
			return;
		}
		if (ifds_hw_parity_error()) {
			ifd_xchg_stop(IFD_XCHG_EVENT_PARITY_ERR);
			return;
		}
	}
}

uint8_t ifd_hw_read_byte(void)
{
	__ifd_hw_wait_flag();
	return ifds_hw_read_byte();
}

void ifd_hw_write_byte(uint8_t byte)
{
	ifds_hw_write_byte(byte);
}

boolean ifd_hw_activate(void)
{
	ifds_hw_write_pin(PORT_VCC, PIN_VCC, 0x00);
	return true;
}

void ifd_hw_deactivate(void)
{
	ifds_hw_write_pin(PORT_VCC, PIN_VCC, 0x01);
}

void ifd_hw_reset(void)
{
	ifds_hw_write_pin(PORT_RST, PIN_RST, 0x01);
	ifds_hw_write_pin(PORT_IO, PIN_IO, 0x01);
}

void ifd_hw_warm_reset(void)
{
	ifds_hw_write_pin(PORT_RST, PIN_RST, 0x01);
}

void ifd_hw_slot_reset(void)
{
}

void ifd_hw_ctrl_init(void)
{
	ifds_hw_ctrl_init();
	ifds_hw_ctrl_start();
}
