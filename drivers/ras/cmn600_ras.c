#include <target/noc.h>
#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/arch.h>
#include <target/ras.h>

#define CMN_MODNAME	"cmn_ras"

#define CMN600_WLEN			256

bh_t cmn600_ras_bh;

void cmn600_ras_config(caddr_t base)
{
	uint64_t errfr = __raw_readq(CMN_errfr(base));
	uint8_t ed = CMN_errfr_ED(errfr);
	uint8_t de = CMN_errfr_DE(errfr);
	uint8_t ui = CMN_errfr_UI(errfr);
	uint8_t fi = CMN_errfr_FI(errfr);
	uint8_t cfi = CMN_errfr_CFI(errfr);
	uint8_t cec = CMN_errfr_CEC(errfr);
	uint64_t errfr_NS = __raw_readq(CMN_errfr_NS(base));
	uint8_t ed_NS = CMN_errfr_ED(errfr_NS);
	uint8_t de_NS = CMN_errfr_DE(errfr_NS);
	uint8_t ui_NS = CMN_errfr_UI(errfr_NS);
	uint8_t fi_NS = CMN_errfr_FI(errfr_NS);
	uint8_t cfi_NS = CMN_errfr_CFI(errfr_NS);
	uint8_t cec_NS = CMN_errfr_CEC(errfr_NS);
	con_dbg(CMN_MODNAME ": errfr=%llx, nid=%d, ED=%d, DE=%d, UI=%d, FI=%d, CFI=%d, CEC=%d\n", errfr, cmn_node_id(base), ed, de, ui, fi, cfi, cec);
	con_dbg(CMN_MODNAME ": errfr_NS=%llx, ED=%d, DE=%d, UI=%d, FI=%d, CFI=%d, CEC=%d\n", errfr_NS, ed_NS, de_NS, ui_NS, fi_NS, cfi_NS, cec_NS);
	if (cmn_ras_support_ed(base))
		cmn_ras_enable_ed(base);
	if (cmn_ras_support_ed_NS(base))
		cmn_ras_enable_ed_NS(base);
	if (cmn_ras_support_de(base))
		cmn_ras_enable_de(base);
	if (cmn_ras_support_de_NS(base))
		cmn_ras_enable_de_NS(base);
	if (cmn_ras_support_ui(base))
		cmn_ras_enable_ui(base);
	if (cmn_ras_support_ui_NS(base))
		cmn_ras_enable_ui_NS(base);
	if (cmn_ras_support_fi(base))
		cmn_ras_enable_fi(base);
	if (cmn_ras_support_fi_NS(base))
		cmn_ras_enable_fi_NS(base);	
	if (cmn_ras_support_cfi(base))
		cmn_ras_enable_cfi(base);
	if (cmn_ras_support_cfi_NS(base))
		cmn_ras_enable_cfi_NS(base);
}

void cmn600_ras_report(caddr_t base)
{
	uint64_t status = __raw_readq(CMN_errstatus(base));
	con_dbg(CMN_MODNAME ": ras_report: %llx\n", status);
	if (status & CMN_errstatus_AV)
		con_dbg(CMN_MODNAME ": AV Error: addr=%08llx\n", __raw_readq(CMN_erraddr(base)));
	if (status & CMN_errstatus_V)
		con_log(CMN_MODNAME ": V Error\n");
	if (status & CMN_errstatus_UE)
		con_log(CMN_MODNAME ": UE Error\n");
	if (status & CMN_errstatus_OF)
		con_log(CMN_MODNAME ": OF Error\n");
	if (status & CMN_errstatus_MV)
		con_log(CMN_MODNAME ": MV Error\n");
	if (status & CMN_errstatus_CE)
		con_log(CMN_MODNAME ": CE Error\n");
	if (status & CMN_errstatus_DE)
		con_log(CMN_MODNAME ": DE Error\n");

	cmn_writeq(status, CMN_errstatus(base), "CMN_errstatus", -1);
}

