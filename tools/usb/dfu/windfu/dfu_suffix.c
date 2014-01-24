#include <windows.h>
#include <windowsx.h>
#include <io.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <host/types.h>
#include <host/bitops.h>
#include <host/usbdfu.h>

#define ftruncate(fd, size)		_chsize(fd, size)

#define DFU_CRC_SIZE		4
#define DFU_ATTRIB_SIZE		(DFU_SUFFIX_SIZE-DFU_CRC_SIZE)

/* Layout of SUFFIX (LSB):
 * +-------------------+-------------------+-----------------+-----------------+
 * |      2 Bytes      |      2 Bytes      |     2 Bytes     |     2 Bytes     |
 * +-------------------+-------------------+-----------------+-----------------+
 * |     bcdDevice     |     idProduct     |     idVendor    |      bcdDFU     |
 * +-------------------+---------+---------+-----------------+-----------------+
 * |           3 Bytes           | 1 Byte  |              4 Bytes              |
 * +-----------------------------+---------+-----------------------------------+
 * |       ucDfuSignature        | bLength |               dwCRC               |
 * +-----------------------------+---------+-----------------------------------+
 */
uint8_t _suffix[DFU_SUFFIX_SIZE];
FILE *_fp = NULL;

void __atexit__(void)
{
	if (_fp) {
		fclose(_fp);
		_fp = NULL;
	}
}

void _fatal(int err, char *_str)
{
	if (err != 0)
		errno = err;
	perror(_str);
	exit(-1);
}

void _usage(void)
{
	const char *instr =
	"\nusage: dfu path [options]\n" \
	" to dump a suffix use: dfu path\n" \
	" to remove a suffix use: dfu path -del\n" \
	" to append a suffix use: dfu path -add -did val -pid val -vid val\n" \
	"   dfu myfile -add -did 0x0102 -pid 2345 -vid 017\n" \
	"   sets idDevice 0x0102 idProduct 0x0929 idVendor 0x000F\n\n";

	fprintf(stdout, "%s", instr);
	_fatal(EINVAL, "usage");
}

#define _getarg(ident, val);					\
	if (!strcmp(argv[_i], (ident))) {			\
		char *_charp;					\
		if (argc-1 == _i) _usage();			\
		val = (uint16_t)strtol(argv[_i+1], &_charp, 0);	\
	}

void dfu_suffix_writew(uint8_t *buf, uint16_t val)
{
	buf[0] = LOBYTE(val);
	buf[1] = HIBYTE(val);
}

void dfu_suffix_writel(uint8_t *buf, uint32_t val)
{
	dfu_suffix_writew(buf, LOWORD(val));
	dfu_suffix_writew(buf+2, HIWORD(val));
}

uint16_t dfu_suffix_readw(const uint8_t *buf)
{
	return MAKEWORD(buf[0], buf[1]);
}

uint32_t dfu_suffix_readl(const uint8_t *buf)
{
	return MAKELONG(dfu_suffix_readw(buf),
			dfu_suffix_readw(buf+2));
}

void dfu_crc_init(uint32_t *crc)
{
	*crc = 0xffffffff;
}

#if 1
#include <host/crc32_table.h>
void dfu_crc_update(uint32_t *crc, const uint8_t *buf, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++)
		crc32_le_byte(*crc, buf[i]);
}
#else
#include <host/crc32.h>
void dfu_crc_update(uint32_t *crc, const uint8_t *buf, size_t len)
{
	*crc = crc32(*crc, buf, len);
}
#endif

int dfu_suffix_validate(const uint8_t *suffix,
			uint16_t version, uint32_t crc)
{
	if (dfu_suffix_readw(suffix+6) > version)
		return 6;
	if (dfu_suffix_readl(suffix+8) != DFU_SUFFIX_MAGIC)
		return 8;
	if (dfu_suffix_readl(suffix+12) != crc)
		return 12;
	return DFU_SUFFIX_SIZE;
}

