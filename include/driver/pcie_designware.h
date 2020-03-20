#ifndef _PCIE_DESIGNWARE_H
#define _PCIE_DESIGNWARE_H

#include "pci_glue.h"

/* Parameters for the waiting for link up routine */
#define LINK_WAIT_MAX_RETRIES		10
#define LINK_WAIT_USLEEP_MIN		90000
#define LINK_WAIT_USLEEP_MAX		100000

/* Parameters for the waiting for iATU enabled routine */
#define LINK_WAIT_MAX_IATU_RETRIES	5
#define LINK_WAIT_IATU			9

/* Synopsys-specific PCIe configuration registers */
#define PCIE_PORT_FORCE_OFF     0x708
#define PORT_LINK_NUM_MASK      GENMASK(7, 0)
#define PORT_LINK_NUM(n)        FIELD_PREP(PORT_LINK_NUM_MASK, n)

#define PCIE_PORT_LINK_CONTROL		0x710
#define PORT_LINK_MODE_MASK		GENMASK(21, 16)
#define PORT_LINK_MODE(n)		FIELD_PREP(PORT_LINK_MODE_MASK, n)
#define PORT_LINK_MODE_4_LANES		PORT_LINK_MODE(0x7)
#define PORT_LINK_MODE_8_LANES		PORT_LINK_MODE(0xf)
#define PORT_LINK_MODE_16_LANES		PORT_LINK_MODE(0x1f)
#define PCIE_TIMER_CTRL_MAX_FUN_NUM_OFF 0x718

#define PCIE_PORT_DEBUG0		0x728
#define PORT_LOGIC_LTSSM_STATE_MASK	0x1f
#define PORT_LOGIC_LTSSM_STATE_L0	0x11
#define PCIE_PORT_DEBUG1		0x72C
#define PCIE_PORT_DEBUG1_LINK_UP		BIT(4)
#define PCIE_PORT_DEBUG1_LINK_IN_TRAINING	BIT(29)

#define PCIE_LINK_WIDTH_SPEED_CONTROL	0x80C
#define PORT_LOGIC_SPEED_CHANGE		BIT(17)
#define PORT_LOGIC_LINK_WIDTH_MASK	GENMASK(12, 8)
#define PORT_LOGIC_LINK_WIDTH(n)	FIELD_PREP(PORT_LOGIC_LINK_WIDTH_MASK, n)
#define PORT_LOGIC_LINK_WIDTH_4_LANES	PORT_LOGIC_LINK_WIDTH(0x4)
#define PORT_LOGIC_LINK_WIDTH_8_LANES	PORT_LOGIC_LINK_WIDTH(0x8)
#define PORT_LOGIC_LINK_WIDTH_16_LANES	PORT_LOGIC_LINK_WIDTH(0x10)

#define PCIE_GEN3_RELATED_OFF   0x890
#define RATE_SHADOW_SEL_MASK    GENMASK(25, 24)
#define RATE_SHADOW_SEL(n)      FIELD_PREP(RATE_SHADOW_SEL_MASK, n)

#define PCIE_MSI_ADDR_LO		0x820
#define PCIE_MSI_ADDR_HI		0x824
#define PCIE_MSI_INTR0_ENABLE		0x828
#define PCIE_MSI_INTR0_MASK		0x82C
#define PCIE_MSI_INTR0_STATUS		0x830

#define PCIE_ATU_VIEWPORT		0x900
#define PCIE_ATU_REGION_INBOUND		BIT(31)
#define PCIE_ATU_REGION_OUTBOUND	0
#define PCIE_ATU_REGION_INDEX2		0x2
#define PCIE_ATU_REGION_INDEX1		0x1
#define PCIE_ATU_REGION_INDEX0		0x0
#define PCIE_ATU_CR1			0x904
#define PCIE_ATU_TYPE_MEM		0x0
#define PCIE_ATU_TYPE_IO		0x2
#define PCIE_ATU_TYPE_CFG0		0x4
#define PCIE_ATU_TYPE_CFG1		0x5
#define PCIE_ATU_CR2			0x908
#define PCIE_ATU_ENABLE			BIT(31)
#define PCIE_ATU_BAR_MODE_ENABLE	BIT(30)
#define PCIE_ATU_LOWER_BASE		0x90C
#define PCIE_ATU_UPPER_BASE		0x910
#define PCIE_ATU_LIMIT			0x914
#define PCIE_ATU_LOWER_TARGET		0x918
#define PCIE_ATU_BUS(x)			FIELD_PREP(GENMASK(31, 24), x)
#define PCIE_ATU_DEV(x)			FIELD_PREP(GENMASK(23, 19), x)
#define PCIE_ATU_FUNC(x)		FIELD_PREP(GENMASK(18, 16), x)
#define PCIE_ATU_UPPER_TARGET		0x91C

