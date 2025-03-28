#include <target/pci.h>

uint32_t pci_get_conf_reg(int bus, int dev, int fun, int reg, int index)
{
	return dw_get_pci_conf_reg(bus, dev, fun, reg, index);
}

void pci_set_conf_reg(int bus, int dev, int fun, int reg, uint32_t val,
		      int index)
{
	dw_set_pci_conf_reg(bus, dev, fun, reg, val, index);
}

uint32_t pci_get_conf_reg_dword(int bus, int dev, int fun, int reg, int index)
{
	return dw_get_pci_conf_reg(bus, dev, fun, reg, index);
}

void pci_set_conf_reg_dword(int bus, int dev, int fun, int reg, uint32_t val,
		      int index)
{
	dw_set_pci_conf_reg(bus, dev, fun, reg, val, index);
}

uint16_t pci_get_conf_reg_word(int bus, int dev, int fun, int reg, int index)
{
    uint32_t val;
	uint8_t shift;
	shift = (reg & 0x2);
	val = dw_get_pci_conf_reg(bus, dev, fun, (reg & 0xfffffffc), index);
	val = val >> (8 * shift);
	return (uint16_t)val;
}

void pci_set_conf_reg_word(int bus, int dev, int fun, int reg, uint32_t val,
		      int index)
{
	uint32_t tmp;
	uint8_t shift;

	shift = (reg & 0x2);
	tmp = dw_get_pci_conf_reg(bus, dev, fun, (reg & 0xfffffffc), index);
	tmp = (tmp & (0xffff0000 >> (shift * 8))) | (val << (shift * 8));

	dw_set_pci_conf_reg(bus, dev, fun, (reg & 0xfffffffc), val, index);
}

uint8_t pci_get_conf_reg_byte(int bus, int dev, int fun, int reg, int index)
{
    uint32_t val;
	uint8_t shift;
	shift = (reg & 0x3);
	val = dw_get_pci_conf_reg(bus, dev, fun, (reg & 0xfffffffc), index);
	val = val >> (8 * shift);
	return (uint8_t)val;
}
void pci_set_conf_reg_byte(int bus, int dev, int fun, int reg, uint32_t val,
		      int index)
{
	uint32_t tmp, mask = 0xff;
	uint8_t shift;

	shift = (reg & 0x3);
	tmp = dw_get_pci_conf_reg(bus, dev, fun, (reg & 0xfffffffc), index);
	mask = mask << (8 * shift);
	mask = ~mask;
	tmp = ((tmp & mask) | val << (8 * shift));

	dw_set_pci_conf_reg(bus, dev, fun, (reg & 0xfffffffc), val, index);
}

bool pci_is_bridge(int bus, int dev, int fun, int index)
{
	uint32_t class_rev;

	class_rev = pci_get_conf_reg(bus, dev, fun, 0x8, index);
	class_rev >>= 8;
	class_rev &= 0xFFFF00;

	return (class_rev == 0x060400);
}

