#include <target/usb_scd.h>

void scd_Escape_out(void)
{
}

void scd_Escape_cmp(void)
{
	scd_CmdOffset_cmp(0);
	/* pn53x_hw_xfr_block(); */
}

void scd_Escape_in(void)
{
	scd_SlotStatus_in();
}

void scd_Escape_init(void)
{
	/* pn53x_hw_ctrl_init(); */
}
