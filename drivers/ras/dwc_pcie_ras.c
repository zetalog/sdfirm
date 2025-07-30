#include <target/pci.h>
#include <target/console.h>
#include <target/spinlock.h>
#include <target/ras.h>

#define PCIE_RAS_MODNAME		"pcie_ras"

#define EINJ_ENABLE_REG		0x30
#define ERROR_INJECTION0_ENABLE 	BIT(0)
#define ERROR_INJECTION1_ENABLE		BIT(1)
#define ERROR_INJECTION2_ENABLE		BIT(2)
#define ERROR_INJECTION3_ENABLE		BIT(3)
#define ERROR_INJECTION4_ENABLE		BIT(4)
#define ERROR_INJECTION5_ENABLE		BIT(5)
#define ERROR_INJECTION6_ENABLE		BIT(6)

#define EINJ0_CSR_REG		0x34
#define EINJ0_COUNT_MASK		GENMASK(7, 0)
#define EINJ0_CRC_TYPE_MASK		GENMASK(11, 8)

#define EINJ1_SEQNUM_REG	0x38
#define EINJ1_COUNT_MASK		GENMASK(7, 0)
#define EINJ1_SEQNUM_TYPE		BIT(8)
#define EINJ1_BAD_SEQNUM_MASK		GENMASK(28, 16)

#define EINJ2_DLLP_REG		0x3C
#define EINJ2_COUNT_MASK		GENMASK(7, 0)
#define EINJ2_DLLP_TYPE_MASK		GENMASK(9, 8)

#define EINJ3_SYMBOL_REG	0x40
#define EINJ3_COUNT_MASK		GENMASK(7, 0)
#define EINJ3_SYMBOL_TYPE_MASK		GENMASK(10, 8)

#define EINJ4_FC_REG		0x44
#define EINJ4_COUNT_MASK		GENMASK(7, 0)
#define EINJ4_UPDFC_TYPE_MASK		GENMASK(10, 8)
#define EINJ4_VC_NUMBER_MASK		GENMASK(14, 12)
#define EINJ4_BAD_UPDFC_VALUE_MASK	GENMASK(28, 16)

#define EINJ5_SP_TLP_REG	0x48
#define EINJ5_COUNT_MASK		GENMASK(7, 0)
#define EINJ5_SPECIFIED_TLP		BIT(8)

#define EINJ6_COMPARE_POINT_H_REG(n)	(0x4C + (n) * 0x4)
#define EINJ6_COMPARE_POINT_H0_MASK	GENMASK(31, 0)
#define EINJ6_COMPARE_VALUE_H_REG(n)	(0x5C + (n) * 0x4)
#define EINJ6_COMPARE_VALUE_H0_MASK	GENMASK(31, 0)
#define EINJ6_CHANGE_POINT_H_REG(n)	(0x6C + (n) * 0x4)
#define EINJ6_CHANGE_POINT_H0_MASK	GENMASK(31, 0)
#define EINJ6_CHANGE_VALUE_H_REG(n)	(0x7C + (n) * 0x4)
#define EINJ6_CHANGE_VALUE_H0_MASK	GENMASK(31, 0)

#define EINJ6_TLP_REG		0x8C
#define EINJ6_COUNT_MASK		GENMASK(7, 0)
#define EINJ6_INVERTED_CONTROL		BIT(8)
#define EINJ6_PACKET_TYPE_MASK		GENMASK(11, 9)

#define ROOT_PORT_INTR_ON_MESG_MASK	(PCI_ERR_ROOT_CMD_COR_EN|	\
					 PCI_ERR_ROOT_CMD_NONFATAL_EN|	\
					 PCI_ERR_ROOT_CMD_FATAL_EN)

#define ERR_COR_ID(d)			(d & 0xffff)
#define ERR_UNCOR_ID(d)			(d >> 16)

#define AER_ERR_STATUS_MASK		(PCI_ERR_ROOT_UNCOR_RCV |	\
					 PCI_ERR_ROOT_COR_RCV |		\
					 PCI_ERR_ROOT_MULTI_COR_RCV |	\
					 PCI_ERR_ROOT_MULTI_UNCOR_RCV)

