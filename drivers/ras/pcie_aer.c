#include <target/ras.h>
#include <target/pci.h>
#include <target/console.h>
#include <target/cmdline.h>
#include <target/irq.h>
#include <target/spinlock.h>
#include <getopt.h>

extern struct dw_pcie pcie_ctrls[];
extern int optind;
extern char *optarg;

spinlock_t global_pcie_cap_lock;
#define PCIE_AER_MODNAME		"pcie_aer"
#define AER_ERROR_SOURCES_MAX		128

#define AER_MAX_TYPEOF_COR_ERRS		16	  /* as per PCI_ERR_COR_STATUS */
#define AER_MAX_TYPEOF_UNCOR_ERRS	27	  /* as per PCI_ERR_UNCOR_STATUS*/

struct aer_err_source {
	u32 status;			/* PCI_ERR_ROOT_STATUS */
	u32 id;				/* PCI_ERR_ROOT_ERR_SRC */
};

/* AER stats for the device */
struct aer_stats {
	/*
	 * Fields for all AER capable devices. They indicate the errors
	 * "as seen by this device". Note that this may mean that if an
	 * Endpoint is causing problems, the AER counters may increment
	 * at its link partner (e.g. Root Port) because the errors will be
	 * "seen" by the link partner and not the problematic Endpoint
	 * itself (which may report all counters as 0 as it never saw any
	 * problems).
	 */
	/* Counters for different type of correctable errors */
	uint64_t dev_cor_errs[AER_MAX_TYPEOF_COR_ERRS];
	/* Counters for different type of fatal uncorrectable errors */
	uint64_t dev_fatal_errs[AER_MAX_TYPEOF_UNCOR_ERRS];
	/* Counters for different type of nonfatal uncorrectable errors */
	uint64_t dev_nonfatal_errs[AER_MAX_TYPEOF_UNCOR_ERRS];
	/* Total number of ERR_COR sent by this device */
	uint64_t dev_total_cor_errs;
	/* Total number of ERR_FATAL sent by this device */
	uint64_t dev_total_fatal_errs;
	/* Total number of ERR_NONFATAL sent by this device */
	uint64_t dev_total_nonfatal_errs;

	/*
	 * Fields for Root Ports & Root Complex Event Collectors only; these
	 * indicate the total number of ERR_COR, ERR_FATAL, and ERR_NONFATAL
	 * messages received by the Root Port / Event Collector, INCLUDING the
	 * ones that are generated internally (by the Root Port itself)
	 */
	uint64_t rootport_total_cor_errs;
	uint64_t rootport_total_fatal_errs;
	uint64_t rootport_total_nonfatal_errs;
};

#define SYSTEM_ERROR_INTR_ON_MESG_MASK	(PCI_EXP_RTCTL_SECEE|	\
					PCI_EXP_RTCTL_SENFEE|	\
					PCI_EXP_RTCTL_SEFEE)

#define ROOT_PORT_INTR_ON_MESG_MASK	(PCI_ERR_ROOT_CMD_COR_EN|	\
					PCI_ERR_ROOT_CMD_NONFATAL_EN|	\
					PCI_ERR_ROOT_CMD_FATAL_EN)

#define ERR_COR_ID(d)			(d & 0xffff)
#define ERR_UNCOR_ID(d)			(d >> 16)

#define AER_ERR_STATUS_MASK		(PCI_ERR_ROOT_UNCOR_RCV |	\
					PCI_ERR_ROOT_COR_RCV |		\
					PCI_ERR_ROOT_MULTI_COR_RCV |	\
					PCI_ERR_ROOT_MULTI_UNCOR_RCV)
uint8_t pcie_mask = 0;
uint16_t aer_cap;	/* AER capability offset */
static struct aer_stats aer_stats;	/* AER stats for this device */
uint16_t pcie_flags_reg; /* device/port type */
int pcie_cap; /* PCIe capability offset */

#define AER_NONFATAL			0
#define AER_FATAL			1
#define AER_CORRECTABLE			2
#define DPC_FATAL			3

