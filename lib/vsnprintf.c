#include <target/types.h>
#include <string.h>
#include <stdarg.h>

#define SIGN			0x01
#define SIGN_PLUS		0x02
#define SIGN_PLUS_SPACE		0x04
#define ZEROPAD			0x08
#define LEFT			0x10
#define LARGE			0x20
#define PREFIX			0x40
#define FLOAT			0x80

static size_t bound_string_length(const char *string, size_t count);
static char *bound_string_output(char *string, const char *end, char c);

static char *format_number(char *string, char *end, uint64_t number,
			   uint8_t base, int32_t width,
			   int32_t precision, uint8_t type);
static char *put_number(char *string, uint64_t number,
			uint8_t base, bool upper);
static const char *scan_number(const char *string, uint64_t *number_ptr);

static size_t bound_string_length(const char *string, size_t count)
{
	uint32_t length = 0;

	while (*string && count) {
		length++;
		string++;
		count--;
	}

	return length;
}

static char *bound_string_output(char *string, const char *end, char c)
{

	if (string < end)
		*string = c;

	++string;
	return string;
}

static char *put_number(char *string, uint64_t number,
			uint8_t base, bool upper)
{
	const char *digits;
	const char *lower_hex_digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	const char *upper_hex_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	uint64_t digit_index;
	char *pos;

	pos = string;
	digits = upper ? upper_hex_digits : lower_hex_digits;

	if (number == 0) {
		*(pos++) = '0';
	} else {
		while (number) {
			digit_index = number % base;
			number /= base;
			*(pos++) = digits[digit_index];
		}
	}

	/* *(Pos++) = '0'; */
	return pos;
}

static const char *scan_number(const char *string, uint64_t *number_ptr)
{
	uint64_t number = 0;

	while (isdigit((int)*string)) {
		number *= 10;
		number += *(string++) - '0';
	}

	*number_ptr = number;
	return string;
}

static char *format_number(char *string, char *end, uint64_t number,
			   uint8_t base, int32_t width,
			   int32_t precision, uint8_t type)
{
	char *pos;
	char sign;
	char zero;
	bool need_prefix;
	bool upper;
	int32_t i;
	char reversed_string[66];

	if (base < 2 || base > 16)
		return (NULL);

	if (type & LEFT)
		type &= ~ZEROPAD;

	need_prefix = ((type & PREFIX) && base != 10) ? true : false;
	upper = (type & LARGE) ? true : false;
	zero = (type & ZEROPAD) ? '0' : ' ';

	/* Calculate size according to sign and prefix */

	sign = '\0';
	if (type & SIGN) {
		if ((int64_t)number < 0) {
			sign = '-';
			number = -(int64_t)number;
			width--;
		} else if (type & SIGN_PLUS) {
			sign = '+';
			width--;
		} else if (type & SIGN_PLUS_SPACE) {
			sign = ' ';
			width--;
		}
	}
	if (need_prefix) {
		width--;
		if (base == 16)
			width--;
	}

	pos = put_number(reversed_string, number, base, upper);
	i = (int32_t)(pos - reversed_string);

	/* Printing 100 using %2d gives "100", not "00" */

	if (i > precision)
		precision = i;

	width -= precision;

	if (!(type & (ZEROPAD | LEFT))) {
		while (--width >= 0)
			string = bound_string_output(string, end, ' ');
	}
	if (sign)
		string = bound_string_output(string, end, sign);
	if (need_prefix) {
		string = bound_string_output(string, end, '0');
		if (base == 16)
			string = bound_string_output(string, end, upper ? 'X' : 'x');
	}
	if (!(type & LEFT)) {
		while (--width >= 0)
			string = bound_string_output(string, end, zero);
	}

	while (i <= --precision)
		string = bound_string_output(string, end, '0');
	while (--i >= 0)
		string = bound_string_output(string, end, reversed_string[i]);
	while (--width >= 0)
		string = bound_string_output(string, end, ' ');

	return string;
}

#ifdef CONFIG_STDLIB_FP
#include "fpmax.h"

#if FPMAX_MAX_10_EXP < -FPMAX_MIN_10_EXP
#error scaling code can not handle FPMAX_MAX_10_EXP < -FPMAX_MIN_10_EXP
#endif

