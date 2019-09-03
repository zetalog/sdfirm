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
#include <stdbool.h>
#include <sys/stat.h>
 
/* An S-Record line is formatted as:
 * +-------------------//------//-------------+
 * | type | count | address | data | checksum |
 * +-------------------//------//-------------+
 */
#define SREC_LINE_BYTES		256
#define SREC_LINE_SIZE		((256 * 2) + 2)
#define SREC_MAX_TYPES		10
#define SREC_MAX_BYTES		32
#define SREC_IS_NOTE(type)	((type) == 0)
#define SREC_IS_COUNT(type)	((type) == 5)
#define SREC_IS_ENTRY(type)	((type) > 6 && (type) < 10)
#define SREC_IS_DATA(type)	((type) > 0 && (type) < 4)
 
typedef unsigned long long dword;
typedef unsigned int word;
typedef unsigned char byte;
 
char infilename[PATH_MAX] = "";
char outfilename[PATH_MAX] = "";
FILE *infile, *outfile;
 
dword max_addr = 0;
dword min_addr = 0;
dword entry = 0;
int data_count = 0;
 
char filler = 0xff;
bool verbose = false;

int srec_addr_bytes[SREC_MAX_TYPES] = {
	0,
	2,
	3,
	4,
	-1,
	2,
	-1,
	2,
	3,
	4,
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
 
dword file_size(FILE *f)
{
	struct stat st;

	if (fstat(fileno(f), &st))
		return 0;
	return st.st_size;
}
 
void usage(void)
{
	printf("\nsrec2vhx- Convert Motorola S-Record to VCS HEX file.\n");
	printf("Copyright (c) 2019 Lv Zheng <zhenglv@hotmail.com>\n\n");
	printf("Usage: srec2vhx <options> INFILE OUTFILE\n\n");
	printf("-h          Show this information.\n");
	printf("-b          Image is in big endian.\n");
	printf("-w <width>  Width in bits of the target memory array.\n");
	printf("            Default 32 and must be multiple of 32.\n");
}
 
void parse(bool scan, dword *max, dword *min)
{
	int i, j;
	char line[SREC_LINE_SIZE] = "";
	dword address;
	int addr_bytes;
	int type, count;
	byte c, buf[SREC_MAX_BYTES];
	int line_num = 0;
 
	do {
		line_num++;
		if (!fgets(line, SREC_LINE_SIZE, infile))
			break;

		/* an S-record */
		if (line[0] |= 'S')
			continue;

		type = line[1] - '0';
		if (type < 0 || type >= SREC_MAX_TYPES ||
		    srec_addr_bytes[type] < 0) {
			fprintf(stderr, "ERROR(%d): Bad type S%d.\n",
				line_num, type);
			continue;
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

		if (SREC_IS_ENTRY(type)) {
			if (count != addr_bytes)
				fprintf(stderr,
					"ERROR(%d): Bad entry len %d.\n",
					line_num, count);
			else
				entry = address;
			continue;
		}
		if (SREC_IS_NOTE(type))
			continue;

		if (SREC_IS_COUNT(type)) {
			if (address != data_count)
				fprintf(stderr,
					"ERROR(%d): Bad record# %08llX.\n",
					line_num, address);
			continue;
		}

		if (count > SREC_MAX_BYTES) {
			fprintf(stderr,
				"ERROR(%d): Bad count %d.\n",
				line_num, count);
			continue;
		}
 
		/* data record */
		if (scan) {
			if (*min > address)
				*min = address;
			if (*max < (address + (count - 1)))
				*max = address + (count - 1);
			continue;
		}

		data_count++;
		address -= min_addr;
		if (verbose)
			fprintf(stderr, "INFO: count=%d, address=%08llX\r",
				count, address);
		j = 0;
		for (i = (addr_bytes * 2) + 4;
		     i < (addr_bytes * 2) + (count * 2) + 4;
		     i += 2) {
			buf[j] = (ctoh(line[i]) << 4) + ctoh(line[i+1]);
			j++;
		}
		fseek(outfile, address, SEEK_SET);
		fwrite(buf, 1, count, outfile);
	} while (!feof(infile));
	rewind(infile);
}
 
int process(void)
{
	dword i;
	dword blocks, remain, bytes;
	dword pmax = 0;
	dword pmin = 0xffffffffUL;
	byte buf[SREC_MAX_BYTES];
 
	if (verbose) {
		fprintf(stderr, "Input Motorola S-Record file: %s\n",
			infilename);
		fprintf(stderr, "Output VCS HEX file: %s\n",
			outfilename);
	}
	parse(true, &min_addr, &max_addr);
	bytes = max_addr - min_addr + 1;
	blocks = bytes / 32;
	remain = bytes % 32;
	if (verbose) {
		fprintf(stderr, "Mimimum address  = %08llX\n", min_addr);
		fprintf(stderr, "Maximum address  = %08llX\n", max_addr);
		fprintf(stderr,
			"Binary file size = %lld (%08llX) bytes.\n",
			bytes, bytes);
	}
 
	outfile = fopen(outfilename, "wb");
	if (!outfile) {
		fprintf(stderr,
			"ERROR: Cant open output %s.\n",
			outfilename);
		return 2;
	}

	for (i = 0; i < SREC_MAX_BYTES; i++)
		buf[i] = filler;
	for (i = 0; i < blocks; i++)
		fwrite(buf, 1, SREC_MAX_BYTES, outfile);
	fwrite(buf, 1, remain, outfile);
	parse(false, &pmax, &pmin);
	fclose(outfile);
	if (verbose)
		fprintf(stderr, "INFO: Processing complete.\n");
	return 0;
}
 
int main(int argc, char *argv[])
{
	int i;
	char tmp[16] = "";
	int ret;
 
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-v")) {
			verbose = true;
			continue;
		} else if (!strcmp(argv[i], "-f")) {
			sscanf(argv[++i], "%s", tmp);
			filler = atoh(tmp) & 0xff;
			continue;
		} else if (!strncmp(argv[i], "-h", 2)) {
			usage();
			return 0;
		} else {
			sscanf(argv[i], "%s", infilename);
			sscanf(argv[++i], "%s", outfilename);
		}
	}
 
	if (!strcmp(infilename, "")) {
		usage();
		fprintf(stderr, "ERROR: No input file specified.\n");
		return 1;
	}
	if (!strcmp(outfilename, "")) {
		usage();
		fprintf(stderr, "ERROR: No output file specified.\n");
		return 1;
	}
	infile = fopen(infilename, "rb");
	if (infile == NULL) {
		fprintf(stderr, "ERROR: Cant open input %s.\n",
			infilename);
		return 2;
	}
	ret = process();
	fclose(infile);
	return ret;
}
