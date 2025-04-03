#include <target/irq.h>

const char *thead_l1ram_names[] = {
	"L1 I$ tag",
	"L1 I$ data",
	"L1 D$ tag",
	"L1 D$ data",
	"JTLB tag",
	"JTLB data",
};

const char *thead_l1ram_name(uint8_t ramid)
{
	if (ramid >= ARRAY_SIZE(thead_l1ram_names))
		return "UNKNOWN";
	return thead_l1ram_names[ramid];
}

const char *thead_l2ram_names[] = {
	"L2 tag",
	"L2 data",
	"L2 dirty",
};

const char *thead_l2ram_name(uint8_t ramid)
{
	if (ramid >= ARRAY_SIZE(thead_l2ram_names))
		return "UNKNOWN";
	return thead_l2ram_names[ramid];
}

static void handle_thead_ecc(irq_t irq)
{
	uint64_t cer;
	bool handled = false;

	cer = csr_read(CSR_MCER);
	if (cer & CER_ERR_VLD) {
		if (cer & CER_ECC_ERR) {
			if (cer & CER_ERR_FATAL) {
				con_log("thead: CPU%d 2-bit %s/%d/%d ECC fatal error\n",
					smp_processor_id(),
					thead_l1ram_name(CER_RAMID(cer)),
					(uint8_t)CER_ERR_WAY(cer),
					(uint32_t)CER_ERR_INDEX(cer));
				csr_clear(CSR_MCER, CER_ERR_FATAL);
			} else {
				con_log("thead: CPU%d 1-bit %s/%d ECC error, fixed %d\n",
					smp_processor_id(),
					thead_l1ram_name(CER_RAMID(cer)),
					(uint8_t)CER_ERR_WAY(cer),
					(uint16_t)CER_FIX_CNT(cer));
			}
			csr_clear(CSR_MCER, CER_ECC_ERR);
			handled = true;
		}
		if (cer & CER_BUS_ERR) {
			con_log("thead: CPU%d load bus error\n",
				smp_processor_id());
			csr_clear(CSR_MCER, CER_BUS_ERR);
			handled = true;
		}
		csr_clear(CSR_MCER, CER_ERR_VLD);
	}
	cer = csr_read(CSR_MCER2);
	if (cer & CER2_ERR_VLD) {
		if (cer & CER2_ECC_FATAL) {
			con_log("thead: CPU%d 2-bit %s/%d/%d ECC fatal error\n",
				smp_processor_id(),
				thead_l2ram_name(CER2_RAMID(cer)),
				(uint8_t)CER2_ERR_WAY(cer),
				(uint32_t)CER2_ERR_INDEX(cer));
			csr_clear(CSR_MCER2, CER2_ECC_FATAL);
		} else {
			con_log("thead: CPU%d 1-bit %s/%d ECC error, fixed %d\n",
				smp_processor_id(),
				thead_l2ram_name(CER2_RAMID(cer)),
				(uint8_t)CER2_ERR_WAY(cer),
				(uint16_t)CER2_FIX_CNT(cer));
		}
		csr_clear(CSR_MCER2, CER2_ERR_VLD);
		handled = true;
	}
	if (!handled)
		con_log("thead: Spurious ECC error on cpu%d\n", smp_processor_id());
}

void thead_ecc_init(void)
{
	irq_register_vector(IRQ_ECC, handle_thead_ecc);
}
