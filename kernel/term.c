#include <target/term.h>
#include <target/generic.h>
#include <target/timer.h>

#define TERM_TIMER_INTERVAL	128
#define TERM_BLINK_INTERVAL	1024
#define TERM_PAINT_INTERVAL	TERM_TIMER_INTERVAL
#define TERM_INTERVAL_PER_BLINK	(TERM_BLINK_INTERVAL/TERM_TIMER_INTERVAL)

struct terminal term_info;

#define term_increment_pos(what, n)	\
	(term_info.what = (term_info.what + n) % term_info.depth)
#define term_decrement_pos(what, n)	\
	(term_info.what = (term_info.what - n + term_info.depth) % term_info.depth)
#define term_slot_lines(top, bottom)	\
	(bottom < top ? bottom - top + term_info.depth : bottom - top)
#define term_is_buffer_full()		\
	((term_info.bottom + 1) % term_info.depth == term_info.top)
#define term_is_buffer_empty()		\
	(term_info.bottom == term_info.top)

bh_t term_bh = INVALID_BH;
tid_t term_tid = INVALID_TID;

video_rgb_t term_cfg_palette[TRM_NUMBER_FGCOLOURS] = {
	TRM_DEFAULT_FOREGROUND,
	TRM_DEFAULT_BOLD_FOREGROUND,
	TRM_DEFAULT_BACKGROUND,
	TRM_DEFAULT_BOLD_BACKGROUND,
	TRM_CURSOR_TEXT,
	TRM_CURSOR_COLOUR,
	TRM_ANSI_X364_BLACK,
	TRM_ANSI_X364_BOLD_BLACK,
	TRM_ANSI_X364_RED,
	TRM_ANSI_X364_BOLD_RED,
	TRM_ANSI_X364_GREEN,
	TRM_ANSI_X364_BOLD_GREEN,
	TRM_ANSI_X364_YELLOW,
	TRM_ANSI_X364_BOLD_YELLOW,
	TRM_ANSI_X364_BLUE,
	TRM_ANSI_X364_BOLD_BLUE,
	TRM_ANSI_X364_MAGENTA,
	TRM_ANSI_X364_BOLD_MAGENTA,
	TRM_ANSI_X364_CYAN,
	TRM_ANSI_X364_BOLD_CYAN,
	TRM_ANSI_X364_WHITE,
	TRM_ANSI_X364_BOLD_WHITE,
};

/* Copy the colour from the configuration data into rgbt_def.
 * This is non-trivial because the colour indices are different.
 */
void term_palette_from_ansi(void)
{
	uint8_t i;
	static const uint16_t ww[] = {
		256, 257, 258, 259, 260, 261,
		0, 8, 1, 9, 2, 10, 3, 11,
		4, 12, 5, 13, 6, 14, 7, 15
	};
	
	for (i = 0; i < 22; i++) {
		uint16_t w = ww[i];
		term_info.rgbt_def[w].red = RGB_R(term_cfg_palette[i]);
		term_info.rgbt_def[w].green = RGB_G(term_cfg_palette[i]);
		term_info.rgbt_def[w].blue = RGB_B(term_cfg_palette[i]);
	}
	for (i = 0; i < TRM_NUMBER_NEXTCOLOURS; i++) {
		if (i < 216) {
			uint8_t r = i / 36, g = (i / 6) % 6, b = i % 6;
			term_info.rgbt_def[i+16].red = r * 0x33;
			term_info.rgbt_def[i+16].green = g * 0x33;
			term_info.rgbt_def[i+16].blue = b * 0x33;
		} else {
			uint8_t shade = i - 216;
			shade = (shade + 1) * 0xFF / (TRM_NUMBER_NEXTCOLOURS - 216 + 1);
			term_info.rgbt_def[i+16].red =
			term_info.rgbt_def[i+16].green =
			term_info.rgbt_def[i+16].blue = shade;
		}
	}
}

/* Set up the colour palette */
void term_palette_init(void)
{
	uint8_t i;
	
	term_palette_from_ansi();
	for (i = 0; i < TRM_NUMBER_ALLCOLOURS; i++) {
		term_info.palette[i] = RGB(term_info.rgbt_def[i].red,
					   term_info.rgbt_def[i].green,
					   term_info.rgbt_def[i].blue);
	}
}

