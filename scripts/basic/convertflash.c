#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define PAD_BYTE	0x00

int main(int argc, char **argv)
{
	FILE *in, *out;
	char *input, *output;
	int base;
	bool inverted = false;
	int i;
	unsigned char byte0, byte1, byte2, byte3;
	int ret;

	if (argc > 1)
		input = argv[1];
	else
		input = "arch/riscv/boot/sdfirm.bin";
	if (argc > 2)
		output = argv[2];
	else
		output = "arch/riscv/boot/sdfirm.inv";
	if (argc > 3)
		base = strtoul(argv[3], NULL, 0);
	else
		base = 0;
	if (argc > 4 && (strcmp(argv[4], "-i") == 0))
		inverted = true;

	printf("%s converting %s to %s...\n",
	       inverted ? "Inverted" : "Direct", input, output);

	in = fopen(input, "ab+");
	if (in == NULL) {
		fprintf(stderr, "Failed to open input file %s!\n", input);
		return -EIO;
	}
	out = fopen(output, "w");
	if (out == NULL) {
		fprintf(stderr, "Failed to open output file %s!\n", output);
		return -EIO;
	}
	byte0 = PAD_BYTE;
	for (i = 0; i < base; i++) {
		fwrite(&byte0, 1, 1, out);
		fwrite(&byte0, 1, 1, out);
		fwrite(&byte0, 1, 1, out);
		fwrite(&byte0, 1, 1, out);
	}
	while (!feof(in)) {
		ret = fread(&byte0, 1, 1, in);
		if (ret != 1)
			byte0 = PAD_BYTE;
		ret = fread(&byte1, 1, 1, in);
		if (ret != 1)
			byte1 = PAD_BYTE;
		ret = fread(&byte2, 1, 1, in);
		if (ret != 1)
			byte2 = PAD_BYTE;
		ret = fread(&byte3, 1, 1, in);
		if (ret != 1)
			byte3 = PAD_BYTE;

		if (inverted) {
			fwrite(&byte3, 1, 1, out);
			fwrite(&byte2, 1, 1, out);
			fwrite(&byte1, 1, 1, out);
			fwrite(&byte0, 1, 1, out);
		} else {
			fwrite(&byte0, 1, 1, out);
			fwrite(&byte1, 1, 1, out);
			fwrite(&byte2, 1, 1, out);
			fwrite(&byte3, 1, 1, out);
		}
		i++;
	}
	printf("%d lines converted.\n", i);
	fclose(in);
	fclose(out);
	return 0;
}