#define PCI_EXP_AER_FLAGS		(PCI_EXP_DEVCTL_CERE | PCI_EXP_DEVCTL_NFERE | \
					PCI_EXP_DEVCTL_FERE | PCI_EXP_DEVCTL_URRE)

uint16_t aer_cap;	/* AER capability offset */
uint16_t ras_cap;	/* RAS capability offset */
uint16_t pcie_flags_reg; /* device/port err_inj_type */
int pcie_cap;
spinlock_t global_pcie_cap_lock;

struct aer_err_source {
	u32 status;			/* PCI_ERR_ROOT_STATUS */
	u32 id;				/* PCI_ERR_ROOT_ERR_SRC */
};

struct aer_err_info {
	unsigned int id;
	unsigned int severity;		/* 0:NONFATAL | 1:FATAL | 2:COR */
	unsigned int multi_error_valid;
	unsigned int first_error;
	unsigned int tlp_header_valid;
	unsigned int status;		/* COR/UNCOR Error Status */
	unsigned int mask;		/* COR/UNCOR Error Mask */
};

uint16_t pcie_mask = 0;
#define AER_NONFATAL			0
#define AER_FATAL			1
#define AER_CORRECTABLE			2
#define DPC_FATAL			3

extern struct dw_pcie pcie_ctrls[];

#define AER_INT_STA_REG		0x80300
#define CFG_AER_RC_ERR_INTSTA	BIT(0)
#define DPC_INSTA		BIT(1)

/**
 * struct dwc_pcie_err_inj - Store details about each error injection
 *				 supported by DWC RAS DES
 * @name: Name of the error that can be injected
 * @err_inj_group: Group number to which the error belongs. The value
 *		   can range from 0 to 5
 * @err_inj_type: Each err_inj_group can have multiple types of error
 */
struct dwc_pcie_err_inj {
	const char *name;
	u32 err_inj_group;
	u32 err_inj_type;
};

static const struct dwc_pcie_err_inj err_inj_list[] = {
	{ "tx_lcrc", 0x0, 0x0 },
	{ "b16_crc_dllp", 0x0, 0x1 },
	{ "b16_crc_upd_fc", 0x0, 0x2 },
	{ "tx_ecrc", 0x0, 0x3 },
	{ "fcrc_tlp", 0x0, 0x4 },
	{ "parity_tsos", 0x0, 0x5 },
	{ "parity_skpos", 0x0, 0x6 },
	{ "rx_lcrc", 0x0, 0x8 },
	{ "rx_ecrc", 0x0, 0xb },
	{ "tlp_err_seq", 0x1, 0x0 },
	{ "ack_nak_dllp_seq", 0x1, 0x1 },
	{ "ack_nak_dllp", 0x2, 0x0 },
	{ "upd_fc_dllp", 0x2, 0x1 },
	{ "nak_dllp", 0x2, 0x2 },
	{ "inv_sync_hdr_sym", 0x3, 0x0 },
	{ "com_pad_ts1", 0x3, 0x1 },
	{ "com_pad_ts2", 0x3, 0x2 },
	{ "com_fts", 0x3, 0x3 },
	{ "com_idl", 0x3, 0x4 },
	{ "end_edb", 0x3, 0x5 },
	{ "stp_sdp", 0x3, 0x6 },
	{ "com_skp", 0x3, 0x7 },
	{ "posted_tlp_hdr", 0x4, 0x0 },
	{ "non_post_tlp_hdr", 0x4, 0x1 },
	{ "cmpl_tlp_hdr", 0x4, 0x2 },
	{ "posted_tlp_data", 0x4, 0x4 },
	{ "non_post_tlp_data", 0x4, 0x5 },
	{ "cmpl_tlp_data", 0x4, 0x6 },
	{ "duplicate_tlp", 0x5, 0x0 },
	{ "nullified_tlp", 0x5, 0x1 },
};

int pci_read_config_byte(struct dw_pcie *dev, uint32_t offset, uint8_t *val)
{
	if (!dev || !val)
		return -1;
	*val = dw_pcie_read_dbi(dev, DW_PCIE_CDM, offset, 1);
	return 0;
}

int pci_write_config_byte(struct dw_pcie *dev, uint32_t offset, uint8_t val)
{
	if (!dev)
		return -1;
	dw_pcie_write_dbi(dev, DW_PCIE_CDM, offset, val, 1);
	return 0;
}

