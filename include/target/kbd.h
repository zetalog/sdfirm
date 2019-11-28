#ifndef __KBD_H_INCLUDE__
#define __KBD_H_INCLUDE__

#include <target/generic.h>

#include <driver/kbd.h>

typedef void (*kbd_event_cb)(uint8_t scancode, uint8_t event);

#define NR_KEY_COLS			KBD_HW_COL_KEYS
#define NR_KEY_ROWS			KBD_HW_ROW_KEYS
#define NR_KEYS				(NR_KEY_COLS * NR_KEY_ROWS)

#define KBD_INVALID_SCANCODE		0xFF

#define KBD_KEY_STATUS_UP		0x00
#define KBD_KEY_STATUS_DOWN		0x01
#define KBD_KEY_STATUS_INIT		0xFF

#define KBD_EVENT_KEY_UP		0x00
#define KBD_EVENT_KEY_DOWN		0x01
#define KBD_EVENT_KEY_TOUT		0x02

/* current key's type:
 * SHORT_HIT
 * LONG_HIT
 * DOUBLE_HITS
 * TWO_KEYS_HIT
 */

#ifdef CONFIG_IO_DEBUG
#define io_debug(tag, val)	dbg_print((tag), (val))
#else
#define io_debug(tag, val)
#endif

#ifdef CONFIG_KBD
void kbd_abort_capture(void);
kbd_event_cb kbd_set_capture(kbd_event_cb new_kh,
			     uint8_t timeout);
void kbd_event_up(uint8_t scancode);
void kbd_event_down(uint8_t scancode);
#else
#define kbd_set_capture(new_kh, timeout)		(NULL)
#define kbd_abort_capture()
#define kbd_event_up(scancode)
#define kbd_event_down(scancode)
#endif

#define kbd_translate_key(scancode)	kbd_hw_translate_key(scancode)

#endif /* __KBD_H_INCLUDE__ */
