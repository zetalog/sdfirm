/*
 * Author: Lv Zheng <lv.zheng@hxt-semitech.com>
 */
#ifndef __QDSS_QDF2400_H_INCLUDE__
#define __QDSS_QDF2400_H_INCLUDE__

#include <driver/coresight.h>

/* ROM Tables */

/* QDSS APB */
/* Base */
#define ROM_DAP					0x0FF10000000
/* Entries */
#define ROM_CSR					0x0FF10010000 /* QDSSCSR */
#define ROM_STM					0x0FF10020000 /* CXSTM500 */
#define ROM_CTI0				0x0FF10100000
#define ROM_CTI1				0x0FF10110000
#define ROM_CTI2				0x0FF10120000
#define ROM_CTI3				0x0FF10130000
#define ROM_CTI4				0x0FF10140000
#define ROM_CTI5				0x0FF10150000
#define ROM_CTI6				0x0FF10160000
#define ROM_EXTPSEL_1M				0x0FF10300000
#define ROM_EXTPSEL_4M				0x0FF10400000
#define ROM_TPIU				0x0FF10800000
#define ROM_TFUN0				0x0FF10810000
#define ROM_TREPL				0x0FF10860000
#define ROM_ETBETF				0x0FF10870000
#define ROM_ETR					0x0FF10880000
#define ROM_EXTPSEL2_128M			0x0FF11000000 /* -> AJU ROM1 */

