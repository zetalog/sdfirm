#include <target/noc.h>
#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>

bh_t cmn600_ras_bh;

void cmn600_ras_config(cmn_nid_t nid)
{
	uint64_t errfr = __raw_readq(CMN_errfr(cmn_child_node(nid, 0)));
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
	uint64_t status = __raw_readq(CMN_errstatus(cmn_child_node(nid, 0)));
	printf("ras_report: %x\n");
	if (status & CMN_errstatus_AV)
		con_log("cmn_ras: AV Error: addr=%08llx\n", __raw_readq(CMN_erraddr(cmn_child_node(nid, 0))));
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

	__raw_writeq(status, CMN_errstatus(cmn_child_node(nid, 0)));
}

cmn_nid_t CMN_ras_nid(uint8_t errg, uint8_t bit)
{
	cmn_nid_t nid = 0;
	unsigned long long base;

	switch (errg)
	{
	case ERRGSR_XP:
		base = cmn_bases[cmn_xp_ids[bit]];
		nid = cmn_node_id(base);
		break;

	case ERRGSR_HNI:
		base = cmn_bases[cmn_hni_ids[bit]];
		nid = cmn_node_id(base);
		break;

	case ERRGSR_HNF:
		base = cmn_bases[cmn_hnf_ids[bit]];
		nid = cmn_node_id(base);
		break;

	case ERRGSR_SBSX:
		base = cmn_bases[cmn_sbsx_ids[bit]];
		nid = cmn_node_id(base);
		break;

	case ERRGSR_CXHA:
		base = cmn_bases[cmn_cxha_ids[bit]];
		nid = cmn_node_id(base);
		break;

	default:
		break;
	}
	return nid;
}

void cmn600_handle_s_errs(void)
{
	uint64_t status[5];
	int i, j;

	for (i = 0; i < 5; i++) {
		status[i] = __raw_readq(CMN_cfgm_errgsr(i));
		for (j = 0; j < 64; j++) {
			if (status[i] & _BV(j)) {
				cmn600_ras_report(CMN_ras_nid(i, j));
			}
		}
	}
}

void cmn600_handle_s_faults(void)
{
	uint64_t status[5];
	int i, j;

	for (i = 0; i < 5; i++) {
		status[i] = __raw_readq(CMN_cfgm_errgsr(i + 5));
		for (j = 0; j < 64; j++) {
			if (status[i] & _BV(j)) {
				cmn600_ras_report(CMN_ras_nid(i, j));
			}
		}
	}
}

void cmn600_handle_ns_errs(void)
{
	uint64_t status[5];
	int i, j;

	for (i = 0; i < 5; i++) {
		status[i] = __raw_readq(CMN_cfgm_errgsr_NS(i));
		for (j = 0; j < 64; j++) {
			if (status[i] & _BV(j)) {
				cmn600_ras_report(CMN_ras_nid(i, j));
			}
		}
	}
}

void cmn600_handle_ns_faults(void)
{
	uint64_t status[5];
	int i, j;

	for (i = 0; i < 5; i++) {
		status[i] = __raw_readq(CMN_cfgm_errgsr_NS(i + 5));
		for (j = 0; j < 64; j++) {
			if (status[i] & _BV(j)) {
				cmn600_ras_report(CMN_ras_nid(i, j));
			}
		}
	}
}

void cmn600_handle_irq(void)
{
	cmn600_handle_s_errs();
	cmn600_handle_s_faults();
	cmn600_handle_ns_errs();
	cmn600_handle_ns_faults();
}

static void cmn600_ras_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		cmn600_handle_irq();
		return;
	}
}

#ifdef SYS_REALTIME
static void cmn600_ras_poll_init(void)
{
	irq_register_poller(cmn600_ras_bh);
}
#define cmn600_ras_irq_init()	do {} while (0)
#else
static void cmn600_ras_irq_init(void)
{
	irqc_configure_irq(IRQ_N100_REQERRS_NID0, 0, IRQ_LEVEL_TRIGGERED);
	irqc_configure_irq(IRQ_N100_REQFLTS_NID0, 0, IRQ_LEVEL_TRIGGERED);
	irqc_configure_irq(IRQ_N100_REQERRNS_NID0, 0, IRQ_LEVEL_TRIGGERED);
	irqc_configure_irq(IRQ_N100_REQFLTNS_NID0, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_N100_REQERRS_NID0, cmn600_handle_s_errs);
	irq_register_vector(IRQ_N100_REQFLTS_NID0, cmn600_handle_s_faults);
	irq_register_vector(IRQ_N100_REQERRNS_NID0, cmn600_handle_ns_errs);
	irq_register_vector(IRQ_N100_REQFLTNS_NID0, cmn600_handle_ns_faults);
	irqc_enable_irq(IRQ_N100_REQERRS_NID0);
	irqc_enable_irq(IRQ_N100_REQFLTS_NID0);
	irqc_enable_irq(IRQ_N100_REQERRNS_NID0);
	irqc_enable_irq(IRQ_N100_REQFLTNS_NID0);
}
#define cmn600_ras_poll_init()	do {} while (0)
#endif

void cmn600_ras_init(void)
{
	cmn600_ras_bh = bh_register_handler(cmn600_ras_bh_handler);
	cmn600_ras_irq_init();
	cmn600_ras_poll_init();
}

void cmn600_ras_err_inj(uint32_t id, uint32_t srcid, uint32_t lpid)
{
	uint32_t data = (srcid << CMN_hnf_err_inj_srcid_OFFSET & CMN_hnf_err_inj_srcid_MASK) | 
		(lpid << CMN_hnf_err_inj_lpid_OFFSET & CMN_hnf_err_inj_lpid_MASK) | 
		CMN_hnf_err_inj_en;
	__raw_writel(data, cmn_bases[cmn_hnf_ids[id]]);
}

void cmn600_ras_hnf_par_err_inj(uint32_t id, uint32_t lane)
{
	__raw_writel(lane, CMN_mxp_byte_par_err_inj(cmn_bases[cmn_hnf_ids[id]], lane));
}

void cmn600_ras_mxp_par_err_inj(uint32_t id, uint32_t port, uint32_t lane)
{
	printf("mxp_errinj: xp_id=%d, xp_base=0x%lx, port=%d, lane=0x%x\n", cmn_xp_ids[id], cmn_bases[cmn_xp_ids[id]], port, lane);
	__raw_writel(lane, CMN_mxp_byte_par_err_inj(cmn_bases[cmn_xp_ids[id]], port));
}

static int do_cmn_ras(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "init") == 0) {
		cmn600_ras_init();
		return 0;
	}
	if (strcmp(argv[1], "mxp") == 0) {
		if (strcmp(argv[2], "errinj") == 0) {
			cmn600_ras_mxp_par_err_inj((uint32_t)strtoull(argv[3], 0, 0), 
				(uint32_t)strtoull(argv[4], 0, 0), 
				(uint32_t)strtoull(argv[5], 0, 0));
			return 0;
		}
		return -EINVAL;
	}
	return -EINVAL;
}

DEFINE_COMMAND(cmn_ras, do_cmn_ras, "SpacemiT CMN RAS Debug commands",
	"cmn_ras init\n"
	"cmn_ras mxp errinj <id> <port> <lane>\n"
);