/*
 * AER and DPC capabilities TLP Logging register sizes (PCIe r6.2, sec 7.8.4
 * & 7.9.14).
 */
#define PCIE_STD_NUM_TLP_HEADERLOG     4
#define PCIE_STD_MAX_TLP_PREFIXLOG     4
#define PCIE_STD_MAX_TLP_HEADERLOG      (PCIE_STD_NUM_TLP_HEADERLOG + 10)

struct pcie_tlp_log {
	union {
		u32 dw[PCIE_STD_MAX_TLP_HEADERLOG];
		struct {
			u32 _do_not_use[PCIE_STD_NUM_TLP_HEADERLOG];
			u32 prefix[PCIE_STD_MAX_TLP_PREFIXLOG];
		};
	};
	u8 header_len;          /* Length of the Logged TLP Header in DWORDs */
	bool flit;              /* TLP was logged when in Flit mode */
};

/* AER error strings */
static const char * const pcie_aer_error_severity_string[] = {
	"Correctable",
	"Uncorrectable (Non-Fatal)",
	"Uncorrectable (Fatal)"
};

struct pci_cap_saved_data {
	u16		cap_nr;
	bool		cap_extended;
	unsigned int	size;
	u32		data[];
};

struct pci_cap_saved_state {
	struct hlist_node		next;
	struct pci_cap_saved_data	cap;
};

#define AER_MAX_MULTI_ERR_DEVICES       5       /* Not likely to have more */
struct aer_err_info {
	struct pci_dev *dev[AER_MAX_MULTI_ERR_DEVICES];
	int error_dev_num;

	unsigned int id:16;

	unsigned int severity:2;	/* 0:NONFATAL | 1:FATAL | 2:COR */
	unsigned int __pad1:5;
	unsigned int multi_error_valid:1;

	unsigned int first_error:5;
	unsigned int __pad2:2;
	unsigned int tlp_header_valid:1;

	unsigned int status;		/* COR/UNCOR Error Status */
	unsigned int mask;		/* COR/UNCOR Error Mask */
	struct pcie_tlp_log tlp;	/* TLP Header */
};

#define	PCI_ERR_UNC_TRAIN	0x00000001			/* Training */
#define	PCI_ERR_UNC_DLP		0x00000010			/* Data Link Protocol */
#define	PCI_ERR_UNC_POISON_TLP	0x00001000			/* Poisoned TLP */
#define	PCI_ERR_UNC_FCP		0x00002000			/* Flow Control Protocol */
#define	PCI_ERR_UNC_COMP_TIME	0x00004000			/* Completion Timeout */
#define	PCI_ERR_UNC_COMP_ABORT	0x00008000			/* Completer Abort */
#define	PCI_ERR_UNC_UNX_COMP	0x00010000			/* Unexpected Completion */
#define	PCI_ERR_UNC_RX_OVER	0x00020000			/* Receiver Overflow */
#define	PCI_ERR_UNC_MALF_TLP	0x00040000			/* Malformed TLP */
#define	PCI_ERR_UNC_ECRC	0x00080000			/* ECRC Error Status */
#define	PCI_ERR_UNC_UNSUP	0x00100000			/* Unsupported Request */
#define	PCI_ERR_COR_RCVR	0x00000001			/* Receiver Error Status */
#define	PCI_ERR_COR_BAD_TLP	0x00000040			/* Bad TLP Status */
#define	PCI_ERR_COR_BAD_DLLP	0x00000080			/* Bad DLLP Status */
#define	PCI_ERR_COR_REP_ROLL	0x00000100			/* REPLAY_NUM Rollover */
#define	PCI_ERR_COR_REP_TIMER	0x00001000			/* Replay Timer Timeout */

#define PCI_EXP_AER_FLAGS	(PCI_EXP_DEVCTL_CERE | PCI_EXP_DEVCTL_NFERE | \
				 PCI_EXP_DEVCTL_FERE | PCI_EXP_DEVCTL_URRE)

