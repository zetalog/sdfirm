/*
 * cmdline-editing related codes from vivi.
 *
 * Author: Janghoon Lyu <nandy@mizi.com>
 *         Lv Zheng <lv.zheng@hxt-semitech.com>
 */
#include <target/compiler.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <target/readline.h>

#define putnstr(str, n)		printf("%.*s", (int)n, str)
#define cbeep()			putch('\a')

#define CTL_CH(c)		((c) - 'a' + 1)
#define CTL_BACKSPACE		('\b')
#define DEL			((char)255)
#define DEL7			((char)127)
#define CREAD_HIST_CHAR		('!')

char readline_buffer[READLINE_BUFFER_SIZE + 1];	/* console I/O buffer	*/
int readline_length;
int readline_eol_length;
int readline_esc_len = 0;
char readline_esc_save[8];
readline_cmd_handler readline_handler = NULL;

#define HIST_MAX		20
#define HIST_SIZE		READLINE_BUFFER_SIZE

static int hist_max;
static int hist_add_idx;
static int hist_cur = -1;
static unsigned hist_num;
static char *hist_list[HIST_MAX];
static char hist_lines[HIST_MAX][HIST_SIZE + 1];	/* Save room for NULL */
#define add_idx_minus_one()	\
	((hist_add_idx == 0) ? hist_max : hist_add_idx-1)

static void hist_init(void)
{
	int i;

	hist_max = 0;
	hist_add_idx = 0;
	hist_cur = -1;
	hist_num = 0;

	for (i = 0; i < HIST_MAX; i++) {
		hist_list[i] = hist_lines[i];
		hist_list[i][0] = '\0';
	}
}

static void cread_add_to_hist(char *line)
{
	strcpy(hist_list[hist_add_idx], line);

	if (++hist_add_idx >= HIST_MAX)
		hist_add_idx = 0;

	if (hist_add_idx > hist_max)
		hist_max = hist_add_idx;

	hist_num++;
}

static char *hist_prev(void)
{
	char *ret;
	int old_cur;

	if (hist_cur < 0)
		return NULL;

	old_cur = hist_cur;
	if (--hist_cur < 0)
		hist_cur = hist_max;

	if (hist_cur == hist_add_idx) {
		hist_cur = old_cur;
		ret = NULL;
	} else {
		ret = hist_list[hist_cur];
	}

	return ret;
}

static char *hist_next(void)
{
	char *ret;

	if (hist_cur < 0)
		return NULL;

	if (hist_cur == hist_add_idx)
		return NULL;

	if (++hist_cur > hist_max)
		hist_cur = 0;

	if (hist_cur == hist_add_idx)
		ret = "";
	else
		ret = hist_list[hist_cur];

	return ret;
}

#if 0
static void cread_print_hist_list(void)
{
	int i;
	unsigned long n;

	n = hist_num - hist_max;
	i = hist_add_idx + 1;
	while (1) {
		if (i > hist_max)
			i = 0;
		if (i == hist_add_idx)
			break;
		printf("%s\n", hist_list[i]);
		n++;
		i++;
	}
}
#endif

#define BEGINNING_OF_LINE() {				\
	while (readline_length) {			\
		putch(CTL_BACKSPACE);			\
		readline_length--;			\
	}						\
}

#define ERASE_TO_EOL() {						\
	if (readline_length < readline_eol_length) {			\
		wlen = readline_eol_length - readline_length;		\
		printf("%*s", (int)wlen, "");				\
		do {							\
			putch(CTL_BACKSPACE);				\
		} while (--readline_eol_length > readline_length);	\
	}								\
}

#define REFRESH_TO_EOL() {						\
	if (readline_length < readline_eol_length) {			\
		wlen = readline_eol_length - readline_length;		\
		putnstr(readline_buffer + readline_length, wlen);	\
		readline_length = readline_eol_length;			\
	}								\
}

