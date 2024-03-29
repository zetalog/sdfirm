#include <target/lpc.h>
#include <target/irq.h>
#include <target/kcs.h>

uint16_t corelpc_kadr = KCS_BASE;

static void corelpc_irq_handler(void)
{
	uint8_t status = __raw_readl(STS);
	if (status & KCS_IBF) {
		kcs_raise_event(KCS_EVENT_IBF);
	} else if (status & KCS_OBF) {
		kcs_raise_event(KCS_EVENT_OBF);
	}
}

void corelpc_config_kadr(uint16_t kadr)
{
	__raw_writel(HIBYTE(kadr), KADRH);
	__raw_writel(LOBYTE(kadr), KADRL);
	corelpc_kadr = kadr;
}

void corelpc_poll_irqs(void)
{
	corelpc_irq_handler();
}

void corelpc_kcs_init(void)
{
	__raw_setl(KEINTR, GCFG);
	__raw_setl(KELPC, GCFG);
}

void corelpc_init(void)
{
	//kcs->apb  APB BASE(arch)
	corelpc_config_kadr(KCS_BASE);
	corelpc_kcs_init();
}