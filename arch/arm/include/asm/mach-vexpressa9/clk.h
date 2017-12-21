#ifndef __CLK_VEXPRESSA9_H_INCLUDE__
#define __CLK_VEXPRESSA9_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

#define CLK_AXI			45000	/* AMBA AXI ACLK clock */
#define CLK_CLCDC		23750	/* CLCD controller clock */
#define CLK_TC			66670	/* Test chip reference clock */
#define CLK_MCLK		266000	/* DDR 2 controller */
#define CLK_FAXI		200000	/* Fast AXI matrix */
#define CLK_SAXI		50000	/* Slow AXI matrix */
#define CLK_SMC			50000	/* Motherboad peripheral clock */
#define CLK_FCLK		400000	/* Cortex-A9 MPCore clock */

#define CLK_CPU			CLK_FCLK
#define CLK_MPCORE_PERIPH	(CLK_FCLK/2)

void clk_hw_set_config(uint32_t cfg);

#endif /* __CLK_VEXPRESSA9_H_INCLUDE__ */
