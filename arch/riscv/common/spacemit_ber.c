#include <target/irq.h>

static void handle_spacet_ber(irq_t irq)
{
	uint64_t buserr;
	bool handled = false;

	buserr = csr_read(CSR_MBUSERR);
	if (buserr & BER_ERR) {
		con_log("spacet: CPU%d bus error\n", smp_processor_id());
		csr_clear(CSR_MBUSERR, BER_ERR);
		handled = true;
	}
	if (buserr & BER_ST) {
		con_log("spacet: CPU%d store bus error\n", smp_processor_id());
		csr_clear(CSR_MBUSERR, BER_ST);
		handled = true;
	}
	if (buserr & BER_LD) {
		con_log("spacet: CPU%d load bus error\n", smp_processor_id());
		csr_clear(CSR_MBUSERR, BER_LD);
		handled = true;
	}
	buserr = csr_read(CSR_ML2BUSERR);
	if (buserr & L2BER_ERR) {
		con_log("spacet: CPU%d L2 bus error, core%d info%d\n",
			smp_processor_id(),
			(uint8_t)((buserr & L2BER_CORE) >> L2BER_CORE_SHIFT),
			(uint8_t)((buserr & L2BER_INFO) >> L2BER_INFO_SHIFT));
		csr_clear(CSR_ML2BUSERR, L2BER_ERR);
		handled = true;
	}
	if (!handled)
		con_log("spacet: Spurious BUS error on cpu%d\n", smp_processor_id());
}

void spacemit_ber_init(void)
{
	irq_register_vector(IRQ_SERR, handle_spacet_ber);
}
