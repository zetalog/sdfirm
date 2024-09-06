#include <target/noc.h>
#include <target/irq.h>

void cmn600_ras_config(cmn_nid_t nid)
{
	uint64_t errfr = __raw_readq(CMN_errfr(cmn_child_node(nid)));
	uint8_t ed = errfr & CMN_errfr_ED(CMN_errfr_ED_MASK);
	uint8_t de = errfr & CMN_errfr_DE(CMN_errfr_DE_MASK);
	uint8_t ui = errfr & CMN_errfr_UI(CMN_errfr_UI_MASK);
	uint8_t fi = errfr & CMN_errfr_FI(CMN_errfr_FI_MASK);
	uint8_t cfi = errfr & CMN_errfr_CFI(CMN_errfr_CFI_MASK);
	uint8_t cec = errfr & CMN_errfr_CEC(CMN_errfr_CEC_MASK);
	con_log("cmn_ras: ED=%d, DE=%d, UI=%d, FI=%d, CFI=%d, CEC=%d\n", ed, de, ui, fi, cfi, cec);
	if (cmn_ras_support_ed(nid))
		cmn_ras_enable_ed(nid);
	if (cmn_ras_support_de(nid))
		cmn_ras_enable_de(nid);
	if (cmn_ras_support_ui(nid))
		cmn_ras_enable_ui(nid);
	if (cmn_ras_support_fi(nid))
		cmn_ras_enable_fi(nid);	
	if (cmn_ras_support_cfi(nid))
		cmn_ras_enable_cfi(nid);
}

void cmn600_ras_report(cmn_nid_t nid)
{
	uint64_t status = __raw_readq(CMN_errstatus(CMN_node2addr(nid)));

	if (status & CMN_errstatus_AV)
		con_log("cmn_ras: AV Error: addr=%08llx\n", __raw_readq(CMN_erraddr));
	if (status & CMN_errstatus_V)
		con_log("cmn_ras: V Error\n");
	if (status & CMN_errstatus_UE)
		con_log("cmn_ras: UE Error\n");
	if (status & CMN_errstatus_OF)
		con_log("cmn_ras: OF Error\n");
	if (status & CMN_errstatus_MV)
		con_log("cmn_ras: MV Error\n");
	if (status & CMN_errstatus_CE)
		con_log("cmn_ras: CE Error\n");
	if (status & CMN_errstatus_DE)
		con_log("cmn_ras: DE Error\n");

	__raw_writeq(status, CMN_errstatus(CMN_node2addr(nid)));
}

cmn_nid_t CMN_ras_nid(uint8_t errg, uint8_t bit)
{
	
}

void cmn600_handle_s_errs(irq_t irq)
{
	uint64_t status[5];
	int i, j;

	for (i = 0; i < 5; i++) {
		status[i] = __raw_readq(CMN_errgsr(i));
		for (j = 0; j < 64; j++) {
			if (status[i] & _BV(j)) {
				cmn600_ras_report(CMN_ras_nid(i, j));
			}
		}
	}
}

void cmn600_handle_s_faults(irq_t irq)
{
	uint64_t status[5];
	int i, j;

	for (i = 0; i < 5; i++) {
		status[i] = __raw_readq(CMN_errgsr(i + 5));
		for (j = 0; j < 64; j++) {
			if (status[i] & _BV(j)) {
				cmn600_ras_report(CMN_ras_nid(i, j));
			}
		}
	}
}

void cmn600_handle_ns_errs(irq_t irq)
{
	uint64_t status[5];
	int i, j;

	for (i = 0; i < 5; i++) {
		status[i] = __raw_readq(CMN_errgsr_NS(i));
		for (j = 0; j < 64; j++) {
			if (status[i] & _BV(j)) {
				cmn600_ras_report(CMN_ras_nid(i, j));
			}
		}
	}
}

void cmn600_handle_ns_faults(irq_t irq)
{
	uint64_t status[5];
	int i, j;

	for (i = 0; i < 5; i++) {
		status[i] = __raw_readq(CMN_errgsr_NS(i + 5));
		for (j = 0; j < 64; j++) {
			if (status[i] & _BV(j)) {
				cmn600_ras_report(CMN_ras_nid(i, j));
			}
		}
	}
}

void cmn600_handle_irq(irq_t irq)
{
	cmn600_handle_s_errs(irq);
	cmn600_handle_s_faults(irq);
	cmn600_handle_ns_errs(irq);
	cmn600_handle_ns_faults(irq);
}

void cmn600_ras_init(void)
{
	irq_register_vector(IRQ_N100_REQERRS_NID0, cmn600_handle_s_errs);
	irq_register_vector(IRQ_N100_REQFLTS_NID0, cmn600_handle_s_faults);
	irq_register_vector(IRQ_N100_REQERRNS_NID0, cmn600_handle_ns_errs);
	irq_register_vector(IRQ_N100_REQFLTNS_NID0, cmn600_handle_ns_faults);
}

static int do_cmn_ras(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "init") == 0) {
		cmn600_ras_init();
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(cmn_ras, do_cmn_ras, "SpacemiT CMN RAS Debug commands",
	"cmn_ras init\n"
);
