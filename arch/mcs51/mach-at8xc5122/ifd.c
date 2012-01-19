/* 
 * This file defines the low level driver with smart card interface block
 * It is used by the scib state machine
 */
#include <target/ifd.h>
#include <target/delay.h>
#include <target/state.h>

#define __IFD_HW_DEBOUNSING_DELAY	20

static void __ifd_hw_detect_init(void);
static void __ifd_hw_config_boost(void);
static void __ifd_hw_enable_io(void);
static boolean __ifd_hw_enable_vcc(void);
static boolean __ifd_hw_config_voltage(void);
static void __ifd_hw_config_current(void);

#if NR_IFD_SLOTS > 1
__near__ uint8_t __ifd_hw_slot_voltage[NR_IFD_SLOTS];
__near__ uint8_t __ifd_hw_dcdc_boost[NR_IFD_SLOTS];
#define __ifd_hw_voltage __ifd_hw_slot_voltage[ifd_slid]
#define __ifd_hw_boost __ifd_hw_dcdc_boost[ifd_slid]
#else
__near__ uint8_t __ifd_hw_voltage;
__near__ uint8_t __ifd_hw_boost;
#endif

__text__ uint32_t __ifd_hw_freqs[] = {
	4000,
#if (IFD_HW_FREQ_MAX >= 8000)
	8000,
#endif
#if (IFD_HW_FREQ_MAX >= 12000)
	12000,
#endif
};

uint8_t ifd_hw_cf_nr_freq(void)
{
	return sizeof (__ifd_hw_freqs) / sizeof (uint32_t);
}

uint32_t ifd_hw_cf_get_freq(uint8_t index)
{
	if (index >= ifd_hw_cf_nr_freq())
		return 0;
	return __ifd_hw_freqs[index];
}

/**
 * clock settings and status
 * Fck_iso = Fck_pll / (2 * (48 - SCICLK[5:0])) when SCICLK[5:0] < 48mhz 
 * the fck_iso = 4mhz, then the per_cycle_time = 0.25us
 * There is datasheet bugs in SCICLK.XTSCS
 */
void ifd_hw_set_freq(uint32_t freq)
{
	uint8_t v_sciclk;

	v_sciclk = (uint8_t)div32u(PLL_FREQ, freq);
	v_sciclk /= 2;
	v_sciclk = 48 - v_sciclk;

	BUG_ON(v_sciclk >= 48);

	SCSR |= MSK_SCSR_SCRS;
	SCICLK &= ~MSK_SCICLK_XTSCS;
	SCICLK = v_sciclk;
}

static void __ifd_hw_detect_init(void)
{
	__ifd_hw_detect_config();
	__ifd_hw_detect_pullup();
	__ifd_hw_detect_pres();
#ifdef CONFIG_IFD_PRES_IRQ
	irq_hw_enable_vector(IRQ_INT1, true);
	__ifd_hw_enable_detect();
#endif
}

static boolean __ifd_hw_enable_vcc(void)
{
	SCSR &= ~MSK_SCSR_SCRS;

	if (__ifd_hw_voltage == SCIB_VOLTAGE_5V)
		DCCKPS &= ~DCCKPS_MODE_REGULATOR;	/* pump mode */
	else
		DCCKPS |= DCCKPS_MODE_REGULATOR;	/* regulator mode */

retry:
	if (!__ifd_hw_config_voltage())
		return false;
	SCCON |= MSK_SCCON_CARDVCC;
	mdelay(3);
	if (!__ifd_hw_flag_raised(VCARDOK)) {
		if (__ifd_hw_voltage != SCIB_VOLTAGE_5V)
			return false;
		else
			goto retry;
	} else {
		/* If VCC > 4V and startup current > 30 mA, it will be
		 * necessary to increment the BOOST[1:0] bits until the
		 * DC/DC converter is ready.
		 */
		if (__ifd_hw_voltage == SCIB_VOLTAGE_5V)
			__ifd_hw_config_current();
		return true;
	}
}

static void __ifd_hw_config_boost(void)
{
	DCCKPS &= ~DCCKPS_BOOST_MASK;
	DCCKPS |= (__ifd_hw_boost << DCCKPS_BOOST_OFFSET);
}

static boolean __ifd_hw_config_voltage(void)
{
	if (__ifd_hw_voltage == SCIB_VOLTAGE_5V) {
		if (__ifd_hw_boost == DCCKPS_BOOST_MAX)
			return false;
		__ifd_hw_boost++;
	}
	__ifd_hw_config_boost();
	return true;
}

static void __ifd_hw_config_current(void)
{
	if (__ifd_hw_boost == DCCKPS_BOOST_OVFADJ)
		return;
	__ifd_hw_boost--;
	__ifd_hw_config_boost();
}

static void __ifd_hw_disable_uart(void)
{
	SCSR |= MSK_SCSR_SCRS;
	SCICR &= ~MSK_SCICR_UART;
}

