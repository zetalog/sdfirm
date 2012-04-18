#ifndef __TERM_H_INCLUDE__
#define __TERM_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>
#include <target/io.h>
#include <target/bulk.h>

typedef uint8_t term_pos_t;
typedef uint8_t term_len_t;

#define NR_TERM_CONS	CONFIG_TERM_MAX_CONS
typedef uint8_t term_t;

typedef uint32_t video_rgb_t;
#define RGB(r,g,b)	((video_rgb_t)(((uint8_t)(r)|((uint16_t)((uint8_t)(g))<<8))|(((uint16_t)(uint8_t)(b))<<16)))
#define RGB_R(rgb)	((uint8_t)(rgb))
#define RGB_G(rgb)	((uint8_t)(((uint16_t)(rgb)) >> 8))
#define RGB_B(rgb)	((uint8_t)((rgb)>>16))

struct video_rbg_triple {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
};

#ifdef CONFIG_TERM_WIDTH
#define TRM_DEFAULT_WIDTH	CONFIG_TERM_WIDTH
#else
#define TRM_DEFAULT_WIDTH	80
#endif
#ifdef CONFIG_TERM_HEIGHT
#define TRM_DEFAULT_HEIGHT	CONFIG_TERM_HEIGHT
#else
#define TRM_DEFAULT_HEIGHT	24
#endif
#ifdef CONFIG_TERM_DEPTH
#define TRM_DEFAULT_DEPTH	CONFIG_TERM_DEPTH
#else
#define TRM_DEFAULT_DEPTH	1*TRM_DEFAULT_HEIGHT
#endif

/*======================================================================*
 * Attributes will be encoded into four bytes:
 *
 *   MSB                                                           LSB
 *   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *   |            Byte 3             |            Byte 2             |
 *   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *   |            Byte 1             |            Byte 0             |
 *   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *
 * The meaning of fields used in the attributes are as follows:
 *   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *   |            Reserved           | N | W | L | R | U | B |      ->
 *   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *   <-Background Colour         |         Foreground Colour         |
 *   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *
 * Where:
 *  N:  Narrow character
 *  W:  Wide character
 *  L:  Blink
 *  R:  Reverse colour
 *  U:  Under line
 *  B:  ANSI Bold colour
 *======================================================================*/
#define TRM_ATTRIB_NARROW	0x00800000U
#define TRM_ATTRIB_WIDE		0x00400000U
#define TRM_ATTRIB_BLINK	0x00200000U
#define TRM_ATTRIB_REVERSE	0x00100000U
#define TRM_ATTRIB_UNDER	0x00080000U
#define TRM_ATTRIB_BOLD		0x00040000U
#define TRM_ATTRIB_BGMASK	0x0003FE00U
#define TRM_ATTRIB_FGMASK	0x000001FFU
#define TRM_ATTRIB_COLOURS	0x0003FFFFU

#define TRM_ATTRIB_FGSHIFT	0
#define TRM_ATTRIB_BGSHIFT	9

#define TRM_ATTRIB_FGCOLOUR(attrib)	(attrib & TRM_ATTRIB_FGMASK) >> TRM_ATTRIB_FGSHIFT
#define TRM_ATTRIB_BGCOLOUR(attrib)	(attrib & TRM_ATTRIB_BGMASK) >> TRM_ATTRIB_BGSHIFT

#define TRM_ATTRIB_ISBOLD(attrib)	(attrib & TRM_ATTRIB_BOLD)
#define TRM_ATTRIB_ISUNDER(attrib)	(attrib & TRM_ATTRIB_UNDER)
#define TRM_ATTRIB_ISBLINK(attrib)	(attrib & TRM_ATTRIB_BLINK)
#define TRM_ATTRIB_ISREVERSE(attrib)	(attrib & TRM_ATTRIB_REVERSE)
#define TRM_ATTRIB_SETREVERSE(attrib)	(attrib ^= TRM_ATTRIB_REVERSE)

