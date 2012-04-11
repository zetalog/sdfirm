#ifndef __EZIO_H_INCLUDE__
#define __EZIO_H_INCLUDE__

#include <host/uart.h>

#define EZIO_WRITE			0xFE
#define EZIO_READ			0xFD

#define EZIO_CMD_StartOfHEX		0x28
#define EZIO_CMD_EndOfHex		0x37
#define EZIO_CMD_ClearScreen		0x01
#define EZIO_CMD_HomeCursor		0x02
#define EZIO_CMD_ReadKey		0x06
#define EZIO_CMD_BlankDisplay		0x08
#define EZIO_CMD_HideCursor		0x0C
#define EZIO_CMD_TurnOn			0x0D
#define EZIO_CMD_ShowCursor		0x0E
#define EZIO_CMD_MoveLeft		0x10
#define EZIO_CMD_MoveRight		0x14
#define EZIO_CMD_ScrollLeft		0x18
#define EZIO_CMD_ScrollRight		0x1C
#define EZIO_CMD_SetDispAddr		0x80
#define EZIO_CMD_SetCharAddr		0x40

#define EZIO_SCAN_Escape		0x00
#define EZIO_SCAN_UpArrow		0x01
#define EZIO_SCAN_Enter			0x02
#define EZIO_SCAN_DownArrow		0x03
#define EZIO_MAX_SCAN			4
#define EZIO_KEY(scan)			(1<<(scan))
#define EZIO_KEY_VAL(key, scan)		(((key) & EZIO_KEY(scan)) >> (scan))
#define EZIO_KEY_DOWN(key, scan)	(EZIO_KEY_VAL(key, scan) == 1)

#define EZIO_MAX_LINE			16
#define EZIO_MAX_BUF			(2*EZIO_MAX_LINE)

#define EZIO_STATE_IDLE		0x00
#define EZIO_STATE_SCAN		0x01
#define EZIO_STATE_DUMP		0x02

#define ezio_keypad_scan()	ezio_sync_cmd(EZIO_CMD_ReadKey)
#define ezio_screen_clear()	ezio_sync_cmd(EZIO_CMD_ClearScreen)
#define ezio_screen_blank()	ezio_sync_cmd(EZIO_CMD_BlankDisplay)
#define ezio_data_start()	ezio_sync_cmd(EZIO_CMD_StartOfHEX)
#define ezio_data_stop()	ezio_sync_cmd(EZIO_CMD_EndOfHex)
#define ezio_data_left()	ezio_sync_cmd(EZIO_CMD_ScrollLeft)
#define ezio_data_right()	ezio_sync_cmd(EZIO_CMD_ScrollRight)
#define ezio_cursor_home()	ezio_sync_cmd(EZIO_CMD_HomeCursor)
#define ezio_cursor_blink()	ezio_sync_cmd(EZIO_CMD_TurnOn)
#define ezio_cursor_hide()	ezio_sync_cmd(EZIO_CMD_HideCursor)
#define ezio_cursor_show()	ezio_sync_cmd(EZIO_CMD_ShowCursor)
#define ezio_cursor_left()	ezio_sync_cmd(EZIO_CMD_MoveLeft)
#define ezio_cursor_right()	ezio_sync_cmd(EZIO_CMD_MoveRight)
#define ezio_cursor_locate()	ezio_sync_cmd(EZIO_CMD_SetDispAddr)

const char *ezio_key_name(int i);
int ezio_init(const char *port, const char *baudrate);
void ezio_exit(void);
void ezio_sync_cmd(int cmd);
void ezio_sync_display(void);

void ezio_async_display(unsigned char *str2);
int ezio_async_read(void);

extern uart_t ezio_uart;
extern int ezio_port;
extern int ezio_state;

#endif /* __EZIO_H_INCLUDE__ */