void term_draw_invalid(term_pos_t x, term_pos_t y,
		       term_len_t w, term_len_t h)
{
	if (term_info.invalid) {
		term_pos_t x2, y2;
		term_pos_t inv_x2, inv_y2;

		x2 = x + w;
		y2 = y + h;
		inv_x2 = term_info.inv_x + term_info.inv_width;
		inv_y2 = term_info.inv_y + term_info.inv_height;
		if (x < term_info.inv_x)
			term_info.inv_x = x;
		if (y < term_info.inv_y)
			term_info.inv_y = y;
		term_info.inv_width = max(x2, inv_x2) -
				      term_info.inv_x;
		term_info.inv_height = max(y2, inv_y2) -
				       term_info.inv_y;
	} else {
		term_info.inv_x = x;
		term_info.inv_y = y;
		term_info.inv_width = w;
		term_info.inv_height = h;
		term_info.invalid = 1;
	}
}

void term_draw_paint(void)
{
}

void term_draw_init(void)
{
	term_info.invalid = 0;
}

void term_init_buffer_line(term_pos_t line)
{
#if 0
	char *text = term_info.output + line * term_info.width;
	uint32_t *attrib = term_info.attrib + line * term_info.width;
#endif
	term_pos_t i = 0;
	size_t len = term_info.width + line * term_info.width;

	for (i = line * term_info.width; i < len; i++) {
		term_info.output[i] = ' ';
		term_info.attrib[i] = TRM_ATTRIB_DEFAULT;
	}
}

term_len_t term_buffer_lines(void)
{
	return term_slot_lines(term_info.top, term_info.bottom);
}

term_len_t term_screen_lines(void)
{
	return term_slot_lines(term_info.screen, term_info.bottom);
}

void term_del_top_buffer_line(void)
{
	if (term_is_buffer_empty())
		return;
	term_increment_pos(top, 1);
	term_init_buffer_line(term_info.top);
}

void term_get_new_buffer_line(void)
{
	term_len_t scr_lines = term_screen_lines();

	if (term_info.y < scr_lines)
		return;

	if (term_is_buffer_full())
		term_del_top_buffer_line();
	term_increment_pos(bottom, term_info.y - scr_lines + 1);
	term_init_buffer_line(term_info.bottom);
}

void term_table_clear(void)
{
	term_pos_t i = 0;

	for (i = 0; i < term_info.width; i++)
		term_info.tabs[i] = ' ';
}

void term_table_init(void)
{
	term_pos_t i = 0;

	term_table_clear();
	for (i = 0; i < term_info.width; i += TRM_TAB_SPACE)
		term_info.tabs[i] = 'x';
	term_info.tabs[term_info.width - 1] = 'x';
}

void term_cursor_restore(void)
{
	term_info.x = term_info.Px;
	term_info.y = term_info.Py;
}

void term_cursor_save(void)
{
	term_info.Px = term_info.x;
	term_info.Py = term_info.y;
}

size_t term_get_index_by_Y(term_pos_t y)
{
	size_t offset;
	
	offset = term_info.view + y;
	if (offset >= term_info.depth)
		offset -= term_info.depth;
	
	return offset;
}

size_t term_get_index_by_XY(term_pos_t x, term_pos_t y)
{
	return mul16u((uint16_t)term_get_index_by_Y(y),
		      (uint16_t)(term_info.width + x));
}

char term_get_text_at(term_pos_t x, term_pos_t y)
{
	size_t offset = mul16u((uint16_t)term_get_index_by_Y(y),
			       (uint16_t)term_info.width);
	return term_info.output[offset + x];
}

void term_set_text_at(term_pos_t x, term_pos_t y, char output)
{
	size_t offset = mul16u((uint16_t)term_get_index_by_Y(y),
			       (uint16_t)term_info.width);
	term_info.output[offset + x] = output;
}

uint32_t term_get_attr_at(term_pos_t x, term_pos_t y)
{
	size_t offset = mul16u((uint16_t)term_get_index_by_Y(y),
			       (uint16_t)term_info.width);
	return term_info.attrib[offset + x];
}

void term_set_attr_at(term_pos_t x, term_pos_t y, uint32_t attrib)
{
	size_t offset = mul16u((uint16_t)term_get_index_by_Y(y),
			       (uint16_t)term_info.width);
	term_info.attrib[offset + x] = attrib;
}