void pci_get_bridge_resource(int bus, int dev, int fun, int index)
{
	uint32_t i;
    pci_cfg cfg_space = {0};
    for(i = 0; i < PCI_STD_HEADER_SIZEOF/sizeof(uint32_t); i ++) {
        cfg_space.value[i] = pci_get_conf_reg(bus, dev, fun, i * sizeof(uint32_t), index);
    }
    if(PCI_HeaderLayout(cfg_space.cfg0.HeaderType) == PCI_HeaderType0) {
        pcie_dbg("Type0 : \n");
        pcie_dbg("\tVendorID                 : 0x%x\n",cfg_space.cfg0.VendorID);
        pcie_dbg("\tDeviceID                 : 0x%x\n",cfg_space.cfg0.DeviceID);
        pcie_dbg("\tCommand                  : 0x%x\n",cfg_space.cfg0.Command);
        pcie_dbg("\tStatus                   : 0x%x\n",cfg_space.cfg0.Status);
        pcie_dbg("\tRevisionID               : 0x%x\n",cfg_space.cfg0.RevisionID);
        pcie_dbg("\tClassCode                : 0x%x\n",cfg_space.cfg0.ClassCode);
        pcie_dbg("\tCacheLineSize            : 0x%x\n",cfg_space.cfg0.CacheLineSize);
        pcie_dbg("\tLatencyTimer             : 0x%x\n",cfg_space.cfg0.LatencyTimer);
        pcie_dbg("\tHeaderType               : 0x%x\n",cfg_space.cfg0.HeaderType);
        pcie_dbg("\tBIST                     : 0x%x\n",cfg_space.cfg0.BIST);
        pcie_dbg("\tBAR0                     : 0x%x\n",cfg_space.cfg0.BAR0);
        pcie_dbg("\tBAR1                     : 0x%x\n",cfg_space.cfg0.BAR1);
        pcie_dbg("\tBAR2                     : 0x%x\n",cfg_space.cfg0.BAR2);
        pcie_dbg("\tBAR3                     : 0x%x\n",cfg_space.cfg0.BAR3);
        pcie_dbg("\tBAR4                     : 0x%x\n",cfg_space.cfg0.BAR4);
        pcie_dbg("\tBAR5                     : 0x%x\n",cfg_space.cfg0.BAR5);
        pcie_dbg("\tCardbusCISPointer        : 0x%x\n",cfg_space.cfg0.CardbusCISPointer);
        pcie_dbg("\tSubsystemVendorID        : 0x%x\n",cfg_space.cfg0.SubsystemVendorID);
        pcie_dbg("\tSubsystemID              : 0x%x\n",cfg_space.cfg0.SubsystemID);
        pcie_dbg("\tExpansionROMBaseAddress0 : 0x%x\n",cfg_space.cfg0.ExpansionROMBaseAddress0);
        pcie_dbg("\tCapPointer               : 0x%x\n",cfg_space.cfg0.CapPointer);
        pcie_dbg("\tMinGnt                   : 0x%x\n",cfg_space.cfg0.MinGnt);
        pcie_dbg("\tMaxLat                   : 0x%x\n",cfg_space.cfg0.MaxLat);
        pcie_dbg("\tInterruptLine            : 0x%x\n",cfg_space.cfg0.InterruptLine);
        pcie_dbg("\tInterruptPin             : 0x%x\n",cfg_space.cfg0.InterruptPin);
    } else if(PCI_HeaderLayout(cfg_space.cfg1.HeaderType) == PCI_HeaderType1) {
        pcie_dbg("Type1 : \n");
        pcie_dbg("\tVendorID                 : 0x%x\n",cfg_space.cfg1.VendorID);
        pcie_dbg("\tDeviceID                 : 0x%x\n",cfg_space.cfg1.DeviceID);
        pcie_dbg("\tCommand                  : 0x%x\n",cfg_space.cfg1.Command);
        pcie_dbg("\tStatus                   : 0x%x\n",cfg_space.cfg1.Status);
        pcie_dbg("\tRevisionID               : 0x%x\n",cfg_space.cfg1.RevisionID);
        pcie_dbg("\tClassCode                : 0x%x\n",cfg_space.cfg1.ClassCode);
        pcie_dbg("\tCacheLineSize            : 0x%x\n",cfg_space.cfg1.CacheLineSize);
        pcie_dbg("\tLatencyTimer             : 0x%x\n",cfg_space.cfg1.LatencyTimer);
        pcie_dbg("\tHeaderType               : 0x%x\n",cfg_space.cfg1.HeaderType);
        pcie_dbg("\tBIST                     : 0x%x\n",cfg_space.cfg1.BIST);
        pcie_dbg("\tBAR0                     : 0x%x\n",cfg_space.cfg1.BAR0);
        pcie_dbg("\tBAR1                     : 0x%x\n",cfg_space.cfg1.BAR1);
        pcie_dbg("\tPrimaryBusNumber         : 0x%x\n",cfg_space.cfg1.PrimaryBusNumber);
        pcie_dbg("\tSecondaryBusNumber       : 0x%x\n",cfg_space.cfg1.SecondaryBusNumber);
        pcie_dbg("\tSubordinateBusNumber     : 0x%x\n",cfg_space.cfg1.SubordinateBusNumber);
        pcie_dbg("\tSecondaryLatencyTimer    : 0x%x\n",cfg_space.cfg1.SecondaryLatencyTimer);
        pcie_dbg("\tIOBase                   : 0x%x\n",cfg_space.cfg1.IOBase);
        pcie_dbg("\tIOLimit                  : 0x%x\n",cfg_space.cfg1.IOLimit);
        pcie_dbg("\tSecondaryStatus          : 0x%x\n",cfg_space.cfg1.SecondaryStatus);
        pcie_dbg("\tMemoryBase               : 0x%x\n",cfg_space.cfg1.MemoryBase);
        pcie_dbg("\tMemoryLimit              : 0x%x\n",cfg_space.cfg1.MemoryLimit);
        pcie_dbg("\tPrefetchableMemoryBase   : 0x%x\n",cfg_space.cfg1.PrefetchableMemoryBase);
        pcie_dbg("\tPrefetchableMemoryLimit  : 0x%x\n",cfg_space.cfg1.PrefetchableMemoryLimit);
        pcie_dbg("\tPrefetchableMemoryBaseUpper32Bits : 0x%x\n",cfg_space.cfg1.PrefetchableMemoryBaseUpper32Bits);
        pcie_dbg("\tPrefetchableMemoryLimitUpper32Bits : 0x%x\n",cfg_space.cfg1.PrefetchableMemoryLimitUpper32Bits);
        pcie_dbg("\tIOBaseUpper16Bits        : 0x%x\n",cfg_space.cfg1.IOBaseUpper16Bits);
        pcie_dbg("\tIOLimitUpper16Bits       : 0x%x\n",cfg_space.cfg1.IOLimitUpper16Bits);
        pcie_dbg("\tExpansionROMBaseAddress0 : 0x%x\n",cfg_space.cfg1.ExpansionROMBaseAddress0);
        pcie_dbg("\tCapPointer               : 0x%x\n",cfg_space.cfg1.CapPointer);
        pcie_dbg("\tBridgeControl            : 0x%x\n",cfg_space.cfg1.BridgeControl);
        pcie_dbg("\tInterruptLine            : 0x%x\n",cfg_space.cfg1.InterruptLine);
        pcie_dbg("\tInterruptPin             : 0x%x\n",cfg_space.cfg1.InterruptPin);
    }
}
	
