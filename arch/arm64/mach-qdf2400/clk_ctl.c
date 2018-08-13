#include <target/panic.h>
#include <target/bitops.h>
#include <target/muldiv.h>
#include <asm/reg.h>
#include <asm/mach/clk.h>

void __clk_config_root(caddr_t cmd_rcgr, uint8_t mode, uint8_t clk_sel,
		       uint8_t hid, uint16_t m, uint16_t n)
{
	__raw_writel(RCG_MODE(mode) | RCG_SRC_SEL(clk_sel) |
		     RCG_SRC_DIV(hid),
		     cmd_rcgr + RCG_REG_OFFSET_CFG_RCGR);
	__raw_writel(RCG_M(m), cmd_rcgr + RCG_REG_OFFSET_M);
	__raw_writel(RCG_N(~(n - m)), cmd_rcgr + RCG_REG_OFFSET_N);
	__raw_writel(RCG_D(~n), cmd_rcgr + RCG_REG_OFFSET_D);
}

void __clk_generate_root(caddr_t cmd_rcgr, uint8_t mode, uint8_t clk_sel,
			 uint32_t input_hz, uint32_t output_hz)
{
	uint8_t hid = 0;
	uint32_t m, n, n1, g;
	uint32_t output_hz1, output_err, err_last;
	uint32_t div_input_hz;

	/* This is not a frequency multiplexer */
	BUG_ON(output_hz > input_hz);

	/* try to avoid overlows */
	if (input_hz > 65535) {
		g = gcd32u(input_hz, output_hz);
		input_hz /= g;
		output_hz /= g;
	}
	div_input_hz = input_hz;
	while ((div_input_hz / output_hz) > 65535) {
		hid++;
		div_input_hz = RCG_HID_OUT_HZ(input_hz, hid);
	}
	input_hz = div_input_hz;

	/* fast path: GCD */
	if (input_hz < 65536) {
		n = input_hz;
		m = output_hz;
		goto do_cfg_clk;
	}

	/* slow path: calibration */
	n = input_hz / output_hz;
	m = 1;
	err_last = output_hz;
	do {
		output_hz1 = input_hz * m / n;
		if (output_hz1 < output_hz)
			output_err = output_hz - output_hz1;
		else
			output_err = output_hz1 - output_hz;
		if (!output_err)
			break;
		if (output_err < err_last)
			err_last = output_err;
		else
			break;
		n1 = input_hz / output_err;
		if (output_hz1 < output_hz)
			m = n + n1 * m;
		else
			m = n1 * m - n;
		n *= n1;
		while (n > 65535) {
			g = min(__roundup32(n / 65536), m);
			n /= g;
			m /= g;
		}
	} while (1);

do_cfg_clk:
	__clk_config_root(cmd_rcgr, mode, clk_sel, hid, m, n);
}
