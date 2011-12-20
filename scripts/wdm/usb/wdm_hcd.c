#include <wdm/os_hcd.h>

struct os_hcd_driver *os_hcd_driver;
struct os_hcd_port_driver *os_hcd_port_driver;

os_device os_hcd_device(os_usbhc *hcd)
{
	return hcd->dev;
}

status_t os_hcd_start_device(os_device dev)
{
	os_usbhc *hcd = dev_priv(dev);
	status_t status;

	/* TODO: What should we do here? */
	status = hcd->hcd_driver->start(hcd);
	if (!NT_SUCCESS(status)) goto end;
end:
	return status;
}

void os_hcd_stop_device(os_device dev)
{
	os_usbhc *hcd = dev_priv(dev);

	hcd->hcd_driver->stop(hcd);
	/* TODO: What should we do here? */
}

status_t os_hcd_init_device(os_device dev)
{
	os_usbhc *hcd = dev_priv(dev);

	hcd->hcd_driver = os_hcd_driver;
	hcd->dev = dev;
	os_hcd_init_ports(hcd);
	return hcd->hcd_driver->init(hcd);
}

void os_hcd_exit_device(os_device dev)
{
	os_usbhc *hcd = dev_priv(dev);

	/* !usb_driver means not initialized */
	if (hcd->hcd_driver) {
		hcd->hcd_driver->exit(hcd);
	}
	/* TODO: What should we do here? */
}

void os_hcd_suspend_device(os_device dev)
{
	os_usbhc *hcd = dev_priv(dev);

	if (hcd->hcd_driver->suspend)
		hcd->hcd_driver->suspend(hcd);
	hcd->resumed_state = hcd->state;
	hcd->state = USB_STATE_SUSPENDED;
}

void os_hcd_resume_device(os_device dev)
{
	os_usbhc *hcd = dev_priv(dev);
	if (hcd->hcd_driver->resume)
		hcd->hcd_driver->resume(hcd);
	hcd->state = hcd->resumed_state;
}

int wdm_usb_port_by_name(PDEVICE_OBJECT dev, PUNICODE_STRING file)
{
	os_usbhc *usb = dev_priv(dev);
	LONG ix;
	ULONG uval; 
	ULONG length;
	ULONG umultiplier;
	
	length = (file->Length / sizeof(WCHAR));
	if (length != 0) {
		os_dbg(OS_DBG_DEBUG, "file = %ws length = %d\n", file->Buffer, length);
		/* Parse the port#. */
		ix = length - 1;
		/* If last char isn't digit, decrement it. */
		while ((ix > -1) &&
		       ((file->Buffer[ix] < (WCHAR) '0') ||
			(file->Buffer[ix] > (WCHAR) '9'))) {
			ix--;
		}
		if (ix > -1) {
			uval = 0;
			umultiplier = 1;
			/* Traversing least to most significant digits. */
			while ((ix > -1) &&
			       (file->Buffer[ix] >= (WCHAR) '0') &&
			       (file->Buffer[ix] <= (WCHAR) '9')) {
				uval += (umultiplier *
					(ULONG) (file->Buffer[ix] - (WCHAR) '0'));
				ix--;
				umultiplier *= 10;
			}
			if (uval < NR_HCD_PORTS) {
				return uval;
			}
		}
	}
	return INVALID_HCD_PORT;
}

struct os_hcd_file *wdm_hcd_open_file(os_usbhc *hcd,
				      int port_no, os_file *file)
{
	struct os_hcd_file *hcd_file = NULL;
	status_t status;

	hcd_file = os_mem_alloc(sizeof (struct os_hcd_file), "HCD_FILE");
	if (!hcd_file) goto end;
	hcd_file->file = file;
	hcd_file->port_no = port_no;
	hcd_file->usb = hcd;
	status = hcd->hcd_driver->open(hcd_file);
	if (!NT_SUCCESS(status)) goto end;
	file->priv = hcd_file;
end:
	return hcd_file;
}

void wdm_hcd_close_file(os_usbhc *hcd, os_file *file)
{
	struct os_hcd_file *hcd_file = file_priv(file);

	if (hcd_file) {
		ASSERT(hcd == hcd_file->usb);
		ASSERT(file == hcd_file->file);
		hcd->hcd_driver->close(hcd_file);
		file->priv = NULL;
		os_mem_free(hcd_file);
	}
}