#define PCI_DEVFN(slot, func)	((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCIE_MAX_DEVICE		32
#define PCIE_MAX_FUNCTION	8
#define PCIE_MAX_BUS		255
#define MULTI_FUNC_BIT			0x80
#define IS_MULTI_FUNC_BIT_SET(h)	(((h) & MULTI_FUNC_BIT) == MULTI_FUNC_BIT)
#define IS_PCIE_BRIDGE(h)		(((h) & PCI_HEADER_TYPE_MASK) == PCI_HEADER_TYPE_BRIDGE)

/* AER error record structure */
struct pcie_aer_error {
	struct pcie_aer_error *next;
	uint32_t domain;
	uint8_t bus;
	uint8_t devfn;
	uint16_t pos_cap_err;

	uint32_t uncor_status;
	uint32_t cor_status;
	uint32_t header_log0;
	uint32_t header_log1;
	uint32_t header_log2;
	uint32_t header_log3;
	uint32_t root_status;
	uint32_t source_id;
};

int pcie_rc_count = 13;

static const int pcie_aer_irqs[] = {
	IRQ_PCIE0_AER,
	IRQ_PCIE1_AER,
	IRQ_PCIE2_AER,
	IRQ_PCIE3_AER,
	IRQ_PCIE4_AER,
	IRQ_PCIE5_AER,
	IRQ_PCIE6_AER,
	IRQ_PCIE7_AER,
	IRQ_PCIE8_AER,
	IRQ_PCIE9_AER,
	IRQ_PCIE10_AER,
	IRQ_PCIE11_AER,
	IRQ_PCIE12_AER,
};

struct dw_pcie *get_dw_pcie_by_bdf(uint8_t bus, uint8_t dev, uint8_t func) {
	for (int i = 0; i < pcie_rc_count; i++) {
		if (!(pcie_mask & (1 << i)))
			continue;
		struct dw_pcie *rc = &pcie_ctrls[i];
		if (bus == rc->pp.primary_bus_nr ||
			(bus >= rc->pp.second_bus_nr && bus <= rc->pp.sub_bus_nr)) {
			return rc;
		}
	}
	return NULL;
}

/* AER error injection list */
static struct pcie_aer_error *pcie_aer_injected = NULL;

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

/* get pcie port type */
static inline int pci_pcie_type(const struct dw_pcie *dev)
{
	return pcie_flags_reg & PCI_EXP_FLAGS_TYPE >> 4;
}

