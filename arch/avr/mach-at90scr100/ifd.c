/* 
 * This file defines the low level driver with smart card interface block
 * It is used by the scib state machine
 */
#include <target/ifd.h>
#include <target/delay.h>
#include <target/irq.h>

#define __IFD_HW_DEBOUNSING_DELAY	20

uint32_t __ifd_hw_freqs[] = {
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

void ifd_hw_set_freq(uint32_t freq)
{
#if 0
	/* FIXME: BUG in AT90SCR100 Datasheet
	 *
	 * According to 15.7.11 SCICLK - Smart Card Clock Register, it is
	 * said that SCICLK is a frequency divider.  But oscilloscope
	 * tells us it is just a frequency assigner for the sample board's
	 * silicon revision.  Thus we think the datasheet is telling the
	 * false.
	 * Since we are not sure which one will be changed, the datasheet
	 * or the silicon, we leave the frequency divider codes here.
	 */
	uint8_t v_sciclk;

	v_sciclk = (uint8_t)div32u(CLK_PLL, (uint16_t)freq);
	if (v_sciclk < 12)
		v_sciclk -= 2;
	else if (v_sciclk < 40)
		v_sciclk = v_sciclk / 4 + 2;
	else if (v_sciclk <= 48)
		v_sciclk = v_sciclk / 4;
	else
		BUG();
	SCICLK = v_sciclk;
#else
	SCICLK = (uint8_t)div16u((uint16_t)freq, 1000);
#endif
}

#ifdef CONFIG_IFD_AT90SCR100_PRES_HIGH
#define __ifd_hw_detecting_out()	(SCICR &   _BV(CARDDET))
#define __ifd_hw_detect_in()		(SCICR &= ~_BV(CARDDET))
#else
#define __ifd_hw_detecting_out()	(!(SCICR & _BV(CARDDET)))
#define __ifd_hw_detect_in()		(SCICR |=  _BV(CARDDET))
#endif
#define __ifd_hw_detect_raised()	__ifd_hw_flag_raised(CARDIN)

#ifdef CONFIG_IFD_PRES_IRQ
static void __ifd_hw_detect_toggle(void)
{
	__ifd_hw_irq_disable(CARDINE);
	SCICR ^= _BV(CARDDET);
	__ifd_hw_irq_enable(CARDINE);
}

boolean ifd_hw_icc_present(void)
{
	boolean raised = __ifd_hw_detect_raised();
	while (1) {
		mdelay(__IFD_HW_DEBOUNSING_DELAY);
		if (__ifd_hw_detect_raised() == raised) {
			/* since both CARDIN = CARDDET = 6 */
			return __ifd_hw_detecting_out() ^ raised;
		} else {
			__ifd_hw_detect_toggle();
		}
	}
}

DEFINE_ISR(IRQ_CPRES)
{
	ifd_sid_t sid, ssid;

	for (sid = 0; sid < NR_IFD_SLOTS; sid++) {
		ssid = ifd_sid_save(sid);
		if (ifd_hw_icc_present()) {
			__ifd_hw_detect_toggle();
			ifd_detect_icc_in();
		} else {
			/* FIXME: ICC Responseless after Detection Toggled
			 *
			 * If we toggle CPRES detection level to force a
			 * generation of IRQ for the ICC absence, ICC will
			 * become responseless.
			 * If this can be fixed, please add
			 * ARCH_HAS_IFD_PRES_IRQ dependency (select
			 * ARCH_HAS_IFD_PRES_IRQ for IFD_AT90SCR100) to
			 * the Kconfig file.
			 */
			__ifd_hw_detect_toggle();
			ifd_detect_icc_out();
		}
	}
}

static void __ifd_hw_detect_start(void)
{
	irq_register_vector(IRQ_CPRES, IRQ_CPRES_isr);
	__ifd_hw_irq_enable(CARDINE);
}
#else
boolean ifd_hw_icc_present(void)
{
	boolean raised = __ifd_hw_detect_raised();
	while (1) {
		mdelay(__IFD_HW_DEBOUNSING_DELAY);
		if (__ifd_hw_detect_raised() == raised) {
			/* since both CARDIN = CARDDET = 6 */
			return __ifd_hw_detecting_out() ^ raised;
		} else {
			raised ^= _BV(CARDIN);
		}
	}
}

#define __ifd_hw_detect_start()
#endif

static void __ifd_hw_detect_init(void)
{
	__ifd_hw_detect_pullup();
	__ifd_hw_detect_in();
	__ifd_hw_detect_start();
}

void __ifd_hw_dcdc_on(void)
{
	if (!__ifd_hw_dcdc_ready()) {
		DCCR |= _BV(DCON);
		while (!__ifd_hw_dcdc_ready());
	}
}

#if 0
void __ifd_hw_dcdc_off(void)
{
	if (__ifd_hw_dcdc_ready()) {
		while (__ifd_hw_dcdc_busy());
		DCCR &= ~_BV(DCON);
		while (__ifd_hw_dcdc_ready());
	}
}
#endif

void ifd_hw_config_crep(boolean on)
{
	if (on)
		SCICR |= _BV(CREP);
	else
		SCICR &= ~_BV(CREP);
}

void ifd_hw_config_conv(uint8_t conv)
{
	if (conv == IFD_CONV_DIRECT)
		SCICR &= ~_BV(CONV);
	if (conv == IFD_CONV_INVERSE)
		SCICR |= _BV(CONV);
}

void ifd_hw_config_cls(uint8_t cls)
{
	/* XXX: Change Class Carefully
	 *
	 * According to 16.3.2 Changing VCard Level Parameter, it is
	 * forbidden to modify the voltage delivered by CVcc while DC/DC
	 * is loaded.
	 */
	if (__ifd_hw_dcdc_ready()) {
		while (__ifd_hw_dcdc_busy());
		SCICR &= ~SCICR_VCARD_MASK;
		SCICR |= (SCIB_CLS(cls) << SCICR_VCARD_OFFSET);
	}
}

void ifd_hw_config_etu(uint16_t F, uint8_t D)
{
	uint16_t etu = 0;

	etu = div16u(F, D);

	if (mod16u(F, D) > (D / 2))
		etu += 1;

	SCETUL = LOBYTE(etu);
	SCETUH = HIBYTE(etu) & ~_BV(COMPH);

	if ((D != 1) && (mod16u(F, D) > (D / 2)))
		SCETUH |= _BV(COMPH);
}

void ifd_hw_delay_gt(uint32_t gt, boolean blk)
{
	SCSR &= ~_BV(BGTEN);
	SCGTL = LOBYTE(gt);
	SCGTH = HIBYTE(gt) & 0x01;
	if (blk)
		SCSR |= _BV(BGTEN);
}

void ifd_hw_delay_wt(uint32_t wt, boolean blk)
{
	ifd_hw_stop_wtc();
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
			__ifd_hw_unraise_xchg(SCPE);
			return;
		}
		if (flags & SCIB_IRQ(flag)) {
			return;
		}
	}
}

