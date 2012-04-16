#include <target/term.h>
#include <target/generic.h>

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

#define term_draw_screen(x, y, w, h)
#define term_draw_cursor(onoff)
#define term_set_cursor_dot(x, y)

void term_init_buffer_line(term_pos_t line)
{
#if 0
	char *text = term_info.output + line * term_info.width;
	long *attrib = term_info.attrib + line * term_info.width;
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

void term_clear_screen_tab(void)
{
	term_pos_t i = 0;
	for (i = 0; i < term_info.width; i++)
		term_info.tabs[i] = ' ';
}

void term_init_screen_tab(void)
{
	term_pos_t i = 0;
	term_clear_screen_tab();
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

long term_get_attr_at(term_pos_t x, term_pos_t y)
{
	size_t offset = mul16u((uint16_t)term_get_index_by_Y(y),
			       (uint16_t)term_info.width);
	return term_info.attrib[offset + x];
}

void term_set_attr_at(term_pos_t x, term_pos_t y, long attrib)
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
	term_draw_screen(term_info.x, y, term_info.width, y + 1);
}

void term_erase_to_EOL(void)
{
	term_pos_t i = 0;
	for (i = term_info.x; i < term_info.width; i++) {
		term_set_text_at(i, term_info.y, 0);
		term_set_attr_at(i, term_info.y, TRM_ATTRIB_DEFAULT);
	}
	term_draw_screen(term_info.x, term_info.y,
			 term_info.width - term_info.x + 1, 1);
}

void term_erase_to_BOL(void)
{
	term_pos_t i = 0;
	for (i = 0; i < term_info.x; i++) {
		term_set_text_at(i, term_info.y, 0);
		term_set_attr_at(i, term_info.y, TRM_ATTRIB_DEFAULT);
	}
	term_draw_screen(term_info.x, term_info.y,
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
	term_draw_screen(term_info.x, term_info.y,
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

	index_Y = (term_info.screen + term_info.y) % term_info.depth;
	y = term_slot_lines(term_info.view, index_Y);
	term_set_cursor_dot(term_info.x * term_info.cxChar,
			    y * term_info.cyChar);
	term_draw_cursor(true);
}

void term_cursor_off(void)
{
	term_draw_cursor(false);
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
	term_init_screen_tab();
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

void term_screen_set_mode(void)
{
	if (term_info.esc_parms[0] == -2 && term_info.esc_parms[1] == 1)
		term_info.decckm = 1;
}

void term_screen_reset_mode(void)
{
	if (term_info.esc_parms[0] == -2 && term_info.esc_parms[1] == 1)
		term_info.decckm = 0;
}

void term_init_screen(void)
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

	/* terminal modes */
	term_info.mode = TRM_MODE_REPLACE;
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

	term_init_screen_tab();
}