static void __ifd_hw_enable_uart(void)
{
	SCSR |= MSK_SCSR_SCRS;
	SCICR |= MSK_SCICR_UART;
}

static void __ifd_hw_enable_ioclk(void)
{
	/* set io in reception mode */
	SCSR &= ~MSK_SCSR_SCRS;
	SCCON |= MSK_SCCON_CARDIO;
	SCCON |= MSK_SCCON_CLK;
}

boolean __ifd_hw_detect_present(void)
{
	SCSR &= ~MSK_SCSR_SCRS;
	return __ifd_hw_flag_raised(CARDIN);
}

boolean ifd_hw_icc_present(void)
{
	if (__ifd_hw_detect_present()) {
present:
		mdelay(__IFD_HW_DEBOUNSING_DELAY);
		if (__ifd_hw_detect_present()) {
			return true;
		} else {
			goto notpresent;
		}
	} else {
notpresent:
		mdelay(__IFD_HW_DEBOUNSING_DELAY);
		if (!__ifd_hw_detect_present()) {
			return false;
		} else {
			goto present;
		}
	}
}

#ifdef CONFIG_IFD_PRES_IRQ
void DEFINE_ISR(scib_pres_isr(void), IRQ_INT1)
{
	ifd_sid_t sid, ssid;

	for (sid = 0; sid < NR_IFD_SLOTS; sid++) {
		ssid = ifd_sid_save(sid);
		if (ifd_hw_icc_present()) {
			__ifd_hw_detect_abse();
			ifd_detect_icc_in();
		} else {
			__ifd_hw_detect_pres();
			ifd_detect_icc_out();
		}
		__ifd_hw_unraise_detect();
	}
}
#endif

void ifd_hw_config_conv(uint8_t conv)
{
	SCSR |= MSK_SCSR_SCRS;
	if (conv == IFD_CONV_DIRECT)
		SCICR &= ~MSK_SCICR_CONV;
	if (conv == IFD_CONV_INVERSE)
		SCICR |= MSK_SCICR_CONV;
}

void ifd_hw_config_cls(uint8_t cls)
{
	SCSR |= MSK_SCSR_SCRS;

	if (cls == IFD_CLASS_A) {
		__ifd_hw_boost = SCIB_BOOST_NORMAL;
		__ifd_hw_voltage = SCIB_VOLTAGE_5V;
		SCICR |= MSK_SCICR_VCARD;
	} else if (cls == IFD_CLASS_B) {
		__ifd_hw_boost = SCIB_BOOST_30_PERCENT;
		__ifd_hw_voltage = SCIB_VOLTAGE_3V;
		SCICR &= ~MSK_SCICR_VCARD0;
		SCICR |= MSK_SCICR_VCARD1;
	} else if (cls == IFD_CLASS_C) {
		__ifd_hw_boost = SCIB_BOOST_30_PERCENT;
		__ifd_hw_voltage = SCIB_VOLTAGE_1_8V;
		SCICR |= MSK_SCICR_VCARD0;
		SCICR &= ~MSK_SCICR_VCARD1;
	} else {
		__ifd_hw_boost = SCIB_BOOST_NORMAL;
		__ifd_hw_voltage = SCIB_VOLTAGE_0V;
		SCICR &= ~MSK_SCICR_VCARD;
	}
}

void ifd_hw_config_etu(uint16_t F, uint8_t D)
{
	uint16_t etu = 0;

	etu = div16u(F, D);

	if (mod16u(F, D) > (D / 2))
		etu += 1;

	SCSR |= MSK_SCSR_SCRS;
	SCETU0 = LOBYTE(etu);
	SCETU1 = HIBYTE(etu) & ~MSK_SCETU_COMP;

	if ((D != 1) && (mod16u(F, D) > (D / 2)))
		SCETU1 |= MSK_SCETU_COMP;
}

void ifd_hw_delay_gt(uint32_t gt, boolean blk)
{
	SCSR |= MSK_SCSR_SCRS;
	SCGT0 = LOBYTE((gt));
	SCGT1 = HIBYTE((gt)) & 0x01;
	if (blk)
		SCSR |= MSK_SCSR_BGTEN;
}

void ifd_hw_start_wtc(void)
{
	SCSR |= MSK_SCSR_SCRS;
	SCICR |= MSK_SCICR_WTEN;
}

void ifd_hw_stop_wtc(void)
{
	SCSR |= MSK_SCSR_SCRS;
	SCICR &= ~MSK_SCICR_WTEN;
}

void ifd_hw_delay_wt(uint32_t wt, boolean blk)
{
	ifd_hw_stop_wtc();
	SCSR &= ~MSK_SCSR_SCRS;
	SCWT0 = LOBYTE(LOWORD(wt));
	SCWT1 = HIBYTE(LOWORD(wt));
	SCWT3 = HIBYTE(HIWORD(wt));
	SCWT2 = LOBYTE(HIWORD(wt));
	ifd_hw_start_wtc();
}

