#include <host/uartdbg.h>

static void pn53x_dump_usb_state(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "state=IDLE");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "state=CMD");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "state=RESP");
		break;
	default:
		dbg_dumper(ctx, cmd, "state=%02x", data);
		break;
	}
}

static void pn53x_dump_usb_flags(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	int i;
	uint8_t flags[] = "O=RESP, O=POLL";
#define PN53X_USB_FLAG_LEN	8
	for (i = 0; i < 2; i++) {
		if (data & (1<<i)) {
			flags[PN53X_USB_FLAG_LEN*i] = 'O';
		} else {
			flags[PN53X_USB_FLAG_LEN*i] = 'X';
		}
	}
}

static void pn53x_dump_cmd_code(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "cmd=Diagnose", data);
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "cmd=GetFirmwareVersion", data);
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "cmd=GetGeneralStatus", data);
		break;
	case 0x06:
		dbg_dumper(ctx, cmd, "cmd=ReadRegister", data);
		break;
	case 0x08:
		dbg_dumper(ctx, cmd, "cmd=WriteRegister", data);
		break;
	case 0x0C:
		dbg_dumper(ctx, cmd, "cmd=ReadGPIO", data);
		break;
	case 0x0E:
		dbg_dumper(ctx, cmd, "cmd=WriteGPIO", data);
		break;
	case 0x10:
		dbg_dumper(ctx, cmd, "cmd=SetSerialBaudRate", data);
		break;
	case 0x12:
		dbg_dumper(ctx, cmd, "cmd=SetParameters", data);
		break;
	case 0x14:
		dbg_dumper(ctx, cmd, "cmd=SAMConfiguration", data);
		break;
	case 0x16:
		dbg_dumper(ctx, cmd, "cmd=PowerDown", data);
		break;
	case 0x18:
		dbg_dumper(ctx, cmd, "cmd=AlparCommandForTDA", data);
		break;
	case 0x32:
		dbg_dumper(ctx, cmd, "cmd=RFConfiguration", data);
		break;
	case 0x58:
		dbg_dumper(ctx, cmd, "cmd=RFRegulationTest", data);
		break;
	case 0x56:
		dbg_dumper(ctx, cmd, "cmd=InJumpForDEP", data);
		break;
	case 0x46:
		dbg_dumper(ctx, cmd, "cmd=InJumpForPSL", data);
		break;
	case 0x4A:
		dbg_dumper(ctx, cmd, "cmd=InListPassiveTarget", data);
		break;
	case 0x50:
		dbg_dumper(ctx, cmd, "cmd=InATR", data);
		break;
	case 0x4E:
		dbg_dumper(ctx, cmd, "cmd=InPSL", data);
		break;
	case 0x40:
		dbg_dumper(ctx, cmd, "cmd=InDataExchange", data);
		break;
	case 0x42:
		dbg_dumper(ctx, cmd, "cmd=InCommunicateThru", data);
		break;
	case 0x38:
		dbg_dumper(ctx, cmd, "cmd=InQuartetByteExchange", data);
		break;
	case 0x44:
		dbg_dumper(ctx, cmd, "cmd=InDeselect", data);
		break;
	case 0x52:
		dbg_dumper(ctx, cmd, "cmd=InRelease", data);
		break;
	case 0x54:
		dbg_dumper(ctx, cmd, "cmd=InSelect", data);
		break;
	case 0x48:
		dbg_dumper(ctx, cmd, "cmd=InActivateDeactivatePaypass", data);
		break;
	case 0x60:
		dbg_dumper(ctx, cmd, "cmd=InAutoPoll", data);
		break;
	case 0x8C:
		dbg_dumper(ctx, cmd, "cmd=TgInitAsTarget", data);
		break;
	case 0x92:
		dbg_dumper(ctx, cmd, "cmd=TgSetGeneralBytes", data);
		break;
	case 0x86:
		dbg_dumper(ctx, cmd, "cmd=TgGetData", data);
		break;
	case 0x8E:
		dbg_dumper(ctx, cmd, "cmd=TgSetData", data);
		break;
	case 0x96:
		dbg_dumper(ctx, cmd, "cmd=TgSetDataSecure", data);
		break;
	case 0x94:
		dbg_dumper(ctx, cmd, "cmd=TgSetMetaData", data);
		break;
	case 0x98:
		dbg_dumper(ctx, cmd, "cmd=TgSetMetaDataSecure", data);
		break;
	case 0x88:
		dbg_dumper(ctx, cmd, "cmd=TgGetInitiatorCommand", data);
		break;
	case 0x90:
		dbg_dumper(ctx, cmd, "cmd=TgResponseToInitiator", data);
		break;
	case 0x8A:
		dbg_dumper(ctx, cmd, "cmd=TgGetTargetStatus", data);
		break;
	default:
		dbg_dumper(ctx, cmd, "cmd=%02x", data);
		break;
	}
}