#define PCIE_IATU_REGION_CTRL1_OFF_OUTBOUND         0x0
#define PCIE_IATU_REGION_CTRL2_OFF_OUTBOUND         0x4
#define PCIE_IATU_LWR_BASE_ADDR_OFF_OUTBOUND        0x8
#define PCIE_IATU_UPPER_BASE_ADDR_OFF_OUTBOUND      0xC
#define PCIE_IATU_LWR_LIMIT_ADDR_OFF_OUTBOUND       0x10
#define PCIE_IATU_LWR_TARGET_ADDR_OFF_OUTBOUND      0x14
#define PCIE_IATU_UPPER_TARGET_ADDR_OFF_OUTBOUND    0x18
#define PCIE_IATU_REGION_CTRL3_OFF_OUTBOUND         0x1c
#define PCIE_IATU_UPPER_LIMIT_ADDR_OFF_OUTBOUND     0x20

#define PCIE_MISC_CONTROL_1_OFF		0x8BC
#define PCIE_DBI_RO_WR_EN		BIT(0)

#define PCIE_PL_CHK_REG_CONTROL_STATUS			0xB20
#define PCIE_PL_CHK_REG_CHK_REG_START			BIT(0)
#define PCIE_PL_CHK_REG_CHK_REG_CONTINUOUS		BIT(1)
#define PCIE_PL_CHK_REG_CHK_REG_COMPARISON_ERROR	BIT(16)
#define PCIE_PL_CHK_REG_CHK_REG_LOGIC_ERROR		BIT(17)
#define PCIE_PL_CHK_REG_CHK_REG_COMPLETE		BIT(18)

#define PCIE_PL_CHK_REG_ERR_ADDR			0xB28

/*
 * iATU Unroll-specific register definitions
 * From 4.80 core version the address translation will be made by unroll
 */
#define PCIE_ATU_UNR_REGION_CTRL1	0x00
#define PCIE_ATU_UNR_REGION_CTRL2	0x04
#define PCIE_ATU_UNR_LOWER_BASE		0x08
#define PCIE_ATU_UNR_UPPER_BASE		0x0C
#define PCIE_ATU_UNR_LIMIT		0x10
#define PCIE_ATU_UNR_LOWER_TARGET	0x14
#define PCIE_ATU_UNR_UPPER_TARGET	0x18

/*
 * The default address offset between dbi_base and atu_base. Root controller
 * drivers are not required to initialize atu_base if the offset matches this
 * default; the driver core automatically derives atu_base from dbi_base using
 * this offset, if atu_base not set.
 */
#define DEFAULT_DBI_ATU_OFFSET (0x3 << 20)

/* Register address builder */
#define PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(region) \
		((region) << 9)

#define PCIE_GET_ATU_INB_UNR_REG_OFFSET(region) \
		(((region) << 9) | BIT(8))

#define MAX_MSI_IRQS			256
#define MAX_MSI_IRQS_PER_CTRL		32
#define MAX_MSI_CTRLS			(MAX_MSI_IRQS / MAX_MSI_IRQS_PER_CTRL)
#define MSI_REG_CTRL_BLOCK_SIZE		12
#define MSI_DEF_NUM_VECTORS		32

/* Maximum number of inbound/outbound iATUs */
#define MAX_IATU_IN			256
#define MAX_IATU_OUT			256

#define PCIBIOS_SUCCESSFUL		0x00
#define PCIBIOS_FUNC_NOT_SUPPORTED	0x81
#define PCIBIOS_BAD_VENDOR_ID		0x83
#define PCIBIOS_DEVICE_NOT_FOUND	0x86
#define PCIBIOS_BAD_REGISTER_NUMBER	0x87
#define PCIBIOS_SET_FAILED		0x88
#define PCIBIOS_BUFFER_TOO_SMALL	0x89

/* Device classes and subclasses */