/* Amberwing Debug AJU ROM1 */
/* Base */
#define ROM_AJU_ROM1				0x0FF11000000
/* Entries */
#define ROM_ROM1_ROM1				0x0FF18000000
#define ROM_ROM1_ROM2				0x0FF11010000 /* -> AJU ROM2 */
#define ROM_AJU_S0_FUNNEL			0x0FF11020000
#define ROM_AJU_S1_FUNNEL			0x0FF11030000
#define ROM_AJU_S2_FUNNEL			0x0FF11040000
#define ROM_AJU_E0_FUNNEL			0x0FF11050000
#define ROM_AJU_E1_FUNNEL			0x0FF11060000
#define ROM_AJU_W0_FUNNEL			0x0FF11070000
#define ROM_ROM1_A53				0x0FF11200000 /* -> A53 Cortex */
#define ROM_IMC_TPDA				0x0FF11300000
#define ROM_IMC_TPDM0_IMC			0x0FF11310000
#define ROM_IMC_TPDM1_SMMU			0x0FF11320000
#define ROM_IMC_FUNNEL				0x0FF11330000
#define ROM_IMC_DEBUG_UI			0x0FF11340000
#define ROM_IMC_CTI0				0x0FF11380000
#define ROM_IMC_CTI1				0x0FF11390000
#define ROM_IMC_CTI2				0x0FF113A0000
#define ROM_IMC_CTI3				0x0FF113B0000
#define ROM_IMC_CTI4				0x0FF113C0000
#define ROM_IMC_CTI5				0x0FF113D0000
#define ROM_DDR_SW_TPDA				0x0FF11480000
#define ROM_DDR_SW_TPDM0			0x0FF11490000
#define ROM_DDR_SW_CTI0				0x0FF114C0000
#define ROM_DDR_SW_CTI1				0x0FF114D0000
#define ROM_DDR_SW_CTI2				0x0FF114E0000
#define ROM_DDR_SW_CTI3				0x0FF114F0000
#define ROM_DDR_S_TPDA				0x0FF11500000
#define ROM_DDR_S_TPDM0				0x0FF11510000
#define ROM_DDR_S_CTI0				0x0FF11540000
#define ROM_DDR_S_CTI1				0x0FF11550000
#define ROM_DDR_S_CTI2				0x0FF11560000
#define ROM_DDR_S_CTI3				0x0FF11570000
#define ROM_DDR_SE_TPDA				0x0FF11580000
#define ROM_DDR_SE_TPDM0			0x0FF11590000
#define ROM_DDR_SE_CTI0				0x0FF115C0000
#define ROM_DDR_SE_CTI1				0x0FF115D0000
#define ROM_DDR_SE_CTI2				0x0FF115E0000
#define ROM_DDR_SE_CTI3				0x0FF115f0000
#define ROM_CPU_EVENTS_E0_ETB1_FUNNEL_1		0x0FF11600000
#define ROM_CPU_EVENTS_E0_ETB1_FUNNEL_2		0x0FF11610000
#define ROM_CPU_EVENTS_E0_ETB2_FUNNEL_1		0x0FF11620000
#define ROM_CPU_EVENTS_E0_ETB2_FUNNEL_2		0x0FF11630000
#define ROM_CPU_EVENTS_E0_TPDA			0x0FF11640000
#define ROM_CPU_EVENTS_E0_TPDM0			0x0FF11650000
#define ROM_CPU_EVENTS_E0_ETB1_REPLICATOR	0x0FF11660000
#define ROM_CPU_EVENTS_E0_ETB2_REPLICATOR	0x0FF11670000
#define ROM_CPU_EVENTS_E0_CTI0			0x0FF11680000
#define ROM_CPU_EVENTS_E0_CTI1			0x0FF11690000
#define ROM_CPU_EVENTS_E0_ETB1_TMC		0x0FF116C0000
#define ROM_CPU_EVENTS_E0_ETB2_TMC		0x0FF116D0000
#define ROM_CPU_EVENTS_E0_ETF_PRESERVATION	0x0FF116E0000
#define ROM_PERIPH_TPDA				0x0FF11700000
#define ROM_PERIPH_TPDM0_HWEVENTS		0x0FF11710000
#define ROM_PERIPH_TPDM1_SMMU			0x0FF11720000
#define ROM_PERIPH_CTI0				0x0FF11740000
#define ROM_PERIPH_CTI1				0x0FF11750000
#define ROM_PERIPH_CTI2				0x0FF11760000
#define ROM_PERIPH_CTI3				0x0FF11770000
#define ROM_PERIPH_TPDM2_PRNG0			0x0FF11780000
#define ROM_PERIPH_TPDM3_PRNG1			0x0FF11790000
#define ROM_PCIE1_TPDA				0x0FF11800000
#define ROM_PCIE1_TPDM0_SMMU			0x0FF11810000
#define ROM_PCIE1_TPDM1_SMMU			0x0FF11820000
#define ROM_PCIE1_TPDM2_SMMU			0x0FF11830000
#define ROM_PCIE1_TPDM3_SMMU			0x0FF11840000
#define ROM_PCIE1_TPDM4_VBU			0x0FF11850000
#define ROM_PCIE1_TPDM5_HWEVENTS		0x0FF11860000
#define ROM_PCIE1_CTI0				0x0FF11900000
#define ROM_PCIE1_CTI1				0x0FF11910000
#define ROM_PCIE1_CTI2				0x0FF11920000
#define ROM_PCIE1_CTI3				0x0FF11930000
#define ROM_PCIE1_CTI4				0x0FF11940000
#define ROM_PCIE1_CTI5				0x0FF11950000
#define ROM_PCIE1_CTI6				0x0FF11960000
#define ROM_PCIE1_CTI7				0x0FF11970000
#define ROM_PCIE1_CTI8				0x0FF11980000
#define ROM_PCIE0_TPDA				0x0FF11A00000
#define ROM_PCIE0_TPDM0_SMMU			0x0FF11A10000
#define ROM_PCIE0_TPDM1_SMMU			0x0FF11A20000
#define ROM_PCIE0_TPDM2_SMMU			0x0FF11A30000
#define ROM_PCIE0_TPDM3_SMMU			0x0FF11A40000
#define ROM_PCIE0_TPDM4_VBU			0x0FF11A50000
#define ROM_PCIE0_TPDM5_HWEVENTS		0x0FF11A60000
#define ROM_PCIE0_CTI0				0x0FF11B00000
#define ROM_PCIE0_CTI1				0x0FF11B10000
#define ROM_PCIE0_CTI2				0x0FF11B20000
#define ROM_PCIE0_CTI3				0x0FF11B30000
#define ROM_PCIE0_CTI4				0x0FF11B40000
#define ROM_PCIE0_CTI5				0x0FF11B50000
#define ROM_PCIE0_CTI6				0x0FF11B60000
#define ROM_PCIE0_CTI7				0x0FF11B70000
#define ROM_PCIE0_CTI8				0x0FF11B80000
#define ROM_CPU_EVENTS_E1_ETB1_FUNNEL_1		0x0FF11C00000
#define ROM_CPU_EVENTS_E1_ETB1_FUNNEL_2		0x0FF11C10000
#define ROM_CPU_EVENTS_E1_ETB2_FUNNEL_1		0x0FF11C20000
#define ROM_CPU_EVENTS_E1_ETB2_FUNNEL_2		0x0FF11C30000
#define ROM_CPU_EVENTS_E1_TPDA			0x0FF11C40000
#define ROM_CPU_EVENTS_E1_TPDM0			0x0FF11C50000
#define ROM_CPU_EVENTS_E1_ETB1_REPLICATOR	0x0FF11C60000
#define ROM_CPU_EVENTS_E1_ETB2_REPLICATOR	0x0FF11C70000
#define ROM_CPU_EVENTS_E1_CTI0			0x0FF11C80000
#define ROM_CPU_EVENTS_E1_CTI1			0x0FF11C90000
#define ROM_CPU_EVENTS_E1_ETB1_TMC		0x0FF11CC0000
#define ROM_CPU_EVENTS_E1_ETB2_TMC		0x0FF11CD0000
#define ROM_CPU_EVENTS_E1_ETF_PRESERVATION	0x0FF11CE0000
#define ROM_AJU_ROM1_RING_ROM			0x0FF12000000 /* RING ROM */
#define ROM_DFTSU_CTI0				0x0FF12010000
#define ROM_DFTSU_CFG				0x0FF12020000
#define ROM_PBSU2_DBG				0x0FF12040000
#define ROM_PBSU2_PMU				0x0FF12050000
#define ROM_PBSU2_CTI0				0x0FF12060000
#define ROM_PBSU3_DBG				0x0FF12080000
#define ROM_PBSU3_PMU				0x0FF12090000
#define ROM_PBSU3_CTI0				0x0FF120A0000
#define ROM_PBSU4_DBG				0x0FF120C0000
#define ROM_PBSU4_PMU				0x0FF120D0000
#define ROM_PBSU4_CTI0				0x0FF120E0000
#define ROM_MBSU0_DBG				0x0FF12100000
#define ROM_MBSU0_PMU				0x0FF12110000
#define ROM_L30_DBG				0x0FF12120000
#define ROM_L30_PMU				0x0FF12130000
#define ROM_MBSU0_CTI0				0x0FF12140000
#define ROM_L30_CTI0				0x0FF12150000
#define ROM_MBSU1_DBG				0x0FF12180000
#define ROM_MBSU1_PMU				0x0FF12190000
#define ROM_L31_DBG				0x0FF121A0000
#define ROM_L31_PMU				0x0FF121B0000
#define ROM_MBSU1_CTI0				0x0FF121C0000
#define ROM_L31_CTI0				0x0FF121D0000
#define ROM_MBSU2_DBG				0x0FF12200000
#define ROM_MBSU2_PMU				0x0FF12210000
#define ROM_L32_DBG				0x0FF12220000
#define ROM_L32_PMU				0x0FF12230000
#define ROM_MBSU2_CTI0				0x0FF12240000
#define ROM_L32_CTI0				0x0FF12250000
#define ROM_MBSU3_DBG				0x0FF12280000
#define ROM_MBSU3_PMU				0x0FF12290000
#define ROM_L33_DBG				0x0FF122A0000
#define ROM_L33_PMU				0x0FF122B0000
#define ROM_MBSU3_CTI0				0x0FF122C0000
#define ROM_L33_CTI0				0x0FF122D0000
#define ROM_MBSU4_DBG				0x0FF12300000
#define ROM_MBSU4_PMU				0x0FF12310000
#define ROM_L34_DBG				0x0FF12320000
#define ROM_L34_PMU				0x0FF12330000
#define ROM_MBSU4_CTI0				0x0FF12340000
#define ROM_L34_CTI0				0x0FF12350000
#define ROM_MBSU5_DBG				0x0FF12380000
#define ROM_MBSU5_PMU				0x0FF12390000
#define ROM_L35_DBG				0x0FF123A0000
#define ROM_L35_PMU				0x0FF123B0000
#define ROM_MBSU5_CTI0				0x0FF123C0000
#define ROM_L35_CTI0				0x0FF123D0000
#define ROM_MCSU0_DBG				0x0FF12400000
#define ROM_MCSU0_PMU				0x0FF12410000
#define ROM_MCSU0_CTI0				0x0FF12420000
#define ROM_MCSU1_DBG				0x0FF12440000
#define ROM_MCSU1_PMU				0x0FF12450000
#define ROM_MCSU1_CTI0				0x0FF12460000
#define ROM_MCSU2_DBG				0x0FF12480000
#define ROM_MCSU2_PMU				0x0FF12490000
#define ROM_MCSU2_CTI0				0x0FF124A0000
#define ROM_PBSU0_DBG				0x0FF124C0000
#define ROM_PBSU0_PMU				0x0FF124D0000
#define ROM_PBSU0_CTI0				0x0FF124E0000
#define ROM_DMSU5_DBG				0x0FF12600000
#define ROM_DMSU5_PMU				0x0FF12610000
#define ROM_DMSU5_CTI0				0x0FF12620000
#define ROM_DMSU4_DBG				0x0FF12640000
#define ROM_DMSU4_PMU				0x0FF12650000
#define ROM_DMSU4_CTI0				0x0FF12660000
#define ROM_DMSU3_DBG				0x0FF12680000
#define ROM_DMSU3_PMU				0x0FF12690000
#define ROM_DMSU3_CTI0				0x0FF126A0000
#define ROM_DMSU2_DBG				0x0FF126C0000
#define ROM_DMSU2_PMU				0x0FF126D0000
#define ROM_DMSU2_CTI0				0x0FF126E0000
#define ROM_DMSU1_DBG				0x0FF12700000
#define ROM_DMSU1_PMU				0x0FF12710000
#define ROM_DMSU1_CTI0				0x0FF12720000
#define ROM_DMSU0_DBG				0x0FF12740000
#define ROM_DMSU0_PMU				0x0FF12750000
#define ROM_DMSU0_CTI0				0x0FF12760000
#define ROM_MBSU11_DBG				0x0FF12800000
#define ROM_MBSU11_PMU				0x0FF12810000
#define ROM_L311_DBG				0x0FF12820000
#define ROM_L311_PMU				0x0FF12830000
#define ROM_MBSU11_CTI0				0x0FF12840000
#define ROM_L311_CTI0				0x0FF12850000
#define ROM_MBSU10_DBG				0x0FF12880000
#define ROM_MBSU10_PMU				0x0FF12890000
#define ROM_L310_DBG				0x0FF128A0000
#define ROM_L310_PMU				0x0FF128B0000
#define ROM_MBSU10_CTI0				0x0FF128C0000
#define ROM_L310_CTI0				0x0FF128D0000
#define ROM_MBSU9_DBG				0x0FF12900000
#define ROM_MBSU9_PMU				0x0FF12910000
#define ROM_L39_DBG				0x0FF12920000
#define ROM_L39_PMU				0x0FF12930000
#define ROM_MBSU9_CTI0				0x0FF12940000
#define ROM_L39_CTI0				0x0FF12950000
#define ROM_MBSU8_DBG				0x0FF12980000
#define ROM_MBSU8_PMU				0x0FF12990000
#define ROM_L38_DBG				0x0FF129A0000
#define ROM_L38_PMU				0x0FF129B0000
#define ROM_MBSU8_CTI0				0x0FF129C0000
#define ROM_L38_CTI0				0x0FF129D0000
#define ROM_MBSU7_DBG				0x0FF12A00000
#define ROM_MBSU7_PMU				0x0FF12A10000
#define ROM_L37_DBG				0x0FF12A20000
#define ROM_L37_PMU				0x0FF12A30000
#define ROM_MBSU7_CTI0				0x0FF12A40000
#define ROM_L37_CTI0				0x0FF12A50000
#define ROM_MBSU6_DBG				0x0FF12A80000
#define ROM_MBSU6_PMU				0x0FF12A90000
#define ROM_L36_DBG				0x0FF12AA0000
#define ROM_L36_PMU				0x0FF12AB0000
#define ROM_MBSU6_CTI0				0x0FF12AC0000
#define ROM_L36_CTI0				0x0FF12AD0000
#define ROM_MCSU5_DBG				0x0FF12B00000
#define ROM_MCSU5_PMU				0x0FF12B10000
#define ROM_MCSU5_CTI0				0x0FF12B20000
#define ROM_MCSU4_DBG				0x0FF12B40000
#define ROM_MCSU4_PMU				0x0FF12B50000
#define ROM_MCSU4_CTI0				0x0FF12B60000
#define ROM_MCSU3_DBG				0x0FF12B80000
#define ROM_MCSU3_PMU				0x0FF12B90000
#define ROM_MCSU3_CTI0				0x0FF12BA0000
#define ROM_PBSU1_DBG				0x0FF12C00000
#define ROM_PBSU1_PMU				0x0FF12C10000
#define ROM_PBSU1_CTI0				0x0FF12C20000