uint8_t ifd_hw_read_byte(void)
{
	__ifd_hw_wait_flag(SCRC);
	__ifd_hw_unraise_xchg(SCRC);
	return SCIBUF;
}

void ifd_hw_write_byte(uint8_t b)
{
	__ifd_hw_unraise_xchg(SCTC);

	__ifd_hw_wait_flag(SCTBE);
	SCIBUF = b;
	/* FIXME: This is not the same as 15.6.3 description.
	 * Is this right?
	 */
	while (!__ifd_hw_xchg_raised(SCTC));
}

void __ifd_hw_disable_vcc(void)
{
	__ifd_hw_pulldown_contact(CARDVCC);
}

boolean __ifd_hw_enable_vcc(void)
{
	/* XXX: Enabling CARDVCC Pin Carefully
	 *
	 * Though every bits in SCCON is R/W capable, following code CAN'T
	 * work:
	 * SCCON = _BV(CARDVCC);
	 * According to the 15.7.2 SCCON - Smart Card Contacts Register,
	 * VCARDOK=1 (SCISR.4 bit) condition must be true to change the
	 * state of the CARDC8, CARDC4, CARDIO, CARDCLK and CARDRST pins.
	 * Is this statement implies that the assignment instruction above
	 * will fail if VCARDOK!=1?
	 */
	__ifd_hw_pullup_contact(CARDVCC);
	/* TODO: use udelay(20) instead of mdelay(2) */
	mdelay(2);
	return __ifd_hw_flag_raised(VCARDOK);
}

boolean ifd_hw_activate(void)
{
	if (!__ifd_hw_enable_vcc())
		return false;

	/* XXX: Put RST to State L after VCC
	 *
	 * According to ISO7816-3, cold reset requires RST to be put to
	 * state L before VCC is powered while this silicon does not allow
	 * so.  In AT90SCR100, VCARDOK=1 (SCISR.4 bit) condition must be
	 * true to change the state of the CARDRST pin.  Please refer to
	 * the 15.7.2 SCCON - Smart Card Contacts Register.
	 */
	__ifd_hw_pulldown_contact(CARDRST);

	__ifd_hw_pullup_contact(CARDIO);
	__ifd_hw_enable_clk();
	return true;
}

void ifd_hw_deactivate(void)
{
	__ifd_hw_pulldown_contact(CARDRST);
	__ifd_hw_disable_clk();
	__ifd_hw_pulldown_contact(CARDCLK);
	__ifd_hw_disable_io();
	__ifd_hw_pulldown_contact(CARDIO);
	__ifd_hw_disable_vcc();
}

void ifd_hw_reset(void)
{
	__ifd_hw_pullup_contact(CARDRST);
	__ifd_hw_enable_io();
}

void ifd_hw_warm_reset(void)
{
	__ifd_hw_pulldown_contact(CARDRST);
}

#ifdef CONFIG_IFD_CLOCK_CONTROL
void ifd_hw_suspend_clock(uint8_t level)
{
	SCCON &= ~_BV(CLK);
	if (level == IFD_CLOCK_STOPPED_H)
		SCCON |= _BV(CARDCLK);
	else
		SCCON &= ~_BV(CARDCLK);
}

void ifd_hw_resume_clock(void)
{
	SCCON |= _BV(CLK);
}
#endif

boolean ifd_hw_get_contact(uint8_t pin)
{
	uint8_t ret = false;
	
	if (pin == ICC_CONTACT_IO) {
		uint8_t old_uart;
		old_uart = SCICR & _BV(UART);

		if (old_uart)
			__ifd_hw_disable_io();
		SCCON |= _BV(CARDIO);
		ret = (SCCON & _BV(CARDIO)) ? true : false;
		if (old_uart)
			__ifd_hw_enable_io();
	}
	if (pin == ICC_CONTACT_CLK) {
		ret = SCCON & _BV(CARDCLK);
	}
	return ret;
}

void ifd_hw_slot_reset(void)
{
	__ifd_hw_disable_io();
}

static void __ifd_hw_ctrl_reset(void)
{
	SCICR |= _BV(SCIRESET);
	SCICR &= ~_BV(SCIRESET);
	while (SCICR & _BV(SCIRESET));
	SCIER = 0x00;
	SCISR = 0x00;
	SCCON = 0x00;
}

void ifd_hw_ctrl_init(void)
{
	__ifd_hw_ctrl_reset();
	__ifd_hw_detect_init();
	__ifd_hw_dcdc_on();
}