#define PCI_CLASS_NOT_DEFINED       0x0000
#define PCI_CLASS_NOT_DEFINED_VGA   0x0001

#define PCI_BASE_CLASS_STORAGE      0x01
#define PCI_CLASS_STORAGE_SCSI      0x0100
#define PCI_CLASS_STORAGE_IDE       0x0101
#define PCI_CLASS_STORAGE_FLOPPY    0x0102
#define PCI_CLASS_STORAGE_IPI       0x0103
#define PCI_CLASS_STORAGE_RAID      0x0104
#define PCI_CLASS_STORAGE_SATA      0x0106
#define PCI_CLASS_STORAGE_SATA_AHCI 0x010601
#define PCI_CLASS_STORAGE_SAS       0x0107
#define PCI_CLASS_STORAGE_EXPRESS   0x010802
#define PCI_CLASS_STORAGE_OTHER     0x0180


#define PCI_BASE_CLASS_NETWORK      0x02
#define PCI_CLASS_NETWORK_ETHERNET  0x0200
#define PCI_CLASS_NETWORK_TOKEN_RING    0x0201
#define PCI_CLASS_NETWORK_FDDI      0x0202
#define PCI_CLASS_NETWORK_ATM       0x0203
#define PCI_CLASS_NETWORK_OTHER     0x0280

#define PCI_BASE_CLASS_DISPLAY      0x03
#define PCI_CLASS_DISPLAY_VGA       0x0300
#define PCI_CLASS_DISPLAY_XGA       0x0301
#define PCI_CLASS_DISPLAY_3D        0x0302
#define PCI_CLASS_DISPLAY_OTHER     0x0380

#define PCI_BASE_CLASS_MULTIMEDIA   0x04
#define PCI_CLASS_MULTIMEDIA_VIDEO  0x0400
#define PCI_CLASS_MULTIMEDIA_AUDIO  0x0401
#define PCI_CLASS_MULTIMEDIA_PHONE  0x0402
#define PCI_CLASS_MULTIMEDIA_HD_AUDIO   0x0403
#define PCI_CLASS_MULTIMEDIA_OTHER  0x0480

#define PCI_BASE_CLASS_MEMORY       0x05
#define PCI_CLASS_MEMORY_RAM        0x0500
#define PCI_CLASS_MEMORY_FLASH      0x0501
#define PCI_CLASS_MEMORY_OTHER      0x0580

#define PCI_BASE_CLASS_BRIDGE       0x06
#define PCI_CLASS_BRIDGE_HOST       0x0600
#define PCI_CLASS_BRIDGE_ISA        0x0601
#define PCI_CLASS_BRIDGE_EISA       0x0602
#define PCI_CLASS_BRIDGE_MC     0x0603
#define PCI_CLASS_BRIDGE_PCI        0x0604
#define PCI_CLASS_BRIDGE_PCMCIA     0x0605
#define PCI_CLASS_BRIDGE_NUBUS      0x0606
#define PCI_CLASS_BRIDGE_CARDBUS    0x0607
#define PCI_CLASS_BRIDGE_RACEWAY    0x0608
#define PCI_CLASS_BRIDGE_OTHER      0x0680

#define PCI_BASE_CLASS_COMMUNICATION    0x07
#define PCI_CLASS_COMMUNICATION_SERIAL  0x0700
#define PCI_CLASS_COMMUNICATION_PARALLEL 0x0701
#define PCI_CLASS_COMMUNICATION_MULTISERIAL 0x0702
#define PCI_CLASS_COMMUNICATION_MODEM   0x0703
#define PCI_CLASS_COMMUNICATION_OTHER   0x0780

#define PCI_BASE_CLASS_SYSTEM       0x08
#define PCI_CLASS_SYSTEM_PIC        0x0800
#define PCI_CLASS_SYSTEM_PIC_IOAPIC 0x080010
#define PCI_CLASS_SYSTEM_PIC_IOXAPIC    0x080020
#define PCI_CLASS_SYSTEM_DMA        0x0801
#define PCI_CLASS_SYSTEM_TIMER      0x0802
#define PCI_CLASS_SYSTEM_RTC        0x0803
#define PCI_CLASS_SYSTEM_PCI_HOTPLUG    0x0804
#define PCI_CLASS_SYSTEM_SDHCI      0x0805
#define PCI_CLASS_SYSTEM_OTHER      0x0880