void main(int argc, char **argv)
{
	int _remove_suffix = 0;
	int _append_suffix = 0;
	uint32_t _filecrc;
	uint32_t _fullcrc;
	long _i;
	size_t len, pos;
#define BUFSIZE	32
	uint8_t buf[BUFSIZE];
	uint16_t vid, pid, did;

	/* Parse arguments. */
	if (argc < 2) _usage();
	for (_i = 1; _i < argc; _i++) {
		if (!strcmp(argv[_i], "-del"))
			_remove_suffix = 1;
		if (!strcmp(argv[_i], "-add"))
			_append_suffix = 1;
		_getarg("-did", did);
		_getarg("-pid", pid);
		_getarg("-vid", vid);
	}
	if (_remove_suffix && _append_suffix)
		_usage();

	/* Make sure the file is there. */
	_fp = fopen(argv[1], "r+b");
	if (!_fp) _fatal(0, argv[1]);
	atexit(__atexit__);

	/* Get firmware length. */
	fseek(_fp, -DFU_SUFFIX_SIZE, SEEK_END);
	len = ftell(_fp);
	rewind(_fp);

	/* Prepare _filecrc. */
	dfu_crc_init(&_filecrc);

	/* Compute the CRC up to the firmware length. */
	pos = 0;
	while (pos < len) {
		_i = fread(buf, 1, min(BUFSIZE, len-pos), _fp);
		if (_i < 0) _fatal(0, "fread");
		if (_i == 0) _fatal(EBADF, "EOF");
		dfu_crc_update(&_filecrc, buf, _i);
		pos += _i;
	}

	/* Compute the CRC up to the suffix attributes. */
	pos = 0;
	while (pos < DFU_ATTRIB_SIZE) {
		_i = fread(_suffix+pos, 1, DFU_ATTRIB_SIZE-pos, _fp);
		if (_i < 0) _fatal(0, "fread");
		if (_i == 0) _fatal(EBADF, "EOF");
		pos += _i;
	}
	dfu_crc_update(&_filecrc, _suffix, DFU_ATTRIB_SIZE);

	/* Prepare _fullcrc. */
	_fullcrc = _filecrc;

	/* Compute the CRC of everything including the last 4 bytes. */
	pos = 0;
	while (pos < DFU_CRC_SIZE) {
		_i = fread(_suffix+DFU_ATTRIB_SIZE+pos, 1, DFU_CRC_SIZE-pos, _fp);
		if (_i < 0) _fatal(0, "fread");
		if (_i == 0) _fatal(EBADF, "EOF");
		pos += _i;
	}
	dfu_crc_update(&_fullcrc, _suffix+DFU_ATTRIB_SIZE, DFU_CRC_SIZE);

	/* Validate DFU suffix. */
	_i = dfu_suffix_validate(_suffix, DFU_VERSION, _filecrc);
	if (_i != DFU_SUFFIX_SIZE) {
		/* Update suffix attributes. */
		dfu_suffix_writew(_suffix, did);
		dfu_suffix_writew(_suffix+2, pid);
		dfu_suffix_writew(_suffix+4, vid);
		dfu_suffix_writew(_suffix+6, DFU_VERSION);
		dfu_suffix_writel(_suffix+8, DFU_SUFFIX_MAGIC);
		dfu_crc_update(&_fullcrc, _suffix, DFU_ATTRIB_SIZE);
		dfu_suffix_writel(_suffix+12, _fullcrc);

		/* Display suffix attributes. */
		if (_append_suffix) {
			printf("DFU suffix:\n");
			printf(" idDevice: 0x%04X\n", did);
			printf("idProduct: 0x%04X\n", pid);
			printf(" idDevice: 0x%04X\n", vid);
			printf("    dwCRC: 0x%08X\n", _fullcrc);
		}

		/* Validate operations. */
		if (!_append_suffix) {
			if (_i < 8)
				printf("bad bcdDFU\n");
			else if (_i < 12)
				printf("bad ucDfuSignature or bLength\n");
			else if (_i < 16)
				printf("bad dwCRC\n");
		}
		if (_remove_suffix)
			_fatal(EINVAL, "remove");
	} else {
		/* Display suffix attributes. */
		printf("DFU suffix:\n");
		printf(" idDevice: 0x%04X\n", dfu_suffix_readw(_suffix));
		printf("idProduct: 0x%04X\n", dfu_suffix_readw(_suffix+2));
		printf(" idDevice: 0x%04X\n", dfu_suffix_readw(_suffix+4));
		printf("    dwCRC: 0x%08X\n", _filecrc);

		/* Validate operations. */
		if (_append_suffix)
			_fatal(EINVAL, "append");
	}

	if (_remove_suffix) {
		/* Remove a suffix. */
		_i = fseek(_fp, -DFU_SUFFIX_SIZE, SEEK_END);
		if (_i < 0) _fatal(EBADF, "remove");
		ftruncate(fileno(_fp), ftell(_fp));
		printf("Removed from %s\n", argv[1]);
	} else if (_append_suffix) {
		/* Append a suffix. */
		fseek(_fp, 0L, SEEK_END);
		for (_i = 0; _i < DFU_SUFFIX_SIZE; _i++) {
			fputc(_suffix[_i], _fp);
		}
		printf("Appended to %s\n", argv[1]);
	}
}
