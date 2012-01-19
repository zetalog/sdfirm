#include <target/generic.h>
#include <target/arch.h>

#ifdef CONFIG_XRAM_EXTERNAL
static void xram_init(void)
{
	xram_external_on();
	xram_external_ale_on();
}
#else
static void xram_init(void)
{
	xram_internal_on();
	xram_internal_512();
	xram_external_ale_off();
}
#endif
#define xram_init_early()	xram_init()

static void clk_init(void)
{
	clk_hw_enable_pll();
	clk_hw_enable_cpu();
}

static void mem_init(void)
{
	xram_init();
}

#ifdef CONFIG_USB_AT8XC5122
#include <target/usb.h>
#define usbd_hw_pulldown_dplus()	usbd_hw_bus_detach()
#else
#define usbd_hw_pulldown_dplus()
#endif

void board_init_early(void)
{
	xram_init_early();
#if 0
	usbd_hw_pulldown_dplus();
#endif
}

void board_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_MCS51);
	clk_init();
	mem_init();
}

void appl_init(void)
{
}
