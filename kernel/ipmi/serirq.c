#include <target/lpc.h>
#include <target/cmdline.h>

int do_sirq(int argc, char *argv[])
{
	uint8_t irq;

	if (argc < 3)
		return -EINVAL;
	irq = (uint8_t)strtoull(argv[2], 0, 0);
	if (strcmp(argv[1], "mask") == 0)
		lpc_sirq_mask(irq);
	else if (strcmp(argv[1], "unmask") == 0)
		lpc_sirq_unmask(irq);
	else if (strcmp(argv[1], "clear") == 0)
		lpc_sirq_clear(irq);
	else if (strcmp(argv[1], "status") == 0)
		return lpc_sirq_status(irq);
	else
		return -EINVAL;
	return 0;
}

DEFINE_COMMAND(sirq, do_sirq, "Super IO Control Commands",
	"sirq mask <irq>\n"
	"sirq unmask <irq>\n"
	"sirq clear <irq>\n"
	"sirq status <irq>\n"
	"    -LPC control SERIRQs\n"
);
