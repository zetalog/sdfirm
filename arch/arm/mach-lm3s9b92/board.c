#include <target/generic.h>
#include <target/arch.h>
#include <target/irq.h>
#include <target/heap.h>
#include <target/mtd.h>

extern caddr_t __bss_stop[];
mtd_t board_mtd = INVALID_MTD_ID;

void mem_init(void)
{
	heap_range_init((caddr_t)__bss_stop);
}

#define cpu_init()	clk_hw_set_config(CLK_CONFIG)
#define gpio_init()	gpio_hw_ctrl_init()

#ifdef CONFIG_USB_LM3S9B92
/* USB External Power Control PINs configuration is only known by the
 * board designer
 */
void usb_epc_pin_init(void)
{
	pm_hw_resume_device(DEV_GPIOH, DEV_MODE_ON);
	/* PH3 USBEPEN */
	gpio_hw_config_mux(GPIOH, 3, GPIOH3_MUX_USB0EPEN);
	gpio_hw_config_pad(GPIOH, 3, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_8MA);
	/* PH4 USBPFLT */
	gpio_hw_config_mux(GPIOH, 4, GPIOH4_MUX_USB0PFLT);
	gpio_hw_config_pad(GPIOH, 4, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_8MA);
}

#if 0
void usb_epc_pin_exit(void)
{
	/* PH3 USBEPEN */
	gpio_hw_config_mux(GPIOH, 3, GPIO_MUX_NONE);
	gpio_hw_config_pad(GPIOH, 3, GPIO_DIR_OUT, GPIO_PAD_PP, GPIO_DRIVE_8MA);
	/* PH4 USBPFLT */
	gpio_hw_config_mux(GPIOH, 4, GPIO_MUX_NONE);
	gpio_hw_config_pad(GPIOH, 4, GPIO_DIR_IN, GPIO_PAD_PP, GPIO_DRIVE_8MA);
}
#endif
#endif

void board_init(void)
{
	cpu_init();
	mem_init();
	gpio_init();

	DEVICE_ARCH(DEVICE_ARCH_ARM);
}

void appl_init(void)
{
}