caddr_t CMN_ras_nid(uint8_t ns, uint8_t grp, uint8_t rec, uint8_t idx)
{
	cmn_nid_t nid = 0;
	uint64_t base = 0;
	int i;

	switch (rec)
	{
	case CMN_error_record_mxp:
		for (i = 0; i < CMN_MAX_MXP_COUNT; i++) {
			if (cmn_logical_id(cmn_bases[cmn_xp_ids[i]]) == idx)
				break;
		}
		base = cmn_bases[cmn_xp_ids[i]];
		nid = cmn_node_id(base);
		break;

	case CMN_error_record_hni:
		for (i = 0; i < CMN_MAX_HNI_COUNT; i++) {
			if (cmn_logical_id(cmn_bases[cmn_hni_ids[i]]) == idx)
				break;
		}
		base = cmn_bases[cmn_hni_ids[i]];
		nid = cmn_node_id(base);
		break;

	case CMN_error_record_hnf:
		for (i = 0; i < CMN_MAX_HNF_COUNT; i++) {
			if (cmn_logical_id(cmn_bases[cmn_hnf_ids[i]]) == idx)
				break;
		}
		base = cmn_bases[cmn_hnf_ids[i]];
		nid = cmn_node_id(base);
		break;

	case CMN_error_record_sbsx:
		for (i = 0; i < CMN_MAX_SBSX_COUNT; i++) {
			if (cmn_logical_id(cmn_bases[cmn_sbsx_ids[i]]) == idx)
				break;
		}
		base = cmn_bases[cmn_sbsx_ids[i]];
		nid = cmn_node_id(base);
		break;

	case CMN_error_record_cxg:
		for (i = 0; i < CMN_MAX_CXG_COUNT; i++) {
			if (cmn_logical_id(cmn_bases[cmn_cxha_ids[i]]) == idx)
				break;
		}
		base = cmn_bases[cmn_cxha_ids[i]];
		nid = cmn_node_id(base);
		break;

	default:
		break;
	}
	con_dbg(CMN_MODNAME ": errns = %d, errgrp = %d, errrec = %d, erridx = %d, errnid = %d\n", ns, grp, rec, idx, nid);
	base += ns << 8;
	return base;
}

