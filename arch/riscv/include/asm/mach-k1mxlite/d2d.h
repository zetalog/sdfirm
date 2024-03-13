#ifndef __D2D_K1MXLITE_H_INCLUDE__
#define __D2D_K1MXLITE_H_INCLUDE__

/* ===========================================================================
 * D2D Information:
 * ===========================================================================
 * Stored by firmware from local EFUSE to global registers.
 * 1. CPU core valid
 * 2. BOOT hart ID
 *
 * D2D_REG(0):
 *  31                0
 * --+----------------+
 *   | CPU Core Valid |
 * --+----------------+
 *
 * D2D_REG(1):
 *  31     9           8           7           6           5              0
 * --+-----+-----------+-----------+-----------+-----------+--------------+
 *   | ... | PCIe INIT | N100 INIT | PCIe CCIX | N100 CCIX | Boot Hart ID |
 * --+-----+-----------+-----------+-----------+-----------+--------------+
 *
 * D2D_REG(2):
 *  31     2           1           0
 * --+-----+-----------+-----------+
 *   | ... | DIE1 BOOT | DIE0 BOOT |
 * --+-----+-----------+-----------+
 */
#define D2D_REG(n)		(__D2D_BASE + ((n) << 2))

/* D2D_REG(0) */
#define D2D_BOOTHARTID_OFFSET	0
#define D2D_BOOTHARTID_MASK	REG_5BIT_MASK
#define D2D_BOOTHARTID(value)	_SET_FV(D2D_BOOTHARTID, value)

/* D2D_REG(1) */
#define D2D_N100_CCIX		_BV(5)
#define D2D_PCIE_CCIX		_BV(6)
#define D2D_N100_INIT		_BV(7)
#define D2D_PCIE_INIT		_BV(8)

/* D2D_REG(2) */
#define D2D_DIE0_BOOT		_BV(0) /* DIE0 Boot permission bit */
#define D2D_DIE1_BOOT		_BV(1) /* DIE0 Boot permission bit */

#endif /* __D2D_K1MXLITE_H_INCLUDE__ */
