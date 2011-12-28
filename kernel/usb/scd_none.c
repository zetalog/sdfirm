#include <target/usb_scd.h>

void scd_Escape_out(void)
{
}

void scd_Escape_cmp(void)
{
	scd_CmdOffset_cmp(0);
}

void scd_Escape_in(void)
{
	scd_SlotStatus_in();
}
