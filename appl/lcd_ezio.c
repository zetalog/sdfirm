#include <target/uart.h>
#include <target/lcd.h>
#include <target/kbd.h>
#include <target/usb.h>

#define EZIO_UART_PID		CONFIG_EZIO_UART_PORT
#define EZIO_MAX_BUF		32

#define EZIO_WRITE		0xFE
#define EZIO_READ		0xFD

#define EZIO_CMD_StartOfHEX	0x28
#define EZIO_CMD_EndOfHex	0x37
#define EZIO_CMD_ClearScreen	0x01
#define EZIO_CMD_HomeCursor	0x02
#define EZIO_CMD_ReadKey	0x06
#define EZIO_CMD_BlankDisplay	0x08
#define EZIO_CMD_HideCursor	0x0C
#define EZIO_CMD_TurnOn		0x0D
#define EZIO_CMD_ShowCursor	0x0E
#define EZIO_CMD_MoveLeft	0x10
#define EZIO_CMD_MoveRight	0x14
#define EZIO_CMD_ScrollLeft	0x18
#define EZIO_CMD_ScrollRight	0x1C
#define EZIO_CMD_SetDispAddr	0x80
#define EZIO_CMD_SetCharAddr	0x40

#define EZIO_DISPADDR_MASK	0x4F

#define ezio_cmd_prefix_is_valid()			\
	(ezio_cmd.prefix == EZIO_WRITE)
#define ezio_cmd_addrdisp_is_valid()			\
	((ezio_cmd.addr & ~EZIO_DISPADDR_MASK) == 0)

#define __ezio_cmd_has_addr()				\
	((ezio_cmd.cmd == EZIO_CMD_SetDispAddr) ||	\
	 (ezio_cmd.cmd == EZIO_CMD_SetCharAddr))
#define ezio_cmd_has_addr()				\
	(ezio_cmd_prefix_is_valid() &&	__ezio_cmd_has_addr())

#define __ezio_hex_is_end(hex)				\
	(hex == EZIO_CMD_EndOfHex)

#define EZIO_KEY_Escape		0
#define EZIO_KEY_UpArrow	1
#define EZIO_KEY_Enter		2
#define EZIO_KEY_DownArrow	3

#define EZIO_STATE_CMD		0x00
#define EZIO_STATE_RESP		0x01
#define EZIO_STATE_HEX		0x02

#define EZIO_HEX_IDLE		0x00
#define EZIO_HEX_STARTED	0x01
#define EZIO_HEX_ENDING		0x02

struct ezio_cmd {
	uint8_t prefix;
	uint8_t cmd;
	uint8_t addr;
};

static boolean ezio_sync_cmd(uint8_t *cmd);
static void ezio_resp_poll(void);
static void ezio_resp_iocb(void);
static void ezio_resp_done(void);
static void ezio_cmd_poll(void);
static void ezio_cmd_iocb(void);
static void ezio_cmd_done(void);

static void ezio_hex_transmit(void);

uint8_t ezio_rxbuf[EZIO_MAX_BUF];
uint8_t ezio_state;
uint8_t ezio_hex;
struct ezio_cmd ezio_cmd;
uint8_t ezio_keys;
kbd_event_cb ezio_kh = NULL;
uint8_t ezio_oob[1];

bulk_user_t ezio_bulk_resp = {
	ezio_resp_poll,
	ezio_resp_iocb,
	ezio_resp_done,
};

bulk_user_t ezio_bulk_cmd = {
	ezio_cmd_poll,
	ezio_cmd_iocb,
	ezio_cmd_done,
};

uart_user_t ezio_uart = {
	UART_DEF_PARAMS,
	2400,
	NULL,
	ezio_rxbuf,
	0,
	EZIO_MAX_BUF,
	&ezio_bulk_resp,
	&ezio_bulk_cmd,
	ezio_sync_cmd,
	ezio_oob,
	1,
};

#define ezio_cmd_halt()		\
	bulk_channel_halt(uart_bulk_rx(EZIO_UART_PID))

