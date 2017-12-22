#ifndef __REG_MPCORE_H_INCLUDE__
#define __REG_MPCORE_H_INCLUDE__

/* Cortex-A9 MPCore Multiprocessor Private Memory Region */
#define MPCORE_SCU		MPCORE_PERIPH(0)	/* Snoop Control Unit */
#define MPCORE_GICC		MPCORE_PERIPH(1)	/* GIC CPU Interface */
#define MPCORE_GTMR		MPCORE_PERIPH(2)	/* Global Timer */
#define MPCORE_PTMR		MPCORE_PERIPH(6)	/* Private timer and watchdog */
#define MPCORE_GICD		MPCORE_PERIPH(16)	/* GIC Distributor */

#endif /* __REG_MPCORE_H_INCLUDE__ */
