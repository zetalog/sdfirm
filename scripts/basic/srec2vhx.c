/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)srec2vhx.c: Motorola S-Record to VCS HEX converter
 * $Id: srec2vhx.c,v 1.1 2019-09-04 24:03:00 zhenglv Exp $
 */

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* An S-Record line is formatted as:
 * +-------------------//------//-------------+
 * | type | count | address | data | checksum |
 * +-------------------//------//-------------+
 *
 * This utility converts it to ARM VHX memory array format, with the load
 * address information added. The ARM VHX conversion tool can be found in
 * scripts/bin2vhx.pl, which works with "binary" format files.
 */
#define SREC_LINE_BYTES		256
#define SREC_LINE_SIZE		((SREC_LINE_BYTES * 2) + 2)
#define SREC_MAX_TYPES		10
#define SREC_MAX_BYTES		32
#define SREC_IS_NOTE(type)	((type) == 0)
#define SREC_IS_COUNT(type)	((type) == 5)
#define SREC_IS_ENTRY(type)	((type) > 6 && (type) < 10)
#define SREC_IS_DATA(type)	((type) > 0 && (type) < 4)

#define VHX_WIDTH_BITS		32

#define IS_ALIGNED(x, a)	(((x) & (((dword)(a)) - 1)) == 0)
#define ROUNDDOWN(x, a)		(((dword)(x)) & (~(((dword)(a)) - 1)))
#define ROUNDUP(x, a)		((((dword)(x)) + (((dword)(a)) - 1)) &	\
				 (~(((dword)(a)) - 1)))

typedef uint64_t dword;
typedef uint32_t word;
typedef uint8_t byte;

char infilename[PATH_MAX] = "";
char outfilename[PATH_MAX] = "";
FILE *infile, *outfile;

dword max_addr = 0;
dword min_addr = 0;
bool has_max = false;
bool has_min = false;
dword entry = 0;
int data_count = 0;

bool verbose = false;
dword prog_base = 0;

byte filler = 0xFF;
bool bigendian = false;
byte *byte_data;
int byte_width = VHX_WIDTH_BITS / 8;

int srec_addr_bytes[SREC_MAX_TYPES] = {
	0,
	2,
	3,
	4,
	-1,
	2,
	-1,
	4,
	3,
	2,
};

int ctoh(char c)
{
	int res = 0;

	if (c >= '0' && c <= '9')
		res = (c - '0');
	else if (c >= 'A' && c <= 'F')
		res = (c - 'A' + 10);
	else if (c >= 'a' && c <= 'f')
		res = (c - 'a' + 10);
	return res;
}

dword atoh(char *s)
{
	int i;
	char c;
	dword res = 0;

	for (i = 0; i < strlen(s); i++) {
		c = s[i];
		res <<= 4;
		res += ctoh(c);
	}
	return res;
}

void pack(char b, int i)
{
	if (bigendian)
		byte_data[i] = b;
	else
		byte_data[byte_width-i-1] = b;
}

void unpack(void)
{
	int i;

	for (i = 0; i < byte_width; i++)
		fprintf(outfile, "%02x", byte_data[i]);
	fprintf(outfile, "\n");
}

dword pad_last(dword address, dword index)
{
	int i = 0;

	while (i < (ROUNDUP(address, byte_width) - address)) {
		pack(filler, index % byte_width);
		if (((index + 1) % byte_width) == 0)
			unpack();
		i++;
		index++;
	}
	return index;
}

dword pad_next(dword address, dword index)
{
	int i = 0;

	while (i < (address - ROUNDDOWN(address, byte_width))) {
		pack(filler, index % byte_width);
		if (((index + 1) % byte_width) == 0)
			unpack();
		i++;
		index++;
	}
	return index;
}

void usage(void)
{
	printf("\nsrec2vhx- Convert Motorola S-Record to VCS HEX file.\n");
	printf("Copyright (c) 2019 Lv Zheng <zhenglv@hotmail.com>\n\n");
	printf("Usage: srec2vhx <options> INFILE OUTFILE\n\n");
	printf("-h          Show this information.\n");
	printf("-b          Image is in big endian.\n");
	printf("-f <value>  Filling holes with value, default %02x.\n",
	       filler);
	printf("-p <addr>   Specify program base address, default %08lx.\n",
	       prog_base);
	printf("-w <width>  Width in bits of the target memory array.\n");
	printf("            Default 32 and must be multiple of 32.\n");
	printf("-v          Enable verbosity.\n");
}