void pci_update_bridge_bus_resource(int bus, int dev, int fun,
				    int sec_bus, int sub_bus, int index)
{
	uint32_t val = pci_get_conf_reg(bus, dev, fun, 0x4, index);
	uint32_t bus_reg = (sub_bus << 16) | (sec_bus << 8) | bus;

	val &= 0xFF000000;
	val |= bus_reg;

	pci_set_conf_reg(bus, dev, fun, 0x18, val, index);
	pcie_dbg("Write bus resource to BDF (%d-%d-%d): 0x%08lx\n",
	       bus, dev, fun, (unsigned long)val);
}
uint32_t pci_get_ids(int bus, int dev, int fun, int index)
{
	return pci_get_conf_reg(bus, dev, fun, 0x0, index);
}

uint32_t pci_get_headertype(int bus, int dev, int fun, int index)
{
    uint32_t val = pci_get_conf_reg(bus, dev, fun, 0xc, index);
	val &= 0x00FF0000;
	val = val >> 16 ;
    return PCI_HeaderLayout(val);
}

void pci_set_mem_en(int bus, int dev, int fun, int index)
{
    uint32_t val = pci_get_conf_reg(bus, dev, fun, PCI_COMMAND, index);
    pci_set_conf_reg(bus, dev, fun, PCI_COMMAND, val|PCI_COMMAND_MEMORY|PCI_COMMAND_MASTER, index);
    val = pci_get_conf_reg(bus, dev, fun, PCI_COMMAND, index);
    pcie_dbg("PCI_COMMAND : 0x%x\n", val);
}

void pci_set_io_en(int bus, int dev, int fun, int index)
{
    uint32_t val = pci_get_conf_reg(bus, dev, fun, PCI_COMMAND, index);
    pci_set_conf_reg(bus, dev, fun, PCI_COMMAND, val|PCI_COMMAND_IO|PCI_COMMAND_MASTER, index);
    val = pci_get_conf_reg(bus, dev, fun, PCI_COMMAND, index);
    pcie_dbg("PCI_COMMAND : 0x%x\n", val);
}

#define IORESOURCE_IO     0x00000001
#define IORESOURCE_MEM    0x00000002
#define IORESOURCE_PREFETCH  0x00000010
#define IORESOURCE_NONPREFETCH  0x00000020

typedef struct {
    uint64_t start;  
    uint64_t end;  
    uint32_t type;  
} io_resources;

io_resources bar[6] = {0};