/*======================================================================*
 * The definitive list of colour numbers stored in terminal
 * attribute words is kept here. It is:
 * 
 *  0-7:    ANSI colours (KRGYBMCW).
 *  8-15:   bold versions of ANSI colours.
 *  16-255: the remains of the xterm 256-colour mode (a and 216-colour
 *          cube with R at most significant B at least, followed by a
 *          uniform series of grey shades running between black and
 *          white but not including either on grounds of redundancy).
 *  256:    default foreground
 *  257:    default bold foreground
 *  258:    default background
 *  259:    default bold background
 *  260:    cursor foreground
 *  261:    cursor background
 *======================================================================*/
#define TRM_ATTRIB_DEFFG	(256 << TRM_ATTRIB_FGSHIFT)
#define TRM_ATTRIB_DEFBG	(258 << TRM_ATTRIB_BGSHIFT)
#define TRM_ATTRIB_DEFAULT	(TRM_ATTRIB_DEFFG | TRM_ATTRIB_DEFBG)
#define TRM_NUMBER_FGCOLOURS	22
#define TRM_NUMBER_NEXTCOLOURS	240
#define TRM_NUMBER_ALLCOLOURS	(TRM_NUMBER_FGCOLOURS + TRM_NUMBER_NEXTCOLOURS)

/*======================================================================*
 * ANSI colour definitions
 *======================================================================*/
#define TRM_DEFAULT_FOREGROUND			RGB(187, 187, 187)
#define TRM_DEFAULT_BOLD_FOREGROUND		RGB(255, 255, 255)
#define TRM_DEFAULT_BACKGROUND			RGB(0, 0, 0)
#define TRM_DEFAULT_BOLD_BACKGROUND		RGB(85, 85, 85)
#define TRM_CURSOR_TEXT				RGB(0, 0, 0)
#define TRM_CURSOR_COLOUR			RGB(0, 255, 0)
#define TRM_ANSI_X364_BLACK			RGB(0, 0, 0)
#define TRM_ANSI_X364_BOLD_BLACK		RGB(85, 85, 85)
#define TRM_ANSI_X364_RED			RGB(187, 0, 0)
#define TRM_ANSI_X364_BOLD_RED			RGB(255, 85, 85)
#define TRM_ANSI_X364_GREEN			RGB(0, 187, 0)
#define TRM_ANSI_X364_BOLD_GREEN		RGB(85, 255, 85)
#define TRM_ANSI_X364_YELLOW			RGB(187, 187, 0)
#define TRM_ANSI_X364_BOLD_YELLOW		RGB(255, 255, 85)
#define TRM_ANSI_X364_BLUE			RGB(0, 0, 187)
#define TRM_ANSI_X364_BOLD_BLUE			RGB(85, 85, 255)
#define TRM_ANSI_X364_MAGENTA			RGB(187, 0, 187)
#define TRM_ANSI_X364_BOLD_MAGENTA		RGB(255, 85, 255)
#define TRM_ANSI_X364_CYAN			RGB(0, 187, 187)
#define TRM_ANSI_X364_BOLD_CYAN			RGB(85, 255, 255)
#define TRM_ANSI_X364_WHITE			RGB(187, 187, 187)
#define TRM_ANSI_X364_BOLD_WHITE		RGB(255, 255, 255)

#define TRM_MODE_INSERT		0x00000001U
#define TRM_MODE_REPLACE	0x00000002U

#define TRM_TAB_SPACE                   8

#define TRM_OUTPUT_SIZE		\
	(TRM_DEFAULT_DEPTH * TRM_DEFAULT_WIDTH * sizeof (char))
#define TRM_INPUT_SIZE		\
	(TRM_DEFAULT_WIDTH * sizeof (char))
#define TRM_ATTRIB_SIZE		\
	(TRM_DEFAULT_DEPTH * TRM_DEFAULT_WIDTH * sizeof (long))

struct terminal {
	/* terminal screen / buffer stuff */
	char output[TRM_OUTPUT_SIZE];
	long attrib[TRM_ATTRIB_SIZE];
	term_pos_t top, bottom;		/* circular buffer cursors */
	term_pos_t depth;		/* circular buffer boundary */
	term_pos_t screen;		/* terminal screen cursor */
	term_pos_t offset;		/* current text's offset in the row */
	term_len_t width, height;	/* terminal screen boundary */
	term_pos_t view;		/* view port cursor */
	term_len_t rows, columns;	/* view port boundary */
	term_pos_t margin_top, margin_bottom;

