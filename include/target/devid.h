#ifndef __DEVID_H_INCLUDE__
#define __DEVID_H_INCLUDE__

#ifdef CONFIG_VENDOR_ID
#define DEV_VENDOR_ID			((uint16_t)CONFIG_VENDOR_ID)
#else
#define DEV_VENDOR_ID			((uint16_t)990)
#endif
#ifdef CONFIG_PRODUCT_ID
#define DEV_PRODUCT_ID			((uint8_t)CONFIG_PRODUCT_ID)
#else
#define DEV_PRODUCT_ID			((uint8_t)0)
#endif
#ifdef CONFIG_SERIAL_NO
#define DEV_SERIAL_NO			((uint16_t)CONFIG_SERIAL_NO)
#else
#define DEV_SERIAL_NO			((uint16_t)0)
#endif

/* device features have impacts on reported device ID */
extern __near__ uint32_t system_device_id;

#define DEVICE_ARCH_MASK	0xFF000000
#define DEVICE_INTF_MASK	0x00FF0000
#define DEVICE_FUNC_MASK	0x0000FFFF

#define DEVICE_ARCH_MCS51	0x01000000
#define DEVICE_ARCH_AVR		0x02000000
#define DEVICE_ARCH_ARM		0x04000000

#define DEVICE_ARCH(arch)	\
	(system_device_id |= ((arch) & DEVICE_ARCH_MASK))
#define DEVICE_INTF(intf)	\
	(system_device_id |= ((intf) & DEVICE_INTF_MASK))
#define DEVICE_FUNC(func)	\
	(system_device_id |= ((func) & DEVICE_FUNC_MASK))

#define DEVICE_INTF_USB		0x00010000

#define DEVICE_FUNC_KBD		0x00000100
#define DEVICE_FUNC_LCD		0x00000200
#define DEVICE_FUNC_IFD		0x00000400
#define DEVICE_FUNC_ICC		0x00000800
#define DEVICE_FUNC_COS		0x00001000

extern text_char_t system_vendor_name[];
extern text_char_t system_device_name[];
uint16_t system_product_id(void);

#endif