status_t os_hcd_open_file(os_device dev, os_file *file)
{
	int port_no;
	NTSTATUS status = STATUS_SUCCESS;
	os_usbhc *hcd = dev_priv(dev);
	struct os_hcd_file *hcd_file = NULL;

	if (0 == file->file_name.Length) {
		/* control interfacing */
		port_no = INVALID_HCD_PORT;
	} else {
		/* IO interfacing */
		port_no = wdm_usb_port_by_name(dev, &file->file_name);
		if (port_no == INVALID_HCD_PORT) {
			status = STATUS_INVALID_PARAMETER;
			goto end;
		}
	}
	hcd_file = wdm_hcd_open_file(hcd, port_no, file);
	if (!hcd_file)
		status = STATUS_INSUFFICIENT_RESOURCES;
end:
	return status;
}

void os_hcd_close_file(os_device dev, os_file *file)
{
	os_usbhc *hcd = dev_priv(dev);
	wdm_hcd_close_file(hcd, file);
}

status_t os_hcd_ioctl_file(os_device dev, os_file *file,
			   unsigned long code, void *arg,
			   int *plength)
{
	os_usbhc *hcd = dev_priv(dev);
	status_t status = STATUS_SUCCESS;
	int length = *plength;

	switch (code) {
	default:
		if (hcd->hcd_driver->ioctl && file_priv(file)) {
			return hcd->hcd_driver->ioctl(file_priv(file), code, arg, plength);
		} else {
			*plength = 0;
			status = STATUS_INVALID_PARAMETER;
		}
		break;
	}
	return status;
}

status_t os_hcd_read_file(os_device dev, os_file *file,
			  char *buf, size_t *count)
{
	os_usbhc *usb = dev_priv(dev);
	status_t status = STATUS_SUCCESS;
	if (usb->hcd_driver->read && file_priv(file)) {
		return usb->hcd_driver->read(file_priv(file), buf, count);
	} else {
		*count = 0;
		return STATUS_INVALID_PARAMETER;
	}
}

status_t os_hcd_write_file(os_device dev, os_file *file,
			   const char *buf, size_t *count)
{
	os_usbhc *usb = dev_priv(dev);
	status_t status = STATUS_SUCCESS;
	if (usb->hcd_driver->read && file_priv(file)) {
		return usb->hcd_driver->write(file_priv(file), buf, count);
	} else {
		*count = 0;
		return STATUS_INVALID_PARAMETER;
	}
}

struct os_driver wdm_hcd_driver = {
	os_hcd_start_device,	/* start device */
	os_hcd_stop_device,	/* stop device */
	os_hcd_init_device,	/* alloc memory */
	os_hcd_exit_device,	/* free memory */
	os_hcd_suspend_device,
	os_hcd_resume_device,
	os_hcd_open_file,
	os_hcd_close_file,
	os_hcd_ioctl_file,
	os_hcd_read_file,
	os_hcd_write_file,
};

status_t os_usb_declare_controller(struct os_hcd_driver *uhcd,
				   size_t objsize)
{
	unsigned long flags = OS_DEVICE_REMOVE;

	if (uhcd->caps & HCD_CAPS_CAN_SUSPEND)
		flags |= OS_DEVICE_SUSPEND;
	if (uhcd->caps & HCD_CAPS_CAN_WAKEUP)
		flags |= OS_DEVICE_WAKEUP;

	os_hcd_driver = uhcd;
	return os_dev_register_driver(&wdm_hcd_driver,
				      objsize+sizeof(struct os_usb_hci),
				      flags);
}

NTSTATUS wdm_hcd_dispatch_ioctl(PDEVICE_OBJECT dev, PIRP irp)
{
	struct wdm_device *priv = wdm_dev_priv(dev);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	PFILE_OBJECT filp = stack->FileObject;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG code;
	PVOID buffer;
	ULONG length;

	/* Only port PDOs will handle IRP_MJ_INTERNAL_DEVICE_CONTROL. */
	if (priv->is_fdo) {
		return wdm_dev_io_cmpl(dev, irp,
				       STATUS_INVALID_DEVICE_REQUEST, 0);
	}

	code = stack->Parameters.DeviceIoControl.IoControlCode;
	buffer = irp->AssociatedIrp.SystemBuffer;

	os_dbg(OS_DBG_DEBUG, "IRP_MJ_INTERNAL_DEVICE_CONTROL - %s(%d)\n",
	       wdm_usb_ioctl_string(code), code);

	switch (code) {
	case IOCTL_INTERNAL_USB_SUBMIT_URB:
		break;
	case IOCTL_INTERNAL_USB_GET_PORT_STATUS:
		break;
	case IOCTL_INTERNAL_USB_RESET_PORT:
		break;
	case IOCTL_INTERNAL_USB_CYCLE_PORT:
		break;
	default:
		break;
	}

	return wdm_dev_io_cmpl(dev, irp, status, 0);
}