/* Amberwing Debug AJU ROM2 */
/* Base */
#define ROM_AJU_ROM2				0x0FF11010000
/* Entries */
#define ROM_AJU_ROM2_ROM2			0x0FF19010000
#define ROM_AJU_QDSS_REPLICATOR			0x0FF13010000
#define ROM_AJU_W1_FUNNEL			0x0FF13020000
#define ROM_AJU_W2_FUNNEL			0x0FF13030000
#define ROM_AJU_W3_FUNNEL			0x0FF13040000
#define ROM_AJU_N0_FUNNEL			0x0FF13050000
#define ROM_AJU_N1_FUNNEL			0x0FF13060000
#define ROM_AJU_N2_FUNNEL			0x0FF13070000
#define ROM_AJU_QDSS_FUNNEL			0x0FF13080000
#define ROM_WEST_TPDA				0x0FF13100000
#define ROM_WEST_TPDM0_SYS_VBU			0x0FF13110000
#define ROM_WEST_TPDM1_TSENS			0x0FF13120000
#define ROM_WEST_CTI0				0x0FF13180000
#define ROM_WEST_CTI1				0x0FF13190000
#define ROM_WEST_CTI2				0x0FF131A0000
#define ROM_WEST_CTI3				0x0FF131B0000
#define ROM_WEST_CTI4				0x0FF131C0000
#define ROM_WEST_CTI5				0x0FF131D0000
#define ROM_WEST_CTI6				0x0FF131E0000
#define ROM_WEST_CTI7				0x0FF131F0000
#define ROM_CPU_EVENTS_W0_ETB1_FUNNEL_1		0x0FF13200000
#define ROM_CPU_EVENTS_W0_ETB1_FUNNEL_2		0x0FF13210000
#define ROM_CPU_EVENTS_W0_ETB2_FUNNEL_1		0x0FF13220000
#define ROM_CPU_EVENTS_W0_ETB2_FUNNEL_2		0x0FF13230000
#define ROM_CPU_EVENTS_W0_TPDA			0x0FF13240000
#define ROM_CPU_EVENTS_W0_TPDM0			0x0FF13250000
#define ROM_CPU_EVENTS_W0_ETB1_REPLICATOR	0x0FF13260000
#define ROM_CPU_EVENTS_W0_ETB2_REPLICATOR	0x0FF13270000
#define ROM_CPU_EVENTS_W0_CTI0			0x0FF13280000
#define ROM_CPU_EVENTS_W0_CTI1			0x0FF13290000
#define ROM_CPU_EVENTS_W0_ETB1_TMC		0x0FF132C0000
#define ROM_CPU_EVENTS_W0_ETB2_TMC		0x0FF132D0000
#define ROM_CPU_EVENTS_W0_ETF_PRESERVATION	0x0FF132E0000
#define ROM_CRYPTO_TPDA				0x0FF13300000
#define ROM_CRYPTO_TPDM0_SMMU			0x0FF13310000
#define ROM_CRYPTO_TPDM1_SMMU			0x0FF13320000
#define ROM_CRYPTO_CTI0				0x0FF13340000
#define ROM_CRYPTO_CTI1				0x0FF13350000
#define ROM_CRYPTO_CTI2				0x0FF13360000
#define ROM_EMAC_TPDA				0x0FF13380000
#define ROM_EMAC_TPDM0_CPU_EVENTS		0x0FF13390000
#define ROM_EMAC_TPDM1_SMMU_EMAC0		0x0FF133A0000
#define ROM_EMAC_TPDM2_SMMU_EMAC1		0x0FF133B0000
#define ROM_EMAC_CTI0				0x0FF133C0000
#define ROM_EMAC_CTI1				0x0FF133D0000
#define ROM_EMAC_CTI2				0x0FF133E0000
#define ROM_EMAC_CTI3				0x0FF133F0000
#define ROM_CPU_EVENTS_W1_ETB1_FUNNEL_1		0x0FF13400000
#define ROM_CPU_EVENTS_W1_ETB1_FUNNEL_2		0x0FF13410000
#define ROM_CPU_EVENTS_W1_ETB2_FUNNEL_1		0x0FF13420000
#define ROM_CPU_EVENTS_W1_ETB2_FUNNEL_2		0x0FF13430000
#define ROM_CPU_EVENTS_W1_TPDA			0x0FF13440000
#define ROM_CPU_EVENTS_W1_TPDM0			0x0FF13450000
#define ROM_CPU_EVENTS_W1_ETB1_REPLICATOR	0x0FF13460000
#define ROM_CPU_EVENTS_W1_ETB2_REPLICATOR	0x0FF13470000
#define ROM_CPU_EVENTS_W1_CTI0			0x0FF13480000
#define ROM_CPU_EVENTS_W1_CTI1			0x0FF13490000
#define ROM_CPU_EVENTS_W1_ETB1_TMC		0x0FF134C0000
#define ROM_CPU_EVENTS_W1_ETB2_TMC		0x0FF134D0000
#define ROM_CPU_EVENTS_W1_ETF_PRESERVATION	0x0FF134E0000
#define ROM_HIDMA_TPDA				0x0FF13600000
#define ROM_HIDMA_TPDM0_SMMU			0x0FF13610000
#define ROM_HIDMA_TPDM1_SMMU			0x0FF13620000
#define ROM_HIDMA_TPDM2_SMMU			0x0FF13630000
#define ROM_HIDMA_TPDM3_SMMU			0x0FF13640000
#define ROM_HIDMA_TPDM4_VBU			0x0FF13650000
#define ROM_HIDMA_TPDM5_HWEVENTS		0x0FF13660000
#define ROM_HIDMA_CTI0				0x0FF13700000
#define ROM_HIDMA_CTI1				0x0FF13710000
#define ROM_HIDMA_CTI2				0x0FF13720000
#define ROM_HIDMA_CTI3				0x0FF13730000
#define ROM_HIDMA_CTI4				0x0FF13740000
#define ROM_HIDMA_CTI5				0x0FF13750000
#define ROM_HIDMA_CTI6				0x0FF13760000
#define ROM_HIDMA_CTI7				0x0FF13770000
#define ROM_HIDMA_CTI8				0x0FF13780000
#define ROM_SATA_TPDA				0x0FF13800000
#define ROM_SATA_TPDM0_SMMU0			0x0FF13810000
#define ROM_SATA_TPDM1_SMMU1			0x0FF13820000
#define ROM_SATA_TPDM2_SMMU2			0x0FF13830000
#define ROM_SATA_TPDM3_SMMU3			0x0FF13840000
#define ROM_SATA_TPDM4_SMMU4			0x0FF13850000
#define ROM_SATA_TPDM5_SMMU5			0x0FF13860000
#define ROM_SATA_TPDM6_SMMU6			0x0FF13870000
#define ROM_SATA_TPDM7_SMMU7			0x0FF13880000
#define ROM_SATA_TPDM8_VBU			0x0FF13890000
#define ROM_SATA_TPDM9_HWEVENTS			0x0FF138A0000
#define ROM_SATA_CTI0				0x0FF13900000
#define ROM_SATA_CTI1				0x0FF13910000
#define ROM_SATA_CTI2				0x0FF13920000
#define ROM_SATA_CTI3				0x0FF13930000
#define ROM_SATA_CTI4				0x0FF13940000
#define ROM_SATA_CTI5				0x0FF13950000
#define ROM_SATA_CTI6				0x0FF13960000
#define ROM_SATA_CTI7				0x0FF13970000
#define ROM_SATA_CTI8				0x0FF13980000
#define ROM_SATA_CTI9				0x0FF13990000
#define ROM_SATA_CTI10				0x0FF139A0000
#define ROM_SATA_CTI11				0x0FF139B0000
#define ROM_SATA_CTI12				0x0FF139C0000
#define ROM_SATA_CTI13				0x0FF139D0000
#define ROM_DDR_NW_TPDA				0x0FF13A00000
#define ROM_DDR_NW_TPDM0			0x0FF13A10000
#define ROM_DDR_NW_CTI0				0x0FF13A40000
#define ROM_DDR_NW_CTI1				0x0FF13A50000
#define ROM_DDR_NW_CTI2				0x0FF13A60000
#define ROM_DDR_NW_CTI3				0x0FF13A70000
#define ROM_DDR_N_TPDA				0x0FF13A80000
#define ROM_DDR_N_TPDM0				0x0FF13A90000
#define ROM_DDR_N_CTI0				0x0FF13AC0000
#define ROM_DDR_N_CTI1				0x0FF13AD0000
#define ROM_DDR_N_CTI2				0x0FF13AE0000
#define ROM_DDR_N_CTI3				0x0FF13AF0000
#define ROM_DDR_NE_TPDA				0x0FF13B00000
#define ROM_DDR_NE_TPDM0			0x0FF13B10000
#define ROM_DDR_NE_CTI0				0x0FF13B40000
#define ROM_DDR_NE_CTI1				0x0FF13B50000
#define ROM_DDR_NE_CTI2				0x0FF13B60000
#define ROM_DDR_NE_CTI3				0x0FF13B70000
#define ROM_APC0_CPU0_DBG			0x0FF14010000
#define ROM_APC0_CPU0_CTI			0x0FF14020000
#define ROM_APC0_CPU0_PMU			0x0FF14030000
#define ROM_APC0_CPU0_TRC			0x0FF14040000
#define ROM_APC0_FUNNEL				0x0FF140C0000
#define ROM_APC0_CPU1_DBG			0x0FF14110000
#define ROM_APC0_CPU1_CTI			0x0FF14120000
#define ROM_APC0_CPU1_PMU			0x0FF14130000
#define ROM_APC0_CPU1_TRC			0x0FF14140000
#define ROM_APC0_CPU0_PMU_CTI			0x0FF141C0000
#define ROM_APC0_CPU1_PMU_CTI			0x0FF141D0000
#define ROM_APC0_L2_PMU_CTI			0x0FF141E0000
#define ROM_APC0_LLM_CTI			0x0FF141F0000
#define ROM_APC1_CPU0_DBG			0x0FF14210000
#define ROM_APC1_CPU0_CTI			0x0FF14220000
#define ROM_APC1_CPU0_PMU			0x0FF14230000
#define ROM_APC1_CPU0_TRC			0x0FF14240000
#define ROM_APC1_FUNNEL				0x0FF142C0000
#define ROM_APC1_CPU1_DBG			0x0FF14310000
#define ROM_APC1_CPU1_CTI			0x0FF14320000
#define ROM_APC1_CPU1_PMU			0x0FF14330000
#define ROM_APC1_CPU1_TRC			0x0FF14340000
#define ROM_APC1_CPU0_PMU_CTI			0x0FF143C0000
#define ROM_APC1_CPU1_PMU_CTI			0x0FF143D0000
#define ROM_APC1_L2_PMU_CTI			0x0FF143E0000
#define ROM_APC1_LLM_CTI			0x0FF143F0000
#define ROM_APC2_CPU0_DBG			0x0FF14410000
#define ROM_APC2_CPU0_CTI			0x0FF14420000
#define ROM_APC2_CPU0_PMU			0x0FF14430000
#define ROM_APC2_CPU0_TRC			0x0FF14440000
#define ROM_APC2_FUNNEL				0x0FF144C0000
#define ROM_APC2_CPU1_DBG			0x0FF14510000
#define ROM_APC2_CPU1_CTI			0x0FF14520000
#define ROM_APC2_CPU1_PMU			0x0FF14530000
#define ROM_APC2_CPU1_TRC			0x0FF14540000
#define ROM_APC2_CPU0_PMU_CTI			0x0FF145C0000
#define ROM_APC2_CPU1_PMU_CTI			0x0FF145D0000
#define ROM_APC2_L2_PMU_CTI			0x0FF145E0000
#define ROM_APC2_LLM_CTI			0x0FF145F0000
#define ROM_APC3_CPU0_DBG			0x0FF14610000
#define ROM_APC3_CPU0_CTI			0x0FF14620000
#define ROM_APC3_CPU0_PMU			0x0FF14630000
#define ROM_APC3_CPU0_TRC			0x0FF14640000
#define ROM_APC3_FUNNEL				0x0FF146C0000
#define ROM_APC3_CPU1_DBG			0x0FF14710000
#define ROM_APC3_CPU1_CTI			0x0FF14720000
#define ROM_APC3_CPU1_PMU			0x0FF14730000
#define ROM_APC3_CPU1_TRC			0x0FF14740000
#define ROM_APC3_CPU0_PMU_CTI			0x0FF147C0000
#define ROM_APC3_CPU1_PMU_CTI			0x0FF147D0000
#define ROM_APC3_L2_PMU_CTI			0x0FF147E0000
#define ROM_APC3_LLM_CTI			0x0FF147F0000
#define ROM_APC4_CPU0_DBG			0x0FF14810000
#define ROM_APC4_CPU0_CTI			0x0FF14820000
#define ROM_APC4_CPU0_PMU			0x0FF14830000
#define ROM_APC4_CPU0_TRC			0x0FF14840000
#define ROM_APC4_FUNNEL				0x0FF148C0000
#define ROM_APC4_CPU1_DBG			0x0FF14910000
#define ROM_APC4_CPU1_CTI			0x0FF14920000
#define ROM_APC4_CPU1_PMU			0x0FF14930000
#define ROM_APC4_CPU1_TRC			0x0FF14940000
#define ROM_APC4_CPU0_PMU_CTI			0x0FF149C0000
#define ROM_APC4_CPU1_PMU_CTI			0x0FF149D0000
#define ROM_APC4_L2_PMU_CTI			0x0FF149E0000
#define ROM_APC4_LLM_CTI			0x0FF149F0000
#define ROM_APC5_CPU0_DBG			0x0FF14A10000
#define ROM_APC5_CPU0_CTI			0x0FF14A20000
#define ROM_APC5_CPU0_PMU			0x0FF14A30000
#define ROM_APC5_CPU0_TRC			0x0FF14A40000
#define ROM_APC5_FUNNEL				0x0FF14AC0000
#define ROM_APC5_CPU1_DBG			0x0FF14B10000
#define ROM_APC5_CPU1_CTI			0x0FF14B20000
#define ROM_APC5_CPU1_PMU			0x0FF14B30000
#define ROM_APC5_CPU1_TRC			0x0FF14B40000
#define ROM_APC5_CPU0_PMU_CTI			0x0FF14BC0000
#define ROM_APC5_CPU1_PMU_CTI			0x0FF14BD0000
#define ROM_APC5_L2_PMU_CTI			0x0FF14BE0000
#define ROM_APC5_LLM_CTI			0x0FF14BF0000
#define ROM_APC6_CPU0_DBG			0x0FF14C10000
#define ROM_APC6_CPU0_CTI			0x0FF14C20000
#define ROM_APC6_CPU0_PMU			0x0FF14C30000
#define ROM_APC6_CPU0_TRC			0x0FF14C40000
#define ROM_APC6_FUNNEL				0x0FF14CC0000
#define ROM_APC6_CPU1_DBG			0x0FF14D10000
#define ROM_APC6_CPU1_CTI			0x0FF14D20000
#define ROM_APC6_CPU1_PMU			0x0FF14D30000
#define ROM_APC6_CPU1_TRC			0x0FF14D40000
#define ROM_APC6_CPU0_PMU_CTI			0x0FF14DC0000
#define ROM_APC6_CPU1_PMU_CTI			0x0FF14DD0000
#define ROM_APC6_L2_PMU_CTI			0x0FF14DE0000
#define ROM_APC6_LLM_CTI			0x0FF14DF0000
#define ROM_APC7_CPU0_DBG			0x0FF14E10000
#define ROM_APC7_CPU0_CTI			0x0FF14E20000
#define ROM_APC7_CPU0_PMU			0x0FF14E30000
#define ROM_APC7_CPU0_TRC			0x0FF14E40000
#define ROM_APC7_FUNNEL				0x0FF14EC0000
#define ROM_APC7_CPU1_DBG			0x0FF14F10000
#define ROM_APC7_CPU1_CTI			0x0FF14F20000
#define ROM_APC7_CPU1_PMU			0x0FF14F30000
#define ROM_APC7_CPU1_TRC			0x0FF14F40000
#define ROM_APC7_CPU0_PMU_CTI			0x0FF14FC0000
#define ROM_APC7_CPU1_PMU_CTI			0x0FF14FD0000
#define ROM_APC7_L2_PMU_CTI			0x0FF14FE0000
#define ROM_APC7_LLM_CTI			0x0FF14FF0000
#define ROM_APC8_CPU0_DBG			0x0FF15010000
#define ROM_APC8_CPU0_CTI			0x0FF15020000
#define ROM_APC8_CPU0_PMU			0x0FF15030000
#define ROM_APC8_CPU0_TRC			0x0FF15040000
#define ROM_APC8_FUNNEL				0x0FF150C0000
#define ROM_APC8_CPU1_DBG			0x0FF15110000
#define ROM_APC8_CPU1_CTI			0x0FF15120000
#define ROM_APC8_CPU1_PMU			0x0FF15130000
#define ROM_APC8_CPU1_TRC			0x0FF15140000
#define ROM_APC8_CPU0_PMU_CTI			0x0FF151C0000
#define ROM_APC8_CPU1_PMU_CTI			0x0FF151D0000
#define ROM_APC8_L2_PMU_CTI			0x0FF151E0000
#define ROM_APC8_LLM_CTI			0x0FF151F0000
#define ROM_APC9_CPU0_DBG			0x0FF15210000
#define ROM_APC9_CPU0_CTI			0x0FF15220000
#define ROM_APC9_CPU0_PMU			0x0FF15230000
#define ROM_APC9_CPU0_TRC			0x0FF15240000
#define ROM_APC9_FUNNEL				0x0FF152C0000
#define ROM_APC9_CPU1_DBG			0x0FF15310000
#define ROM_APC9_CPU1_CTI			0x0FF15320000
#define ROM_APC9_CPU1_PMU			0x0FF15330000
#define ROM_APC9_CPU1_TRC			0x0FF15340000
#define ROM_APC9_CPU0_PMU_CTI			0x0FF153C0000
#define ROM_APC9_CPU1_PMU_CTI			0x0FF153D0000
#define ROM_APC9_L2_PMU_CTI			0x0FF153E0000
#define ROM_APC9_LLM_CTI			0x0FF153F0000
#define ROM_APC10_CPU0_DBG			0x0FF15410000
#define ROM_APC10_CPU0_CTI			0x0FF15420000
#define ROM_APC10_CPU0_PMU			0x0FF15430000
#define ROM_APC10_CPU0_TRC			0x0FF15440000
#define ROM_APC10_FUNNEL			0x0FF154C0000
#define ROM_APC10_CPU1_DBG			0x0FF15510000
#define ROM_APC10_CPU1_CTI			0x0FF15520000
#define ROM_APC10_CPU1_PMU			0x0FF15530000
#define ROM_APC10_CPU1_TRC			0x0FF15540000
#define ROM_APC10_CPU0_PMU_CTI			0x0FF155C0000
#define ROM_APC10_CPU1_PMU_CTI			0x0FF155D0000
#define ROM_APC10_L2_PMU_CTI			0x0FF155E0000
#define ROM_APC10_LLM_CTI			0x0FF155F0000
#define ROM_APC11_CPU0_DBG			0x0FF15610000
#define ROM_APC11_CPU0_CTI			0x0FF15620000
#define ROM_APC11_CPU0_PMU			0x0FF15630000
#define ROM_APC11_CPU0_TRC			0x0FF15640000
#define ROM_APC11_FUNNEL			0x0FF156C0000
#define ROM_APC11_CPU1_DBG			0x0FF15710000
#define ROM_APC11_CPU1_CTI			0x0FF15720000
#define ROM_APC11_CPU1_PMU			0x0FF15730000
#define ROM_APC11_CPU1_TRC			0x0FF15740000
#define ROM_APC11_CPU0_PMU_CTI			0x0FF157C0000
#define ROM_APC11_CPU1_PMU_CTI			0x0FF157D0000
#define ROM_APC11_L2_PMU_CTI			0x0FF157E0000
#define ROM_APC11_LLM_CTI			0x0FF157F0000
#define ROM_HMSS_NCHNL_FUNNEL_W1_0		0x0FF15800000
#define ROM_HMSS_NCHNL_FUNNEL_W1_1		0x0FF15810000
#define ROM_HMSS_NCHNL_FUNNEL_W2_0		0x0FF15820000
#define ROM_HMSS_NCHNL_FUNNEL_W2_1		0x0FF15830000
#define ROM_HMSS_NCHNL_FUNNEL_W3_0		0x0FF15840000
#define ROM_HMSS_NCHNL_FUNNEL_W3_1		0x0FF15850000
#define ROM_HMSS_NCHNL_FUNNEL_E1_0		0x0FF15860000
#define ROM_HMSS_NCHNL_FUNNEL_E1_1		0x0FF15870000
#define ROM_HMSS_NCHNL_FUNNEL_E2_0		0x0FF15880000
#define ROM_HMSS_NCHNL_FUNNEL_E2_1		0x0FF15890000
#define ROM_HMSS_NCHNL_FUNNEL_E3_0		0x0FF158A0000
#define ROM_HMSS_NCHNL_FUNNEL_E3_1		0x0FF158B0000
#define ROM_APC12_CPU0_DBG			0x0FF16010000
#define ROM_APC12_CPU0_CTI			0x0FF16020000
#define ROM_APC12_CPU0_PMU			0x0FF16030000
#define ROM_APC12_CPU0_TRC			0x0FF16040000
#define ROM_APC12_FUNNEL			0x0FF160C0000
#define ROM_APC12_CPU1_DBG			0x0FF16110000
#define ROM_APC12_CPU1_CTI			0x0FF16120000
#define ROM_APC12_CPU1_PMU			0x0FF16130000
#define ROM_APC12_CPU1_TRC			0x0FF16140000
#define ROM_APC12_CPU0_PMU_CTI			0x0FF161C0000
#define ROM_APC12_CPU1_PMU_CTI			0x0FF161D0000
#define ROM_APC12_L2_PMU_CTI			0x0FF161E0000
#define ROM_APC12_LLM_CTI			0x0FF161F0000
#define ROM_APC13_CPU0_DBG			0x0FF16210000
#define ROM_APC13_CPU0_CTI			0x0FF16220000
#define ROM_APC13_CPU0_PMU			0x0FF16230000
#define ROM_APC13_CPU0_TRC			0x0FF16240000
#define ROM_APC13_FUNNEL			0x0FF162C0000
#define ROM_APC13_CPU1_DBG			0x0FF16310000
#define ROM_APC13_CPU1_CTI			0x0FF16320000
#define ROM_APC13_CPU1_PMU			0x0FF16330000
#define ROM_APC13_CPU1_TRC			0x0FF16340000
#define ROM_APC13_CPU0_PMU_CTI			0x0FF164C0000
#define ROM_APC13_CPU1_PMU_CTI			0x0FF163D0000
#define ROM_APC13_L2_PMU_CTI			0x0FF163E0000
#define ROM_APC13_LLM_CTI			0x0FF163F0000
#define ROM_APC14_CPU0_DBG			0x0FF16410000
#define ROM_APC14_CPU0_CTI			0x0FF16420000
#define ROM_APC14_CPU0_PMU			0x0FF16430000
#define ROM_APC14_CPU0_TRC			0x0FF16440000
#define ROM_APC14_FUNNEL			0x0FF164C0000
#define ROM_APC14_CPU1_DBG			0x0FF16510000
#define ROM_APC14_CPU1_CTI			0x0FF16520000
#define ROM_APC14_CPU1_PMU			0x0FF16530000
#define ROM_APC14_CPU1_TRC			0x0FF16540000
#define ROM_APC14_CPU0_PMU_CTI			0x0FF165C0000
#define ROM_APC14_CPU1_PMU_CTI			0x0FF165D0000
#define ROM_APC14_L2_PMU_CTI			0x0FF165E0000
#define ROM_APC14_LLM_CTI			0x0FF165F0000
#define ROM_APC15_CPU0_DBG			0x0FF16610000
#define ROM_APC15_CPU0_CTI			0x0FF16620000
#define ROM_APC15_CPU0_PMU			0x0FF16630000
#define ROM_APC15_CPU0_TRC			0x0FF16640000
#define ROM_APC15_FUNNEL			0x0FF166C0000
#define ROM_APC15_CPU1_DBG			0x0FF16710000
#define ROM_APC15_CPU1_CTI			0x0FF16720000
#define ROM_APC15_CPU1_PMU			0x0FF16730000
#define ROM_APC15_CPU1_TRC			0x0FF16740000
#define ROM_APC15_CPU0_PMU_CTI			0x0FF167C0000
#define ROM_APC15_CPU1_PMU_CTI			0x0FF167D0000
#define ROM_APC15_L2_PMU_CTI			0x0FF167E0000
#define ROM_APC15_LLM_CTI			0x0FF167F0000
#define ROM_APC16_CPU0_DBG			0x0FF16810000
#define ROM_APC16_CPU0_CTI			0x0FF16820000
#define ROM_APC16_CPU0_PMU			0x0FF16830000
#define ROM_APC16_CPU0_TRC			0x0FF16840000
#define ROM_APC16_FUNNEL			0x0FF168C0000
#define ROM_APC16_CPU1_DBG			0x0FF16910000
#define ROM_APC16_CPU1_CTI			0x0FF16920000
#define ROM_APC16_CPU1_PMU			0x0FF16930000
#define ROM_APC16_CPU1_TRC			0x0FF16940000
#define ROM_APC16_CPU0_PMU_CTI			0x0FF169C0000
#define ROM_APC16_CPU1_PMU_CTI			0x0FF169D0000
#define ROM_APC16_L2_PMU_CTI			0x0FF169E0000
#define ROM_APC16_LLM_CTI			0x0FF169F0000
#define ROM_APC17_CPU0_DBG			0x0FF16A10000
#define ROM_APC17_CPU0_CTI			0x0FF16A20000
#define ROM_APC17_CPU0_PMU			0x0FF16A30000
#define ROM_APC17_CPU0_TRC			0x0FF16A40000
#define ROM_APC17_FUNNEL			0x0FF16AC0000
#define ROM_APC17_CPU1_DBG			0x0FF16B10000
#define ROM_APC17_CPU1_CTI			0x0FF16B20000
#define ROM_APC17_CPU1_PMU			0x0FF16B30000
#define ROM_APC17_CPU1_TRC			0x0FF16B40000
#define ROM_APC17_CPU0_PMU_CTI			0x0FF16BC0000
#define ROM_APC17_CPU1_PMU_CTI			0x0FF16BD0000
#define ROM_APC17_L2_PMU_CTI			0x0FF16BE0000
#define ROM_APC17_LLM_CTI			0x0FF16BF0000
#define ROM_APC18_CPU0_DBG			0x0FF16C10000
#define ROM_APC18_CPU0_CTI			0x0FF16C20000
#define ROM_APC18_CPU0_PMU			0x0FF16C30000
#define ROM_APC18_CPU0_TRC			0x0FF16C40000
#define ROM_APC18_FUNNEL			0x0FF16CC0000
#define ROM_APC18_CPU1_DBG			0x0FF16D10000
#define ROM_APC18_CPU1_CTI			0x0FF16D20000
#define ROM_APC18_CPU1_PMU			0x0FF16D30000
#define ROM_APC18_CPU1_TRC			0x0FF16D40000
#define ROM_APC18_CPU0_PMU_CTI			0x0FF16DC0000
#define ROM_APC18_CPU1_PMU_CTI			0x0FF16DD0000
#define ROM_APC18_L2_PMU_CTI			0x0FF16DE0000
#define ROM_APC18_LLM_CTI			0x0FF16DF0000
#define ROM_APC19_CPU0_DBG			0x0FF16E10000
#define ROM_APC19_CPU0_CTI			0x0FF16E20000
#define ROM_APC19_CPU0_PMU			0x0FF16E30000
#define ROM_APC19_CPU0_TRC			0x0FF16E40000
#define ROM_APC19_FUNNEL			0x0FF16EC0000
#define ROM_APC19_CPU1_DBG			0x0FF16F10000
#define ROM_APC19_CPU1_CTI			0x0FF16F20000
#define ROM_APC19_CPU1_PMU			0x0FF16F30000
#define ROM_APC19_CPU1_TRC			0x0FF16F40000
#define ROM_APC19_CPU0_PMU_CTI			0x0FF16FC0000
#define ROM_APC19_CPU1_PMU_CTI			0x0FF16FD0000
#define ROM_APC19_L2_PMU_CTI			0x0FF16FE0000
#define ROM_APC19_LLM_CTI			0x0FF16FF0000
#define ROM_APC20_CPU0_DBG			0x0FF17010000
#define ROM_APC20_CPU0_CTI			0x0FF17020000
#define ROM_APC20_CPU0_PMU			0x0FF17030000
#define ROM_APC20_CPU0_TRC			0x0FF17040000
#define ROM_APC20_FUNNEL			0x0FF170C0000
#define ROM_APC20_CPU1_DBG			0x0FF17110000
#define ROM_APC20_CPU1_CTI			0x0FF17120000
#define ROM_APC20_CPU1_PMU			0x0FF17130000
#define ROM_APC20_CPU1_TRC			0x0FF17140000
#define ROM_APC20_CPU0_PMU_CTI			0x0FF171C0000
#define ROM_APC20_CPU1_PMU_CTI			0x0FF171D0000
#define ROM_APC20_L2_PMU_CTI			0x0FF171E0000
#define ROM_APC20_LLM_CTI			0x0FF171F0000
#define ROM_APC21_CPU0_DBG			0x0FF17210000
#define ROM_APC21_CPU0_CTI			0x0FF17220000
#define ROM_APC21_CPU0_PMU			0x0FF17230000
#define ROM_APC21_CPU0_TRC			0x0FF17240000
#define ROM_APC21_FUNNEL			0x0FF172C0000
#define ROM_APC21_CPU1_DBG			0x0FF17310000
#define ROM_APC21_CPU1_CTI			0x0FF17320000
#define ROM_APC21_CPU1_PMU			0x0FF17330000
#define ROM_APC21_CPU1_TRC			0x0FF17340000
#define ROM_APC21_CPU0_PMU_CTI			0x0FF173C0000
#define ROM_APC21_CPU1_PMU_CTI			0x0FF173D0000
#define ROM_APC21_L2_PMU_CTI			0x0FF173E0000
#define ROM_APC21_LLM_CTI			0x0FF173F0000
#define ROM_APC22_CPU0_DBG			0x0FF17410000
#define ROM_APC22_CPU0_CTI			0x0FF17420000
#define ROM_APC22_CPU0_PMU			0x0FF17430000
#define ROM_APC22_CPU0_TRC			0x0FF17440000
#define ROM_APC22_FUNNEL			0x0FF174C0000
#define ROM_APC22_CPU1_DBG			0x0FF17510000
#define ROM_APC22_CPU1_CTI			0x0FF17520000
#define ROM_APC22_CPU1_PMU			0x0FF17530000
#define ROM_APC22_CPU1_TRC			0x0FF17540000
#define ROM_APC22_CPU0_PMU_CTI			0x0FF175C0000
#define ROM_APC22_CPU1_PMU_CTI			0x0FF175D0000
#define ROM_APC22_L2_PMU_CTI			0x0FF175E0000
#define ROM_APC22_LLM_CTI			0x0FF175F0000
#define ROM_APC23_CPU0_DBG			0x0FF17610000
#define ROM_APC23_CPU0_CTI			0x0FF17620000
#define ROM_APC23_CPU0_PMU			0x0FF17630000
#define ROM_APC23_CPU0_TRC			0x0FF17640000
#define ROM_APC23_FUNNEL			0x0FF176C0000
#define ROM_APC23_CPU1_DBG			0x0FF17710000
#define ROM_APC23_CPU1_CTI			0x0FF17720000
#define ROM_APC23_CPU1_PMU			0x0FF17730000
#define ROM_APC23_CPU1_TRC			0x0FF17740000
#define ROM_APC23_CPU0_PMU_CTI			0x0FF177C0000
#define ROM_APC23_CPU1_PMU_CTI			0x0FF177D0000
#define ROM_APC23_L2_PMU_CTI			0x0FF177E0000
#define ROM_APC23_LLM_CTI			0x0FF177F0000
#define ROM_HMSS_SCHNL_FUNNEL_W1_0		0x0FF17800000
#define ROM_HMSS_SCHNL_FUNNEL_W1_1		0x0FF17810000
#define ROM_HMSS_SCHNL_FUNNEL_W2_0		0x0FF17820000
#define ROM_HMSS_SCHNL_FUNNEL_W2_1		0x0FF17830000
#define ROM_HMSS_SCHNL_FUNNEL_W3_0		0x0FF17840000
#define ROM_HMSS_SCHNL_FUNNEL_W3_1		0x0FF17850000
#define ROM_HMSS_SCHNL_FUNNEL_E1_0		0x0FF17860000
#define ROM_HMSS_SCHNL_FUNNEL_E1_1		0x0FF17870000
#define ROM_HMSS_SCHNL_FUNNEL_E2_0		0x0FF17880000
#define ROM_HMSS_SCHNL_FUNNEL_E2_1		0x0FF17890000
#define ROM_HMSS_SCHNL_FUNNEL_E3_0		0x0FF178A0000
#define ROM_HMSS_SCHNL_FUNNEL_E3_1		0x0FF178B0000
#define ROM_SKITTER_CFG				0x0FF178E0000

