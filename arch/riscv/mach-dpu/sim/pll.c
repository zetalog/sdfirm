#include <target/arch.h>
#include <target/clk.h>
#include <target/panic.h>

#define PLL_REG_RW_MASK				\
	(PLL_REG_READ | PLL_REG_WRITE |		\
	 PLL_REG_SEL(PLL_REG_SEL_MASK) |	\
	 PLL_REG_WDATA(PLL_REG_WDATA_MASK))
#define PLL_REG_RO_MASK				\
	(PLL_REG_IDLE | PLL_REG_INVALID |	\
	 PLL_REG_RDATA(PLL_REG_RDATA_MASK))

#define pll_mfrac(value)	_GET_FV(PLL_MFRAC, value)
#define pll_mint(value)		_GET_FV(PLL_MINT, value)
#define pll_prediv(value)	_GET_FV(PLL_PREDIV, value)
#define pll_divvcor(value)	_GET_FV(PLL_DIVVCOR, value)
#define pll_divvcop(value)	_GET_FV(PLL_DIVVCOP, value)
#define pll_r(value)		_GET_FV(PLL_R, value)
#define pll_p(value)		_GET_FV(PLL_P, value)

#define pll_reg_rdata(value)	_SET_FV(PLL_REG_RDATA, value)
#define pll_reg_wdata(value)	_GET_FV(PLL_REG_WDATA, value)
#define pll_reg_sel(value)	_GET_FV(PLL_REG_SEL, value)

#define PLL_STATE_POWER_DOWN		0
#define PLL_STATE_FAST_STARTUP		1
#define PLL_STATE_SLOW_STARTUP		2
#define PLL_STATE_NORMAL		3
#define PLL_STATE_RELOCK		4
#define PLL_STATE_STANDBY		5

struct pll_ctrl {
	uint32_t cfg0;
	uint32_t cfg1;
	uint32_t status;
	uint32_t clk_cfg;
};

struct dpu_srst {
	uint32_t soft_rst;
	uint32_t cluster_soft_rst;
};

struct pll_reg {
	uint8_t loopcnt1;
	uint8_t loopcnt2;
	uint8_t ctm1;
	uint8_t ctm2;
	uint8_t anareg05;
	uint8_t anareg06;
	uint8_t anareg07;
	uint8_t anareg10;
	uint8_t anareg11;
	uint8_t lock_control;
	uint8_t lock_window;
	uint8_t unlock_window;
	uint8_t lock_counts;
	uint8_t unlock_counts;
	uint8_t scfrac_cnt;
};

struct pll_ctrl pll5ghz_tsmc12ffc[NR_PLLS] = {
	[0] = {
		.cfg0 = 0x0,
		.cfg1 = 0x00400000,
		.status = 0x0,
		.clk_cfg = 0x3,
	},
	[1] = {
		.cfg0 = 0x0,
		.cfg1 = 0x00400000,
		.status = 0x0,
		.clk_cfg = 0x3,
	},
	[2] = {
		.cfg0 = 0x0,
		.cfg1 = 0x00400000,
		.status = 0x0,
		.clk_cfg = 0x3,
	},
	[3] = {
		.cfg0 = 0x0,
		.cfg1 = 0x00400000,
		.status = 0x0,
		.clk_cfg = 0xF,
	},
	[4] = {
		.cfg0 = 0x0,
		.cfg1 = 0x00400000,
		.status = 0x0,
		.clk_cfg = 0x3,
	},
	[5] = {
		.cfg0 = 0x0,
		.cfg1 = 0x00400000,
		.status = 0x0,
		.clk_cfg = 0x3,
	},
};
struct dpu_srst dpu_srst = {
	.soft_rst = _BV(SRST_GPIO) | _BV(SRST_RAM) |
		    _BV(SRST_ROM) | _BV(SRST_TMR) |
		    _BV(SRST_TCSR) | _BV(SRST_IMC) |
		    _BV(SRST_NOC) | _BV(SRST_FLASH) |
		    SRST_SYS | WDT_RST_DIS,
	.cluster_soft_rst = 0xFFFFFFFF,
};
uint32_t pll_reg_access[NR_PLLS] = {
	[0] = PLL_REG_IDLE,
	[1] = PLL_REG_IDLE,
	[2] = PLL_REG_IDLE,
	[3] = PLL_REG_IDLE,
	[4] = PLL_REG_IDLE,
};
unsigned long pll_states[NR_PLLS];
struct pll_reg pll_regs[NR_PLLS];