	/* input buffer for send schedular */
	char input[TRM_INPUT_SIZE];
	term_len_t size;
	term_pos_t head, tail;		/* circular buffer cursors */

	/* cursor stuff */
	term_pos_t x, y;		/* cursor position on screen */
	term_pos_t Px, Py;
	long def_attrib, cur_attrib;
	long Pattrib;

	/* edit stuff */
	int mode;			/* insert / replace mode */
	uint8_t blinking : 1;		/* controlled by timer */
	uint8_t alert : 1;
	uint8_t decawm : 1;		/* AutoWrap mode 0=off */
	uint8_t wrap_pending : 1;	/* AutoWrap mode is on - wrap on next character */
	uint8_t decckm : 1;		/* Cursor key mode */
	uint8_t decpam : 1;		/* keyPad Application mode */

#if 0
	uint8_t resizeable : 1;
#endif
	int backspace;
	int ctrl_backspace;
	char tabs[TRM_DEFAULT_WIDTH];

	/* escape sequence stuff */
	int esc_level;			/* current escape level */      
	int esc_nargs;
	int esc_parms[6];		/* ANSI params */

	video_rgb_t palette[TRM_NUMBER_ALLCOLOURS];
	struct video_rbg_triple rgbt_def[TRM_NUMBER_ALLCOLOURS];
};

struct term_driver {
	void (*init)(void);
	void (*exit)(void);
	void (*set_origin)(void);
	void (*clear)(term_pos_t sx, term_pos_t sy,
		      term_len_t height, term_len_t width);
	void (*putc)(uint8_t c, term_pos_t ypos, term_pos_t xpos);
	void (*cursor)(uint8_t mode /* CM_XXX */);
	void (*scroll)(int t, int b,
		       uint8_t dir /* SM_XXX */, size_t count);
	void (*scrolldelta)(term_pos_t delta_lines);
	uint16_t *(*screen_pos)(loff_t offset);
#if 0
	void (*resize)(term_len_t height, term_len_t width);
	void (*select)(term_t);
	int (*blank)(int, int);
	int (*set_palette)(unsigned char *);
	void (*puts)(text_char_t *, size_t len,
		     term_pos_t ypos, term_pos_t xpos);
	int (*font_set)(struct console_font *, unsigned);
	int (*font_get)(struct console_font *);
	unsigned long (*getxy)(unsigned long, int *, int *);
#endif
};
__TEXT_TYPE__(struct term_driver, term_driver_t);

term_len_t term_buffer_lines(void);
term_len_t term_screen_lines(void);

size_t term_get_index_by_Y(term_pos_t y);
size_t term_get_index_by_XY(term_pos_t x, term_pos_t y);
char term_get_text_at(term_pos_t x, term_pos_t y);
void term_set_text_at(term_pos_t x, term_pos_t y, char output);
long term_get_attr_at(term_pos_t x, term_pos_t y);
void term_set_attr_at(term_pos_t x, term_pos_t y, long attrib);

void term_scroll_up(term_len_t lines);
void term_scroll_down(term_len_t lines);

void term_screen_set_mode(void);
void term_screen_reset_mode(void);

void term_get_new_buffer_line(void);
void term_del_top_buffer_line(void);

void term_erase_to_pos(void);
void term_erase_to_EOL(void);
void term_erase_to_EOS_line(term_pos_t y);
void term_erase_to_EOS(void);
void term_erase_line(term_pos_t line);
void term_erase_screen(void);

void term_screen_ap_clear(void);
void term_cursor_on(void);
void term_cursor_off(void);
void term_cursor_restore(void);
void term_cursor_save(void);
void term_cursor_up(term_len_t len);
void term_cursor_down(term_len_t len);
void term_cursor_right(term_len_t len);
void term_cursor_left(term_len_t len);
void term_cursor_pos(term_pos_t y, term_pos_t x);

void term_blink_enable(void);
void term_blink_disable(void);

void term_screen_reset(void);
void term_screen_recalc(void);

#endif /* __TERM_H_INCLUDE__ */