#define PCI_BASE_CLASS_INPUT        0x09
#define PCI_CLASS_INPUT_KEYBOARD    0x0900
#define PCI_CLASS_INPUT_PEN     0x0901
#define PCI_CLASS_INPUT_MOUSE       0x0902
#define PCI_CLASS_INPUT_SCANNER     0x0903
#define PCI_CLASS_INPUT_GAMEPORT    0x0904
#define PCI_CLASS_INPUT_OTHER       0x0980

#define PCI_BASE_CLASS_DOCKING      0x0a
#define PCI_CLASS_DOCKING_GENERIC   0x0a00
#define PCI_CLASS_DOCKING_OTHER     0x0a80

#define PCI_BASE_CLASS_PROCESSOR    0x0b
#define PCI_CLASS_PROCESSOR_386     0x0b00
#define PCI_CLASS_PROCESSOR_486     0x0b01
#define PCI_CLASS_PROCESSOR_PENTIUM 0x0b02
#define PCI_CLASS_PROCESSOR_ALPHA   0x0b10
#define PCI_CLASS_PROCESSOR_POWERPC 0x0b20
#define PCI_CLASS_PROCESSOR_MIPS    0x0b30
#define PCI_CLASS_PROCESSOR_CO      0x0b40

#define PCI_BASE_CLASS_SERIAL       0x0c
#define PCI_CLASS_SERIAL_FIREWIRE   0x0c00
#define PCI_CLASS_SERIAL_FIREWIRE_OHCI  0x0c0010
#define PCI_CLASS_SERIAL_ACCESS     0x0c01
#define PCI_CLASS_SERIAL_SSA        0x0c02
#define PCI_CLASS_SERIAL_USB        0x0c03
#define PCI_CLASS_SERIAL_USB_UHCI   0x0c0300
#define PCI_CLASS_SERIAL_USB_OHCI   0x0c0310
#define PCI_CLASS_SERIAL_USB_EHCI   0x0c0320
#define PCI_CLASS_SERIAL_USB_XHCI   0x0c0330
#define PCI_CLASS_SERIAL_USB_DEVICE 0x0c03fe
#define PCI_CLASS_SERIAL_FIBER      0x0c04
#define PCI_CLASS_SERIAL_SMBUS      0x0c05
#define PCI_CLASS_SERIAL_IPMI       0x0c07
#define PCI_CLASS_SERIAL_IPMI_SMIC  0x0c0700
#define PCI_CLASS_SERIAL_IPMI_KCS   0x0c0701
#define PCI_CLASS_SERIAL_IPMI_BT    0x0c0702

#define PCI_BASE_CLASS_WIRELESS         0x0d
#define PCI_CLASS_WIRELESS_RF_CONTROLLER    0x0d10
#define PCI_CLASS_WIRELESS_WHCI         0x0d1010

#define PCI_BASE_CLASS_INTELLIGENT  0x0e
#define PCI_CLASS_INTELLIGENT_I2O   0x0e00

#define PCI_BASE_CLASS_SATELLITE    0x0f
#define PCI_CLASS_SATELLITE_TV      0x0f00
#define PCI_CLASS_SATELLITE_AUDIO   0x0f01
#define PCI_CLASS_SATELLITE_VOICE   0x0f03
#define PCI_CLASS_SATELLITE_DATA    0x0f04

#define PCI_BASE_CLASS_CRYPT        0x10
#define PCI_CLASS_CRYPT_NETWORK     0x1000
#define PCI_CLASS_CRYPT_ENTERTAINMENT   0x1001
#define PCI_CLASS_CRYPT_OTHER       0x1080

#define PCI_BASE_CLASS_SIGNAL_PROCESSING 0x11
#define PCI_CLASS_SP_DPIO       0x1100
#define PCI_CLASS_SP_OTHER      0x1180

#define PCI_CLASS_OTHERS        0xff

struct pcie_port;
struct dw_pcie;
struct dw_pcie_ep;

enum dw_pcie_region_type {
	DW_PCIE_REGION_UNKNOWN,
	DW_PCIE_REGION_INBOUND,
	DW_PCIE_REGION_OUTBOUND,
};

enum dw_pcie_device_mode {
	DW_PCIE_UNKNOWN_TYPE,
	DW_PCIE_EP_TYPE,
	DW_PCIE_LEG_EP_TYPE,
	DW_PCIE_RC_TYPE,
};