uint32_t sim_readl(caddr_t a)
{
	int pll;
	int offset;

	if (a >= 0x4014000 && a < 0x4014050) {
		pll = (a & 0xF0) >> 4;
		offset = (a & 0x0F) / 4;
		if (offset == 0)
			return pll5ghz_tsmc12ffc[pll].cfg0;
		if (offset == 1)
			return pll5ghz_tsmc12ffc[pll].cfg1;
		if (offset == 2)
			return pll5ghz_tsmc12ffc[pll].status;
		if (offset == 3)
			return pll5ghz_tsmc12ffc[pll].clk_cfg;
	} else if (a == 0x40140050)
		return dpu_srst.soft_rst;
	else if (a == 0x40140054)
		return dpu_srst.cluster_soft_rst;
	else {
		pll = (int)((a - 0x40140058) / 4);
		return pll_reg_access[pll];
	}
	return 0;
}

void write_pll_cfg0(int pll, uint32_t n)
{
	uint32_t o = pll5ghz_tsmc12ffc[pll].cfg0;

	pll5ghz_tsmc12ffc[pll].cfg0 = n;
	printf("PLL %d:   PREDIV: %d\n", pll,  pll_prediv(n));
	printf("PLL %d:     MINT: %d\n", pll, pll_mint(n));
	printf("PLL %d:    MFRAC: %d\n", pll, pll_mfrac(n));
}

void pll_enter(int pll, unsigned long state)
{
	pll_states[pll] = state;
	switch (state) {
	case PLL_STATE_POWER_DOWN:
		printf("PLL %d: Power-down\n", pll);
		if (!(pll5ghz_tsmc12ffc[pll].cfg1 & PLL_PWRON))
			pll5ghz_tsmc12ffc[pll].status &= ~PLL_LOCKED;
		break;
	case PLL_STATE_SLOW_STARTUP:
		printf("PLL %d: Slow Startup\n", pll);
		pll_enter(pll, PLL_STATE_NORMAL);
		break;
	case PLL_STATE_FAST_STARTUP:
		printf("PLL %d: Fast Startup\n", pll);
		pll_enter(pll, PLL_STATE_NORMAL);
		break;
	case PLL_STATE_NORMAL:
		printf("PLL %d: Normal Operation\n", pll);
		if (pll5ghz_tsmc12ffc[pll].cfg1 & PLL_PWRON)
			pll5ghz_tsmc12ffc[pll].status |= PLL_LOCKED;
		printf("PLL %d: VCO_MODE: %d\n", pll,
		       !!(pll5ghz_tsmc12ffc[pll].cfg1 & PLL_VCO_MODE));
		printf("PLL %d:  LOWFFRQ: %d\n", pll,
		       !!(pll5ghz_tsmc12ffc[pll].cfg1 & PLL_LOWFREQ));
		break;
	case PLL_STATE_STANDBY:
		printf("PLL %d: Standby\n", pll);
		break;
	case PLL_STATE_RELOCK:
		printf("PLL %d: Relock\n", pll);
		pll_enter(pll, PLL_STATE_NORMAL);
		break;
	}
}

