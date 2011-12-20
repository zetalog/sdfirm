#include <target/usb_ccid.h>

void ccid_Escape_out(void)
{
}

void ccid_Escape_cmp(void)
{
	ccid_CmdOffset_cmp(0);
}

void ccid_Escape_in(void)
{
	ccid_SlotStatus_in();
}