enum dw_pcie_access_type {
    DW_PCIE_CDM,
    DW_PCIE_SHADOW,
    DW_PCIE_ELBI,
    DW_PCIE_ATU,
    DW_PCIE_DMA,
    DW_PCIE_MSI_TABLE,
    DW_PCIE_MSI_PBA
};

struct pci_bus {
	//struct list_head node;		/* Node in list of buses */
	//struct pci_bus	*parent;	/* Parent bus this bridge is on */
	//struct list_head children;	/* List of child buses */
	//struct list_head devices;	/* List of devices on this bus */
	//struct pci_dev	*self;		/* Bridge device as seen by parent */
	//struct list_head slots;		/* List of slots on this bus;
					   //protected by pci_slot_mutex */
	//struct resource *resource[PCI_BRIDGE_RESOURCE_NUM];
	//struct list_head resources;	/* Address space routed to this bus */
	//struct resource busn_res;	/* Bus numbers routed to this bus */

	//struct pci_ops	*ops;		/* Configuration access functions */
	//struct msi_controller *msi;	/* MSI controller */
	//void		*sysdata;	/* Hook for sys-specific extension */
	//struct proc_dir_entry *procdir;	/* Directory entry in /proc/bus/pci */

	//unsigned char	number;		/* Bus number */
	//unsigned char	primary;	/* Number of primary bridge */
	//unsigned char	max_bus_speed;	/* enum pci_bus_speed */
	//unsigned char	cur_bus_speed;	/* enum pci_bus_speed */
#ifdef CONFIG_PCI_DOMAINS_GENERIC
	//int		domain_nr;
#endif

	char		name[48];

	//unsigned short	bridge_ctl;	/* Manage NO_ISA/FBB/et al behaviors */
	//pci_bus_flags_t bus_flags;	/* Inherited by child buses */
	//struct device		*bridge;
	//struct device		dev;
	//struct bin_attribute	*legacy_io;	/* Legacy I/O for this bus */
	//struct bin_attribute	*legacy_mem;	/* Legacy mem */
	//unsigned int		is_added:1;
};


struct dw_pcie_host_ops {
	int (*rd_own_conf)(struct pcie_port *pp, int where, int size, uint32_t *val);
	int (*wr_own_conf)(struct pcie_port *pp, int where, int size, uint32_t val);
	int (*rd_other_conf)(struct pcie_port *pp, struct pci_bus *bus,
			     unsigned int devfn, int where, int size, uint32_t *val);
	int (*wr_other_conf)(struct pcie_port *pp, struct pci_bus *bus,
			     unsigned int devfn, int where, int size, uint32_t val);
	int (*host_init)(struct pcie_port *pp);
	void (*scan_bus)(struct pcie_port *pp);
	void (*set_num_vectors)(struct pcie_port *pp);
	int (*msi_host_init)(struct pcie_port *pp);
};

struct pcie_port {
	uint8_t			primary_bus_nr;
    uint8_t         second_bus_nr;
    uint8_t         sub_bus_nr;
	uint64_t		cfg_bar0;
	uint64_t		cfg_bar1;
	uint64_t		cfg_size;
	uint64_t		io_base;
	uint64_t		io_size;
	uint64_t		mem_base;
	uint64_t		mem_size;
    uint64_t        prefetch_base;
    uint64_t        prefetch_size;
    struct list_head        *pcie_dev; // In resource decrease order
    struct list_head        *pcie_bridge; 
	//struct resource		*cfg;
	//struct resource		*io;
	//struct resource		*mem;
	//struct resource		*busn;
	int			irq;
	const struct dw_pcie_host_ops *ops;
	//int			msi_irq;
	//struct irq_domain	*irq_domain;
	//struct irq_domain	*msi_domain;
	//dma_addr_t		msi_data;
	//struct page		*msi_page;
	//struct irq_chip		*msi_irq_chip;
	uint32_t			num_vectors;
	uint32_t			irq_mask[MAX_MSI_CTRLS];
	//struct pci_bus		*root_bus;
	//raw_spinlock_t		lock;
	//DECLARE_BITMAP(msi_irq_in_use, MAX_MSI_IRQS);
};

enum dw_pcie_as_type {
	DW_PCIE_AS_UNKNOWN,
	DW_PCIE_AS_MEM,
	DW_PCIE_AS_IO,
};