static const float_max_t exp10_table[] = {
	1e1L, 1e2L, 1e4L, 1e8L, 1e16L, 1e32L,	/* floats */
#if FPMAX_MAX_10_EXP < 32
#error unsupported FPMAX_MAX_10_EXP (< 32).  ANSI/ISO C requires >= 37.
#endif
#if FPMAX_MAX_10_EXP >= 64
	1e64L,
#endif
#if FPMAX_MAX_10_EXP >= 128
	1e128L,
#endif
#if FPMAX_MAX_10_EXP >= 256
	1e256L,
#endif
#if FPMAX_MAX_10_EXP >= 512
	1e512L,
#endif
#if FPMAX_MAX_10_EXP >= 1024
	1e1024L,
#endif
#if FPMAX_MAX_10_EXP >= 2048
	1e2048L,
#endif
#if FPMAX_MAX_10_EXP >= 4096
	1e4096L
#endif
#if FPMAX_MAX_10_EXP >= 8192
#error unsupported FPMAX_MAX_10_EXP.  please increase table
#endif
};

#define EXP10_TABLE_SIZE	(sizeof (exp10_table) / sizeof (exp10_table[0]))
#define EXP10_TABLE_MAX		(1U << (EXP10_TABLE_SIZE - 1))

static const char fmt[] = "inf\0INF\0nan\0NAN\0.\0,";

#define INF_OFFSET		0	/* Must be 1st */
#define NAN_OFFSET		8	/* Must be 2nd due to hex sign handling */
#define DECPT_OFFSET		16
#define THOUSEP_OFFSET		18
#define EMPTY_STRING_OFFSET	3

/* With 32 bit ints, we can get 9 decimal digits per block. */
#define DIGITS_PER_BLOCK     9

/* Maximum number of subcases to output double is...
 *  0 - sign
 *  1 - padding and initial digit
 *  2 - digits left of the radix
 *  3 - 0s left of the radix        or   radix
 *  4 - radix                       or   digits right of the radix
 *  5 - 0s right of the radix
 *  6 - exponent
 *  7 - trailing space padding
 * although not all cases may occur.
 */
#define MAX_CALLS		8

#define NUM_DIGIT_BLOCKS	((DECIMAL_DIG+DIGITS_PER_BLOCK-1)/DIGITS_PER_BLOCK)
/* extra space for '-', '.', 'e+###', and nul */
#define BUF_SIZE		(3 + NUM_DIGIT_BLOCKS * DIGITS_PER_BLOCK)

#define FPO_ZERO_PAD    (0x80 | '0')
#define FPO_STR_WIDTH   (0x80 | ' ');
#define FPO_STR_PREC    'p'

static char *output_buffer(char *pos, const char *end,
			   caddr_t type, int len, const char *buf)
{
	int buflen;

	if (type & 0x80) {
		/* Some type of padding needed. */
		buflen = strlen(buf);
		len -= buflen;
		while (--len >= 0)
			pos = bound_string_output(pos, end, (type & 0x7f));
		len = buflen;
	}
	while (--len >= 0) {
		pos = bound_string_output(pos, end, *buf);
		buf++;
	}
	return pos;
}