/* Cortex A53 */
/* Base */
#define ROM_A53					0x0FF11200000
/* Entries */
#define ROM_DBG_CPU0				0x0FF11210000
#define ROM_CTI_CPU0				0x0FF11220000
#define ROM_PMU_CPU0				0x0FF11230000
#define ROM_ETM_CPU0				0x0FF11240000

/* Ring ROM */
#define ROM_RING				0x0FF12000000

/* Reserved ROM */
#define ROM_RESERVED				0x0FF14000000

/* Qualcomm CTIs use special DEVARCH value */
#define CORESIGHT_JEP106_QCOM_CTI		\
	CORESIGHT_JEP106(0x77, 8)
#define CORESIGHT_ARCH_QCOM_CTI			0x0A14
#define CORESIGHT_ARCH_QCOM_PMU			0x6A16
#define CORESIGHT_ARCH_QCOM_COMPONENT		0x5CDA

#define CORESIGHT_TYPE_QCOM_DBG			\
	CORESIGHT_TYPE(CORESIGHT_TYPE_DEBUG_CONTROL, 0x00)
#define CORESIGHT_TYPE_QCOM_TPDA		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_MISC, 0x08)
#define CORESIGHT_TYPE_QCOM_TPDM		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_MISC, 0x09)
#define CORESIGHT_TYPE_QCOM_SWITCH_DBG		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_MISC, 0x0A)
#define CORESIGHT_TYPE_QCOM_SWITCH_PMU		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_MISC, 0x0B)
#define CORESIGHT_TYPE_QCOM_SWITCH_CFG		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_MISC, 0x0C)
#define CORESIGHT_TYPE_QCOM_SKITTER_CFG		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_MISC, 0x0D)