void __ifd_hw_wait_flag(uint8_t flag)
{
	uint8_t flags;
	
	while (1) {
		flags = SCISR;

		if (!(flags & SCIB_IRQ(CARDIN))) {
			ifd_xchg_stop(IFD_XCHG_EVENT_ICC_OUT);
			return;
		}

		if (flags & SCIB_IRQ(SCWTO)) {
			ifd_xchg_stop(IFD_XCHG_EVENT_WT_EXPIRE);
			//ifd_reconfig_wt();
			return;
		}
		if (flags & SCIB_IRQ(SCPE)) {
			ifd_xchg_stop(IFD_XCHG_EVENT_PARITY_ERR);
			return;
		}
		if (flags & SCIB_IRQ(flag))
			return;
	}
}

uint8_t ifd_hw_read_byte(void)
{
  	uint8_t old_scsr;

	old_scsr = SCSR;
	SCSR &= ~MSK_SCSR_SCRS;
	__ifd_hw_wait_flag(SCRC);
	SCSR = old_scsr;

	return SCIBUF;
}

void ifd_hw_write_byte(uint8_t b)
{
	uint8_t old_scsr;

	old_scsr = SCSR;
	SCSR &= ~MSK_SCSR_SCRS;
	__ifd_hw_wait_flag(SCTBE);
	SCSR = old_scsr;

	SCIBUF = b;
}

void ifd_hw_deactivate(void)
{
	uint8_t tmp;

	ifd_hw_stop_wtc();
	SCSR &= ~MSK_SCSR_SCRS;
	SCCON &= ~MSK_SCCON_CARDRST;
	SCCON &= ~MSK_SCCON_CLK;
	SCCON &= ~MSK_SCCON_CARDCLK;
	__ifd_hw_disable_uart();
	SCCON &= ~MSK_SCCON_CARDIO;
	SCCON &= ~MSK_SCCON_CARDVCC;
	DCCKPS = 0;
}

void ifd_hw_config_crep(boolean on)
{
	SCSR |= MSK_SCSR_SCRS;
	if (on)
		SCICR |= MSK_SCICR_CREP;
	else
		SCICR &= ~MSK_SCICR_CREP;
}

boolean ifd_hw_activate(void)
{
	SCSR &= ~MSK_SCSR_SCRS;
	SCCON &= ~MSK_SCCON_CARDRST;

	if (!__ifd_hw_enable_vcc())
		return false;
	__ifd_hw_enable_ioclk();
	return true;
}

boolean ifd_hw_get_contact(uint8_t pin)
{
	uint8_t ret = false;
	
	if (pin == ICC_CONTACT_IO) {
		uint8_t old_uart;

		SCSR |= MSK_SCSR_SCRS;
		old_uart = SCICR & MSK_SCICR_UART;
		if (old_uart)
			__ifd_hw_disable_uart();

		SCSR &= ~MSK_SCSR_SCRS;
		SCCON |= MSK_SCCON_CARDIO;
		ret = (SCCON & MSK_SCCON_CARDIO) ? true : false;

		SCSR |= MSK_SCSR_SCRS;
		if (old_uart)
			__ifd_hw_enable_uart();
	}
	if (pin == ICC_CONTACT_CLK) {
		SCSR &= ~MSK_SCSR_SCRS;
		SCCON &= ~MSK_SCCON_CLK;
		ret = SCCON & MSK_SCCON_CARDCLK;
	}
	return ret;
}

void ifd_hw_reset(void)
{
	/* pull up RST contact */
	SCSR &= ~MSK_SCSR_SCRS;
	SCCON |= MSK_SCCON_CARDRST;
	__ifd_hw_enable_uart();
}

void ifd_hw_warm_reset(void)
{
	/* pull down RST contact */
	SCSR &= ~MSK_SCSR_SCRS;
	SCCON &= ~MSK_SCCON_CARDRST;
}

#ifdef CONFIG_IFD_CLOCK_CONTROL
void ifd_hw_suspend_clock(uint8_t level)
{
	SCSR &= ~MSK_SCSR_SCRS;
	SCCON &= ~MSK_SCCON_CLK;
	if (level == IFD_CLOCK_STOPPED_H)
		SCCON |= MSK_SCCON_CARDCLK;
	else
		SCCON &= ~MSK_SCCON_CARDCLK;
}

void ifd_hw_resume_clock(void)
{
	SCSR &= ~MSK_SCSR_SCRS;
	SCCON |= MSK_SCCON_CLK;
}
#endif

static void __ifd_hw_ctrl_reset(void)
{
	SCSR |= MSK_SCSR_SCRS;
	SCICR |= MSK_SCICR_RESET;
	SCICR &= ~MSK_SCICR_RESET;
}

void ifd_hw_slot_reset(void)
{
	__ifd_hw_voltage = SCIB_VOLTAGE_0V;
	__ifd_hw_boost = SCIB_BOOST_NORMAL;
	__ifd_hw_disable_uart();
}

void ifd_hw_ctrl_init(void)
{
	__ifd_hw_ctrl_reset();
	__ifd_hw_detect_init();
}
