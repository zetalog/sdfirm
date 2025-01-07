#include <target/noc.h>
#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/arch.h>

#define CMN600_WLEN 256

bh_t cmn600_ras_bh;

void cmn600_ras_config(caddr_t base)
{
	uint64_t errfr = __raw_readq(CMN_errfr(base));
	uint8_t ed = errfr & CMN_errfr_ED(CMN_errfr_ED_MASK);
	uint8_t de = errfr & CMN_errfr_DE(CMN_errfr_DE_MASK);
	uint8_t ui = errfr & CMN_errfr_UI(CMN_errfr_UI_MASK);
	uint8_t fi = errfr & CMN_errfr_FI(CMN_errfr_FI_MASK);
	uint8_t cfi = errfr & CMN_errfr_CFI(CMN_errfr_CFI_MASK);
	uint8_t cec = errfr & CMN_errfr_CEC(CMN_errfr_CEC_MASK);
	con_log("cmn_ras: nid=%d, ED=%d, DE=%d, UI=%d, FI=%d, CFI=%d, CEC=%d\n", cmn_node_id(base), ed, de, ui, fi, cfi, cec);
	if (cmn_ras_support_ed(base))
		cmn_ras_enable_ed(base);
	if (cmn_ras_support_de(base))
		cmn_ras_enable_de(base);
	if (cmn_ras_support_ui(base))
		cmn_ras_enable_ui(base);
	if (cmn_ras_support_fi(base))
		cmn_ras_enable_fi(base);	
	if (cmn_ras_support_cfi(base))
		cmn_ras_enable_cfi(base);
}

void cmn600_ras_report(caddr_t base)
{
	uint64_t status = __raw_readq(CMN_errstatus(base));
	printf("ras_report: %llx\n", status);
	if (status & CMN_errstatus_AV)
		con_log("cmn_ras: AV Error: addr=%08llx\n", __raw_readq(CMN_erraddr(base)));
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

	__raw_writeq(status, CMN_errstatus(base));
}

caddr_t CMN_ras_nid(uint8_t errg, uint8_t bit)
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
	return base;
}