void write_pll_cfg1(int pll, uint32_t n)
{
	uint32_t o = pll5ghz_tsmc12ffc[pll].cfg1;

	pll5ghz_tsmc12ffc[pll].cfg1 = n;
	if (!(o & PLL_ENP) && (n & PLL_ENP)) {
		printf("PLL %d:        P: %d\n", pll, pll_p(n));
		printf("PLL %d:  DIVVCOP: %d\n", pll, pll_divvcop(n));
		return;
	}
	if (!(o & PLL_ENR) && (n & PLL_ENR)) {
		printf("PLL %d:        R: %d\n", pll, pll_r(n));
		printf("PLL %d:  DIVVCOR: %d\n", pll, pll_divvcor(n));
	}
	switch (pll_states[pll]) {
	case PLL_STATE_POWER_DOWN:
		if (n & PLL_PWRON) {
			if (!(n & PLL_STANDBY)) {
				if (n & PLL_GEAR_SHIFT)
					pll_enter(pll, PLL_STATE_FAST_STARTUP);
				else
					pll_enter(pll, PLL_STATE_SLOW_STARTUP);
			}
		}
		break;
	case PLL_STATE_SLOW_STARTUP:
		BUG();
		break;
	case PLL_STATE_FAST_STARTUP:
		BUG();
		break;
	case PLL_STATE_NORMAL:
		if (!(n & PLL_PWRON))
			pll_enter(pll, PLL_STATE_POWER_DOWN);
		else if (n & PLL_STANDBY)
			pll_enter(pll, PLL_STATE_STANDBY);
		break;
	case PLL_STATE_RELOCK:
		BUG();
		break;
	case PLL_STATE_STANDBY:
		if (n & PLL_PWRON && !(n & PLL_STANDBY))
			pll_enter(pll, PLL_STATE_RELOCK);
		break;
	}
}

void complete_pll_access(int pll, bool invalid, uint8_t value)
{
	pll_reg_access[pll] = ((invalid ? PLL_REG_INVALID : 0) | PLL_REG_IDLE);
	pll_reg_access[pll] |= pll_reg_rdata(value);
}

void read_pll_reg(int pll, uint8_t offset)
{
	uint8_t value;

	if (!(pll5ghz_tsmc12ffc[pll].cfg0 & PLL_PWRON)) {
		complete_pll_access(pll, true, 0);
		return;
	}
	switch (offset) {
	case PLL_LOOPCNT1:
		value = pll_regs[pll].loopcnt1;
		break;
	case PLL_LOOPCNT2:
		value = pll_regs[pll].loopcnt2;
		break;
	case PLL_CTM1:
		value = pll_regs[pll].ctm1;
		break;
	case PLL_CTM2:
		value = pll_regs[pll].ctm2;
		break;
	case PLL_ANAREG05:
		value = pll_regs[pll].anareg05;
		break;
	case PLL_ANAREG06:
		value = pll_regs[pll].anareg06;
		break;
	case PLL_ANAREG07:
		value = pll_regs[pll].anareg07;
		break;
	case PLL_ANAREG10:
		value = pll_regs[pll].anareg10;
		break;
	case PLL_ANAREG11:
		value = pll_regs[pll].anareg11;
		break;
	case PLL_LOCK_CONTROL:
		value = pll_regs[pll].lock_control;
		break;
	case PLL_LOCK_WINDOW:
		value = pll_regs[pll].lock_window;
		break;
	case PLL_UNLOCK_WINDOW:
		value = pll_regs[pll].unlock_window;
		break;
	case PLL_LOCK_COUNTS:
		value = pll_regs[pll].lock_counts;
		break;
	case PLL_UNLOCK_COUNTS:
		value = pll_regs[pll].unlock_counts;
		break;
	case PLL_SCFRAC_CNT:
		value = pll_regs[pll].scfrac_cnt;
		break;
	default:
		complete_pll_access(pll, true, 0);
		return;
	}
	complete_pll_access(pll, false, value);
}

