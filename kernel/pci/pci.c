#include <target/pci.h>

#ifdef CONFIG_ARCH_DUOWEN
uint32_t pci_get_conf_reg(int bus, int dev, int fun, int reg, int index)
{
	return dw_get_pci_conf_reg(bus, dev, fun, reg, index);
}

void pci_set_conf_reg(int bus, int dev, int fun, int reg, uint32_t val,
		      int index)
{
	dw_set_pci_conf_reg(bus, dev, fun, reg, val, index);
}

bool pci_is_bridge(int bus, int dev, int fun, int index)
{
	uint32_t class_rev;

	class_rev = pci_get_conf_reg(bus, dev, fun, 0x8, index);
	class_rev >>= 8;
	class_rev &= 0xFFFF00;

	return (class_rev == 0x060400);
}

void pci_update_bridge_bus_resource(int bus, int dev, int fun,
				    int sec_bus, int sub_bus, int index)
{
	uint32_t val = pci_get_conf_reg(bus, dev, fun, 0x18, index);
	uint32_t bus_reg = (sub_bus << 16) | (sec_bus << 8) | bus;

	val &= 0xFF000000;
	val |= bus_reg;

	pci_set_conf_reg(bus, dev, fun, 0x18, val, index);
	printf("Write bus resource to BDF (%d-%d-%d): 0x%08lx\n",
	       bus, dev, fun, (unsigned long)val);
}

uint32_t pci_get_ids(int bus, int dev, int fun, int index)
{
	return pci_get_conf_reg(bus, dev, fun, 0x0, index);
}

bool pci_is_multi_fun(int bus, int dev, int index)
{
	uint32_t val = pci_get_conf_reg(bus, dev, 0, 0xC, index);

	val &= 0xFF0000;
	val >>= 16;

	return !!(val & _BV(7));
}

int pci_enum(int bus, int index)
{
	int sec_bus = bus + 1;
	int sub_bus = bus;
	int dev, fun;
	uint32_t id;

	for (dev = 0; dev < 32; dev++) {
		for (fun = 0; fun < 8; fun++) {
			id = pci_get_ids(bus, dev, fun, index);
			if (id != 0xFFFFFFFF) {
				printf("BDF (%d-%d-%d) found, id: 0x%08lx\n",
				       bus, dev, fun, (unsigned long)id);
				sub_bus = pci_enum(sec_bus, index);
				pci_update_bridge_bus_resource(bus, dev, fun,
					sec_bus, sub_bus, index);
				printf("  Brdge (%d-%d-%d): "
				       "sec bus: %d, sub bus: %d\n",
				       bus, dev, fun, sec_bus, sub_bus);
				sec_bus = sub_bus + 1;
			}
			if (!pci_is_multi_fun(bus, dev, index))
				fun = 8;
			else
				break;
		}
		if (bus == 1)
			break;
	}
	return sub_bus;
}
#endif

void pci_init(void)
{
}