int pci_read_config_word(struct dw_pcie *dev, uint32_t offset, uint16_t *val)
{
	if (!dev || !val)
		return -1;
	*val = dw_pcie_read_dbi(dev, DW_PCIE_CDM, offset, 2);
	return 0;
}

int pci_write_config_word(struct dw_pcie *dev, uint32_t offset, uint16_t val)
{
	if (!dev)
		return -1;
	dw_pcie_write_dbi(dev, DW_PCIE_CDM, offset, val, 2);
	return 0;
}

int pci_read_config_dword(struct dw_pcie *dev, uint32_t offset, uint32_t *val)
{
	if (!dev || !val)
		return -1;
	*val = dw_pcie_read_dbi(dev, DW_PCIE_CDM, offset, 4);
	return 0;
}

int pci_write_config_dword(struct dw_pcie *dev, uint32_t offset, uint32_t val)
{
	if (!dev)
		return -1;
	dw_pcie_write_dbi(dev, DW_PCIE_CDM, offset, val, 4);
	return 0;
}

/* get pcie port err_inj_type */
static inline int dwc_pci_pcie_type(const struct dw_pcie *dev)
{
	return pcie_flags_reg & PCI_EXP_FLAGS_TYPE >> 4;
}

bool dwc_pcie_cap_has_rtctl(const struct dw_pcie *dev)
{
	int err_inj_type = dwc_pci_pcie_type(dev);

	return err_inj_type == PCI_EXP_TYPE_ROOT_PORT ||
		err_inj_type == PCI_EXP_TYPE_RC_EC;
}

static inline bool pcie_downstream_port(const struct dw_pcie *dev)
{
	int err_inj_type = dwc_pci_pcie_type(dev);

	return err_inj_type == PCI_EXP_TYPE_ROOT_PORT ||
		err_inj_type == PCI_EXP_TYPE_DOWNSTREAM ||
		err_inj_type == PCI_EXP_TYPE_PCIE_BRIDGE;
}

/*
 * Note that these accessor functions are only for the "PCI Express
 * Capability" (see PCIe spec r3.0, sec 7.8).  They do not apply to the
 * other "PCI Express Extended Capabilities" (AER, VC, ACS, MFVC, etc.)
 */
int pcie_capability_read_word(struct dw_pcie *dev, int pos, u16 *val)
{
	int ret;

	*val = 0;
	if (pos & 1)
		return -1;

	ret = pci_read_config_word(dev, pcie_cap + pos, val);
	/*
	 * Reset *val to 0 if pci_read_config_word() fails; it may
	 * have been written as 0xFFFF (PCI_ERROR_RESPONSE) if the
	 * config read failed on PCI.
	 */
	if (ret)
		*val = 0;
	return ret;

	/*
	 * For Functions that do not implement the Slot Capabilities,
	 * Slot Status, and Slot Control registers, these spaces must
	 * be hardwired to 0b, with the exception of the Presence Detect
	 * State bit in the Slot Status register of Downstream Ports,
	 * which must be hardwired to 1b.  (PCIe Base Spec 3.0, sec 7.8)
	 */
	if (pcie_cap != 0 && pcie_downstream_port(dev) && pos == PCI_EXP_SLTSTA)
		*val = PCI_EXP_SLTSTA_PDS;

	return 0;
}

int pcie_capability_read_dword(struct dw_pcie *dev, int pos, u32 *val)
{
	int ret;

	*val = 0;
	if (pos & 3)
		return -1;

	ret = pci_read_config_dword(dev, pcie_cap + pos, val);
	/*
	 * Reset *val to 0 if pci_read_config_dword() fails; it may
	 * have been written as 0xFFFFFFFF (PCI_ERROR_RESPONSE) if
	 * the config read failed on PCI.
	 */
	if (ret)
		*val = 0;
	return ret;

	if (pcie_cap != 0 && pcie_downstream_port(dev) && pos == PCI_EXP_SLTSTA)
		*val = PCI_EXP_SLTSTA_PDS;

	return 0;
}

int pcie_capability_write_word(struct dw_pcie *dev, int pos, u16 val)
{
	if (pos & 1)
		return -1;

	return pci_write_config_word(dev, pcie_cap + pos, val);
}