int parse(bool scan)
{
	int i, j;
	dword index = 0;
	char line[SREC_LINE_SIZE];
	dword address, last_addr = prog_base, next_addr;
	int addr_bytes;
	int type, count;
	byte c, buf[SREC_MAX_BYTES];
	int line_num = 0;

	while (!feof(infile)) {
		line_num++;
		if (!fgets(line, SREC_LINE_SIZE, infile)) {
			if (!feof(infile)) {
				fprintf(stderr,
					"ERROR(%d): Read %s failure.\n",
					line_num, infilename);
				return 2;
			}
			break;
		}

		/* an S-record */
		if (line[0] != 'S')
			continue;

		if (verbose)
			fprintf(stderr, "INFO(%d): %s.\n",
				line_num, line);

		type = line[1] - '0';
		if (type < 0 || type >= SREC_MAX_TYPES ||
		    srec_addr_bytes[type] < 0) {
			fprintf(stderr, "ERROR(%d): Bad type S%d.\n",
				line_num, type);
			return 3;
		}
		addr_bytes = srec_addr_bytes[type];

		count = (ctoh(line[2]) << 4) + ctoh(line[3]);
		count -= (addr_bytes + 1);
		address = 0;
		for (i = 4; i < (addr_bytes * 2) + 4; i++) {
			c = line[i];
			address <<= 4;
			address += ctoh(c);
		}
		if (verbose)
			fprintf(stderr,
				"INFO(%d): count=%d, address=%08lX.\n",
				line_num, count, address);

		if (SREC_IS_ENTRY(type)) {
			if (count != 0) {
				fprintf(stderr,
					"ERROR(%d): Bad entry len %d.\n",
					line_num, count);
				return 3;
			}
			entry = address;
			continue;
		}
		if (SREC_IS_NOTE(type))
			continue;

		if (SREC_IS_COUNT(type)) {
			if (address != data_count)
				fprintf(stderr,
					"ERROR(%d): Bad record# %08lX.\n",
					line_num, address);
			continue;
		}

		if (count > SREC_MAX_BYTES) {
			fprintf(stderr,
				"ERROR(%d): Bad count %d.\n",
				line_num, count);
			return 3;
		}

		/* data record */
		if (scan) {
			if (!has_min || min_addr > address) {
				min_addr = address;
				has_min = true;
			}
			if (!has_max || max_addr < (address + (count - 1))) {
				has_max = true;
				max_addr = address + (count - 1);
			}
			if (verbose)
				fprintf(stderr,
					"INFO(%d): min=%08lX, max=%08lX.\n",
					line_num, min_addr, max_addr);
			continue;
		}

		data_count++;
		j = 0;
		for (i = (addr_bytes * 2) + 4;
		     i < (addr_bytes * 2) + (count * 2) + 4;
		     i += 2) {
			buf[j] = (ctoh(line[i]) << 4) + ctoh(line[i+1]);
			j++;
		}
		if (last_addr != address) {
			if (address < last_addr) {
				fprintf(stderr,
					"ERROR(%d): Address %08lX < %08lX\n",
					line_num, address, last_addr);
				return 3;
			}
			next_addr = ROUNDDOWN(address, byte_width);
			if (next_addr <= last_addr) {
				index = address % byte_width;
				goto fill_last;
			} else {
				index = pad_last(last_addr, index);
				fprintf(outfile, "@%08lX\n",
					(next_addr - prog_base) / byte_width);
			}
			index = pad_next(address, 0);
		}
fill_last:
		for (i = 0; i < count; i++, index++) {
			pack(buf[i], index % byte_width);
			if (((index + 1) % byte_width) == 0)
				unpack();
		}
		last_addr = address + count;
	};
	if (!scan)
		index = pad_last(last_addr, index);
	if (verbose)
		fprintf(stderr, "INFO(%d): Parse complete.\n",
			line_num);
	rewind(infile);
	return 0;
}

int process(void)
{
	int ret;

	if (verbose) {
		fprintf(stderr, "Input Motorola S-Record file: %s\n",
			infilename);
		fprintf(stderr, "Output VCS HEX file: %s\n",
			outfilename);
	}
	ret = parse(true);
	if (ret)
		return ret;
	if (verbose) {
		fprintf(stderr, "Mimimum address  = %08lX\n", min_addr);
		fprintf(stderr, "Maximum address  = %08lX\n", max_addr);
	}

	outfile = fopen(outfilename, "w");
	if (!outfile) {
		fprintf(stderr,
			"ERROR: Cant open output %s.\n",
			outfilename);
		return 2;
	}
	ret = parse(false);
	fclose(outfile);
	return ret;
}

int main(int argc, char *argv[])
{
	int i;
	int ret;
	int bit_width;

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-v")) {
			verbose = true;
			continue;
		} else if (!strcmp(argv[i], "-b")) {
			bigendian = true;
			continue;
		} else if (!strcmp(argv[i], "-f")) {
			filler = strtoul(argv[++i], NULL, 0);
			continue;
		} else if (!strcmp(argv[i], "-p")) {
			prog_base = strtoull(argv[++i], NULL, 0);
			continue;
		} else if (!strcmp(argv[i], "-w")) {
			bit_width = strtoul(argv[++i], NULL, 0);
			if (bit_width % VHX_WIDTH_BITS) {
				fprintf(stderr,
					"ERROR: Bad width %d.\n",
					bit_width);
				usage();
				return 0;
			}
			byte_width = bit_width / 8;
			continue;
		} else if (!strncmp(argv[i], "-h", 2)) {
			usage();
			return 0;
		} else if (argc - i < 2) {
			fprintf(stderr, "ERROR: Too few argument.\n");
			usage();
			return 1;
		} else {
			sscanf(argv[i], "%s", infilename);
			sscanf(argv[++i], "%s", outfilename);
		}
	}

	if (!strcmp(infilename, "")) {
		fprintf(stderr, "ERROR: No input file specified.\n");
		usage();
		return 1;
	}
	if (!strcmp(outfilename, "")) {
		fprintf(stderr, "ERROR: No output file specified.\n");
		usage();
		return 1;
	}
	byte_data = malloc(byte_width / 8);
	if (!byte_data) {
		fprintf(stderr, "ERROR: Cant malloc buffer %d.\n",
			byte_width);
		return 2;
	}
	infile = fopen(infilename, "r");
	if (infile == NULL) {
		fprintf(stderr, "ERROR: Cant open input %s.\n",
			infilename);
		return 2;
	}
	ret = process();
	fclose(infile);
	return ret;
}
