#include <target/console.h>

#define DEFAULT_LINE_LENGTH_BYTES	16
#define MAX_LINE_LENGTH_BYTES		64
#define DISP_LINE_LEN			16

void hexdump(caddr_t addr, const void *data, uint8_t width, size_t count)
{
	/* linebuf as a union causes proper alignment */
	union linebuf {
		uint64_t uq[MAX_LINE_LENGTH_BYTES/sizeof(uint64_t) + 1];
		uint32_t ui[MAX_LINE_LENGTH_BYTES/sizeof(uint32_t) + 1];
		uint16_t us[MAX_LINE_LENGTH_BYTES/sizeof(uint16_t) + 1];
		uint8_t  uc[MAX_LINE_LENGTH_BYTES/sizeof(uint8_t) + 1];
	} lb;
	int i;
	uint64_t x;
	size_t linelen = DISP_LINE_LEN / width;

	if (linelen*width > MAX_LINE_LENGTH_BYTES)
		linelen = MAX_LINE_LENGTH_BYTES / width;
	if (linelen < 1)
		linelen = DEFAULT_LINE_LENGTH_BYTES / width;

	while (count) {
		unsigned int thislinelen = linelen;
		printf("%08lx", addr);

		/* check for overflow condition */
		if (count < thislinelen)
			thislinelen = count;

		/* Copy from memory into linebuf and print hex values */
		for (i = 0; i < thislinelen; i++) {
			if (width == 4)
				x = lb.ui[i] = *(volatile uint32_t *)data;
			else if (width == 8)
				x = lb.uq[i] = *(volatile uint64_t *)data;
			else if (width == 2)
				x = lb.us[i] = *(volatile uint16_t *)data;
			else
				x = lb.uc[i] = *(volatile uint8_t *)data;
			if ((i % 8) == 0)
				printf(" ");
			printf(" %0*llx", width * 2, (long long)x);
			data += width;
		}

		while (thislinelen < linelen) {
			/* fill line with whitespace for nice ASCII print */
			for (i = 0; i < width*2+1; i++)
				printf(" ");
			linelen--;
		}

		/* Print data in ASCII characters */
		for (i = 0; i < thislinelen * width; i++) {
			if (!isprint(lb.uc[i]) || lb.uc[i] >= 0x80)
				lb.uc[i] = '.';
		}
		lb.uc[i] = '\0';
		printf("  |%s|\n", lb.uc);

		/* update references */
		addr += thislinelen * width;
		count -= thislinelen;
	}
}