int pcie_capability_write_dword(struct dw_pcie *dev, int pos, u32 val)
{
	if (pos & 3)
		return -1;

	return pci_write_config_dword(dev, pcie_cap + pos, val);
}

int pcie_capability_clear_and_set_word_unlocked(struct dw_pcie *dev, int pos,
						u16 clear, u16 set)
{
	int ret;
	u16 val;

	ret = pcie_capability_read_word(dev, pos, &val);
	if (ret)
		return ret;

	val &= ~clear;
	val |= set;
	return pcie_capability_write_word(dev, pos, val);
}

int pcie_capability_clear_and_set_word_locked(struct dw_pcie *dev, int pos,
						  u16 clear, u16 set)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&global_pcie_cap_lock, flags);
	ret = pcie_capability_clear_and_set_word_unlocked(dev, pos, clear, set);
	spin_unlock_irqrestore(&global_pcie_cap_lock, flags);

	return ret;
}

int pcie_capability_clear_and_set_dword(struct dw_pcie *dev, int pos,
					u32 clear, u32 set)
{
	int ret;
	u32 val;

	ret = pcie_capability_read_dword(dev, pos, &val);
	if (ret)
		return ret;

	val &= ~clear;
	val |= set;
	return pcie_capability_write_dword(dev, pos, val);
}

static inline int pcie_capability_clear_and_set_word(struct dw_pcie *dev,
							 int pos,
							 u16 clear, u16 set)
{
	switch (pos) {
	case PCI_EXP_LNKCTL:
	case PCI_EXP_LNKCTL2:
	case PCI_EXP_RTCTL:
		return pcie_capability_clear_and_set_word_locked(dev, pos,
								 clear, set);
	default:
		return pcie_capability_clear_and_set_word_unlocked(dev, pos,
								   clear, set);
	}
}

static inline int pcie_capability_set_word(struct dw_pcie *dev, int pos,
						u16 set)
{
	return pcie_capability_clear_and_set_word(dev, pos, 0, set);
}

static inline int pcie_capability_set_dword(struct dw_pcie *dev, int pos,
						u32 set)
{
	return pcie_capability_clear_and_set_dword(dev, pos, 0, set);
}

static inline int pcie_capability_clear_word(struct dw_pcie *dev, int pos,
						 u16 clear)
{
	return pcie_capability_clear_and_set_word(dev, pos, clear, 0);
}

static inline int pcie_capability_clear_dword(struct dw_pcie *dev, int pos,
						  u32 clear)
{
	return pcie_capability_clear_and_set_dword(dev, pos, clear, 0);
}

uint16_t dwc_pci_find_next_ext_capability(struct dw_pcie *dev, int cap)
{
	uint32_t header = 0;
	int ttl;
	uint16_t pos = PCI_CFG_SPACE_SIZE;

	/* minimum 8 bytes per capability */
	ttl = (PCI_CFG_SPACE_EXP_SIZE - PCI_CFG_SPACE_SIZE) / 8;

	if (pci_read_config_dword(dev, pos, &header) != 0)
		return 0;
	/*
	 * If we have no capabilities, this is indicated by cap ID,
	 * cap version and next pointer all being 0.
	 */
	if (!header)
		return 0;

	while (ttl-- > 0) {
		if (PCI_EXT_CAP_ID(header) == cap && pos != 0)
			return pos;

		pos = PCI_EXT_CAP_NEXT(header);
		if (pos < PCI_CFG_SPACE_SIZE)
			break;

		if(pci_read_config_dword(dev, pos, &header) != 0)
			break;
	}

	return 0;
}