struct dw_pcie_ops {
	uint64_t	(*cpu_addr_fixup)(struct dw_pcie *pcie, uint64_t cpu_addr);
	uint32_t	(*read_dbi)(struct dw_pcie *pcie, enum dw_pcie_access_type type,
                    uint32_t reg, size_t size);
	void	(*write_dbi)(struct dw_pcie *pcie, enum dw_pcie_access_type type,
                    uint32_t reg, size_t size, uint32_t val);
	int	(*link_up)(struct dw_pcie *pcie);
	int	(*start_link)(struct dw_pcie *pcie);
	void	(*stop_link)(struct dw_pcie *pcie);
};

struct dw_pcie {
	struct device		*dev;
	uint64_t 	dbi_base;
    uint64_t    mem32_base;
    uint64_t    mem32_size;
    uint64_t    mem64_base;
    uint64_t    mem64_size;
    uint64_t    io_base;
    uint64_t    io_size;
    uint32_t    lane_num;
    uint8_t     axi_dbi_port;
    uint8_t     order;
	/* Used when iatu_unroll_enabled is true */
	void 		*atu_base;
	uint32_t			num_viewport;
	uint8_t			iatu_unroll_enabled;
	struct pcie_port	pp;
	//struct dw_pcie_ep	ep;
	const struct dw_pcie_ops *ops;
    bool        active;
	unsigned int		version;
};

#define to_dw_pcie_from_pp(port) container_of((port), struct dw_pcie, pp)

#define to_dw_pcie_from_ep(endpoint)   \
		container_of((endpoint), struct dw_pcie, ep)

uint8_t dw_pcie_find_capability(struct dw_pcie *pci, uint8_t cap);
uint16_t dw_pcie_find_ext_capability(struct dw_pcie *pci, uint8_t cap);

int dw_pcie_read(void  *addr, int size, uint32_t *val);
int dw_pcie_write(void  *addr, int size, uint32_t val);

uint32_t dw_pcie_read_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint32_t reg, size_t size);
void dw_pcie_write_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint32_t reg, uint32_t val, size_t size);
uint32_t dw_pcie_read_dbi2(struct dw_pcie *pci, uint32_t reg, size_t size);
void dw_pcie_write_dbi2(struct dw_pcie *pci, uint32_t reg, size_t size, uint32_t val);
uint32_t dw_pcie_read_atu(struct dw_pcie *pci, enum dw_pcie_region_type region, uint32_t index,  uint32_t reg, size_t size);
void dw_pcie_write_atu(struct dw_pcie *pci, enum dw_pcie_region_type region, uint32_t index,  uint32_t reg, size_t size, uint32_t val);
int dw_pcie_link_up(struct dw_pcie *pci);
int dw_pcie_wait_for_link(struct dw_pcie *pci);
void dw_pcie_prog_outbound_atu(struct dw_pcie *pci, int index,
			       int type, uint64_t cpu_addr, uint64_t pci_addr,
			       uint64_t size);
int dw_pcie_prog_inbound_atu(struct dw_pcie *pci, int index, int bar,
			     uint64_t cpu_addr, enum dw_pcie_as_type as_type);
void dw_pcie_disable_atu(struct dw_pcie *pci, int index,
			 enum dw_pcie_region_type type);
void dw_pcie_setup(struct dw_pcie *pci);

static inline void dw_pcie_dbi_ro_wr_en(struct dw_pcie *pci)
{
	uint32_t reg;
	uint32_t val;

	reg = PCIE_MISC_CONTROL_1_OFF;
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, reg, 0x4);
	val |= PCIE_DBI_RO_WR_EN;
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, reg, val, 0x4);
}

static inline void dw_pcie_dbi_ro_wr_dis(struct dw_pcie *pci)
{
	uint32_t reg;
	uint32_t val;

	reg = PCIE_MISC_CONTROL_1_OFF;
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, reg, 0x4);
	val &= ~PCIE_DBI_RO_WR_EN;
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, reg, val, 0x4);
}

void dw_pcie_msi_init(struct pcie_port *pp);
void dw_pcie_free_msi(struct pcie_port *pp);
void dw_pcie_setup_rc(struct pcie_port *pp);
int dw_pcie_host_init(struct pcie_port *pp);
void dw_pcie_host_deinit(struct pcie_port *pp);
int dw_pcie_allocate_domains(struct pcie_port *pp);

#endif /* _PCIE_DESIGNWARE_H */