static void cread_add_char(char ichar, int insert)
{
	unsigned long wlen;

	/* room ??? */
	if (insert || readline_length == readline_eol_length) {
		if (readline_eol_length > READLINE_BUFFER_SIZE - 1) {
			cbeep();
			return;
		}
		readline_eol_length++;
	}

	if (insert) {
		wlen = readline_eol_length - readline_length;
		if (wlen > 1)
			memmove(&readline_buffer[readline_length +1],
				&readline_buffer[readline_length], wlen-1);

		readline_buffer[readline_length] = ichar;
		putnstr(readline_buffer + readline_length, wlen);
		readline_length++;
		while (--wlen)
			putch(CTL_BACKSPACE);
	} else {
		/* echo the character */
		wlen = 1;
		readline_buffer[readline_length] = ichar;
		putnstr(readline_buffer + readline_length, wlen);
		readline_length++;
	}
}

static void cread_add_str(char *str, int strsize, int insert)
{
	while (strsize--) {
		cread_add_char(*str, insert);
		str++;
	}
}

static void readline_reset_buffer(void)
{
	memset(readline_buffer, 0, READLINE_BUFFER_SIZE);
	readline_length = 0;
	readline_eol_length = 0;
	readline_esc_len = 0;
}

int readline_async(void)
{
	unsigned long wlen;
	char *hline;
	char ichar;
	int insert = 1;
	readline_cmd_handler handler;

	ichar = getch();
	if ((ichar == '\n') || (ichar == '\r')) {
		putch('\r');
		putch('\n');
		goto end;
	}
	/*
	 * handle standard linux xterm esc sequences for arrow key, etc.
	 */
	if (readline_esc_len != 0) {
		enum { ESC_REJECT, ESC_SAVE, ESC_CONVERTED } act = ESC_REJECT;
			if (readline_esc_len == 1) {
			if (ichar == '[' || ichar == 'O')
				act = ESC_SAVE;
		} else if (readline_esc_len == 2) {
			switch (ichar) {
			case 'D':	/* <- key */
				ichar = CTL_CH('b');
				act = ESC_CONVERTED;
				break;	/* pass off to ^B handler */
			case 'C':	/* -> key */
				ichar = CTL_CH('f');
				act = ESC_CONVERTED;
				break;	/* pass off to ^F handler */
			case 'H':	/* Home key */
				ichar = CTL_CH('a');
				act = ESC_CONVERTED;
				break;	/* pass off to ^A handler */
			case 'F':	/* End key */
				ichar = CTL_CH('e');
				act = ESC_CONVERTED;
				break;	/* pass off to ^E handler */
			case 'A':	/* up arrow */
				ichar = CTL_CH('p');
				act = ESC_CONVERTED;
				break;	/* pass off to ^P handler */
			case 'B':	/* down arrow */
				ichar = CTL_CH('n');
				act = ESC_CONVERTED;
				break;	/* pass off to ^N handler */
			case '1':
			case '3':
			case '4':
			case '7':
			case '8':
				if (readline_esc_save[1] == '[') {
					/* see if next character is ~ */
					act = ESC_SAVE;
				}
				break;
			}
		} else if (readline_esc_len == 3) {
			if (ichar == '~') {
				switch (readline_esc_save[2]) {
				case '3':	/* Delete key */
					ichar = CTL_CH('d');
					act = ESC_CONVERTED;
					break;	/* pass to ^D handler */
				case '1':	/* Home key */
				case '7':
					ichar = CTL_CH('a');
					act = ESC_CONVERTED;
					break;	/* pass to ^A handler */
				case '4':	/* End key */
				case '8':
					ichar = CTL_CH('e');
					act = ESC_CONVERTED;
					break;	/* pass to ^E handler */
				}
			}
		}
		switch (act) {
		case ESC_SAVE:
			readline_esc_save[readline_esc_len++] = ichar;
			return -EINTR;
		case ESC_REJECT:
			readline_esc_save[readline_esc_len++] = ichar;
			cread_add_str(readline_esc_save, readline_esc_len, insert);
			readline_esc_len = 0;
			return -EINTR;
		case ESC_CONVERTED:
			readline_esc_len = 0;
			break;
		}
	}
	switch (ichar) {
	case 0x1b:
		if (readline_esc_len == 0) {
			readline_esc_save[readline_esc_len] = ichar;
			readline_esc_len = 1;
		} else {
			puts("impossible condition #876\n");
			readline_esc_len = 0;
		}
		break;
	case CTL_CH('a'):
		BEGINNING_OF_LINE();
		break;
	case CTL_CH('c'):	/* ^C - break */
		*readline_buffer = '\0';	/* discard input */
		return -1;
	case CTL_CH('f'):
		if (readline_length < readline_eol_length) {
			putch(readline_buffer[readline_length]);
			readline_length++;
		}
		break;
	case CTL_CH('b'):
		if (readline_length) {
			putch(CTL_BACKSPACE);
			readline_length--;
		}
		break;
	case CTL_CH('d'):
		if (readline_length < readline_eol_length) {
			wlen = readline_eol_length - readline_length - 1;
			if (wlen) {
				memmove(&readline_buffer[readline_length],
					&readline_buffer[readline_length +1], wlen);
				putnstr(readline_buffer + readline_length, wlen);
			}
			putch(' ');
			do {
				putch(CTL_BACKSPACE);
			} while (wlen--);
			readline_eol_length--;
		}
		break;
	case CTL_CH('k'):
		ERASE_TO_EOL();
		break;
	case CTL_CH('e'):
		REFRESH_TO_EOL();
		break;
	case CTL_CH('o'):
		insert = !insert;
		break;
	case CTL_CH('x'):
	case CTL_CH('u'):
		BEGINNING_OF_LINE();
		ERASE_TO_EOL();
		break;
	case DEL:
	case DEL7:
	case 8:
		if (readline_length) {
			wlen = readline_eol_length - readline_length;
			readline_length--;
			memmove(&readline_buffer[readline_length],
				&readline_buffer[readline_length +1], wlen);
			putch(CTL_BACKSPACE);
			putnstr(readline_buffer + readline_length, wlen);
			putch(' ');
			do {
				putch(CTL_BACKSPACE);
			} while (wlen--);
			readline_eol_length--;
		}
		break;
	case CTL_CH('p'):
	case CTL_CH('n'):
		readline_esc_len = 0;
		if (ichar == CTL_CH('p'))
			hline = hist_prev();
		else
			hline = hist_next();
		if (!hline) {
			cbeep();
			return -EINTR;
		}
		/* nuke the current line */
		/* first, go home */
		BEGINNING_OF_LINE();
		/* erase to end of line */
		ERASE_TO_EOL();
		/* copy new line into place and display */
		strcpy(readline_buffer, hline);
		readline_eol_length = strlen(readline_buffer);
		REFRESH_TO_EOL();
		return -EINTR;
	default:
		cread_add_char(ichar, insert);
		break;
	}
	return -EINTR;

end:
	readline_buffer[readline_eol_length] = '\0';	/* lose the newline */
	if (readline_buffer[0] && readline_buffer[0] != CREAD_HIST_CHAR)
		cread_add_to_hist(readline_buffer);
	hist_cur = hist_add_idx;
	if (readline_handler) {
		handler = readline_handler;
		if (handler) {
			/* TODO: oneshot handler */
			/* readline_handler = NULL; */
			handler(readline_buffer, readline_eol_length);
			readline_reset_buffer();
		}
	}
	return readline_eol_length;
}

int readline(void)
{
	int ret;

	while (1) {
		ret = readline_async();
		if (ret != -EINTR)
			break;
	}
	return ret;
}

int readline_register_handler(readline_cmd_handler handler)
{
	if (readline_handler)
		return -EBUSY;
	readline_handler = handler;
	return 0;
}

int readline_init(void)
{
	readline_reset_buffer();
	hist_init();
	return 0;
}
