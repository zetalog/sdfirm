/*
 * Author: Lv Zheng <lv.zheng@hxt-semitech.com>
 */
#include <errno.h>
#include <target/bitops.h>
#include <target/dts.h>

struct qdss_generic_device {
	uintptr_t base;
	uint16_t jep106_ident;
	uint16_t arch_id;
};

struct qdss_part {
	uint16_t part_no;
	uint16_t arch_id;
};

static uintptr_t qdss_blacklist[] = {
	ROM_CPU_EVENTS_W0_ETF_PRESERVATION,
	ROM_CPU_EVENTS_W1_ETF_PRESERVATION,
	ROM_CPU_EVENTS_E0_ETF_PRESERVATION,
	ROM_CPU_EVENTS_E1_ETF_PRESERVATION,
	/* Ring ROM (RingBUS ROM) has been deprectaed since Qualcomm 1.0,
	 * entries in Ring ROM has been merged into AJU ROM2.
	 */
	ROM_RING,
	ROM_RESERVED,
	/* Valid reserved ROM but not used */
	ROM_EXTPSEL_4M,
	0,
};

static struct qdss_part qdss_qcom_parts[] = {
	{ 0xE02, CORESIGHT_TYPE_QCOM_TPDM },
	{ 0xE04, CORESIGHT_TYPE_QCOM_TPDM },
	{ 0xE16, CORESIGHT_TYPE_QCOM_TPDM },
	{ 0xE22, CORESIGHT_TYPE_QCOM_TPDM },
	{ 0xE30, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ 0xE32, CORESIGHT_TYPE_QCOM_TPDM },
	{ 0xE34, CORESIGHT_TYPE_QCOM_TPDM },
	{ 0xE36, CORESIGHT_TYPE_QCOM_TPDM },
	{ 0xE3E, CORESIGHT_TYPE_QCOM_TPDM },
	{ 0xE40, CORESIGHT_TYPE_QCOM_TPDM },
	{ 0xF00, CORESIGHT_TYPE_QCOM_TPDA },
	{ 0xF01, CORESIGHT_TYPE_QCOM_TPDA },
	{ 0xF02, CORESIGHT_TYPE_QCOM_TPDA },
	{ 0xF03, CORESIGHT_TYPE_QCOM_TPDA },
	{ 0xF05, CORESIGHT_TYPE_QCOM_TPDA },
	{ 0xF09, CORESIGHT_TYPE_QCOM_TPDA },
};

static struct qdss_generic_device qdss_qcom_devices[] = {
	/* PCIe */
	{ ROM_PCIE0_TPDM3_SMMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_TPDM },
	{ ROM_PCIE1_TPDM3_SMMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_TPDM },

	/* PBSU */
	{ ROM_PBSU0_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_PBSU1_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_PBSU2_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_PBSU3_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_PBSU4_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },

	/* MBSU */
	{ ROM_MBSU0_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU1_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU2_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU3_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU4_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU5_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU6_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU7_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU8_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU9_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU10_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MBSU11_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },

	/* L3 */
	{ ROM_L30_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L30_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L31_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L31_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L32_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L32_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L33_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L33_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L34_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L34_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L35_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L35_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L36_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L36_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L37_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L37_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L38_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L38_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L39_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L39_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L310_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L310_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },
	{ ROM_L311_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_L311_PMU,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_PMU },

	/* MCSU */
	{ ROM_MCSU0_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MCSU1_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MCSU2_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MCSU3_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MCSU4_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_MCSU5_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },

	/* DMSU */
	{ ROM_DMSU0_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_DMSU1_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_DMSU2_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_DMSU3_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_DMSU4_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },
	{ ROM_DMSU5_DBG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_DBG },

	/* DFTSU */
	{ ROM_DFTSU_CFG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SWITCH_CFG },

	/* SKITTER */
	{ ROM_SKITTER_CFG,
	  CORESIGHT_JEP106_QCOM, CORESIGHT_TYPE_QCOM_SKITTER_CFG },
};

static int qdss_visit_part(uintptr_t rom_table_base)
{
	int i;
	struct coresight_rom_device device;
	uint16_t part_no = coresight_get_part(rom_table_base);

	for (i = 0; i < ARRAY_SIZE(qdss_qcom_devices); i++) {
		if (qdss_qcom_parts[i].part_no == part_no) {
			device.base = rom_table_base;
			device.jep106_ident = CORESIGHT_JEP106_QCOM;
			device.arch_id = qdss_qcom_parts[i].arch_id;
			device.part_no = part_no;
			return __coresight_visit_device(&device);
		}
	}
	printf("Unknown QCOM part %02x (%016llx)\n", part_no, rom_table_base);
	return -ENODEV;
}

static int qdss_visit_device(uintptr_t rom_table_base)
{
	int i;
	struct coresight_rom_device device;

	for (i = 0; i < ARRAY_SIZE(qdss_qcom_devices); i++) {
		if (qdss_qcom_devices[i].base == rom_table_base) {
			device.base = rom_table_base;
			device.jep106_ident = qdss_qcom_devices[i].jep106_ident;
			device.arch_id = qdss_qcom_devices[i].arch_id;
			return __coresight_visit_device(&device);
		}
	}
	printf("Unknown QCOM device (%016llx)\n", rom_table_base);
	return -ENODEV;
}

static int qdss_handle_qcom_device(struct coresight_rom_device *device)
{
	return qdss_visit_part(device->base);
}

struct coresight_device qcom_component = {
	.name = "Qualcomm Component",
	.flags = CORESIGHT_FLAGS_ENUMERATOR,
	.jep106_ident = CORESIGHT_JEP106_QCOM,
	.arch_id = CORESIGHT_ARCH_QCOM_COMPONENT,
	.handler = qdss_handle_qcom_device,
};

static int qdss_class0_table_handler(struct coresight_rom_table *table)
{
	return qdss_visit_device(table->base);
}

struct coresight_table qdss_class0_rom_table = {
	.name = "Qualcomm Component",
	.clazz = 0x00,
	.handler = qdss_class0_table_handler,
};

int qdss_init(void)
{
	/* Generic verification components */
	coresight_register_table(&qdss_class0_rom_table);

	/* DEVTYPE enumerated components */
	qdss_dbg_init();

	/* DEVARCH enumerated components */
	coresight_register_device(&qcom_component);
	qdss_cti_init();
	qdss_pmu_init();

	/* Special Qualcomm compmonents */
	qdss_tpda_init();
	qdss_tpdm_init();
	qdss_switch_dbg_init();
	qdss_switch_pmu_init();
	qdss_switch_cfg_init();
	qdss_skitter_cfg_init();
	return coresight_init(ROM_DAP, qdss_blacklist);
}