/* query for devices' capabilities */
u8 dwc_pci_find_capability(struct dw_pcie *dev, int cap)
{
	uint8_t id;
	int ttl;
	uint16_t status = 0;
	uint8_t hdr_type, pos = 0;
	uint16_t ent;

	/* find capability list start */
	pci_read_config_word(dev, PCI_STATUS, &status);
	con_log(PCIE_RAS_MODNAME ": PCI_STATUS: %s\n",
		status & PCI_STATUS_CAP_LIST ? "support CAP_LIST" : "NO_CAP_LIST");
	if (!(status & PCI_STATUS_CAP_LIST))
		return 0;

	pci_read_config_byte(dev, PCI_HEADER_TYPE, &hdr_type);
	con_log(PCIE_RAS_MODNAME ": PCI_HEADER_TYPE: %s\n",
		hdr_type == PCI_HEADER_TYPE_BRIDGE ? "BRIDGE" : "NORMAL");

	switch (hdr_type) {
	case PCI_HEADER_TYPE_NORMAL:
	case PCI_HEADER_TYPE_BRIDGE:
		pos = PCI_CAPABILITY_LIST;
		break;
	case PCI_HEADER_TYPE_CARDBUS:
		pos = PCI_CB_CAPABILITY_LIST;
		break;
	}

	con_log(PCIE_RAS_MODNAME ": pos: %s\n",pos == PCI_CAPABILITY_LIST ?
		"PCI_CAPABILITY_LIST" : "PCI_CB_CAPABILITY_LIST");

	if (pos) {
		pci_read_config_byte(dev, pos, &pos);

		ttl = PCI_FIND_CAP_TTL;
		while (ttl--) {
			if (pos < 0x40)
				break;
			pos &= ~0x3;
			pci_read_config_word(dev, pos, &ent);
			id = ent & 0xFF;
			if (id == 0xFF)
				break;
			if (id == cap)
				return pos;
			pos = (ent >> 8);
		}
	}

	return 0;
}

/* find pcie extended capability */
uint16_t dwc_pci_find_ext_capability(struct dw_pcie *dev, int cap)
{
	return dwc_pci_find_next_ext_capability(dev, cap);
}

int dwc_pci_aer_clear_status(struct dw_pcie *dev)
{
	int aer = aer_cap;
	uint32_t status;
	int port_type;

	if (!aer)
		return -EIO;

	port_type = dwc_pci_pcie_type(dev);
	if (port_type == PCI_EXP_TYPE_ROOT_PORT ||
		port_type == PCI_EXP_TYPE_RC_EC) {
		pci_read_config_dword(dev, aer + PCI_ERR_ROOT_STATUS, &status);
		pci_write_config_dword(dev, aer + PCI_ERR_ROOT_STATUS, status);
	}

	pci_read_config_dword(dev, aer + PCI_ERR_COR_STATUS, &status);
	pci_write_config_dword(dev, aer + PCI_ERR_COR_STATUS, status);

	pci_read_config_dword(dev, aer + PCI_ERR_UNCOR_STATUS, &status);
	pci_write_config_dword(dev, aer + PCI_ERR_UNCOR_STATUS, status);

	return 0;
}

