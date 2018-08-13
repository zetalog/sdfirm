#ifndef __QGIC2_QDF2400_H_INCLUDE__
#define __QGIC2_QDF2400_H_INCLUDE__

#define GICD_BASE			(IMC_FABRIC_START + 0x00240000)
#define GICC_BASE			(GICD_BASE + 0x00002000)
#define GIC_PRIORITY_SHIFT		3

#include <asm/gicv2.h>

/* 2.16.1.7.3.4. GICD_ANSACR (0x0FF 6FA4 0020) */
#define GICD_ANSACR			GICD_REG(0x020)
#define GICD_GICD_CGCR			_BV(0) /* NS access to GICD_CGCR */

/* 2.16.1.7.3.5. GICD_CGCR (0x0FF 6FA4 0024) */
#define GICD_CGCR			GICD_REG(0x024)
#define GICD_TOP			_BV(16)
#define GICD_DI_SGI_STATE		_BV(3)
#define GICD_DI_PPI_SPI_STATE		_BV(2)
#define GICD_DI_DEMET			_BV(1)
#define GICD_DI_RD			_BV(0)

/* 2.16.1.7.3.6. GICD_HW_VERSION (0x0FF 6FA4 0030) */
#define GICD_HW_VERSION			GICD_REG(0x030)

/* 2.16.1.7.3.7. GICD_ISR%n% (0xFF6FA40000 + 0x80+0x4 * n (n=[0..18-1])) */
#define GICD_ISR(n)			GICD_1BIT_REG(0x80, n)

/* 2.16.1.7.3.41. GICC_CTLR (0x0FF 6FA4 2000) */
#define GICC_ENABLE_GRP1_NS		_BV(1)
#define GICC_ACKCTL			_BV(2)
#define GICC_S_DEST			_BV(3)
#define GICC_SBPR			_BV(4)
#define GICC_FIQ_BYP_DIS_NS		_BV(7)
#define GICC_IRQ_BYP_DIS_NS		_BV(8)
#define GICC_EOIMODE			GICC_EOI_MODE_NS
#define GICC_EOIMODE_NS			_BV(10)

/* 2.16.1.7.3.42. GICC_PMR (0x0FF 6FA4 2004) */
#define GICC_PRIORITY_NS		_BV(7)

void gic_hw_ctrl_init(void);

#endif /* __QGIC2_QDF2400_H_INCLUDE__ */