void term_scroll_up(term_len_t lines)
{
	term_len_t max_lines;
	
	max_lines = term_slot_lines(term_info.top, term_info.view);
	if (lines > max_lines)
		lines = max_lines;
	term_decrement_pos(view, lines);
}

void term_scroll_down(term_len_t lines)
{
	term_len_t max_lines;
	
	max_lines = term_info.depth -
		    term_slot_lines(term_info.top, term_info.view) - 1;
	if (lines > max_lines)
		lines = max_lines;
	term_increment_pos(view, lines);
}

void term_erase_to_EOS_line(term_pos_t y)
{
	if (y < 0)
		y = term_info.y;
	term_get_new_buffer_line();
	term_erase_line(y);
	term_draw_invalid(term_info.x, y, term_info.width, y + 1);
}

void term_erase_to_EOL(void)
{
	term_pos_t i = 0;

	for (i = term_info.x; i < term_info.width; i++) {
		term_set_text_at(i, term_info.y, 0);
		term_set_attr_at(i, term_info.y, TRM_ATTRIB_DEFAULT);
	}
	term_draw_invalid(term_info.x, term_info.y,
			 term_info.width - term_info.x + 1, 1);
}

void term_erase_to_BOL(void)
{
	term_pos_t i = 0;

	for (i = 0; i < term_info.x; i++) {
		term_set_text_at(i, term_info.y, 0);
		term_set_attr_at(i, term_info.y, TRM_ATTRIB_DEFAULT);
	}
	term_draw_invalid(term_info.x, term_info.y,
			 term_info.x + 1, 1);
}

void term_erase_to_pos(void)
{
	term_pos_t i = 0;

	for (i = 0; i < term_info.y; i++)
		term_erase_line(i);
}

void term_erase_to_EOS(void)
{
	term_pos_t i = 0;
	for (i = term_info.y; i < term_info.height; i++)
		term_erase_to_EOS_line(i);
}

void term_erase_line(term_pos_t line)
{
	term_pos_t i;

	if (line < 0)
		line = 0;
	if (line > term_info.height - 1)
		line = term_info.height - 1;
	for (i = term_info.x; i < term_info.width; i++) {
		term_set_text_at(i, line, 0);
		term_set_attr_at(i, line, TRM_ATTRIB_DEFAULT);
	}
	term_draw_invalid(term_info.x, term_info.y,
			 term_info.width, 1);
}

void term_erase_screen(void)
{
	term_erase_to_pos();
	term_erase_to_EOS();
}

void term_screen_ap_clear(void)
{
	for (term_info.esc_nargs = 5;
	     term_info.esc_nargs >= 0; term_info.esc_nargs--)
		term_info.esc_parms[term_info.esc_nargs] = -1;
	term_info.esc_nargs = 0;
}

void term_cursor_on(void)
{
	term_pos_t index_Y; 
	term_len_t y;

	term_info.cur_attrib |= TRM_ATTRIB_CURSOR;
	index_Y = (term_info.screen + term_info.y) % term_info.depth;
	y = term_slot_lines(term_info.view, index_Y);
	term_draw_invalid(term_info.x, y, 1, 1);
}

void term_cursor_off(void)
{
	term_info.cur_attrib &= ~TRM_ATTRIB_CURSOR;
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
}

void term_cursor_up(term_len_t len)
{
	/* cursor up (CUU) */
	term_info.wrap_pending = 0;
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
	term_info.y -= len;
	if (term_info.y < term_info.margin_top)
		term_info.y = term_info.margin_top;
	term_screen_recalc();
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
}

void term_cursor_down(term_len_t len)
{
	/* cursor down (CUD) */
	term_info.wrap_pending = 0;
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
	term_info.y += len;
	if (term_info.y > term_info.margin_bottom)
		term_info.y = term_info.margin_bottom;
	term_screen_recalc();
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
}
		
void term_cursor_right(term_len_t len)
{
	/* cursor forward (right) (CUF) */
	term_info.wrap_pending = 0;
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
	term_info.x += len;
	term_screen_recalc();
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
}
		