int32_t pcie_report_error(struct dw_pcie *pci)
{
	acpi_ghes_error_info einfo;
	uint8_t source_id = 0x80;
	uint32_t val32 = 0;
	uint16_t val, val16 = 0;
	uint8_t pos;

	acpi_ghes_new_error_source(source_id, source_id);

	memset(&einfo, 0, sizeof(einfo));
	einfo.etype = ERROR_TYPE_PCIE;
	einfo.info.pcie.validation_bits = 0;
	if (pci_read_config_word(pci, PCI_VENDOR_ID, &val16) == 0)
		einfo.info.pcie.vendor_id = val16;

	if (pci_read_config_word(pci, PCI_DEVICE_ID, &val16) == 0) {
		einfo.info.pcie.device_id = val16;
		einfo.info.pcie.validation_bits |= CPER_PCIE_VALID_DEVICE_ID;
	}

	if (pci_read_config_word(pci, PCI_COMMAND, &val16) == 0
	    && pci_read_config_word(pci, PCI_STATUS, &val) == 0) {
		einfo.info.pcie.command = val16;
		einfo.info.pcie.status = val;
		einfo.info.pcie.validation_bits |= CPER_PCIE_VALID_COMMAND_STATUS;
	}

	if (pci_read_config_dword(pci, PCI_CLASS_REVISION, &val32) == 0) {
		einfo.info.pcie.class_code[0] = (val32 >> 8) & 0xFF;
		einfo.info.pcie.class_code[1] = (val32 >> 16) & 0xFF;
		einfo.info.pcie.class_code[2] = (val32 >> 24) & 0xFF;
	}

	einfo.info.pcie.bus = 0;
	einfo.info.pcie.device = 0;
	einfo.info.pcie.function = 0;
	einfo.info.pcie.segment = 0;
	einfo.info.pcie.version_major = 5;
	einfo.info.pcie.version_minor = 0;
	einfo.info.pcie.port_type = pcie_flags_reg;
	einfo.info.pcie.validation_bits |= CPER_PCIE_VALID_VERSION | CPER_PCIE_VALID_PORT_TYPE;

	/* 60-byte, i.e. 0x3C PCIe capability structure size */
	memset(einfo.info.pcie.capability, 0, sizeof(einfo.info.pcie.capability));
	einfo.info.pcie.capability[0] = pcie_cap & 0xFF;
	einfo.info.pcie.capability[1] = (pcie_cap >> 8) & 0xFF;
	einfo.info.pcie.capability[2] = (pcie_cap >> 16) & 0xFF;
	einfo.info.pcie.capability[3] = (pcie_cap >> 24) & 0xFF;
	/* 96-byte, i.e. 0x60 AER error information structure size */
	memset(einfo.info.pcie.aer_info, 0, sizeof(einfo.info.pcie.aer_info));
	einfo.info.pcie.aer_info[0] = aer_cap & 0xFF;
	einfo.info.pcie.aer_info[1] = (aer_cap >> 8) & 0xFF;

	pos = dwc_pci_find_ext_capability(pci, PCI_EXT_CAP_ID_DSN);
	if (pos) {
		pci_read_config_dword(pci, pos + 0x04, &val32);
		einfo.info.pcie.serial_number_lower = val32;
		pci_read_config_dword(pci, pos + 0x08, &val32);
		einfo.info.pcie.serial_number_upper = val32;
		einfo.info.pcie.validation_bits |= CPER_PCIE_VALID_SERIAL_NUMBER;
	}

	// acpi_ghes_record_errors(source_id, &einfo);

	return 0;
}

static void pcie_aer_intr_handle_irq(irq_t irq)
{
	int aer = aer_cap;
	struct aer_err_source e_src = {};
	struct aer_err_info e_info = {};
	struct dw_pcie *dev = NULL;

	con_log(PCIE_RAS_MODNAME ": DEVSTA: 0x%x\n", __raw_readl(0x5f0080008c));
	for (int i = 0; i < pcie_rc_count; i++) {
		if (!(pcie_mask & (1 << i)))
			continue;
		if (irq == pcie_aer_irqs[i]) {
			dev = &pcie_ctrls[i];
			con_log(PCIE_RAS_MODNAME ": PCIe AER IRQ: %d, %d\n", irq_ext(irq), i);
			break;
		}
	}

	con_log(PCIE_RAS_MODNAME ": AER STATUS=0x%08x\n", __raw_readl(dev->app_base + AER_INT_STA_REG));
	pci_read_config_dword(dev, aer + PCI_ERR_ROOT_STATUS, &e_src.status);
	if (!(e_src.status & AER_ERR_STATUS_MASK)) {
		con_log(PCIE_RAS_MODNAME ": Error has been masked!\n");
		return;
	}

	pci_read_config_dword(dev, aer + PCI_ERR_ROOT_ERR_SRC, &e_src.id);
	pci_write_config_dword(dev, aer + PCI_ERR_ROOT_STATUS, e_src.status);

	con_log(PCIE_RAS_MODNAME ": ID= %d, Status=0x%x\n", e_src.id, e_src.status);

	if (e_src.status & PCI_ERR_ROOT_COR_RCV) {
		e_info.id = ERR_COR_ID(e_src.id);
		e_info.severity = AER_CORRECTABLE;

		if (e_src.status & PCI_ERR_ROOT_MULTI_COR_RCV)
			e_info.multi_error_valid = 1;
		else
			e_info.multi_error_valid = 0;

		con_log(PCIE_RAS_MODNAME ": ID: %d, severity: %s, multi_error_valid: %s\n",
			e_info.id, "CORRECTABLE",
			e_info.multi_error_valid ? "true" : "false");
	}

	if (e_src.status & PCI_ERR_ROOT_UNCOR_RCV) {
		e_info.id = ERR_UNCOR_ID(e_src.id);

		if (e_src.status & PCI_ERR_ROOT_FATAL_RCV)
			e_info.severity = AER_FATAL;
		else
			e_info.severity = AER_NONFATAL;

		if (e_src.status & PCI_ERR_ROOT_MULTI_UNCOR_RCV)
			e_info.multi_error_valid = 1;
		else
			e_info.multi_error_valid = 0;

		con_log(PCIE_RAS_MODNAME ": ID: %d, severity: %s, multi_error_valid: %s\n",
			e_info.id,
			e_info.severity == AER_FATAL ? "FATAL" : "NON FATAL",
			e_info.multi_error_valid ? "true" : "false");
	}

	/* clear error status */
	con_log(PCIE_RAS_MODNAME ": clear error status\n");
	dwc_pci_aer_clear_status(dev);

	con_log(PCIE_RAS_MODNAME ": report error\n");
	pcie_report_error(dev);
}