#ifdef CONFIG_QDSS
int qdss_init(void);
#else
#define qdss_init()		0
#endif

#ifdef CONFIG_QDSS_CTI
int qdss_cti_init(void);
#else
#define qdss_cti_init()		0
#endif
#ifdef CONFIG_QDSS_DBG
int qdss_dbg_init(void);
#else
#define qdss_dbg_init()		0
#endif
#ifdef CONFIG_QDSS_PMU
int qdss_pmu_init(void);
#else
#define qdss_pmu_init()		0
#endif
#ifdef CONFIG_QDSS_TPDA
int qdss_tpda_init(void);
#else
#define qdss_tpda_init()	0
#endif
#ifdef CONFIG_QDSS_TPDM
int qdss_tpdm_init(void);
#else
#define qdss_tpdm_init()	0
#endif
#ifdef CONFIG_QDSS_SWITCH_DBG
int qdss_switch_dbg_init(void);
#else
#define qdss_switch_dbg_init()	0
#endif
#ifdef CONFIG_QDSS_SWITCH_PMU
int qdss_switch_pmu_init(void);
#else
#define qdss_switch_pmu_init()	0
#endif
#ifdef CONFIG_QDSS_SWITCH_CFG
int qdss_switch_cfg_init(void);
#else
#define qdss_switch_cfg_init()	0
#endif
#ifdef CONFIG_QDSS_SKITTER_CFG
int qdss_skitter_cfg_init(void);
#else
#define qdss_skitter_cfg_init()	0
#endif

#endif /* __QDSS_QDF2400_H_INCLUDE__ */