void write_pll_reg(int pll, uint8_t offset, uint8_t value)
{
	if (!(pll5ghz_tsmc12ffc[pll].cfg0 & PLL_PWRON)) {
		complete_pll_access(pll, true, 0);
		return;
	}
	switch (offset) {
	case PLL_LOOPCNT1:
		pll_regs[pll].loopcnt1 = value;
		break;
	case PLL_LOOPCNT2:
		pll_regs[pll].loopcnt2 = value;
		break;
	case PLL_CTM1:
		pll_regs[pll].ctm1 = value;
		break;
	case PLL_CTM2:
		pll_regs[pll].ctm2 = value;
		break;
	case PLL_ANAREG05:
		pll_regs[pll].anareg05 = value;
		break;
	case PLL_ANAREG06:
		pll_regs[pll].anareg06 = value;
		break;
	case PLL_ANAREG07:
		pll_regs[pll].anareg07 = value;
		break;
	case PLL_ANAREG10:
		pll_regs[pll].anareg10 = value;
		break;
	case PLL_ANAREG11:
		pll_regs[pll].anareg11 = value;
		break;
	case PLL_LOCK_CONTROL:
		pll_regs[pll].lock_control = value;
		break;
	case PLL_LOCK_WINDOW:
		pll_regs[pll].lock_window = value;
		break;
	case PLL_UNLOCK_WINDOW:
		pll_regs[pll].unlock_window = value;
		break;
	case PLL_LOCK_COUNTS:
		pll_regs[pll].lock_counts = value;
		break;
	case PLL_UNLOCK_COUNTS:
		pll_regs[pll].unlock_counts = value;
		break;
	case PLL_SCFRAC_CNT:
		pll_regs[pll].scfrac_cnt = value;
		break;
	default:
		complete_pll_access(pll, true, 0);
		return;
	}
	complete_pll_access(pll, false, 0);
}

void write_pll_access(int pll, uint32_t v)
{
	uint32_t n = v & PLL_REG_RW_MASK;
	uint8_t offset;
	uint8_t value;
	uint32_t o = pll_reg_access[pll];

	if (o & PLL_REG_IDLE) {
		if (n & PLL_REG_READ) {
			pll_reg_access[pll] &= ~PLL_REG_RO_MASK;
			offset = pll_reg_sel(n);
			read_pll_reg(pll, n);
			n |= (pll_reg_access[pll] & PLL_REG_RO_MASK);
		}
		if (n & PLL_REG_WRITE) {
			pll_reg_access[pll] &= ~PLL_REG_RO_MASK;
			offset = pll_reg_sel(n);
			value = pll_reg_wdata(n);
			write_pll_reg(pll, n, value);
			n |= (pll_reg_access[pll] & PLL_REG_RO_MASK);
		}
	}
	pll_reg_access[pll] = n;
}

void sim_writel(uint32_t v, caddr_t a)
{
	int pll;
	int offset;

	if (a >= 0x4014000 && a < 0x4014050) {
		pll = (int)((a & 0xF0) >> 4);
		offset = (int)((a & 0x0F) / 4);
		if (offset == 0)
			write_pll_cfg0(pll, v);
		if (offset == 1)
			write_pll_cfg1(pll, v);
		if (offset == 3)
			pll5ghz_tsmc12ffc[pll].clk_cfg = v;
	} else if (a == 0x40140050)
		dpu_srst.soft_rst = v;
	else if (a == 0x40140054)
		dpu_srst.cluster_soft_rst = v;
	else {
		pll = (int)((a - 0x40140058) / 4);
		write_pll_access(pll, v);
	}
}

void main(void)
{
	board_init_clock();
	printf("====================\n");
	printf("Applying DDR frequency 1\n");
	ddr_apply_freqplan(1);
	clk_enable(ddr0_clk);
	printf("====================\n");
	printf("Applying DDR frequency 0\n");
	clk_set_frequency(ddr0_clk, freqplan_get_frequency(ddr_clk, 0));
}