static char *format_float(char *pos, const char *end,
			  float_max_t x, char mode,
			  int32_t width, int32_t preci, uint8_t type)
{
	int round, o_exp;
	int exp;
	char *s;
	char *e;
	caddr_t pc_fwi[3 * MAX_CALLS];
	caddr_t *ppc;
	caddr_t *ppc_last;
	char exp_buf[16];
	char buf[BUF_SIZE];
	char sign_str[6];	/* Last 2 are for 1st digit + nul. */
	char o_mode;
	int exp_neg;
	int i, j;
	uint8_t base = 10;
	char zero;

	/* Initializations */
	zero = (type & ZEROPAD) ? '0' : ' ';
	*exp_buf = 'e';
	if ((mode | 0x20) == 'a')
		mode += ('g' - 'a');
	if (preci < 0)
		preci = 6;
	*sign_str = '\0';
	if (type & SIGN_PLUS)
		*sign_str = '+';
	else if (type & SIGN_PLUS_SPACE)
		*sign_str = ' ';
	*(sign_str + 1) = 0;
	pc_fwi[5] = INF_OFFSET;
	if (isnan(x)) {
		pc_fwi[5] = NAN_OFFSET;
		goto INF_NAN;
	}
	if (x == 0) {
		if (isnegz(x))
			*sign_str = '-';
		exp = -1;
		goto GENERATE_DIGITS;
	}
	if (x < 0) {
		/* Convert negatives to positives. */
		*sign_str = '-';
		x = -x;
	}
	/* Zero of infinite? Infinite since zero handled above. */
	if (iszoinf(x)) {
INF_NAN:
		zero = ' ';
		ppc = pc_fwi + 6;
		pc_fwi[3] = FPO_STR_PREC;
		pc_fwi[4] = 3;
		if (mode < 'a')
			pc_fwi[5] += 4;
		pc_fwi[5] = (caddr_t)(fmt + pc_fwi[5]);
		goto EXIT_SPECIAL;
	}

#define lower_bnd	1e8
#define upper_bnd	1e9
#define power_table	exp10_table
#define dpb		DIGITS_PER_BLOCK
#define ndb		NUM_DIGIT_BLOCKS
#define nd		DECIMAL_DIG

	exp = DIGITS_PER_BLOCK - 1;
	i = EXP10_TABLE_SIZE;
	j = EXP10_TABLE_MAX;

	exp_neg = 0;
	/* Do we need to scale up or down? */
	if (x < lower_bnd)
		exp_neg = 1;
	do {
		--i;
		if (exp_neg) {
			if (x * power_table[i] < upper_bnd) {
				x *= power_table[i];
				exp -= j;
			}
		} else {
			if (x / power_table[i] >= lower_bnd) {
				x /= power_table[i];
				exp += j;
			}
		}
		j >>= 1;
	} while (i);

	if (x >= upper_bnd) {		/* Handle bad rounding case. */
		x /= power_table[0];
		++exp;
	}
	/* assert(x < upper_bnd); */

GENERATE_DIGITS:
	s = buf + 2;			/* Leave space for '\0' and '0'. */
	i = 0;
	do {
		uint32_t digit_block = (uint32_t)x;

		/* assert(digit_block < upper_bnd); */
		x = (x - digit_block) * upper_bnd;
		s += dpb;
		j = 0;
		do {
			s[-++j] = '0' + (digit_block % base);
			digit_block /= base;
		} while (j < dpb);
	} while (++i < ndb);

	if (mode < 'a') {
		*exp_buf -= ('a' - 'A'); /* e->E and p->P */
		mode += ('a' - 'A');
	}

	o_mode = mode;
	if ((mode == 'g') && (preci > 0))
		--preci;
	round = preci;

	if (mode == 'f') {
		round += exp;
		if (round < -1) {
			memset(buf, '0', DECIMAL_DIG); /* OK, since 'f' -> decimal case. */
			exp = -1;
			round = -1;
		}
	}

	s = buf;
	*s++ = 0;
	*s = '0';

	i = 0;
	e = s + nd + 1;
	if (round < nd) {
		e = s + round + 2;
		if (*e >= '0' + (base / 2))
			i = 1;
	}
	do {
	   /* Handle rounding and trim trailing 0s. */
		*--e += i;			/* Add the carry. */
	} while ((*e == '0') || (*e > '0' - 1 + base));

	o_exp = exp;
	if (e <= s) {
		/* We carried into an extra digit. */
		++o_exp;
		e = s;
	} else
		++s;
	*++e = 0;

	if ((mode == 'g') &&
	    ((o_exp >= -4) && (o_exp <= round))) {
		mode = 'f';
		preci = round - o_exp;
	}
	exp = o_exp;
	if (mode != 'f')
		o_exp = 0;
	if (o_exp < 0) {
		/* Exponent is < 0, so fake the first 0 digit. */
		*--s = '0';
	}

	pc_fwi[3] = FPO_ZERO_PAD;
	pc_fwi[4] = 1;
	pc_fwi[5] = (caddr_t)(sign_str + 4);
	sign_str[4] = *s++;
	sign_str[5] = 0;
	ppc = pc_fwi + 6;

	i = e - s;			/* Total digits is 'i'. */
	if (o_exp >= 0) {
		ppc[0] = FPO_STR_PREC;
		ppc[2] = (caddr_t)(s);
		if (o_exp >= i) {
			/* all digit(s) left of decimal */
			ppc[1] = i;
			ppc += 3;
			o_exp -= i;
			i = 0;
			if (o_exp > 0) {
				ppc[0] = FPO_ZERO_PAD;
				ppc[1] = o_exp;
				ppc[2] = (caddr_t)(fmt + EMPTY_STRING_OFFSET);
				ppc += 3;
			}
		} else if (o_exp > 0) {
			/* decimal between digits */
			ppc[1] = o_exp;
			ppc += 3;
			s += o_exp;
			i -= o_exp;
		}
		o_exp = -1;
	}
	if ((type & PREFIX) || (i) ||
	    ((o_mode != 'g') && (preci > 0))) {
		ppc[0] = FPO_STR_PREC;
		ppc[1] = 1;
		ppc[2] = (caddr_t)(fmt + DECPT_OFFSET);
		ppc += 3;
	}
	if (++o_exp < 0) {
		/* Have 0s right of decimal. */
		ppc[0] = FPO_ZERO_PAD;
		ppc[1] = -o_exp;
		ppc[2] = (caddr_t)(fmt + EMPTY_STRING_OFFSET);
		ppc += 3;
	}
	if (i) {
		/* Have digit(s) right of decimal. */
		ppc[0] = FPO_STR_PREC;
		ppc[1] = i;
		ppc[2] = (caddr_t)(s);
		ppc += 3;
	}
	if (((o_mode != 'g') || (type & PREFIX))) {
		i -= o_exp;
		if (i < preci) {
			/* Have 0s right of digits. */
			i = preci - i;
			ppc[0] = FPO_ZERO_PAD;
			ppc[1] = i;
			ppc[2] = (caddr_t)(fmt + EMPTY_STRING_OFFSET);
			ppc += 3;
		}
	}

	/* Build exponent string. */
	if (mode != 'f') {
		char *p = exp_buf + sizeof(exp_buf);
		int j;
		char exp_char = *exp_buf;
		char exp_sign = '+';
#define min_exp_dig_plus_2  (2+2)

		if (exp < 0) {
			exp_sign = '-';
			exp = -exp;
		}

		*--p = 0;
		/* Count exp_char and exp_sign. */
		j = 2;
		do {
			*--p = '0' + (exp % 10);
			exp /= 10;
		} while ((++j < min_exp_dig_plus_2) || exp); /* char+sign+mindigits */
		*--p = exp_sign;
		*--p = exp_char;

		ppc[0] = FPO_STR_PREC;
		ppc[1] = j;
		ppc[2] = (caddr_t)(p);
		ppc += 3;
	}

EXIT_SPECIAL:
	{
		int i;
		ppc_last = ppc;
		/* Need width fields starting with second. */
		ppc = pc_fwi + 4;
		do {
			width -= *ppc;
			ppc += 3;
		} while (ppc < ppc_last);

		ppc = pc_fwi;
		ppc[0] = FPO_STR_WIDTH;
		ppc[1] = i = ((*sign_str) != 0);
		ppc[2] = (caddr_t)sign_str;

		if ((width -= i) > 0) {
			if (type & LEFT) {
				/* Left-justified. */
				ppc_last[0] = FPO_STR_WIDTH;
				ppc_last[1] = width;
				ppc_last[2] = (caddr_t)(fmt + EMPTY_STRING_OFFSET);
				ppc_last += 3;
			} else if (zero == '0')
				ppc[4] += width;
			else
				ppc[1] += width;
		}
	}

	do {
		pos = output_buffer(pos, end, *ppc,
				    (int)ppc[1], (char *)ppc[2]);
		ppc += 3;
	} while (ppc < ppc_last);
	return pos;
}
#endif /* CONFIG_STDLIB_FP */