static void ezio_hex_transmit(void)
{
	uint8_t hex[EZIO_MAX_BUF];
	int res, i, end_idx = 0;

	res = bulk_read(hex, EZIO_MAX_BUF);
	if (res <= 0) {
		ezio_cmd_halt();
		return;
	}

started:
	if (ezio_hex == EZIO_HEX_STARTED) {
		for (i = end_idx; i < res; i++) {
			if (hex[i] == EZIO_WRITE) {
				ezio_hex = EZIO_HEX_ENDING;
				end_idx = i+1;
				goto ending;
			} else {
				lcd_putchar(hex[i]);
			}
		}
	}
ending:
	if (ezio_hex == EZIO_HEX_ENDING && res != end_idx) {
		BUG_ON(res > end_idx);
		if ((res - end_idx > 1) ||
		    !__ezio_hex_is_end(hex[end_idx])) {
			ezio_hex = EZIO_HEX_STARTED;
			lcd_putchar(EZIO_WRITE);
			goto started;
		} else {
			bulk_transmit_complete();
		}
	}
}

static void ezio_cmd_poll(void)
{
	if (ezio_state == EZIO_STATE_CMD) {
		bulk_request_submit(2);
	} else if (ezio_state == EZIO_STATE_HEX) {
		if (bulk_transmit_submit())
			ezio_hex = EZIO_HEX_STARTED;
	}
}

static void ezio_cmd_iocb(void)
{
	if (ezio_state == EZIO_STATE_CMD) {
		BULK_READB(ezio_cmd.prefix);
		BULK_READB(ezio_cmd.cmd);
		if (bulk_request_handled() == 2) {
			if (ezio_cmd_has_addr())
				bulk_request_commit(1);
		}
	} else {
		ezio_hex_transmit();
	}
}

static void ezio_cmd_execute(void)
{
	switch (ezio_cmd.cmd) {
	case EZIO_CMD_StartOfHEX:
		ezio_state = EZIO_STATE_HEX;
		break;
	case EZIO_CMD_ClearScreen:
		break;
	case EZIO_CMD_HomeCursor:
		break;
	case EZIO_CMD_ReadKey:
		ezio_state = EZIO_STATE_RESP;
		break;
	case EZIO_CMD_BlankDisplay:
		break;
	case EZIO_CMD_HideCursor:
		break;
	case EZIO_CMD_TurnOn:
		break;
	case EZIO_CMD_ShowCursor:
		break;
	case EZIO_CMD_MoveLeft:
		break;
	case EZIO_CMD_MoveRight:
		break;
	case EZIO_CMD_ScrollLeft:
		break;
	case EZIO_CMD_ScrollRight:
		break;
	case EZIO_CMD_SetDispAddr:
		if (ezio_cmd_addrdisp_is_valid()) {
		}
		break;
	default:
		ezio_cmd_halt();
		break;
	}
}

static void ezio_cmd_done(void)
{
	if (ezio_state == EZIO_STATE_CMD) {
		if (!ezio_cmd_prefix_is_valid()) {
			ezio_cmd_halt();
			return;
		}
		ezio_cmd_execute();
	} else {
		ezio_hex = EZIO_HEX_IDLE;
	}
}

static void ezio_resp_poll(void)
{
	if (ezio_state == EZIO_STATE_RESP)
		bulk_request_submit(2);
}

static void ezio_resp_iocb(void)
{
	BULK_WRITEB(EZIO_READ);
	BULK_WRITEB(0x40 | (ezio_keys & 0x0F));
}

static void ezio_resp_done(void)
{
	if (ezio_state == EZIO_STATE_RESP)
		ezio_state = EZIO_STATE_CMD;
}

static boolean ezio_sync_cmd(uint8_t *cmd)
{
	return cmd[0] == EZIO_WRITE;
}

static void ezio_key_capture(uint8_t scancode, uint8_t event)
{
	switch (event) {
	case KBD_EVENT_KEY_UP:
		ezio_keys &= ~scancode;
		break;
	case KBD_EVENT_KEY_DOWN:
		ezio_keys |= scancode;
		break;
	}
}

void appl_ezio_init(void)
{
	ezio_state = EZIO_STATE_CMD;
	uart_startup(EZIO_UART_PID, &ezio_uart);
	ezio_keys = 0;
	ezio_kh = kbd_set_capture(ezio_key_capture, 0);
}