void term_cursor_left(term_len_t len)
{
	/* cursor backward (left) (CUB) */
	term_info.wrap_pending = 0;
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
	term_info.x -= len;
	term_screen_recalc();
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
}

void term_cursor_pos(term_pos_t y, term_pos_t x)
{
	/* horizontal & vertical position (HVP) */
	term_info.wrap_pending = 0;
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
	term_info.y = y - 1;
	term_info.x = x - 1;
	term_screen_recalc();
	term_draw_invalid(term_info.x, term_info.y, 1, 1);
}

void term_blink_enable(void)
{
	term_info.cur_attrib |= TRM_ATTRIB_BLINK;
	//term_blink_schedule();
}

void term_blink_disable(void)
{
	term_info.cur_attrib &= ~TRM_ATTRIB_BLINK;
}

void term_screen_reset(void)
{
	term_info.margin_top = 0;
	term_info.margin_bottom = term_info.height-1;
	term_info.esc_nargs = 0;
	term_info.esc_level = 0;
	term_info.decawm = 1;
	term_info.decckm = 0;
	term_info.decpam = 0;
	term_info.x = 0;
	term_info.y = 0;
	term_erase_screen();
	term_table_init();
}

void term_screen_recalc(void)
{
	if (term_info.y < 0)
		term_info.y = 0;
	if (term_info.y > term_info.height - 1)
		term_info.y = term_info.height - 1;
	if (term_info.x < 0)
		term_info.x = 0;
	if (term_info.x > term_info.width - 1)
		term_info.x = term_info.width - 1;
}

boolean term_screen_is_mode_cmd(void)
{
	return (term_info.esc_parms[0] == -2) &&
	       (term_info.esc_parms[1] == 1);
}

void term_screen_set_mode(void)
{
	if (term_screen_is_mode_cmd())
		term_info.decckm = 1;
}

void term_screen_reset_mode(void)
{
	if (term_screen_is_mode_cmd())
		term_info.decckm = 0;
}

void term_screen_init(void)
{
	/* output buffer */
	term_info.x = term_info.y = 0;
	term_info.width = term_info.columns = TRM_DEFAULT_WIDTH;
	term_info.height = term_info.rows = TRM_DEFAULT_HEIGHT;
	term_info.depth = TRM_DEFAULT_DEPTH;
	term_info.top = term_info.bottom =
	term_info.screen = term_info.view = 0;
	term_init_buffer_line(term_info.bottom);
	term_info.offset = term_info.width - 1;
	term_info.esc_nargs = 0;
	term_info.esc_level = 0;
	/* input buffer */
	term_info.size = term_info.columns;
	term_info.head = term_info.tail = 0;
	term_info.replace = 1;

	/* terminal modes */
#if 0
	term_info.resizeable = 0;
#endif
	term_info.def_attrib = term_info.cur_attrib = TRM_ATTRIB_DEFAULT;
	term_info.blinking = 1;
	term_info.margin_top = 0;
	term_info.margin_bottom = term_info.height - 1;
	term_info.backspace = 0x08;
	term_info.ctrl_backspace = 0x7f;
	term_info.decawm = 1;
	term_info.wrap_pending = 1;
	term_info.decckm = 0;

	term_table_init();
}

void term_fonts_init(void)
{
	/* TODO: choose font */
}

void term_cursor_init(void)
{
	term_info.def_attrib |= TRM_ATTRIB_CURSOR;
	term_cursor_on();
}

static void term_timer_handler(void)
{
	term_info.blink_count++;
	if ((term_info.blink_count % TERM_INTERVAL_PER_BLINK) == 0) {
		if (term_info.blink_flash)
			term_info.blink_flash = false;
		else
			term_info.blink_flash = true;
	}
	term_draw_paint();
	timer_schedule_shot(term_tid, TERM_TIMER_INTERVAL);
}

static void term_handler(uint8_t event)
{
	switch (event) {
	case BH_TIMEOUT:
		term_timer_handler();
		break;
	default:
		BUG();
		break;
	}
}

void term_init(void)
{
	term_bh = bh_register_handler(term_handler);
	term_tid = timer_register(term_bh, TIMER_DELAYABLE);
	timer_schedule_shot(term_tid, TERM_TIMER_INTERVAL);

	term_palette_init();
	term_fonts_init();
	term_screen_init();
	term_cursor_init();
}
