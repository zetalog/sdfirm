#ifndef __REG_DAUGHTER_VEXPRESSA9_H_INCLUDE__
#define __REG_DAUGHTER_VEXPRESSA9_H_INCLUDE__

/* Daughterboard CoreTile Express A9x4 On-chip Peripherals */
#define CLCDC_CFG		0x10020000	/* PL111: CLCDC Configuration */
#define AXI_RAM			0x10060000	/* AXI RAM */
#define DRAM_CTRL		0x100E0000	/* PL341: Dynamic Memory Controller */
#define SRAM_CTRL		0x100E1000	/* PL354: Static Memory Controller */
#define SYS_CFG_CTRL		0x100E2000	/* System Configuration Controller */
#define DUAL_TIMER		0x100E4000	/* SP804: Dual Timer */
#define WATCHDOG		0x100E5000	/* SP805: Watchdog */
#define TZ_PROT_CTRL		0x100E6000	/* BP147: TrustZone Protection Controller */
#define AXI_FAST		0x100E9000	/* PL301: Fast AXI Matrix */
#define AXI_SLOW		0x100EA000	/* PL301: Slow AXI Matrix */
#define SMC_TZASC		0x100EC000
#define CORE_DBG_APB		0x10200000	/* CoreSight Debug APB */
#define MPCORE_PRIV		0x1E000000	/* Cortex-A9 MPCore Private Memory Region */
#define L2CC_CFG		0x1E00A000	/* PL310: L2CC Config */

/* Cortex-A9 MPCore Multiprocessor Private Memory Region */
#define MPCORE_PERIPH(ao)	(MPCORE_PRIV + ((ao) << 8))
#include <asm/mach/reg-mpcore.h>

#endif /* __REG_DAUGHTER_VEXPRESSA9_H_INCLUDE__ */
