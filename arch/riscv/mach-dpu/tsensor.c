#include <target/arch.h>
#include <target/gpio.h>
#include <target/irq.h>
#include <target/cmdline.h>

static bool tsensor_irq_triggered = false;

static void dpu_tsensor_handler(void)
{
	tsensor_irq_triggered = true;
	printf("cpu high temprature warning!\n");
	irqc_mask_irq(IRQ_TSENSOR0);
}

void dpu_tsensor_irq_conf(void)
{
	__raw_writel(REG_METS_CONFIG_RUN, TC_PADDR_PE16_METS_CONFIG_ADDR);
	__raw_writel(REG_METS_CONFIG_RUN, TC_PADDR_VPU_METS_CONFIG_ADDR);
	__raw_writel(REG_METS_CONFIG_RUN, TC_PADDR_DDR_METS_CONFIG_ADDR);
	__raw_writel(REG_METS_CONFIG_RUN, TC_PADDR_PCIE_METS_CONFIG_ADDR);
	__raw_writel(REG_METS_CONFIG_RUN, TC_PADDR_NOC_METS_CONFIG_ADDR);
}

void dpu_tsensor_irq_init(void)
{
	printf("tsensor init\n");
	clk_enable(srst_tsensor_mets);
	clk_enable(srst_tsensor_xo);
	/* clk_enable(tsensor_mets_clk); */
	irqc_configure_irq(IRQ_TSENSOR0, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_TSENSOR0, dpu_tsensor_handler);
	irqc_enable_irq(IRQ_TSENSOR0);
}

static int dpu_tsensor_irq_trigger(void)
{
	tsensor_irq_triggered = false;
	dpu_tsensor_irq_conf();
	while (!tsensor_irq_triggered) {
		irq_local_enable();
		irq_local_disable();
	}
	irq_ack_irq(IRQ_TSENSOR0);
	irqc_unmask_irq(IRQ_TSENSOR0);
	return 0;
}

static int do_tsensor(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1]. "irq") == 0)
		return dpu_tsensor_irq_trigger();
	return -EINVAL;
}

DEFINE_COMMAND(tsensor, do_tsensor, "TSensor IRQ testing",
	"tsensor irq\n"
	"	- testing TSensor IRQ\n"
);