#ifdef SYS_REALTIME
#define pcie_aer_irq_init()	do {} while (0)
#else
static void pcie_aer_irq_init(void)
{
	int i;

	for (i = 0; i < pcie_rc_count; i++) {
		irqc_configure_irq(pcie_aer_irqs[i], 0, IRQ_LEVEL_TRIGGERED);
		irq_register_vector(pcie_aer_irqs[i], pcie_aer_intr_handle_irq);
		irqc_enable_irq(pcie_aer_irqs[i]);
	}
}
#endif

int pcie_aer_init(struct pcie_port *pp)
{
	int pos;
	u16 reg16;
	u32 reg32;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

	pcie_mask = 0x400; // only enable pcie10
	con_log(PCIE_RAS_MODNAME ": PCIe mask:0x%08x\n", pcie_mask);

	pos = dwc_pci_find_capability(pci, PCI_CAP_ID_EXP);
	if (!pos) {
		con_log(PCIE_RAS_MODNAME ": No PCIe capability found\n");
		return -ENODEV;
	}

	pcie_cap = pos;
	pci_read_config_word(pci, pcie_cap + PCI_EXP_FLAGS, &reg16);
	pcie_flags_reg = reg16;

	aer_cap = dwc_pci_find_ext_capability(pci, PCI_EXT_CAP_ID_ERR);
	con_log(PCIE_RAS_MODNAME ": aer_cap: 0x%x\n", aer_cap);
	if (!aer_cap) {
		con_log(PCIE_RAS_MODNAME ": No AER capability found\n");
		return -ENODEV;
	}

	ras_cap = dwc_pci_find_ext_capability(pci, PCI_EXT_CAP_ID_VNDR);
	con_log(PCIE_RAS_MODNAME ": ras_cap: 0x%x\n", ras_cap);
	if (!ras_cap) {
		con_log(PCIE_RAS_MODNAME ": No RAS capability found\n");
		return -ENODEV;
	}

	dwc_pci_aer_clear_status(pci);

	/* enable pcie error reporting */
	pcie_capability_set_word(pci, PCI_EXP_DEVCTL, PCI_EXP_AER_FLAGS);
	writel(0x10000, 0x5f0080008c); // devsta

	/* enable PCIe ECRC checking for a device */
	pci_read_config_dword(pci, aer_cap + PCI_ERR_CAP, &reg32);
	if (reg32 & PCI_ERR_CAP_ECRC_GENC)
		reg32 |= PCI_ERR_CAP_ECRC_GENE;
	if (reg32 & PCI_ERR_CAP_ECRC_CHKC)
		reg32 |= PCI_ERR_CAP_ECRC_CHKE;
	pci_write_config_dword(pci, aer_cap + PCI_ERR_CAP, reg32);

	/* Enable Root Port's interrupt in response to error messages */
	pci_read_config_dword(pci, aer_cap + PCI_ERR_ROOT_COMMAND, &reg32);
	reg32 |= ROOT_PORT_INTR_ON_MESG_MASK;
	pci_write_config_dword(pci, aer_cap + PCI_ERR_ROOT_COMMAND, reg32);

	pcie_aer_irq_init();

	con_log(PCIE_RAS_MODNAME ": PCIe AER initializated!\n");

	return 0;
}

