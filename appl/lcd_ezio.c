#include <target/uart.h>
#include <target/lcd.h>

#define EZIO_UART_PID	CONFIG_EZIO_UART_PORT
#define EZIO_MAX_CMD	32

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

#define EZIO_KEY_Escape		0x4E
#define EZIO_KEY_UpArrow	0x4D
#define EZIO_KEY_Enter		0x4B
#define EZIO_KEY_DownArrow	0x47

boolean ezio_sync_cmd(uint8_t *cmd);
void ezio_resp_poll(void);
void ezio_resp_iocb(void);
void ezio_resp_done(void);
void ezio_cmd_poll(void);
void ezio_cmd_iocb(void);
void ezio_cmd_done(void);

uint8_t ezio_cmd[EZIO_MAX_CMD];

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
	8 | UART_PARITY_NONE | UART_STOPB_ONE,
	2400,
	NULL,
	ezio_cmd,
	0,
	EZIO_MAX_CMD,
	&ezio_bulk_resp,
	&ezio_bulk_cmd,
	ezio_sync_cmd,
	1,
};

void ezio_resp_poll(void)
{
}

void ezio_resp_iocb(void)
{
}

void ezio_resp_done(void)
{
}

void ezio_cmd_poll(void)
{
}

void ezio_cmd_iocb(void)
{
}

void ezio_cmd_done(void)
{
}

boolean ezio_sync_cmd(uint8_t *cmd)
{
	return cmd[0] == EZIO_WRITE;
}

void appl_ezio_init(void)
{
	uart_startup(EZIO_UART_PID, &ezio_uart);
}