void cmn600_handle_s_errs(void)
{
	uint64_t status[5];
	int i, j;

	for (i = 0; i < 5; i++) {
		status[i] = __raw_readq(CMN_cfgm_errgsr(i));
		if (status[i] != 0) {
			printf("status=0x%llx\n", status[i]);
		}
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
		if (status[i] != 0) {
			printf("status=0x%llx\n", status[i]);
		}
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
		if (status[i] != 0) {
			printf("status=0x%llx\n", status[i]);
		}
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
		if (status[i] != 0) {
			printf("status=0x%llx\n", status[i]);
		}
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

void cmn600_ras_hnf_err_inj(uint64_t id, uint64_t srcid, uint64_t lpid)
{
	printf("hnf_errinj: hnf_id=%d, hnf_base=0x%llx, srcid=0x%llx, lpid=0x%llx\n", cmn_hnf_ids[id], cmn_bases[cmn_hnf_ids[id]], srcid, lpid);
	cmn_writeq_mask(CMN_hnf_err_inj_srcid(srcid), 
			CMN_hnf_err_inj_srcid(CMN_hnf_err_inj_srcid_MASK), 
			CMN_hnf_err_inj(cmn_bases[cmn_hnf_ids[id]]), 
			"CMN_hnf_err_inj", -1);
	cmn_writeq_mask(CMN_hnf_err_inj_lpid(lpid), 
			CMN_hnf_err_inj_lpid(CMN_hnf_err_inj_lpid_MASK), 
			CMN_hnf_err_inj(cmn_bases[cmn_hnf_ids[id]]), 
			"CMN_hnf_err_inj", -1);
	cmn_setq(CMN_hnf_err_inj_en, 
		 CMN_hnf_err_inj(cmn_bases[cmn_hnf_ids[id]]), 
		 "CMN_hnf_err_inj", -1);
}	

void cmn600_ras_hnf_par_err_inj(uint64_t id, uint64_t lane)
{
	printf("hnf_par_errinj: hnf_id=%d, hnf_base=0x%llx, lane=0x%x\n", cmn_hnf_ids[id], cmn_bases[cmn_hnf_ids[id]], lane);
	cmn_writeq(lane, CMN_hnf_byte_par_err_inj(cmn_bases[cmn_hnf_ids[id]]), "CMN_hnf_byte_par_err_inj", -1);
}

void cmn600_ras_mxp_par_err_inj(uint64_t id, uint64_t port, uint64_t lane)
{
	printf("mxp_par_errinj: xp_id=%d, xp_base=0x%llx, port=%d, lane=0x%x\n", cmn_xp_ids[id], cmn_bases[cmn_xp_ids[id]], port, lane);
	cmn_writeq(lane, CMN_mxp_byte_par_err_inj(cmn_bases[cmn_xp_ids[id]], port), "CMN_mxp_byte_par_err_inj", -1);
}

static int do_cmn_ras(int argc, char *argv[])
{
	cmn_nid_t nid;
	cmn_id_t i;
	caddr_t base;

	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "init") == 0) {
		cmn600_ras_init();
		return 0;
	}
	if (strcmp(argv[1], "einj") == 0) {
		if (argc < 5)
			return -EINVAL;

		nid = (uint16_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[2], "ecc") == 0) {
			uint16_t type = cmn_node_type(cmn_bases[nid]);
			printf("Base: 0x%llx type: %d\n", cmn_bases[nid], type);
			if (type != CMN_RNI && type != CMN_RND && type != CMN_RNF) {
				printf("Invalid RN nid %d\n", nid);
				return -EINVAL;
			}
			uint16_t cpu = (uint16_t)strtoull(argv[3], 0, 0);
			uint16_t clst = cpu / CPUS_PER_CLUSTER;
			uint16_t lpid = cpu % CPUS_PER_CLUSTER;
			for (i = 0; i < cmn_hnf_count; i++) {
				base = cmn_bases[cmn_hnf_ids[i]];
				if (cmn_logical_id(base) == clst) {
					cmn600_ras_hnf_err_inj(i, nid, lpid);
					return 0;
				}
			}
			return -EINVAL;
		}
		if (strcmp(argv[2], "par") == 0) {
			for (i = 0; i < cmn_rn_sam_int_count; i++) {
				base = cmn_bases[cmn_rn_sam_int_ids[i]];
				if (!cmn600_rnsam_is_rnf(cmn_node_id(base)))
					continue;
				if (cmn_node_id(base) == nid)
					break;
			}
			if (i == cmn_rn_sam_int_count) {
				for (i = 0; i < cmn_rn_sam_ext_count; i++) {
					base = cmn_bases[cmn_rn_sam_ext_ids[i]];
					if (!cmn600_rnsam_is_rnf(cmn_node_id(base)))
						continue;
					if (cmn_node_id(base) == nid)
						break;
				}
				if (i == cmn_rn_sam_ext_count) {
					printf("Invalid RN-F nid %d\n", nid);
					return -EINVAL;
				}
			}
			uint64_t lane = (uint64_t)strtoull(argv[5], 0, 0);
			if (lane >= (CMN600_WLEN / 8)) {
				printf("Invalid lane %d >= %d\n", lane, (CMN600_WLEN / 8));
				return -EINVAL;
			}
			if (strcmp(argv[3], "hnf") == 0)
				cmn600_ras_hnf_par_err_inj(nid, lane);
			else
				cmn600_ras_mxp_par_err_inj(cmn600_nid2xp(nid),
							   CMN_PID(nid),
							   lane);
			return 0;
		}
		return -EINVAL;
	}
	return -EINVAL;
}

DEFINE_COMMAND(cmn_ras, do_cmn_ras, "SpacemiT CMN RAS Debug commands",
	"cmn_ras init\n"
	"cmn_ras einj <type>\n"
	"    type:\n"
	"        - ecc <cpu> <rn>\n"
	"        - par <hnf/mxp> <rnf> <lane>\n"
);