static void pn53x_dump_err_code(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x01:
		dbg_dumper(ctx, cmd, "err=TIMEOUT", data);
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "err=CRC", data);
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "err=PARITY", data);
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "err=BITCOUNT", data);
		break;
	case 0x05:
		dbg_dumper(ctx, cmd, "err=FRAMING", data);
		break;
	case 0x06:
		dbg_dumper(ctx, cmd, "err=BITCOLL", data);
		break;
	case 0x07:
		dbg_dumper(ctx, cmd, "err=SMALLBUF", data);
		break;
	case 0x09:
		dbg_dumper(ctx, cmd, "err=BUFOVF", data);
		break;
	case 0x0a:
		dbg_dumper(ctx, cmd, "err=RFTIMEOUT", data);
		break;
	case 0x0b:
		dbg_dumper(ctx, cmd, "err=RFPROTO", data);
		break;
	case 0x0d:
		dbg_dumper(ctx, cmd, "err=OVHEAT", data);
		break;
	case 0x0e:
		dbg_dumper(ctx, cmd, "err=INBUFOVF", data);
		break;
	case 0x10:
		dbg_dumper(ctx, cmd, "err=INVPARAM", data);
		break;
	case 0x12:
		dbg_dumper(ctx, cmd, "err=DEPUNKCMD", data);
		break;
	case 0x13:
		dbg_dumper(ctx, cmd, "err=INVRXFRAM", data);
		break;
	case 0x14:
		dbg_dumper(ctx, cmd, "err=MFAUTH", data);
		break;
	case 0x18:
		dbg_dumper(ctx, cmd, "err=NSECNOTSUPP", data);
		break;
	case 0x23:
		dbg_dumper(ctx, cmd, "err=BCC", data);
		break;
	case 0x25:
		dbg_dumper(ctx, cmd, "err=DEPINVSTATE", data);
		break;
	case 0x26:
		dbg_dumper(ctx, cmd, "err=CMD", data);
		break;
	case 0x27:
		dbg_dumper(ctx, cmd, "err=INVPARAM", data);
		break;
	case 0x29:
		dbg_dumper(ctx, cmd, "err=TGREL", data);
		break;
	case 0x2a:
		dbg_dumper(ctx, cmd, "err=CID", data);
		break;
	case 0x2b:
		dbg_dumper(ctx, cmd, "err=CDISCARDED", data);
		break;
	case 0x2c:
		dbg_dumper(ctx, cmd, "err=NFCID3", data);
		break;
	case 0x2d:
		dbg_dumper(ctx, cmd, "err=OVCURRENT", data);
		break;
	case 0x2e:
		dbg_dumper(ctx, cmd, "err=NAD", data);
		break;
	default:
		dbg_dumper(ctx, cmd, "err=%02x", data);
		break;
	}
}

static void ezio_dump_cmd(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x28:
		dbg_dumper(ctx, cmd, "cmd=StartOfHEX");
		break;
	case 0x37:
		dbg_dumper(ctx, cmd, "cmd=EndOfHex");
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "cmd=ClearScreen");
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "cmd=HomeCursor");
		break;
	case 0x06:
		dbg_dumper(ctx, cmd, "cmd=ReadKey");
		break;
	case 0x08:
		dbg_dumper(ctx, cmd, "cmd=BlankDisplay");
		break;
	case 0x0C:
		dbg_dumper(ctx, cmd, "cmd=HideCursor");
		break;
	case 0x0D:
		dbg_dumper(ctx, cmd, "cmd=TurnOn");
		break;
	case 0x0E:
		dbg_dumper(ctx, cmd, "cmd=ShowCursor");
		break;
	case 0x10:
		dbg_dumper(ctx, cmd, "cmd=MoveLeft");
		break;
	case 0x14:
		dbg_dumper(ctx, cmd, "cmd=MoveRight");
		break;
	case 0x18:
		dbg_dumper(ctx, cmd, "cmd=ScrollLeft");
		break;
	case 0x1C:
		dbg_dumper(ctx, cmd, "cmd=ScrollRight");
		break;
	case 0x80:
		dbg_dumper(ctx, cmd, "cmd=SetDispAddr");
		break;
	case 0x40:
		dbg_dumper(ctx, cmd, "cmd=SetCharAddr");
		break;
	default:
		dbg_dumper(ctx, cmd, "cmd=%02x", data);
		break;
	}
}

static void ezio_dump_state(void *ctx, dbg_cmd_t cmd, dbg_data_t data)
{
	switch (data) {
	case 0x00:
		dbg_dumper(ctx, cmd, "state=CMD", data);
		break;
	case 0x01:
		dbg_dumper(ctx, cmd, "state=RESP", data);
		break;
	case 0x02:
		dbg_dumper(ctx, cmd, "state=HEX", data);
		break;
	case 0x03:
		dbg_dumper(ctx, cmd, "state=HALT", data);
		break;
	case 0x04:
		dbg_dumper(ctx, cmd, "state=UNHALT", data);
		break;
	default:
		dbg_dumper(ctx, cmd, "state=%02x", data);
		break;
	}
}

struct dbg_parser dbg_user_events[NR_USER_EVENTS] = {
	{ "PN53X_USBSTATE", 0, pn53x_dump_usb_state },
	{ "PN53X_USBFLAGS", 0, pn53x_dump_usb_flags },
	{ "PN53X_CMD", 0, pn53x_dump_cmd_code },
	{ "PN53X_ERR", 0, pn53x_dump_err_code },
	{ "EZIO_CMD", 0, ezio_dump_cmd },
	{ "EZIO_STATE", 0, ezio_dump_state },
};

struct dbg_source dbg_user_source = {
	"user",
	dbg_user_events,
	NR_USER_EVENTS,
};

void dbg_user_init(void)
{
	dbg_register_source(DBG_SRC_USER, &dbg_user_source);
}