int vsnprintf(char *string, size_t size, const char *format, va_list args)
{
	uint8_t base;
	uint8_t type;
	int32_t width;
	int32_t precision;
	char qualifier;
	uint64_t number;
	char *pos;
	char *end;
	char c;
	const char *s;
	const void *p;
	int32_t length;
	int i;
#ifdef CONFIG_STDLIB_FP
	float_max_t f;
#endif

	pos = string;
	end = string + size;

	for (; *format; ++format) {
		if (*format != '%') {
			pos = bound_string_output(pos, end, *format);
			continue;
		}

		type = 0;
		base = 10;

		/* Process sign */

		do {
			++format;
			if (*format == '#')
				type |= PREFIX;
			else if (*format == '0')
				type |= ZEROPAD;
			else if (*format == '+')
				type |= SIGN_PLUS;
			else if (*format == ' ')
				type |= SIGN_PLUS_SPACE;
			else if (*format == '-')
				type |= LEFT;
			else
				break;
		} while (1);

		/* Process width */

		width = -1;
		if (isdigit((int)*format)) {
			format = scan_number(format, &number);
			width = (int32_t)number;
		} else if (*format == '*') {
			++format;
			width = va_arg(args, int);
			if (width < 0) {
				width = -width;
				type |= LEFT;
			}
		}

		/* Process precision */

		precision = -1;
		if (*format == '.') {
			++format;
			if (isdigit((int)*format)) {
				format = scan_number(format, &number);
				precision = (int32_t)number;
			} else if (*format == '*') {
				++format;
				precision = va_arg(args, int);
			}

			if (precision < 0)
				precision = 0;
		}

		/* Process qualifier */

		qualifier = -1;
		if (*format == 'h' || *format == 'l' || *format == 'L') {
			qualifier = *format;
			++format;

			if (qualifier == 'l' && *format == 'l') {
				qualifier = 'L';
				++format;
			}
		}

		switch (*format) {
		case '%':
			pos = bound_string_output(pos, end, '%');
			continue;

		case 'c':
			if (!(type & LEFT)) {
				while (--width > 0)
					pos = bound_string_output(pos, end, ' ');
			}
			c = (char)va_arg(args, int);
			pos = bound_string_output(pos, end, c);

			while (--width > 0)
				pos = bound_string_output(pos, end, ' ');
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";
			length = (int32_t)bound_string_length(s, precision);
			if (!(type & LEFT)) {
				while (length < width--)
					pos = bound_string_output(pos, end, ' ');
			}
			for (i = 0; i < length; ++i) {
				pos = bound_string_output(pos, end, *s);
				++s;
			}
			while (length < width--)
				pos = bound_string_output(pos, end, ' ');
			continue;

		case 'o':
			base = 8;
			break;

		case 'X':
			type |= LARGE;

		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			type |= SIGN;

		case 'u':
			break;

#ifdef CONFIG_STDLIB_FP
		case 'f':
		case 'e':
		case 'g':
		case 'E':
		case 'G':
			type |= FLOAT;
			break;
#endif

		case 'p':
			if (width == -1) {
				width = 2 * sizeof(void *);
				type |= ZEROPAD;
			}

			p = va_arg(args, void *);
			pos = format_number(pos, end, (caddr_t)p,
					    16, width, precision, type);
			continue;

		default:
			pos = bound_string_output(pos, end, '%');
			if (*format)
				pos = bound_string_output(pos, end, *format);
			else
				--format;
			continue;
		}

		if (qualifier == 'L') {
#ifdef CONFIG_STDLIB_FP
			if (type & FLOAT)
				f = (float_max_t)va_arg(args, double);
			else
#endif
			{
				number = va_arg(args, uint64_t);
				if (type & SIGN)
					number = (int64_t)number;
			}
		} else if (qualifier == 'l') {
#ifdef CONFIG_STDLIB_FP
			if (type & FLOAT)
				f = (float_max_t)va_arg(args, double);
			else
#endif
			{
				number = va_arg(args, unsigned long);
				if (type & SIGN)
					number = (int32_t)number;
			}
		} else if (qualifier == 'h') {
#ifdef CONFIG_STDLIB_FP
			if (type & FLOAT)
				f = (float_max_t)va_arg(args, double);
			else
#endif
			{
				number = (uint16_t)va_arg(args, int);
				if (type & SIGN)
					number = (int16_t)number;
			}
		} else {
#ifdef CONFIG_STDLIB_FP
			if (type & FLOAT)
				f = (float_max_t)va_arg(args, double);
			else
#endif
			{
				number = va_arg(args, unsigned int);
				if (type & SIGN)
					number = (signed int)number;
			}
		}

#ifdef CONFIG_STDLIB_FP
		if (type & FLOAT)
			pos = format_float(pos, end, f, *format,
					   width, precision, type);
		else
#endif
			pos = format_number(pos, end, number, base,
					    width, precision, type);
	}

	if (size > 0) {
		if (pos < end)
			*pos = '\0';
		else
			end[-1] = '\0';
	}

	return (int)(pos - string);
}

#ifdef CONFIG_PRINT_VSPRINTF
int vsprintf(char *buf, const char *fmt, va_list args)
{
	return vsnprintf(buf, UINT32_MAX, fmt, args);
}
#endif

#ifdef CONFIG_PRINT_SNPRINTF
int snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return len;
}
#endif

#ifdef CONFIG_PRINT_SPRINTF
int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vsnprintf(buf, UINT32_MAX, fmt, args);
	va_end(args);

	return len;
}
#endif
