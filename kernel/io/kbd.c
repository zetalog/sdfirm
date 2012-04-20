#include <target/kbd.h>
#include <target/bh.h>

static void kbd_default_capture(uint8_t scancode, uint8_t event);

kbd_event_cb kbd_event_iocb = kbd_default_capture;
uint8_t kbd_event_tout;
uint8_t kbd_event_left;
boolean kbd_sync_aborted;
bh_t kbd_bh = INVALID_BH;

static void kbd_default_capture(uint8_t scancode, uint8_t event)
{
}

void kbd_reset_timeout(void)
{
	if (kbd_event_tout) {
		kbd_event_left = kbd_event_tout;
		BUG_ON(!kbd_event_iocb);
		kbd_event_iocb(kbd_event_left,
			       KBD_EVENT_KEY_TOUT);
	}
}

void kbd_event_down(uint8_t scancode)
{
	io_debug(IO_DEBUG_KEY_DOWN, scancode);
	kbd_reset_timeout();
	BUG_ON(!kbd_event_iocb);
	kbd_event_iocb(scancode, KBD_EVENT_KEY_DOWN);
}

void kbd_event_up(uint8_t scancode)
{
	io_debug(IO_DEBUG_KEY_UP, scancode);
	BUG_ON(!kbd_event_iocb);
	(*kbd_event_iocb)(scancode, KBD_EVENT_KEY_UP);
}

static void kbd_capture_sync(void)
{
	kbd_event_left = kbd_event_tout;
	while (kbd_event_tout != 0) {
		kbd_hw_scan_sync(1);
		kbd_event_left--;

		BUG_ON(!kbd_event_iocb);
		kbd_event_iocb(kbd_event_left,
			       KBD_EVENT_KEY_TOUT);
		if (kbd_sync_aborted) {
			kbd_event_iocb(0,
				       KBD_EVENT_KEY_TOUT);
			break;
		}
	}
	kbd_sync_aborted = true;
}

void kbd_abort_capture(void)
{
	kbd_sync_aborted = true;
}

kbd_event_cb kbd_set_capture(kbd_event_cb new_kh,
			     uint8_t timeout)
{
	kbd_event_cb temp_kh = kbd_event_iocb;
	kbd_event_iocb = new_kh;

	/* reset key states */

	kbd_event_tout = timeout;
	io_debug(IO_DEBUG_WAIT_KEY, kbd_event_tout);
	if (timeout) {
		kbd_sync_aborted = false;
		bh_resume(kbd_bh);
	}
	return temp_kh;
}

static void kbd_handler(uint8_t event)
{
	BUG_ON(event != BH_WAKEUP);
	if (!kbd_sync_aborted) {
		kbd_capture_sync();
	}
}

void kbd_init(void)
{
	DEVICE_FUNC(DEVICE_FUNC_KBD);
	kbd_bh = bh_register_handler(kbd_handler);
	kbd_sync_aborted = true;
	kbd_hw_ctrl_init();
}