bool pcie_cap_has_rtctl(const struct dw_pcie *dev)
{
	int type = pci_pcie_type(dev);

	return type == PCI_EXP_TYPE_ROOT_PORT ||
		type == PCI_EXP_TYPE_RC_EC;
}
static inline bool pcie_downstream_port(const struct dw_pcie *dev)
{
	int type = pci_pcie_type(dev);

	return type == PCI_EXP_TYPE_ROOT_PORT ||
		type == PCI_EXP_TYPE_DOWNSTREAM ||
		type == PCI_EXP_TYPE_PCIE_BRIDGE;
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

uint64_t pci_find_next_ext_capability(struct dw_pcie *dev, int cap)
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
u8 ras_pci_find_capability(struct dw_pcie *dev, int cap)
{
	uint8_t id;
	int ttl;
	uint16_t status = 0;
	uint8_t hdr_type, ent, pos = 0;

	/* find capability list start */
	pci_read_config_word(dev, PCI_STATUS, &status);
	if (!(status & PCI_STATUS_CAP_LIST))
		return 0;

	pci_read_config_byte(dev, PCI_HEADER_TYPE, &hdr_type);
	if (hdr_type & PCI_HEADER_TYPE_BRIDGE)
		return 0;

	switch (hdr_type) {
	case PCI_HEADER_TYPE_NORMAL:
	case PCI_HEADER_TYPE_BRIDGE:
		pos = PCI_CAPABILITY_LIST;
		break;
	case PCI_HEADER_TYPE_CARDBUS:
		pos = PCI_CB_CAPABILITY_LIST;
		break;
	}

	if (pos) {
		pci_read_config_byte(dev, pos, &pos);

		ttl = PCI_FIND_CAP_TTL;
		while (ttl--) {
			if (pos < 0x40)
				break;
			pos &= ~0x3;
			pci_read_config_byte(dev, pos, &ent);
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
uint16_t ras_pci_find_ext_capability(struct dw_pcie *dev, uint16_t cap)
{
	return pci_find_next_ext_capability(dev, cap);
}

// static void pci_add_saved_cap(struct dw_pcie *dev,
// 	struct pci_cap_saved_state *new_cap)
// {
// 	hlist_add_head(&new_cap->next, &dev->saved_cap_space);
// }

/**
 * _pci_add_cap_save_buffer - allocate buffer for saving given
 *				capability registers
 * @dev: the PCI device
 * @cap: the capability to allocate the buffer for
 * @extended: Standard or Extended capability ID
 * @size: requested size of the buffer
 */
static int _pci_add_cap_save_buffer(struct dw_pcie *dev, u16 cap,
					bool extended, unsigned int size)
{
	int pos;

	if (extended)
		pos = ras_pci_find_ext_capability(dev, cap);
	else
		pos = ras_pci_find_capability(dev, cap);

	if (!pos)
		return 0;

	/* Skip capability save buffer allocation for simplicity */
	/* In a real implementation, you might want to use a static buffer pool */
	return 0;
}

int pci_add_cap_save_buffer(struct dw_pcie *dev, char cap, unsigned int size)
{
	return _pci_add_cap_save_buffer(dev, cap, false, size);
}

int pci_add_ext_cap_save_buffer(struct dw_pcie *dev, u16 cap, unsigned int size)
{
	return _pci_add_cap_save_buffer(dev, cap, true, size);
}

static struct pci_cap_saved_state *_pci_find_saved_cap(struct dw_pcie *pci_dev,
								u16 cap, bool extended)
{
	struct pci_cap_saved_state *tmp;
	struct hlist_node *pos;

	hlist_for_each_entry(struct pci_cap_saved_state, tmp, pos, &pci_dev->saved_cap_space, next) {
		if (tmp->cap.cap_extended == extended && tmp->cap.cap_nr == cap)
			return tmp;
	}
	return NULL;
}

struct pci_cap_saved_state *pci_find_saved_cap(struct dw_pcie *dev, char cap)
{
	return _pci_find_saved_cap(dev, cap, false);
}

struct pci_cap_saved_state *pci_find_saved_ext_cap(struct dw_pcie *dev, u16 cap)
{
	return _pci_find_saved_cap(dev, cap, true);
}

/********************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
int pci_aer_clear_nonfatal_status(struct dw_pcie *dev)
{
	int aer = aer_cap;
	uint32_t status, sev;

	/* Clear status bits for ERR_NONFATAL errors only */
	pci_read_config_dword(dev, aer + PCI_ERR_UNCOR_STATUS, &status);
	pci_read_config_dword(dev, aer + PCI_ERR_UNCOR_SEVER, &sev);
	status &= ~sev;
	if (status)
		pci_write_config_dword(dev, aer + PCI_ERR_UNCOR_STATUS, status);

	return 0;
}

void pci_aer_clear_fatal_status(struct dw_pcie *dev)
{
	int aer = aer_cap;
	uint32_t status, sev;

	/* Clear status bits for ERR_FATAL errors only */
	pci_read_config_dword(dev, aer + PCI_ERR_UNCOR_STATUS, &status);
	pci_read_config_dword(dev, aer + PCI_ERR_UNCOR_SEVER, &sev);
	status &= sev;
	if (status)
		pci_write_config_dword(dev, aer + PCI_ERR_UNCOR_STATUS, status);
}

int pci_aer_clear_status(struct dw_pcie *dev)
{
	int aer = aer_cap;
	uint32_t status;
	int port_type;

	if (!aer)
		return -EIO;

	port_type = pci_pcie_type(dev);
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

void pci_save_aer_state(struct dw_pcie *dev)
{
	int aer = aer_cap;
	struct pci_cap_saved_state *save_state;
	u32 *cap;

	if (!aer)
		return;

	save_state = pci_find_saved_ext_cap(dev, PCI_EXT_CAP_ID_ERR);
	if (!save_state)
		return;

	cap = &save_state->cap.data[0];
	pci_read_config_dword(dev, aer + PCI_ERR_UNCOR_MASK, cap++);
	pci_read_config_dword(dev, aer + PCI_ERR_UNCOR_SEVER, cap++);
	pci_read_config_dword(dev, aer + PCI_ERR_COR_MASK, cap++);
	pci_read_config_dword(dev, aer + PCI_ERR_CAP, cap++);
	if (pcie_cap_has_rtctl(dev))
		pci_read_config_dword(dev, aer + PCI_ERR_ROOT_COMMAND, cap++);
}

void pci_restore_aer_state(struct dw_pcie *dev)
{
	int aer = aer_cap;
	struct pci_cap_saved_state *save_state;
	u32 *cap;

	if (!aer)
		return;

	save_state = pci_find_saved_ext_cap(dev, PCI_EXT_CAP_ID_ERR);
	if (!save_state)
		return;

	cap = &save_state->cap.data[0];
	pci_write_config_dword(dev, aer + PCI_ERR_UNCOR_MASK, *cap++);
	pci_write_config_dword(dev, aer + PCI_ERR_UNCOR_SEVER, *cap++);
	pci_write_config_dword(dev, aer + PCI_ERR_COR_MASK, *cap++);
	pci_write_config_dword(dev, aer + PCI_ERR_CAP, *cap++);
	if (pcie_cap_has_rtctl(dev))
		pci_write_config_dword(dev, aer + PCI_ERR_ROOT_COMMAND, *cap++);
}

static void pcie_aer_intr_handle_irq(irq_t irq)
{
	int aer = aer_cap;
	struct aer_err_source e_src = {};
	struct aer_err_info e_info = {};
	struct dw_pcie *dev = NULL;
	int pcie_index = -1;

	for (int i = 0; i < ARRAY_SIZE(pcie_aer_irqs); i++) {
		if (!(pcie_mask & (1 << i)))
			continue;
		if (irq == pcie_aer_irqs[i]) {
			pcie_index = i;
			dev = &pcie_ctrls[i];
			con_log(PCIE_AER_MODNAME ": PCIe AER IRQ: %d, %d\n", irq_ext(irq), i);
			break;
		}
	}

	if (pcie_index < 0 || !dev)
		return;

	if (!(e_src.status & AER_ERR_STATUS_MASK))
		return;
	pci_read_config_dword(dev, aer + PCI_ERR_ROOT_STATUS, &e_src.status);
	if (!(e_src.status & AER_ERR_STATUS_MASK))
		return;

	pci_read_config_dword(dev, aer + PCI_ERR_ROOT_ERR_SRC, &e_src.id);
	pci_write_config_dword(dev, aer + PCI_ERR_ROOT_STATUS, e_src.status);

	if (e_src.status & PCI_ERR_ROOT_COR_RCV) {
		e_info.id = ERR_COR_ID(e_src.id);
		e_info.severity = AER_CORRECTABLE;

		if (e_src.status & PCI_ERR_ROOT_MULTI_COR_RCV)
			e_info.multi_error_valid = 1;
		else
			e_info.multi_error_valid = 0;

		con_log(PCIE_AER_MODNAME ": ID: %d, severity: %d\n", e_info.id, e_info.severity);

		// if (find_source_device(pdev, &e_info))
		// 	aer_process_err_devices(&e_info);
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

		con_log(PCIE_AER_MODNAME ": ID: %d, severity: %d\n", e_info.id, e_info.severity);

		// aer_print_port_info(pdev, &e_info);

		// if (find_source_device(pdev, &e_info))
		// 	aer_process_err_devices(&e_info);
	}
}

#ifdef SYS_REALTIME
#define pcie_aer_irq_init()	do {} while (0)
#else
static void pcie_aer_irq_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(pcie_aer_irqs); i++) {
		if (!(pcie_mask & (1 << i)))
			continue;
		irqc_configure_irq(pcie_aer_irqs[i], 0, IRQ_LEVEL_TRIGGERED);
		irq_register_vector(pcie_aer_irqs[i], pcie_aer_intr_handle_irq);
		irqc_enable_irq(pcie_aer_irqs[i]);
	}
}
#endif

int pcie_aer_init(void)
{
	int n;
	struct dw_pcie *dev = NULL;
	uint16_t aer_cap;
	int pos;
	u16 reg16;
	u32 reg32;
	int i;

	/* pci msi and pci aer are all enabled by default */
	pcie_mask = (~__raw_readl(HW_PCIE_MASK) & ~__raw_readl(SW_PCIE_MASK));
	con_log(PCIE_AER_MODNAME ": PCIe mask:0x%08x\n", pcie_mask);

	/* register aer irq */
	pcie_aer_irq_init();

	pcie_aer_injected = NULL;

	/* Find first active PCIe controller */
	for (i = 0; i < pcie_rc_count; i++) {
		if (pcie_mask & (1 << i)) {
			dev = &pcie_ctrls[i];
			break;
		}
	}

	if (!dev) {
		con_log(PCIE_AER_MODNAME ": No active PCIe controllers found\n");
		return -ENODEV;
	}

	/* set pcie port type */
	pos = ras_pci_find_capability(dev, PCI_CAP_ID_EXP);
	if (!pos)
		return -ENODEV;
	pcie_cap = pos;
	pci_read_config_word(dev, pcie_cap + PCI_EXP_FLAGS, &reg16);
	pcie_flags_reg = reg16;

	aer_cap = ras_pci_find_ext_capability(dev, PCI_EXT_CAP_ID_ERR);
	if (!aer_cap)
		return -ENODEV;

	/* Initialize AER stats */
	memset(&aer_stats, 0, sizeof(aer_stats));

	/*
	 * We save/restore PCI_ERR_UNCOR_MASK, PCI_ERR_UNCOR_SEVER,
	 * PCI_ERR_COR_MASK, and PCI_ERR_CAP.  Root and Root Complex Event
	 * Collectors also implement PCI_ERR_ROOT_COMMAND (PCIe r6.0, sec
	 * 7.8.4.9).
	 */
	n = pcie_cap_has_rtctl(dev) ? 5 : 4;
	pci_add_ext_cap_save_buffer(dev, PCI_EXT_CAP_ID_ERR, sizeof(u32) * n);

	pci_aer_clear_status(dev);

	/* enable pcie error reporting */
	pcie_capability_set_word(dev, PCI_EXP_DEVCTL, PCI_EXP_AER_FLAGS);

	/* Clear PCIe Capability's Device Status */
	pcie_capability_read_word(dev, PCI_EXP_DEVSTA, &reg16);
	pcie_capability_write_word(dev, PCI_EXP_DEVSTA, reg16);

	/* Disable system error generation in response to error messages */
	pcie_capability_clear_word(dev, PCI_EXP_RTCTL,
				   SYSTEM_ERROR_INTR_ON_MESG_MASK);

	/* Enable Root Port's interrupt in response to error messages */
	pci_read_config_dword(dev, aer_cap + PCI_ERR_ROOT_COMMAND, &reg32);
	reg32 |= ROOT_PORT_INTR_ON_MESG_MASK;
	pci_write_config_dword(dev, aer_cap + PCI_ERR_ROOT_COMMAND, reg32);

	return 0;
}

int32_t pcie_report_error(struct pcie_aer_error *err)
{
	acpi_ghes_error_info einfo;
	uint8_t source_id = 0x80;
	struct dw_pcie *pcie_dev = get_dw_pcie_by_bdf(
		err->bus,
		(err->devfn >> 3) & 0x1F,
		err->devfn & 0x7
	);
	uint32_t val32 = 0;
	uint16_t val, val16 = 0;
	uint8_t pos;

	acpi_ghes_new_error_source(source_id, source_id);

	memset(&einfo, 0, sizeof(einfo));
	einfo.etype = ERROR_TYPE_PCIE;
	einfo.info.pcie.validation_bits = 0;
	if (pcie_dev && pci_read_config_word(pcie_dev, PCI_VENDOR_ID, &val16) == 0)
		einfo.info.pcie.vendor_id = val16;

	if (pcie_dev && pci_read_config_word(pcie_dev, PCI_DEVICE_ID, &val16) == 0) {
		einfo.info.pcie.device_id = val16;
		einfo.info.pcie.validation_bits |= CPER_PCIE_VALID_DEVICE_ID;
	}

	if (pcie_dev && pci_read_config_word(pcie_dev, PCI_COMMAND, &val16) == 0
		&& pci_read_config_word(pcie_dev, PCI_STATUS, &val) == 0) {
		einfo.info.pcie.command = val16;
		einfo.info.pcie.status = val;
		einfo.info.pcie.validation_bits |= CPER_PCIE_VALID_COMMAND_STATUS;
	}

	if (pcie_dev && pci_read_config_dword(pcie_dev, PCI_CLASS_PROG, &val32) == 0) {
		einfo.info.pcie.class_code[0] = (val32 >> 8) & 0xFF;
		einfo.info.pcie.class_code[1] = (val32 >> 16) & 0xFF;
		einfo.info.pcie.class_code[2] = (val32 >> 24) & 0xFF;
	}

	einfo.info.pcie.bus = err->bus;
	einfo.info.pcie.device = (err->devfn >> 3) & 0x1F;
	einfo.info.pcie.function = err->devfn & 0x7;
	einfo.info.pcie.segment = err->domain;
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

	pos = ras_pci_find_ext_capability(pcie_dev, PCI_EXT_CAP_ID_DSN);
	if (pos) {
		pci_read_config_dword(pcie_dev, pos + 0x04, &val32);
		einfo.info.pcie.serial_number_lower = val32;
		pci_read_config_dword(pcie_dev, pos + 0x08, &val32);
		einfo.info.pcie.serial_number_upper = val32;
		einfo.info.pcie.validation_bits |= CPER_PCIE_VALID_SERIAL_NUMBER;
	}

	acpi_ghes_record_errors(source_id, &einfo);

	return 0;
}

// /* Find AER error record */
// static struct pcie_aer_error *pcie_aer_find_error(uint32_t domain,
// 						 uint8_t bus, uint8_t devfn)
// {
// 	struct pcie_aer_error *err = pcie_aer_injected;

// 	while (err) {
// 		if (domain == err->domain &&
// 			bus == err->bus &&
// 			devfn == err->devfn) {
// 			return err;
// 		}
// 		err = err->next;
// 	}
// 	return NULL;
// }

/* Print AER error information */
static void pcie_aer_print_error(struct pcie_aer_error *err)
{
	con_log(PCIE_AER_MODNAME ": PCIe Bus Error: severity=%s\n",
		pcie_aer_error_severity_string[err->cor_status ? 0 : 1]);

	con_log(PCIE_AER_MODNAME ": device error status/mask=%08x/%08x\n",
		err->cor_status | err->uncor_status, 0);

	if (err->header_log0 || err->header_log1 ||
		err->header_log2 || err->header_log3) {
		con_log(PCIE_AER_MODNAME ": TLP Header: %08x %08x %08x %08x\n",
			err->header_log0, err->header_log1,
			err->header_log2, err->header_log3);
	}
}

/* Inject AER error */
static int pcie_aer_inject(struct pcie_aer_error *err)
{
	struct dw_pcie *dev;
	int ret = 0;
	u32 sever, cor_mask, uncor_mask;
	uint8_t devfn = err->devfn;

	if (pcie_mask & (1 << err->bus)) {
		con_log(PCIE_AER_MODNAME ": PCIe bus %d is masked\n", err->bus);
		return -EPERM;
	}

	dev = get_dw_pcie_by_bdf(err->bus, (devfn >> 3) & 0x1F, devfn & 0x7);
	if (!dev) {
		con_log(PCIE_AER_MODNAME ": Device not found\n");
		return -ENODEV;
	}

	if (!aer_cap) {
		con_log(PCIE_AER_MODNAME ": Device doesn't support AER\n");
		ret = -ENOTSUP;
		goto out_put;
	}
	pci_read_config_dword(dev, aer_cap + PCI_ERR_UNCOR_SEVER, &sever);
	pci_read_config_dword(dev, aer_cap + PCI_ERR_COR_MASK, &cor_mask);
	pci_read_config_dword(dev, aer_cap + PCI_ERR_UNCOR_MASK, &uncor_mask);

	err->next = pcie_aer_injected;
	pcie_aer_injected = err;

	con_log(PCIE_AER_MODNAME ": Injecting errors %08x/%08x\n",
		err->cor_status, err->uncor_status);

	pcie_aer_print_error(err);
	pcie_report_error(err);

out_put:
	return ret;
}

/* AER error injection command */
static int do_pcie_aer_inject(int argc, char *argv[])
{
	struct pcie_aer_error *err;
	int ret;
	int opt;
	int domain = 0, bus = 0, dev = 0, fn = 0;
	char *error_type = NULL;
	static struct pcie_aer_error err_static;

	err = &err_static;
	memset(err, 0, sizeof(*err));

	/* Reset getopt */
	optind = 1;

	while ((opt = getopt(argc, argv, "d:b:v:f:e:h")) != -1) {
		switch (opt) {
		case 'd':
			domain = strtoull(optarg, NULL, 16);
			break;
		case 'b':
			bus = strtoull(optarg, NULL, 16);
			break;
		case 'v':
			dev = strtoull(optarg, NULL, 16);
			break;
		case 'f':
			fn = strtoull(optarg, NULL, 16);
			break;
		case 'e':
			error_type = optarg;
			break;
		case 'h':
		default:
			con_log("Usage: pcie_aer inject [options]\n");
			con_log("Options:\n");
			con_log("  -d <domain>    PCI domain (hex)\n");
			con_log("  -b <bus>       PCI bus number (hex)\n");
			con_log("  -v <device>    PCI device number (hex)\n");
			con_log("  -f <function>  PCI function number (hex)\n");
			con_log("  -e <type>      Error type: ce|ue\n");
			con_log("  -h             Show this help\n");
			con_log("Example: pcie_aer inject -d 0 -b 1 -v 2 -f 0 -e ce\n");
			return -EINVAL;
		}
	}

	/* Check required parameters */
	if (domain == 0 && bus == 0 && dev == 0 && fn == 0) {
		con_log(PCIE_AER_MODNAME ": No device specified\n");
		return -EINVAL;
	}

	if (!error_type) {
		con_log(PCIE_AER_MODNAME ": No error type specified\n");
		return -EINVAL;
	}

	err->domain = domain;
	err->bus = bus;
	err->devfn = PCI_DEVFN(dev, fn);

	if (strcmp(error_type, "ce") == 0) {
		err->cor_status = PCI_ERR_COR_RCVR;
	} else if (strcmp(error_type, "ue") == 0) {
		err->uncor_status = PCI_ERR_UNC_DLP;
	} else {
		con_log(PCIE_AER_MODNAME ": Invalid error type '%s'. Use 'ce' or 'ue'\n", error_type);
		return -EINVAL;
	}

	ret = pcie_aer_inject(err);
	return ret;
}

DEFINE_COMMAND(pcie_aer, do_pcie_aer_inject,
			 "PCIe AER error injection commands",
			 "pcie_aer inject <domain>:<bus>:<dev>.<func> <error_type>\n"
			 "	- Inject AER error into PCIe device\n"
			 "	error_type: ce|ue\n");