void pci_update_bar_resource(int bus, int dev, int fun, int index)
{
    uint32_t i;
    uint64_t val;  
    uint64_t size;  
	struct pcie_port *pp = &(pcie_ctrls[index].pp);
    struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
    uint64_t io_offset = 0, mem_offset = 0, premem_offset = 0;  
    uint64_t pci_io_offset = 0, pci_mem_offset = 0x10000000, pci_premem_offset = 0x100000000;  
    uint64_t pci_io_base = 0, pci_mem_base = 0x10000000, pci_premem_base = 0x100000000;  
    
    /* uint64_t bar[6];   */
    for(i = 0; i < 6; i += 2) {
        if(pci_get_headertype(bus, dev, fun, index) == PCI_HeaderType0) {
            val = pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), index);
            pcie_dbg("BDF (%d-%d-%d) BAR%d = 0x%llx\n", bus, dev, fun, i, val);
            if ((val & PCI_SpaceIndicator) == PCI_SpaceIndicator_MemoryType) {
                bar[i].type |= IORESOURCE_MEM;
                if (PCI_MemoryType(val) == PCI_Memory_Type_32BIT) {
                    pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), 0xFFFFFFFF, index);
                    val = pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), index);
                    if(val) {
                        size = 0xFFFFFFFF - (val & 0xFFFFFFF0) + 1;
                        if (((val) & PCI_MemoryPrefetchable) == PCI_MemoryPrefetchable) {
                            bar[i].type |= IORESOURCE_PREFETCH;
                            bar[i].start = pp->prefetch_base + premem_offset;
                            bar[i].end = pp->prefetch_base + premem_offset + size - 1;
                            pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), pci_premem_offset, index);
                            pcie_dbg("\tPREFETCH_MEM32 : 0x%llx + 0x%llx\n", bar[i].start, pci_premem_offset);
                            premem_offset += size;
                            pci_premem_offset += size;
                        } else {
                            bar[i].type |= IORESOURCE_NONPREFETCH;
                            bar[i].start = pp->mem_base + mem_offset;
                            bar[i].end = pp->mem_base + mem_offset + size - 1;
                            pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), pci_mem_offset, index);
                            pcie_dbg("\tNONPREFETCH_MEM32 : 0x%llx + 0x%llx\n", bar[i].start, pci_mem_offset);
                            mem_offset += size;
                            pci_mem_offset += size;
                        }
                    } else {
                        bar[i].type = 0;
                        pcie_dbg("\tbar%d is not Used\n", i);
                    }
                    val = pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), index);
                    pcie_dbg("BDF (%d-%d-%d) BAR%d = 0x%llx\n", bus, dev, fun, i + 1, val);
                    pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), 0xFFFFFFFF, index);
                    val = pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), index);
                    if(val) {
                        if ((val & PCI_SpaceIndicator) == PCI_SpaceIndicator_MemoryType) {
                            size = 0xFFFFFFFF - (val & 0xFFFFFFF0) + 1;
                            bar[i+1].type |= IORESOURCE_MEM;
                            if (((val) & PCI_MemoryPrefetchable) == PCI_MemoryPrefetchable) {
                                bar[i+1].type |= IORESOURCE_PREFETCH;
                                bar[i+1].start = pp->prefetch_base + premem_offset;
                                bar[i+1].end = pp->prefetch_base + premem_offset + size - 1;
                                pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), pci_premem_offset, index);
                                pcie_dbg("\tPREFETCH_MEM32 : 0x%llx + 0x%llx\n", bar[i+1].start, pci_premem_offset);
                                premem_offset += size;
                                pci_premem_offset += size;
                            } else {
                                bar[i+1].type |= IORESOURCE_NONPREFETCH;
                                bar[i+1].start = pp->mem_base + mem_offset;
                                bar[i+1].end = pp->mem_base + mem_offset + size - 1;
                                pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), pci_mem_offset, index);
                                pcie_dbg("\tNONPREFETCH_MEM32 : 0x%llx + 0x%llx\n", bar[i+1].start, pci_mem_offset);
                                mem_offset += size;
                                pci_mem_offset += size;
                            }
                        } else {
                            size = 0xFFFFFFFF - (val & 0xFFFFFFC0) + 1;
                            bar[i+1].type |= IORESOURCE_IO;
                            bar[i+1].start = pp->io_base + io_offset;
                            bar[i+1].end = pp->io_base + io_offset + size - 1;
                            pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), pci_io_offset, index);
                            pcie_dbg("\tIO : 0x%llx + 0x%llx\n", bar[i+1].start, pci_io_offset);
                            io_offset += size;
                            pci_io_offset += size;
                        }
                    } else {
                        bar[i+1].type = 0;
                        pcie_dbg("\tbar%d is not Used\n", i + 1);
                    }
                } else if (PCI_MemoryType(val) == PCI_Memory_Type_64BIT) {
                    if (((val) & PCI_MemoryPrefetchable) == PCI_MemoryPrefetchable) {
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), 0xFFFFFFFF, index);
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), 0xFFFFFFFF, index);
                        val = (((uint64_t)(pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), index)))<<32)
                            + (pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), index));
                        size = 0xFFFFFFFFFFFFFFFF - (val & 0xFFFFFFFFFFFFFFF0) + 1;
                        bar[i].type |= IORESOURCE_PREFETCH;
                        bar[i].start = pp->prefetch_base + premem_offset;
                        bar[i].end = pp->prefetch_base + premem_offset + size - 1;
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), pci_premem_offset, index);
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), pci_premem_offset>>32, index);
                        pcie_dbg("\tPREFETCH_MEM64 : 0x%llx + 0x%llx\n", bar[i].start, pci_premem_offset);
                        premem_offset += size;
                        pci_premem_offset += size;
                    } else {
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), 0xFFFFFFFF, index);
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), 0xFFFFFFFF, index);
                        val = (((uint64_t)(pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), index)))<<32)
                            + (pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), index));
                        size = 0xFFFFFFFFFFFFFFFF - (val & 0xFFFFFFFFFFFFFFF0) + 1;
                        bar[i].type |= IORESOURCE_NONPREFETCH;
                        bar[i].start = pp->mem_base + mem_offset;
                        bar[i].end = pp->mem_base + mem_offset + size - 1;
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), pci_mem_offset, index);
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), pci_mem_offset>>32, index);
                        /* bar[i].start = pp->mem_base;
                        bar[i].end = pp->mem_base + size - 1;
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), pp->mem_base, index);
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), pp->mem_base>>32, index); */
                        pcie_dbg("\tNONPREFETCH_MEM64 : 0x%llx + 0x%llx\n", bar[i].start, pci_mem_offset);
                        mem_offset += size;
                        pci_mem_offset += size;
                    }
                } 
            } else {
                pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), 0xFFFFFFFF, index);
                val = pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), index);
                size = 0xFFFFFFFF - (val & 0xFFFFFFC0) + 1;
                bar[i].type |= IORESOURCE_IO;
                bar[i].start = pp->io_base + io_offset;
                bar[i].end = pp->io_base + io_offset + size - 1;
                pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + (i * 4), pci_io_offset, index);
                pcie_dbg("\tIO : 0x%llx + 0x%llx\n", bar[i].start, pci_io_offset);
                io_offset += size;
                pci_io_offset += size;
                val = pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), index);
                pcie_dbg("BDF (%d-%d-%d) BAR%d = 0x%llx\n", bus, dev, fun, i + 1, val);
                pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), 0xFFFFFFFF, index);
                val = pci_get_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), index);
                if(val) {
                    if ((val & PCI_SpaceIndicator) == PCI_SpaceIndicator_MemoryType) {
                        size = 0xFFFFFFFF - (val & 0xFFFFFFF0) + 1;
                        bar[i+1].type |= IORESOURCE_MEM;
                        if (((val) & PCI_MemoryPrefetchable) == PCI_MemoryPrefetchable) {
                            bar[i+1].type |= IORESOURCE_PREFETCH;
                            bar[i+1].start = pp->prefetch_base + premem_offset;
                            bar[i+1].end = pp->prefetch_base + premem_offset + size - 1;
                            pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), pci_premem_offset, index);
                            pcie_dbg("\tPREFETCH_MEM32 : 0x%llx + 0x%llx\n", bar[i+1].start, pci_premem_offset);
                            premem_offset += size;
                            pci_premem_offset += size;
                        } else {
                            bar[i+1].type |= IORESOURCE_NONPREFETCH;
                            bar[i+1].start = pp->mem_base + mem_offset;
                            bar[i+1].end = pp->mem_base + mem_offset + size - 1;
                            pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), pci_mem_offset, index);
                            pcie_dbg("\tNONPREFETCH_MEM32 : 0x%llx + 0x%llx\n", bar[i+1].start, pci_mem_offset);
                            mem_offset += size;
                            pci_mem_offset += size;
                        }
                    } else {
                        size = 0xFFFFFFFF - (val & 0xFFFFFFC0) + 1;
                        bar[i+1].type |= IORESOURCE_IO;
                        bar[i+1].start = pp->io_base + io_offset;
                        bar[i+1].end = pp->io_base + io_offset + size - 1;
                        pci_set_conf_reg(bus, dev, fun, PCI_BaseAddressRegister0 + ((i+1) * 4), pci_io_offset, index);
                        pcie_dbg("\tIO : 0x%llx + 0x%llx\n", bar[i+1].start, pci_io_offset);
                        io_offset += size;
                        pci_io_offset += size;
                    }
                } else {
                    bar[i+1].type = 0;
                    pcie_dbg("\tbar%d is not Used\n", i + 1);
                }
            }
        }
    }
	dw_pcie_dbi_ro_wr_en(pci);
    if(io_offset) {
        pcie_dbg("pci_io_offset = 0x%llx \n", pci_io_offset);
       	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_IO_BASE, 0x1);
        pcie_dbg("initPCI_IO_BASE = 0x%llx \n", val);
       	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_IO_BASE, pci_io_base, 0x1);
       	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_IO_LIMIT, (uint32_t)((pci_io_offset>>8)&PCI_IO_RANGE_MASK), 0x1); //4KB对齐
       	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_IO_BASE, 0x1);
        pcie_dbg("PCI_IO_BASE = 0x%llx \n", val);
       	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_IO_LIMIT, 0x1); //4KB对齐
        pcie_dbg("PCI_IO_LIMIT = 0x%llx \n", val);
    }
    if(mem_offset) {
        pcie_dbg("pci_mem_offset = 0x%llx \n", pci_mem_offset);
       	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_MEMORY_BASE, pci_mem_base, 0x2);
       	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_MEMORY_LIMIT, (uint32_t)((pci_mem_offset>>16)&PCI_PREF_RANGE_MASK), 0x2); //1MB对齐
       	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_MEMORY_BASE, 0x2);
        pcie_dbg("PCI_MEM_BASE = 0x%llx \n", val);
       	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_MEMORY_LIMIT, 0x2); //4KB对齐
        pcie_dbg("PCI_MEM_LIMIT = 0x%llx \n", val);
    }
    if(premem_offset) {
        pcie_dbg("pci_premem_offset = 0x%llx \n", pci_premem_offset);
       	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_PREF_MEMORY_BASE, (uint32_t)(pci_premem_base), 0x2);
       	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_PREF_MEMORY_LIMIT, (uint32_t)(((pci_premem_offset>>16)&PCI_PREF_RANGE_MASK)|PCI_PREF_RANGE_TYPE_64), 0x2); //1MB对齐
       	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_PREF_BASE_UPPER32, (uint32_t)(pci_premem_base>>32), 0x4);
       	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_PREF_LIMIT_UPPER32, (uint32_t)(pci_premem_offset>>32), 0x4); //1MB对齐
       	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_PREF_MEMORY_BASE, 0x2);
        pcie_dbg("PCI_PREF_MEMORY_BASE = 0x%llx \n", val);
       	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_PREF_MEMORY_LIMIT, 0x2);
        pcie_dbg("PCI_PREF_MEMORY_LIMIT = 0x%llx \n", val);
       	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_PREF_BASE_UPPER32, 0x4);
        pcie_dbg("PCI_PREF_BASE_UPPER32 = 0x%llx \n", val);
       	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_PREF_LIMIT_UPPER32, 0x4);
        pcie_dbg("PCI_PREF_LIMIT_UPPER32 = 0x%llx \n", val);
    }
	dw_pcie_dbi_ro_wr_dis(pci);
}