static BOOLEAN USB_BUSIFFN IsDeviceHighSpeed(PVOID ctx)
{
	PDEVICE_OBJECT dev = (PDEVICE_OBJECT)ctx;
	os_dbg(OS_DBG_DEBUG, "IsDeviceHighSpeed\n");
	return FALSE;
}

static NTSTATUS USB_BUSIFFN QueryBusInformation(PVOID ctx,
						ULONG Level,
						PVOID BusInformationBuffer,
						PULONG BusInformationBufferLength,
						PULONG BusInformationActualLength)
{
	os_dbg(OS_DBG_DEBUG, "QueryBusInformation\n");
	return STATUS_UNSUCCESSFUL;
}

static NTSTATUS USB_BUSIFFN SubmitIsoOutUrb(PVOID ctx,
					    PURB urb)
{
	os_dbg(OS_DBG_DEBUG, "SubmitIsoOutUrb\n");
	return STATUS_UNSUCCESSFUL;
}

static NTSTATUS USB_BUSIFFN QueryBusTime(PVOID ctx,
					 PULONG currentusbframe)
{
	os_dbg(OS_DBG_DEBUG, "QueryBusTime\n");
	return STATUS_UNSUCCESSFUL;
}

static VOID USB_BUSIFFN GetUSBDIVersion(PVOID ctx,
					PUSBD_VERSION_INFORMATION inf,
					PULONG HcdCapabilities)
{
	os_dbg(OS_DBG_DEBUG, "GetUSBDIVersion\n");
	*HcdCapabilities = 0;
	inf->USBDI_Version = 0x500; /* Windows XP */
	inf->Supported_USB_Version = 0x200; /* USB 2.0 */
}

static VOID InterfaceReference(PVOID ctx)
{
	PDEVICE_OBJECT dev = (PDEVICE_OBJECT)ctx;
	os_dev_inc_user(dev, "BUS_INTERFACE");
}

static VOID InterfaceDereference(PVOID ctx)
{
	PDEVICE_OBJECT dev = (PDEVICE_OBJECT)ctx;
	os_dev_dec_user(dev, "BUS_INTERFACE");
}

NTSTATUS wdm_hcd_query_interface(PDEVICE_OBJECT dev, USHORT size,
				 USHORT version, PINTERFACE *iface)
{
	USB_BUS_INTERFACE_USBDI_V1 *bi = (USB_BUS_INTERFACE_USBDI_V1 *)iface;
	struct wdm_device *priv = wdm_dev_priv(dev);

	switch (version) {
	default:
		goto inval;
	case USB_BUSIF_USBDI_VERSION_1:
		if (size < sizeof (USB_BUS_INTERFACE_USBDI_V1))
			goto inval;
		bi->IsDeviceHighSpeed    = IsDeviceHighSpeed;
	case USB_BUSIF_USBDI_VERSION_0:
		if (size < sizeof (USB_BUS_INTERFACE_USBDI_V0))
			goto inval;
		bi->QueryBusInformation  = QueryBusInformation;
		bi->SubmitIsoOutUrb      = SubmitIsoOutUrb;
		bi->QueryBusTime         = QueryBusTime;
		bi->GetUSBDIVersion      = GetUSBDIVersion;
		bi->InterfaceReference   = InterfaceReference;
		bi->InterfaceDereference = InterfaceDereference;
		bi->BusContext           = dev;
		break;
	}
	/* TODO: what's this? */
	InterfaceReference(dev);
	return STATUS_SUCCESS;
inval:
	os_dbg(OS_DBG_ERR, "BUS_QUERY_INTERFACE: unsupported version %d\n", version);
	return STATUS_INVALID_PARAMETER;
}
