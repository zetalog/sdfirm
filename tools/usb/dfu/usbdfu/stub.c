#include <initguid.h>
#include "usb_dfu.h"

#define OS_DRIVER_TEXT_DESC		L"USB Device Firmware Upgrade"
#define OS_DRIVER_FDO_NAME		L"\\Device\\usbdfu"
#define OS_DRIVER_FILE_NAME		L"\\DosDevices\\usbdfu-"
#define OS_DBG_TAG			(ULONG)'UFDK'
#define OS_DRIVER_VERSION_STR		"1.0.0"
/* This features should be device specific. */
#define OS_DRIVER_FDO_TYPE		FILE_DEVICE_UNKNOWN
#define OS_DRIVER_FDO_CHAR		0

#define os_driver_reg_path		\
	L"\\REGISTRY\\Machine\\System\\CurrentControlSet\\SERVICES\\USBDFU\\Parameters"
const GUID *os_driver_fdo_guid = &GUID_DEVCLASS_FIRMWARE;

#ifdef CONFIG_SYSCTL
#define OS_MOF_RESOURCE_NAME		L"USBDFU"
WMIGUIDREGINFO os_driver_wmi_guids[1] = {
	/* Driver information. */
	{ &FIRMWARE_WMI_GUID, 1, 0 }
};
int os_driver_wmi_nrguids = sizeof (os_driver_wmi_guids) /
			    sizeof (WMIGUIDREGINFO);
#endif

NTSTATUS wdm_dev_dispatch_ioctl_int(PDEVICE_OBJECT dev, PIRP irp)
{
	return wdm_dev_io_cmpl(dev, irp, STATUS_NOT_SUPPORTED, 0);
}

#include "../../../lib/wdm/wdm_dbg.c"
#include "../../../lib/wdm/wdm_dev.c"
#include "../../../lib/wdm/usb/usb_core.c"
#include "../../../lib/wdm/usb/wdm_core.c"
#include "../../../lib/wdm/usb/usb_dev.c"
#include "../../../lib/wdm/usb/wdm_dev.c"

#include "../../../lib/cmn/bitops.c"
#include "../../../lib/cmn/crc32.c"
