#include <target/generic.h>
#include <target/arch.h>

/* cpu ID functions */
#define __cpu_hw_device_version(did)	((did & VER_MASK) >> VER)
#define __cpu_hw_device_major(did)	((did & MAJOR_MASK) >> MAJOR)
#define __cpu_hw_device_minor(did)	((did & MINOR_MASK) >> MINOR)
#define __cpu_hw_device_revision(did)	((did & REV_MASK) >> REV)
#define __cpu_hw_device_class(did)	((did & CLASS_MASK) >> CLASS)

uint8_t cpu_hw_device_class(void)
{
	uint32_t did;

	did = __raw_readl(DID0);

	switch (__cpu_hw_device_version(did)) {
	case 0:
		return DEVICE_CLASS_SANDSTORM;
	case 1:
		switch (__cpu_hw_device_class(did)) {
		case 0:
			return DEVICE_CLASS_SANDSTORM;
		case 1:
			return DEVICE_CLASS_FURY;
		case 3:
			return DEVICE_CLASS_DUSTDEVIL;
		case 4:
			return DEVICE_CLASS_TEMPEST;
		default:
			return DEVICE_CLASS_UNKNOWN;
		}
		break;
	default:
		return DEVICE_CLASS_UNKNOWN;
	}
}

uint16_t cpu_hw_device_revision(void)
{
	return __cpu_hw_device_revision(__raw_readl(DID0));
}