void mem_test()
{
    uint32_t i, val;
    /* uint32_t j; */

    for (i = 0; i < 6; i ++) {
        if(bar[i].type) {
            /* for (j = 0; (bar[i].start + j * 0x4) < bar[i].end; j ++) {
                val = __raw_readl(bar[i].start + j * 0x4);
                pcie_dbg("init bar%d mem : 0x%llx = 0x%x\n", i, bar[i].start + j * 0x4, val);
            } */
            val = __raw_readl(bar[i].start + 0x24);
            pcie_dbg("init bar%d mem : 0x%llx = 0x%x\n", i, bar[i].start + 0x24, val);
            __raw_writel(0x5a5a5a5a, bar[i].start + 0x24);
            val = __raw_readl(bar[i].start + 0x24);
            pcie_dbg("\twrite bar%d mem : 0x%llx = 0x%x\n", i, bar[i].start + 0x24, val);
            val = __raw_readl(bar[i].start + 0x28);
            pcie_dbg("init bar%d mem : 0x%llx = 0x%x\n", i, bar[i].start + 0x28, val);
            __raw_writel(0x5a5a5a5a, bar[i].start + 0x28);
            val = __raw_readl(bar[i].start + 0x28);
            pcie_dbg("\twrite bar%d mem : 0x%llx = 0x%x\n", i, bar[i].start + 0x28, val);
            val = __raw_readl(bar[i].start + 0x30);
            pcie_dbg("init bar%d mem : 0x%llx = 0x%x\n", i, bar[i].start + 0x30, val);
            __raw_writel(0x5a5a5a5a, bar[i].start + 0x30);
            val = __raw_readl(bar[i].start + 0x30);
            pcie_dbg("\twrite bar%d mem : 0x%llx = 0x%x\n", i, bar[i].start + 0x30, val);
        }
    }
}