void cmn600_handle_irq(void)
{
	uint64_t status[5];
	int ns, grp, rec, idx;

	for (ns = 0; ns < 2; ns++) {
		for (grp = 0; grp < 2; grp++) {
			for (rec = 0; rec < 5; rec++) {
				status[rec] = __raw_readq(CMN_cfgm_errgsr(ns, grp, rec));
				if (status[rec] != 0) {
					for (idx = 0; idx < 64; idx++) {
						if (status[rec] & _BV(idx)) {
							cmn600_ras_report(CMN_ras_nid(ns, grp, rec, idx));
						}
					}
				}
			}
		}
	}
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
	irq_register_vector(IRQ_N100_REQERRS_NID0, cmn600_handle_irq);
	irq_register_vector(IRQ_N100_REQFLTS_NID0, cmn600_handle_irq);
	irq_register_vector(IRQ_N100_REQERRNS_NID0, cmn600_handle_irq);
	irq_register_vector(IRQ_N100_REQFLTNS_NID0, cmn600_handle_irq);
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

void cmn600_ras_hnf_err_inj(cmn_nid_t id, cmn_nid_t srcid, uint16_t lpid)
{
	con_dbg(CMN_MODNAME ": hnf_errinj: hnf_id=%d, hnf_base=0x%lx, srcid=0x%x, lpid=0x%x\n", 
		id, cmn600_hnf_base(id), srcid, lpid);
	cmn_writeq_mask(CMN_hnf_err_inj_srcid(srcid), 
			CMN_hnf_err_inj_srcid(CMN_hnf_err_inj_srcid_MASK), 
			CMN_hnf_err_inj(cmn600_hnf_base(id)), 
			"CMN_hnf_err_inj", -1);
	cmn_writeq_mask(CMN_hnf_err_inj_lpid(lpid), 
			CMN_hnf_err_inj_lpid(CMN_hnf_err_inj_lpid_MASK), 
			CMN_hnf_err_inj(cmn600_hnf_base(id)), 
			"CMN_hnf_err_inj", -1);
	cmn_setq(CMN_hnf_err_inj_en, 
		 CMN_hnf_err_inj(cmn600_hnf_base(id)), 
		 "CMN_hnf_err_inj", -1);
}	

void cmn600_ras_hnf_par_err_inj(cmn_nid_t id, uint16_t lane)
{
	con_dbg(CMN_MODNAME ": hnf_par_errinj: hnf_id=%d, hnf_base=0x%lx, lane=0x%x\n", 
		id, cmn600_hnf_base(id), lane);
	cmn_writeq(lane, 
		   CMN_hnf_byte_par_err_inj(cmn600_hnf_base(id)), 
		   "CMN_hnf_byte_par_err_inj", -1);
}

void cmn600_ras_mxp_par_err_inj(cmn_nid_t id, uint16_t port, uint16_t lane)
{
	con_dbg(CMN_MODNAME ": mxp_par_errinj: xp_id=%d, xp_base=0x%lx, port=%d, lane=0x%x\n", 
		id, cmn_bases[cmn600_nid2xp(id)], port, lane);
	cmn_writeq(lane, 
		   CMN_mxp_byte_par_err_inj(cmn600_nid2xp(id), port), 
		   "CMN_mxp_byte_par_err_inj", -1);
}

static int do_cmn_ras(int argc, char *argv[])
{
	uint32_t i;
	uint16_t hnf, cpu, lane;

	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "einj") == 0) {
		if (strcmp(argv[2], "cpu") == 0) {
			uint16_t clst, lpid, rnf;
			if (argc < 6)
				return -EINVAL;
			hnf = (uint16_t)strtoull(argv[3], 0, 0);
			cpu = (uint16_t)strtoull(argv[4], 0, 0);
			lane = (uint16_t)strtoull(argv[5], 0, 0);
			for (i = 0; i < CMN_MAX_HNF_COUNT; i++) {
				if (hnf == cmn_hnf_table[i])
					break;
			}
			if (i == CMN_MAX_HNF_COUNT) {
				con_err(CMN_MODNAME ": Invalid hnf %d\n", hnf);
				return -EINVAL;
			}
			if (cpu > MAX_CPU_NUM) {
				con_err(CMN_MODNAME ": Invalid cpu %d\n", cpu);
				return -EINVAL;
			}
			if (lane >= (CMN600_WLEN / 8)) {
				con_err(CMN_MODNAME ": Invalid lane %x >= %d\n", lane, (CMN600_WLEN / 8));
				return -EINVAL;
			}
			clst = cpu / CPUS_PER_CLUSTER;
			lpid = cpu % CPUS_PER_CLUSTER;
			rnf = cmn_rnf_table[clst];
			cmn600_ras_hnf_err_inj(hnf, rnf, lpid);
			cmn600_ras_hnf_par_err_inj(hnf, lane);
			return 0;
		}
		if (strcmp(argv[2], "dma") == 0) {
			uint16_t axid, rni, portid;
			uint16_t lpid = 0;
			caddr_t base = 0;
			if (argc < 7)
				return -EINVAL;
			hnf = (uint16_t)strtoull(argv[3], 0, 0);
			rni = (uint16_t)strtoull(argv[4], 0, 0);
			lane = (uint16_t)strtoull(argv[5], 0, 0);
			portid = (uint16_t)strtoull(argv[6], 0, 0);
			for (i = 0; i < CMN_MAX_HNF_COUNT; i++) {
				if (hnf == cmn_hnf_table[i])
					break;
			}
			if (i == CMN_MAX_HNF_COUNT) {
				con_err(CMN_MODNAME ": Invalid hnf %d\n", hnf);
				return -EINVAL;
			}
			if (!cmn600_rnsam_is_rni(rni)) {
				con_err(CMN_MODNAME ": Invalid rni %d\n", rni);
				return -EINVAL;
			}
			if (lane >= (CMN600_WLEN / 8)) {
				con_err(CMN_MODNAME ": Invalid lane %x >= %d\n", lane, (CMN600_WLEN / 8));
				return -EINVAL;
			}
			if (argc > 7) {
				axid = (uint16_t)strtoull(argv[7], 0, 0);
				base = cmn600_rni_base(rni);
				lpid = __raw_readq(CMN_rni_s_port_control(base, portid)) & axid & 0x1;
			}
			lpid |= portid << 1;
			cmn600_ras_hnf_err_inj(hnf, rni, lpid);
			cmn600_ras_hnf_par_err_inj(hnf, lane);
			return 0;
		}
		if (strcmp(argv[2], "mxp") == 0) {
			uint16_t clst, rnf;
			if (argc < 5)
				return -EINVAL;
			cpu = (uint16_t)strtoull(argv[3], 0, 0);
			lane = (uint64_t)strtoull(argv[4], 0, 0);
			if (cpu > MAX_CPU_NUM) {
				con_err(CMN_MODNAME ": Invalid cpu %d\n", cpu);
			}
			clst = cpu / CPUS_PER_CLUSTER;
			rnf = cmn_rnf_table[clst];
			if (lane >= (CMN600_WLEN / 8)) {
				con_err(CMN_MODNAME ": Invalid lane %x >= %d\n", lane, (CMN600_WLEN / 8));
				return -EINVAL;
			}
			cmn600_ras_mxp_par_err_inj(rnf,
						CMN_PID(rnf),
						lane);
			return 0;
		}
		return -EINVAL;
	}
	return -EINVAL;
}

DEFINE_COMMAND(cmn_ras, do_cmn_ras, "SpacemiT CMN RAS Debug commands",
	"cmn_ras einj cpu <hnf> <cpu> <lane>\n"
	"  - inject double-bit ECC error for SLC hit from given CPU\n"
	"cmn_ras einj dma <hnf> <rni> <lane> <portid> [axid]\n"
	"  - inject double-bit ECC error for SLC hit from give DMA\n"
	"cmn_ras einj mxp <cpu> <lane>\n"
	"  - inject parity error into XP from CPU\n"
);