int pcie_err_inject(struct pcie_port *pp, struct pcie_einj_param *param)
{
	uint32_t reg_off, val = 0;
	uint32_t err_inj_group = 0;
	uint32_t err_inj_type = 0;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

	if (!pci) {
		con_log(PCIE_RAS_MODNAME ": PCIe device not found\n");
		return -ENODEV;
	}

	for (size_t i = 0; i < ARRAY_SIZE(err_inj_list); i++) {
		if (strcmp(param->inj_name, err_inj_list[i].name) == 0) {
			err_inj_group = err_inj_list[i].err_inj_group;
			err_inj_type = err_inj_list[i].err_inj_type;
			break;
		}
	}

	switch (err_inj_group) {
	case 0: reg_off = EINJ0_CSR_REG; break;
	case 1: reg_off = EINJ1_SEQNUM_REG; break;
	case 2: reg_off = EINJ2_DLLP_REG; break;
	case 3: reg_off = EINJ3_SYMBOL_REG; break;
	case 4: reg_off = EINJ4_FC_REG; break;
	case 5: reg_off = EINJ5_SP_TLP_REG; break;
	default: return -EINVAL;
	}

	pci_read_config_dword(pci, ras_cap + reg_off, &val);

	switch (err_inj_group) {
	case 0:
		val &= ~(EINJ0_CRC_TYPE_MASK | EINJ0_COUNT_MASK);
		val |= FIELD_PREP(EINJ0_CRC_TYPE_MASK, err_inj_type);
		val |= FIELD_PREP(EINJ0_COUNT_MASK, param->counter);
		break;
	case 1:
		val &= ~(EINJ1_SEQNUM_TYPE | EINJ1_COUNT_MASK);
		val |= FIELD_PREP(EINJ1_SEQNUM_TYPE, err_inj_type);
		val |= FIELD_PREP(EINJ1_COUNT_MASK, param->counter);
		if (param->val_diff) {
			val &= ~EINJ1_BAD_SEQNUM_MASK;
			val |= FIELD_PREP(EINJ1_BAD_SEQNUM_MASK, param->val_diff);
		}
		break;
	case 2:
		val &= ~(EINJ2_DLLP_TYPE_MASK | EINJ2_COUNT_MASK);
		val |= FIELD_PREP(EINJ2_DLLP_TYPE_MASK, err_inj_type);
		val |= FIELD_PREP(EINJ2_COUNT_MASK, param->counter);
		break;
	case 3:
		val &= ~(EINJ3_SYMBOL_TYPE_MASK | EINJ3_COUNT_MASK);
		val |= FIELD_PREP(EINJ3_SYMBOL_TYPE_MASK, err_inj_type);
		val |= FIELD_PREP(EINJ3_COUNT_MASK, param->counter);
		break;
	case 4:
		val &= ~(EINJ4_UPDFC_TYPE_MASK | EINJ4_COUNT_MASK |
			 EINJ4_VC_NUMBER_MASK | EINJ4_BAD_UPDFC_VALUE_MASK);
		val |= FIELD_PREP(EINJ4_UPDFC_TYPE_MASK, err_inj_type);
		val |= FIELD_PREP(EINJ4_COUNT_MASK, param->counter);
		val |= FIELD_PREP(EINJ4_VC_NUMBER_MASK, param->vc_num);
		val |= FIELD_PREP(EINJ4_BAD_UPDFC_VALUE_MASK, param->val_diff);
		break;
	case 5:
		val &= ~(EINJ5_COUNT_MASK | EINJ5_SPECIFIED_TLP);
		val |= FIELD_PREP(EINJ5_COUNT_MASK, param->counter);
		val |= FIELD_PREP(EINJ5_SPECIFIED_TLP, err_inj_type);
		break;
	}

	pci_write_config_dword(pci, ras_cap + reg_off, val);
	pci_write_config_dword(pci, ras_cap + EINJ_ENABLE_REG, (1 << err_inj_group));

	con_log(PCIE_RAS_MODNAME ": Injected %s, counter=%d, val_diff=%d, vc_num=%d\n",
		param->inj_name, param->counter, param->val_diff, param->vc_num);

	con_log(PCIE_RAS_MODNAME ": AER STATUS=0x%08x\n", __raw_readl(pci->app_base + AER_INT_STA_REG));

	return 0;
}