bool pci_is_multi_fun(int bus, int dev, int index)
{
	uint32_t val = pci_get_conf_reg(bus, dev, 0, 0xC, index);

	val &= 0xFF0000;
	val >>= 16;

	return !!(val & _BV(7));
}

int pci_find_capability(int index, int bus, int dev, int fun, int cap)
{
    uint8_t id;
    uint16_t ent;
    int ttl = PCI_FIND_CAP_TTL; 
    uint32_t pos = PCI_CAPABILITY_LIST;

    pos = (uint32_t)pci_get_conf_reg_byte(bus, dev, fun, pos, index);

    while ((ttl)--) {
        if (pos < 0x40)
            break;
        pos &= ~3;
        ent = pci_get_conf_reg_word(bus, dev, fun, pos, index);

        id = ent & 0xff;
        if (id == 0xff)
            break;
        if (id == cap)
            return pos;
        pos = (ent >> 8);
    }
    return 0;
}
void pci_find_capability_list(int index, int bus, int dev, int fun) 
{
    uint8_t id;
    uint16_t ent;
    int ttl = PCI_FIND_CAP_TTL; 
    uint32_t pos = PCI_CAPABILITY_LIST;
    char cap_list[PCI_CAP_ID_MAX][45] = {
        "PM(Power Management)",
        "AGP(Accelerated Graphics Port)",
        "VPD(Vital Product Data)",
        "SLOTID(Slot Identification)",
        "MSI(Message Signalled Interrupts)",
        "CHSWP(CompactPCI HotSwap)",
        "PCIX(PCI-X)",
        "HT(HyperTransport)",
        "VNDR(Vendor-Specific)",
        "DBG(Debug port)",
        "CCRC(CompactPCI Central Resource Control)",
        "SHPC(PCI Standard Hot-Plug Controller)",
        "SSVID(Bridge subsystem vendor/device ID)",
        "AGP3(AGP Target PCI-PCI bridge)",
        "SECDEV(Secure Device)",
        "EXP(PCI Express)",
        "MSIX(MSI-X)",
        "SATA(SATA Data/Index Conf.)",
        "AF(PCI Advanced Features)",
        "EA(PCI Enhanced Allocation)",
    };
    pos = (uint32_t)pci_get_conf_reg_byte(bus, dev, fun, pos, index);

    while ((ttl)--) {
        if (pos < 0x40)
            break;
        pos &= ~3;
        ent = pci_get_conf_reg_word(bus, dev, fun, pos, index);

        id = ent & 0xff;
        if (id == 0xff)
            break;
        printf("\tBDF (%d-%d-%d) cap %s is found at 0x%x\n", bus, dev, fun, cap_list[id], pos);
        pos = (ent >> 8);
    }
}

