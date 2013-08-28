#include <initguid.h>
#include "acpi_mon.h"

#define OS_DRIVER_TEXT_DESC		L"ACPI Interpreter Monitor"
#define OS_DRIVER_FDO_NAME		L"\\Device\\acpimon"
#define OS_DRIVER_FILE_NAME		L"\\DosDevices\\acpimon-"
#define OS_DBG_TAG			(ULONG)'PCAK'
#define OS_DRIVER_VERSION_STR		"1.0.0"
/* This features should be device specific. */
#define OS_DRIVER_FDO_TYPE		FILE_DEVICE_UNKNOWN
#define OS_DRIVER_FDO_CHAR		0

#define os_driver_reg_path		\
	L"\\REGISTRY\\Machine\\System\\CurrentControlSet\\SERVICES\\AcpiMon\\Parameters"
const GUID *os_driver_fdo_guid = &GUID_DEVCLASS_MONITOR;

#ifdef CONFIG_SYSCTL
#define OS_MOF_RESOURCE_NAME		L"ACPIMON"
WMIGUIDREGINFO os_driver_wmi_guids[1] = {
	/* Driver information. */
	{ &MONITOR_WMI_GUID, 1, 0 }
};
int os_driver_wmi_nrguids = sizeof (os_driver_wmi_guids) /
			    sizeof (WMIGUIDREGINFO);
#endif

NTSTATUS wdm_dev_dispatch_ioctl_int(PDEVICE_OBJECT dev, PIRP irp)
{
	return wdm_dev_io_cmpl(dev, irp, STATUS_NOT_SUPPORTED, 0);
}

#include "../wdm/wdm_dbg.c"
#include "../wdm/wdm_dev.c"
