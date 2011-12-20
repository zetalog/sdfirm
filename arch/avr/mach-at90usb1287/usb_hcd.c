#include <target/usb.h>
#include <target/delay.h>

void hcd_hw_ctrl_init(void)
{
	__usb_hw_select_host();
	__usb_hw_config_host();
}