uint16_t pci_find_ext_capability(int index, int bus, int dev, int fun, int cap) //u16 start, 
{
    uint32_t header;
    int ttl;
    uint16_t pos = PCI_CFG_SPACE_SIZE;

    /* minimum 8 bytes per capability */
    ttl = (PCI_CFG_SPACE_EXP_SIZE - PCI_CFG_SPACE_SIZE) / 8;

    /* if (dev->cfg_size <= PCI_CFG_SPACE_SIZE)
        return 0; */

    /* if (start)
        pos = start; */
    header = pci_get_conf_reg_dword(bus, dev, fun, pos, index);
    /* if (header != PCIBIOS_SUCCESSFUL)
        return 0; */

    /*
    ¦* If we have no capabilities, this is indicated by cap ID,
    ¦* cap version and next pointer all being 0.
    ¦*/
    if (header == 0)
        return 0;

    while (ttl-- > 0) {
        if (PCI_EXT_CAP_ID(header) == cap) // && pos != start
            return pos;

        pos = PCI_EXT_CAP_NEXT(header);
        if (pos < PCI_CFG_SPACE_SIZE)
            break;

        header = pci_get_conf_reg_dword(bus, dev, fun, pos, index);
        /* if (pci_get_conf_reg_dword(bus, dev, fun, pos, index) != PCIBIOS_SUCCESSFUL)
            break; */
    }

    return 0;
}

void pci_find_ext_capability_list(int index, int bus, int dev, int fun)
{
    uint32_t header;
    int ttl;
    uint16_t pos = PCI_CFG_SPACE_SIZE;
    char cap_list[PCI_EXT_CAP_ID_MAX][45] = {
        "ERR(Advanced Error Reporting",
        "VC(Virtual Channel Capability)",
        "DSN(Device Serial Number)",
        "PWR(Power Budgeting)",
        "RCLD(Root Complex Link Declaration)",
        "RCILC(Root Complex Internal Link Control)",
        "RCEC(Root Complex Event Collector)",
        "MFVC(Multi-Function VC Capability)",
        "VC9(same as _VC)",
        "RCRB(Root Complex RB?)",
        "VNDR(Vendor-Specific)",
        "CAC(Config Access - obsolete)",
        "ACS(Access Control Services)",
        "ARI(Alternate Routing ID)",
        "ATS(Address Translation Services)",
        "SRIOV(Single Root I/O Virtualization)",
        "MRIOV(Multi Root I/O Virtualization)",
        "MCAST(Multicast)",
        "PRI(Page Request Interface)",
        "AMD_XXX(Reserved for AMD)",
        "REBAR(Resizable BAR)",
        "DPA(Dynamic Power Allocation)",
        "TPH(TPH Requester)",
        "LTR(Latency Tolerance Reporting)",
        "SECPCI(Secondary PCIe Capability)",
        "PMUX(Protocol Multiplexing)",
        "PASID(Process Address Space ID)",
        "Reserved(0x1C)",
        "DPC(Downstream Port Containment)",
        "L1SS(L1 PM Substates)",
        "PTM(Precision Time Measurement)",
        "Reserved(0x20)",
        "Reserved(0x21)",
        "Reserved(0x22)",
        "DVSEC(Designated Vendor-Specific)",
        "Reserved(0x24)",
        "DLF(Data Link Feature)",
        "PL_16GT(Physical Layer 16.0 GT/s)",
        "Reserved(0x27)",
        "Reserved(0x28)",
        "Reserved(0x29)",
        "PL_32GT(Physical Layer 32.0 GT/s)",
        "Reserved(0x2B)",
        "Reserved(0x2C)",
        "Reserved(0x2D)",
        "DOE(Data Object Exchange)",
    };

    /* minimum 8 bytes per capability */
    ttl = (PCI_CFG_SPACE_EXP_SIZE - PCI_CFG_SPACE_SIZE) / 8;

    while (ttl-- > 0) {
        header = pci_get_conf_reg_dword(bus, dev, fun, pos, index);
        if (header == 0)
            return;
        printf("\tBDF (%d-%d-%d) Extended cap %s is found at 0x%x\n", bus, dev, fun, cap_list[PCI_EXT_CAP_ID(header)], pos);

        pos = PCI_EXT_CAP_NEXT(header);
        if (pos < PCI_CFG_SPACE_SIZE)
            break;
    }
}

int pci_enum(int bus, int index)
{
	int sec_bus = bus + 1;
	int sub_bus = bus;
	int dev, fun;
	uint32_t id;

    /* for (dev = 0; dev < 32; dev++) { */
    for (dev = 0; dev < 1; dev++) {
		for (fun = 0; fun < 8; fun++) {
            pcie_dbg("BDF (%d-%d-%d) scan\n", bus, dev, fun);
			id = pci_get_ids(bus, dev, fun, index);
			if (id != 0xFFFFFFFF) {
				printf("\tBDF (%d-%d-%d) found, id: 0x%08lx\n", bus, dev, fun, (unsigned long)id);
                if(pci_get_headertype(bus, dev, fun, index) == PCI_HeaderType1) {
                    sub_bus = pci_enum(sec_bus, index);
                    pci_update_bridge_bus_resource(bus, dev, fun,
                            sec_bus, sub_bus, index);
                    pcie_dbg("\t\tBrdge (%d-%d-%d): "
                            "sec bus: %d, sub bus: %d\n",
                            bus, dev, fun, sec_bus, sub_bus);
                }
                pci_get_bridge_resource(bus, dev, fun, index);
                pci_update_bar_resource(bus, dev, fun, index);
                pci_find_capability_list(index, bus, dev, fun);
                pci_find_ext_capability_list(index, bus, dev, fun);
                
                printf("\tBDF (%d-%d-%d) found, id: 0x%08lx\n", bus, dev, fun, (unsigned long)id);
                pci_set_mem_en(bus, dev, fun, index);
				sec_bus = sub_bus + 1;
			}
			if (!pci_is_multi_fun(bus, dev, index)) {
                /* pcie_dbg("\tBDF (%d-%d-%d) \n", bus, dev, fun); */
				fun = 8;
            }
			/* else
				break; */
		}
		if (bus == 1)
			break;
	}
	return sub_bus;
}

void pci_init(void)
{
}

