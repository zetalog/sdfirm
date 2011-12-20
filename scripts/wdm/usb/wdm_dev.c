#include <wdm/os_usb.h>

#define IDLE_INTERVAL 5000

IO_COMPLETION_ROUTINE __wdm_usb_iocmpl_idle;
IO_COMPLETION_ROUTINE __wdm_usb_iocmpl_urb;
USB_IDLE_NOTIFY_CALLBACK __wdm_usb_notify_idle;

struct os_usb_driver *os_usb_driver;

void wdm_usb_init_idle(os_device dev);
void wdm_usb_exit_idle(os_device dev);
void wdm_usb_start_idle(os_device dev);
void wdm_usb_stop_idle(os_device dev);

void wdm_usb_stop_pipes(os_device dev)
{
	int i;
	os_usbif *usb = dev_priv(dev);
	if (usb->usb_endpoints) {
		for (i = 0; i < usb->nr_usb_endpoints; i++) {
			wdm_usb_stop_pipe(dev, i+1);
		}
		os_mem_free(usb->usb_endpoints);
		usb->usb_endpoints = NULL;
	}
}

void wdm_usb_state_deconf(os_device dev)
{
	os_usbif *usb = dev_priv(dev);

	if (usb->usb_device_desc) {
		os_mem_free(usb->usb_device_desc);
		usb->usb_device_desc = NULL;
	}
	if (usb->usb_config_desc) {
		os_mem_free(usb->usb_config_desc);
		usb->usb_config_desc = NULL;
	}
	if (usb->usb_interface) {
		os_mem_free(usb->usb_interface);
		usb->usb_interface = NULL;
	}
	wdm_usb_stop_pipes(dev);
}

status_t wdm_usb_start_pipes(os_device dev)
{
	int i, nr_pipes;
	status_t status = STATUS_SUCCESS;
	os_usbif *usb = dev_priv(dev);
	struct os_usb_pipe *usb_endpoints = NULL;

	ASSERT(usb->usb_interface);

	wdm_usb_stop_pipes(dev);

	nr_pipes = usb->usb_interface->NumberOfPipes;
	if (nr_pipes) {
		usb_endpoints = os_mem_alloc(nr_pipes *
					     sizeof(struct os_usb_pipe),
					     "USBD_ENDPOINTS");
		if (!usb_endpoints) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			goto end;
		}
		for (i = 0; i < nr_pipes; i++) {
			wdm_usb_start_pipe(dev, i+1,
					   usb->usb_interface->Pipes[i].EndpointAddress,
					   &usb->usb_interface->Pipes[i]);
			os_dbg(OS_DBG_DEBUG, "---------\n");
			os_dbg(OS_DBG_DEBUG, "PipeType 0x%x\n",
			       usb->usb_interface->Pipes[i].PipeType);
			os_dbg(OS_DBG_DEBUG, "EndpointAddress 0x%x\n",
			       usb->usb_interface->Pipes[i].EndpointAddress);
			os_dbg(OS_DBG_DEBUG, "MaxPacketSize 0x%x\n",
			       usb->usb_interface->Pipes[i].MaximumPacketSize);
			os_dbg(OS_DBG_DEBUG, "Interval 0x%x\n",
			       usb->usb_interface->Pipes[i].Interval);
			os_dbg(OS_DBG_DEBUG, "MaximumTransferSize 0x%x\n",
			       usb->usb_interface->Pipes[i].MaximumTransferSize);
		}
	}

	usb->usb_endpoints = usb_endpoints;
	usb->nr_usb_endpoints = nr_pipes;
	usb_endpoints = NULL;
end:
	if (usb_endpoints) os_mem_free(usb_endpoints);
	return status;
}

void os_usb_exit_urb(struct os_usb_urb *urb)
{
	if (urb->urb) {
		if (urb->hint) {
			os_mem_free(urb->urb);
			urb->hint = NULL;
		} else {
			/* URB created by APIs shouldn't be maintained by
			 * memory tracking system if driver debugging is
			 * turned on.  Such API samples:
			 * 1. USBD_CreateConfigurationRequestEx
			 */
			ExFreePool(urb->urb);
		}
		urb->urb = NULL;
	}
}

void os_usb_free_sync(struct os_usb_urb *urb)
{
	if (urb) {
		os_usb_exit_urb(urb);
		os_mem_free(urb);
	}
}

struct os_usb_urb *os_usb_alloc_sync(int action)
{
	struct os_usb_urb *urb = NULL;

	urb = os_mem_alloc(sizeof (struct os_usb_urb), "USB_SYNC_URB");
	if (!urb)
		return NULL;
	urb->action = action;
	urb->length = 0;
	urb->status = USBD_STATUS_SUCCESS;
	return urb;
}

status_t os_usb_init_urb(struct os_usb_urb *urb, uint32_t size,
			 uint32_t length, const char *what)
{
	BUG_ON(!urb || urb->urb);
	urb->urb = os_mem_alloc(size, what);
	if (!urb->urb) {
		urb->status = USBD_STATUS_INSUFFICIENT_RESOURCES;
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	urb->hint = what;
	urb->size = size;
	urb->status = USBD_STATUS_SUCCESS;
	urb->length = length;
	return STATUS_SUCCESS;
}

status_t wdm_usb_init_set_config(struct os_usb_urb *urb,
				 struct usb_conf_desc *conf_desc,
				 PUSBD_INTERFACE_LIST_ENTRY intf_list,
				 uint16_t length,
				 const char *what)
{
	os_usb_exit_urb(urb);
	urb->urb = USBD_CreateConfigurationRequestEx((PVOID)conf_desc, intf_list);
	if (!urb->urb) {
		os_mem_free(urb);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	urb->size = urb->urb->UrbHeader.Length;
	urb->length = length;
	return STATUS_SUCCESS;
}

status_t wdm_usb_save_interface(os_device dev, PUSBD_INTERFACE_INFORMATION intf_info)
{
	PUSBD_INTERFACE_INFORMATION usb_interface = NULL;
	os_usbif *usb = dev_priv(dev);

	usb_interface = os_mem_alloc(intf_info->Length, "USBD_INTERFACE");
	if (!usb_interface) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	os_mem_move(usb_interface, intf_info, intf_info->Length);

	if (usb->usb_interface) {
		os_mem_free(usb->usb_interface);
		usb->usb_interface = NULL;
	}
	usb->usb_interface = usb_interface;
	usb_interface = NULL;

	/* Dump interfaces */
	os_dbg(OS_DBG_DEBUG, "---------\n");
	os_dbg(OS_DBG_DEBUG, "NumberOfPipes 0x%x\n",
	       usb->usb_interface->NumberOfPipes);
	os_dbg(OS_DBG_DEBUG, "Length 0x%x\n",
	       usb->usb_interface->Length);
	os_dbg(OS_DBG_DEBUG, "AlternateSetting 0x%x\n",
	       usb->usb_interface->AlternateSetting);
	os_dbg(OS_DBG_DEBUG, "InterfaceNumber 0x%x\n",
	       usb->usb_interface->InterfaceNumber);
	os_dbg(OS_DBG_DEBUG, "Class 0x%x\n",
	       usb->usb_interface->Class);
	os_dbg(OS_DBG_DEBUG, "SubClass 0x%x\n",
	       usb->usb_interface->SubClass);
	os_dbg(OS_DBG_DEBUG, "Protocol 0x%x\n",
	       usb->usb_interface->Protocol);

	return wdm_usb_start_pipes(dev);
}

status_t wdm_usb_match_configuration(os_device dev,
				     PUSBD_INTERFACE_LIST_ENTRY conf)
{
	status_t status = STATUS_NO_SUCH_DEVICE;
	os_usbif *usb = dev_priv(dev);
	PUSBD_INTERFACE_LIST_ENTRY tmp;
	PUSBD_INTERFACE_INFORMATION usb_interface = NULL;

	ASSERT(usb->usb_config_desc);

	tmp = conf;
	while (tmp) {
		if (tmp->InterfaceDescriptor->bInterfaceClass ==
		    usb->usb_driver->devcls) {
			status = wdm_usb_save_interface(dev, tmp->Interface);
			break;
		}
		tmp++;
	}
	return status;
}

status_t wdm_usb_set_configuration(os_device dev, struct os_usb_urb *urb,
				   uint8_t conf_val, uint16_t length)
{
	os_usbif *usb = dev_priv(dev);
	uint8_t nr_intfs, intf_num, i, j;
	status_t status = STATUS_NO_SUCH_DEVICE;
	struct usb_intf_desc *intf_desc = NULL;
	PUSBD_INTERFACE_LIST_ENTRY intf_list = NULL, tmp;
	PUSBD_INTERFACE_INFORMATION intf = NULL, intf_focus = NULL;
	PUCHAR start_pos;
	const char *what = "CONTROL(SET_CONFIG)";

	ASSERT(usb->usb_config_desc);
	ASSERT(usb->usb_config_desc->bConfigurationValue == conf_val);

	nr_intfs = usb->usb_config_desc->bNumInterfaces;
	intf_list = os_mem_alloc(sizeof (USBD_INTERFACE_LIST_ENTRY)*(nr_intfs+1),
				 "USBD_INTERFACE_LIST");
	if (!intf_list)
		return STATUS_INSUFFICIENT_RESOURCES;

	tmp = intf_list;
	intf_num = 0;
	start_pos = (PUCHAR)usb->usb_config_desc;
	for (intf_num = 0; intf_num < nr_intfs; intf_num++) {
		intf_desc = (struct usb_intf_desc *)USBD_ParseConfigurationDescriptorEx(
			(PVOID)usb->usb_config_desc, start_pos, -1, 0, -1, -1, -1);
		if (!intf_desc) goto end;
		start_pos = (PUCHAR)intf_desc + intf_desc->bLength;
		tmp->InterfaceDescriptor = (PUSB_INTERFACE_DESCRIPTOR)intf_desc;
		tmp->Interface = NULL;
		tmp++;
	}
	tmp->InterfaceDescriptor = NULL;
	tmp->Interface = NULL;

	status = wdm_usb_init_set_config(urb, usb->usb_config_desc, intf_list,
					 length, what);
	if (!OS_IS_SUCCESS(status)) {
		os_dbg(OS_DBG_ERR, "USBD_CreateConfigurationRequestEx - failure\n");
		goto end;
	}
	for (j = 0; j < nr_intfs; j++) {
		intf = intf_list[j].Interface;
		for (i = 0; i < intf->NumberOfPipes; i++) {
			intf->Pipes[i].MaximumTransferSize = USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;
		}
	}
	status = os_usb_submit_urb_sync(dev, 0, urb);
	if (NT_SUCCESS(status)) {
		usb->config_handle = urb->urb->UrbSelectConfiguration.ConfigurationHandle;
		status = wdm_usb_match_configuration(dev, intf_list);
	}
end:
	if (intf_list) os_mem_free(intf_list);
	return status;
}

status_t wdm_usb_set_interface(os_device dev, struct os_usb_urb *urb,
			       uint8_t intf_num, uint8_t alt_num,
			       uint16_t length)
{
	NTSTATUS status = STATUS_SUCCESS;
	os_usbif *usb = dev_priv(dev);
	int i;
	uint16_t size, nr_pipes;
	PUSBD_INTERFACE_INFORMATION intf_info = NULL;
	const char *what = "CONTROL(SET_INTERFACE)";

	ASSERT(usb->usb_config_desc);
	ASSERT(usb->usb_interface_desc);
	ASSERT(usb->usb_interface_desc->bInterfaceNumber == intf_num);
	ASSERT(usb->usb_interface_desc->bAlternateSetting == alt_num);

	nr_pipes = usb->usb_interface_desc->bNumEndpoints;

	size = GET_SELECT_INTERFACE_REQUEST_SIZE(nr_pipes);
	status = os_usb_init_urb(urb, size, length, what);
	if (!OS_IS_SUCCESS(status)) {
		return status;
	}
	UsbBuildSelectInterfaceRequest(urb->urb, size,
				       usb->config_handle, intf_num, alt_num);
	intf_info = &urb->urb->UrbSelectInterface.Interface;
	for (i = 0; i < usb->usb_interface_desc->bNumEndpoints; i++) {
		intf_info->Pipes[i].MaximumTransferSize = USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;
	}
	status = os_usb_submit_urb_sync(dev, 0, urb);
	if (NT_SUCCESS(status))
		status = wdm_usb_save_interface(dev, intf_info);
	return status;
}

status_t __os_usb_submit_default(os_device dev,
				 struct os_usb_urb *urb,
				 BOOLEAN wait,
				 uint8_t request_type,
				 uint8_t request,
				 uint16_t value,
				 uint16_t index,
				 uint16_t length,
				 uint8_t *additional_info)
{
	uint16_t function;
	uint16_t size;
	status_t status = STATUS_NOT_SUPPORTED;
	const char *what = NULL;
	
	if (USB_SETUP2TYPE(request_type) == USB_TYPE_STANDARD) {
		switch (request) {
		case USB_REQ_GET_DESCRIPTOR:
			what = "CONTROL(GET_DESCRIPTOR)";
			size = sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST);
			status = os_usb_init_urb(urb, size, length, what);
			if (!OS_IS_SUCCESS(status)) {
				goto failure;
			}
			UsbBuildGetDescriptorRequest(urb->urb,
						     size,
						     HIBYTE(value), /* device type */
						     LOBYTE(value), /* device index */
						     index, /* language ID */
						     additional_info,
						     NULL, /* MDL */
						     length,
						     NULL);
			switch (USB_SETUP2RECP(request_type)) {
			case USB_RECP_DEVICE:
				urb->urb->UrbHeader.Function = URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE;
				break;
			case USB_RECP_INTERFACE:
				urb->urb->UrbHeader.Function = URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE;
				break;
			case USB_RECP_ENDPOINT:
				urb->urb->UrbHeader.Function = URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT;
				break;
			}
			goto submit;
			break;
		case USB_REQ_GET_CONFIGURATION:
			what = "CONTROL(GET_CONFIGURATION)";
			size = sizeof(struct _URB_CONTROL_GET_CONFIGURATION_REQUEST);
			status = os_usb_init_urb(urb, size, length, what);
			if (!OS_IS_SUCCESS(status)) {
				goto failure;
			}
			urb->urb->UrbHeader.Function = URB_FUNCTION_GET_CONFIGURATION;
			urb->urb->UrbHeader.Length = size;
			urb->urb->UrbControlGetConfigurationRequest.TransferBufferLength = 1;
			urb->urb->UrbControlGetConfigurationRequest.TransferBuffer = additional_info;
			goto submit;
			break;
		case USB_REQ_SET_CONFIGURATION:
			if (!value) {
				wdm_usb_state_deconf(dev);
				what = "CONTROL(SET_CONFIGURATION)";
				size = sizeof(struct _URB_SELECT_CONFIGURATION);
				status = os_usb_init_urb(urb, size, length, what);
				if (!OS_IS_SUCCESS(status)) {
					goto failure;
				}
				UsbBuildSelectConfigurationRequest(urb->urb, size, NULL);
				goto submit;
			} else {
				/* Critical USB operation for WDM */
				ASSERT(wait);
				return wdm_usb_set_configuration(dev, urb, LOBYTE(value), length);
			}
			break;
		case USB_REQ_SET_INTERFACE:
			/* Critical USB operation for WDM */
			ASSERT(wait);
			return wdm_usb_set_interface(dev, urb, LOBYTE(index), LOBYTE(value), length);
		case USB_REQ_CLEAR_FEATURE:
			if (USB_SETUP2RECP(request_type) == USB_RECP_ENDPOINT &&
			    value == USB_ENDPOINT_HALT) {
				uint8_t eid = LOBYTE(index);
				struct os_usb_pipe *pipe;
				
				if (eid == USB_EID_DEFAULT) {
					/* No default endpoint unhalting
					 * is required on Windows side.
					 * Still submitting URB since the
					 * highers call this function for
					 * vendor specific purpose.
					 */
					goto feature_setup;
				}
				/* Critical USB operation for WDM */
				pipe = os_usb_pipe_by_eid(dev, eid);
				if (!pipe) {
					status = STATUS_NO_SUCH_DEVICE;
					goto failure;
				}
				what = "CONTROL(CLEAR_FEATURE)";
				size = sizeof(struct _URB_PIPE_REQUEST);
				status = os_usb_init_urb(urb, size, length, what);
				if (!OS_IS_SUCCESS(status)) {
					goto failure;
				}
				urb->urb->UrbHeader.Length = size;
				urb->urb->UrbHeader.Function = URB_FUNCTION_RESET_PIPE;
				urb->urb->UrbPipeRequest.PipeHandle = pipe->pipe_info ?
					pipe->pipe_info->PipeHandle : NULL;
				goto submit;
			}
		case USB_REQ_SET_FEATURE:
feature_setup:
			what = (request == USB_REQ_CLEAR_FEATURE) ?
			       "CONTROL(CLEAR_FEATURE)" :
			       "CONTROL(SET_FEATURE)";
			size = sizeof(struct _URB_CONTROL_FEATURE_REQUEST);
			status = os_usb_init_urb(urb, size, length, what);
			if (!OS_IS_SUCCESS(status)) {
				goto failure;
			}
			switch (USB_SETUP2RECP(request_type)) {
			case USB_RECP_DEVICE:
				urb->urb->UrbHeader.Function = (request == USB_REQ_CLEAR_FEATURE) ?
							       URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE :
							       URB_FUNCTION_SET_FEATURE_TO_DEVICE;
				break;
			case USB_RECP_INTERFACE:
				urb->urb->UrbHeader.Function = (request == USB_REQ_CLEAR_FEATURE) ?
							       URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE :
							       URB_FUNCTION_SET_FEATURE_TO_INTERFACE;
				break;
			case USB_RECP_ENDPOINT:
				urb->urb->UrbHeader.Function = (request == USB_REQ_CLEAR_FEATURE) ?
							       URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT :
							       URB_FUNCTION_SET_FEATURE_TO_ENDPOINT;
				break;
			default:
				urb->urb->UrbHeader.Function = (request == USB_REQ_CLEAR_FEATURE) ?
							       URB_FUNCTION_CLEAR_FEATURE_TO_OTHER :
							       URB_FUNCTION_SET_FEATURE_TO_OTHER;
				break;
			}
			urb->urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_FEATURE_REQUEST);
			urb->urb->UrbControlFeatureRequest.FeatureSelector = (USHORT)value;
			urb->urb->UrbControlFeatureRequest.Index = (USHORT)index; 
			goto submit;
			break;
		case USB_REQ_GET_INTERFACE:
		case USB_REQ_GET_STATUS:
		case USB_REQ_SYNCH_FRAME: /* No-isochronous support here */
			break;
		case USB_REQ_SET_DESCRIPTOR:
		case USB_REQ_SET_ADDRESS: /* Supported by USBD */
			ASSERT(FALSE);
			break;
		}
	} else if (USB_SETUP2TYPE(request_type) != USB_TYPE_RESERVED) {
		/* Vendor or Class request */
		switch (USB_SETUP2RECP(request_type)) {
		case USB_RECP_DEVICE:
			function = USB_SETUP2TYPE(request_type) == USB_TYPE_CLASS ?
				   URB_FUNCTION_CLASS_DEVICE : URB_FUNCTION_VENDOR_DEVICE;
			break;
		case USB_RECP_INTERFACE:
			function = USB_SETUP2TYPE(request_type) == USB_TYPE_CLASS ?
				   URB_FUNCTION_CLASS_INTERFACE : URB_FUNCTION_VENDOR_INTERFACE;
			break;
		case USB_RECP_ENDPOINT:
			function = USB_SETUP2TYPE(request_type) == USB_TYPE_CLASS ?
				   URB_FUNCTION_CLASS_ENDPOINT : URB_FUNCTION_VENDOR_ENDPOINT;
			break;
		case USB_RECP_OTHER:
			function = USB_SETUP2TYPE(request_type) == USB_TYPE_CLASS ?
				   URB_FUNCTION_CLASS_OTHER : URB_FUNCTION_VENDOR_OTHER;
			break;
		}
		what = "CONTROL(CLASS)";
		size = sizeof (struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
		status = os_usb_init_urb(urb, size, length, what);
		if (!OS_IS_SUCCESS(status)) {
			goto failure;
		}
		UsbBuildVendorRequest(urb->urb, function, size,
				      USB_SETUP2DIR(request_type) == USB_DIR_IN ?
				      USBD_TRANSFER_DIRECTION_IN :
				      USBD_TRANSFER_DIRECTION_OUT,
				      0, /* ReservedBits */
				      request, value, index,
				      additional_info,
				      NULL,  /* MDL */
				      length,
				      NULL);
		goto submit;
	}
submit:
	if (wait) {
		status = os_usb_submit_urb_sync(dev, USB_EID_DEFAULT, urb);
	} else {
		os_usb_submit_urb_async(dev, USB_EID_DEFAULT, urb);
		status = STATUS_SUCCESS;
	}
failure:
	return status;
}

status_t os_usb_submit_default_sync(os_device dev,
				    uint8_t request_type,
				    uint8_t request,
				    uint16_t value,
				    uint16_t index,
				    uint16_t length,
				    uint8_t *additional_info)
{
	struct os_usb_pipe *pipe;
	status_t status = STATUS_SUCCESS;
	struct os_usb_urb *urb;
	
	pipe = os_usb_pipe_by_eid(dev, USB_EID_DEFAULT);
	if (!pipe)
		return STATUS_INVALID_PARAMETER;
	urb = os_usb_alloc_sync(OS_USB_SUBMIT_URB);
	if (!urb)
		return STATUS_INSUFFICIENT_RESOURCES;
	status = __os_usb_submit_default(dev, urb, TRUE, request_type, request,
					 value, index, length, additional_info);
	os_usb_free_sync(urb);
	return status;
}

status_t os_usb_submit_default_aysnc(os_device dev,
				     struct os_usb_urb *urb,
				     uint8_t request_type,
				     uint8_t request,
				     uint16_t value,
				     uint16_t index,
				     uint16_t length,
				     uint8_t *additional_info)
{
	BUG_ON(!urb);
	return __os_usb_submit_default(dev, urb, FALSE, request_type, request,
				       value, index, length, additional_info);
}

status_t __os_usb_submit_dummy(os_device dev, uint8_t eid,
			       struct os_usb_urb *urb)
{
	PIRP irp = NULL;
	os_waiter waiter;
	status_t status = 0;
	IO_STATUS_BLOCK iosb;
	PIO_STACK_LOCATION stack;
	os_usbif *usb = dev_priv(dev);

	/* FIXME: Following code could not work!! */
	ASSERT(METHOD_FROM_CTL_CODE(IOCTL_INTERNAL_USB_SUBMIT_URB) == METHOD_NEITHER);

	irp = IoAllocateIrp(wdm_io_stack_count(dev), FALSE);
	if (!irp) {
		os_dbg(OS_DBG_ERR, "IoAllocateIrp - failure\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	irp->UserIosb = &iosb;
	irp->UserBuffer = NULL;

	stack = IoGetNextIrpStackLocation(irp);
	ASSERT(stack != NULL);

	stack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
	stack->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
	stack->Parameters.DeviceIoControl.InputBufferLength = 0;
	stack->Parameters.DeviceIoControl.OutputBufferLength = 0;
	stack->Parameters.DeviceIoControl.Type3InputBuffer = NULL;
	stack->Parameters.Others.Argument1 = urb;
	return wdm_dev_io_wait(dev, irp);
}

/* This function should be called in the work_aval callback of USB pipe.
 * Thus, URB submissions are serialized.
 */
void os_usb_submit_urb_async(os_device dev, uint8_t eid,
			     struct os_usb_urb *urb)
{
	struct os_usb_pipe *pipe = os_usb_pipe_by_eid(dev, eid);
	PIO_STACK_LOCATION stack;

	/* IRP has been allocated by wdm_alloc_irp. */
	BUG_ON(!pipe->urb_worker.irp);
	/* Pipe is enabled and idle. */
	BUG_ON(pipe->flags & USB_PIPE_MASK);

	stack = IoGetNextIrpStackLocation(pipe->urb_worker.irp);
	BUG_ON(!stack);
	stack->Parameters.Others.Argument1 = urb->urb;
}

status_t os_usb_submit_urb_sync(os_device dev, uint8_t eid,
				struct os_usb_urb *urb)
{
	PIRP irp = NULL;
	status_t status = 0;
	PIO_STACK_LOCATION stack;
	os_usbif *usb = dev_priv(dev);

	ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
	
	os_dbg(OS_DBG_DEBUG, "IOCTL_INTERNAL_USB_SUBMIT_URB - begins\n");

	os_waiter_init(&urb->waiter, FALSE);
	irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_SUBMIT_URB, 
					    wdm_dev_stacked(dev),
					    NULL, 0,  NULL, 0, TRUE,
					    &urb->waiter, &urb->iosb);
	if (!irp) {
		os_dbg(OS_DBG_ERR,
		       "IoBuildDeviceIoControlRequest - failure(%X)\n",
		       urb->iosb.Status);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	stack = IoGetNextIrpStackLocation(irp);
	ASSERT(stack != NULL);
	stack->Parameters.Others.Argument1 = urb->urb;
	
	status = IoCallDriver(wdm_dev_stacked(dev), irp);
	if (!NT_SUCCESS(status)) {
		os_dbg(OS_DBG_ERR,
		       "IoCallDriver(IOCTL_INTERNAL_USB_SUBMIT_URB) - failure(%X)\n",
		       status);
	}
	if (status == STATUS_PENDING) {
		os_waiter_wait(&urb->waiter, OS_WAIT_INFINITE);
		status = urb->iosb.Status;
		os_dbg(OS_DBG_INFO,
		       "IOCTL_INTERNAL_USB_SUBMIT_URB - status=%x\n",
		       urb->iosb.Status);
	}
	urb->status = urb->urb->UrbHeader.Status;
	urb->length = urb->urb->UrbControlTransfer.TransferBufferLength;
	os_dbg(OS_DBG_DEBUG, "IOCTL_INTERNAL_USB_SUBMIT_URB - end(%X)\n",
	       urb->status);
	
	return status;
}

status_t os_usb_submit_async(os_device dev, uint8_t eid,
			     struct os_usb_urb *urb)
{
	struct os_usb_pipe *pipe = os_usb_pipe_by_eid(dev, eid);
	if (!pipe) return STATUS_INVALID_PARAMETER;
	pipe->flags &= ~USB_PIPE_MASK;
	BUG_ON(urb->work->work != &pipe->urb_worker);
	return os_worker_queue_item(&pipe->urb_worker, urb->work);
}

struct os_usb_urb *os_usb_alloc_async(os_device dev, uint8_t eid,
				      int action, int extrasize)
{
	struct os_usb_pipe *pipe;
	struct os_work_item *item;
	struct os_usb_urb *urb = NULL;
	status_t status = STATUS_SUCCESS;

	pipe = os_usb_pipe_by_eid(dev, eid);
	if (!pipe || (pipe->flags & USB_PIPE_MASK))
		return NULL;
	item = os_worker_alloc_item(&pipe->urb_worker,
				    sizeof(struct os_usb_urb)+extrasize,
				    "USB_ASYNC_URB");
	if (!item) return NULL;

	urb = work_priv(item);
	urb->work = item;
	urb->action = action;
	urb->length = 0;
	urb->status = USBD_STATUS_SUCCESS;
	return urb;
}

void os_usb_suspend_async(os_device dev, uint8_t eid)
{
	struct os_usb_pipe *pipe = os_usb_pipe_by_eid(dev, eid);
	if (pipe) {
		pipe->flags |= USB_PIPE_MASK;
		os_worker_suspend_queue(&pipe->urb_worker);
	}
}

void os_usb_resume_async(os_device dev, uint8_t eid)
{
	struct os_usb_pipe *pipe = os_usb_pipe_by_eid(dev, eid);
	if (pipe) {
		os_worker_resume_queue(&pipe->urb_worker);
		pipe->flags &= ~USB_PIPE_MASK;
	}
}

status_t os_usb_abort_pipe(os_device dev, uint8_t eid)
{
	NTSTATUS status = STATUS_SUCCESS;
	os_usbif *usb = dev_priv(dev);
	struct os_usb_urb *urb = NULL;
	uint16_t size;
	struct os_usb_pipe *pipe = NULL;

	if (eid == USB_EID_DEFAULT)
		return STATUS_SUCCESS;
	pipe = os_usb_pipe_by_eid(dev, eid);
	if (!pipe || !pipe->pipe_info)
		return STATUS_NO_SUCH_DEVICE;
	urb = os_usb_alloc_sync(OS_USB_ABORT_PIPE);
	if (!urb)
		return STATUS_INSUFFICIENT_RESOURCES;
	size = sizeof(struct _URB_PIPE_REQUEST);
	status = os_usb_init_urb(urb, size, 0, "CONTROL(ABORT_PIPE)");
	if (!OS_IS_SUCCESS(status)) return status;
	urb->urb->UrbHeader.Length = size;
	urb->urb->UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
	urb->urb->UrbPipeRequest.PipeHandle =  pipe->pipe_info->PipeHandle;
	status = os_usb_submit_urb_sync(dev, 0, urb);
	os_usb_free_sync(urb);
	return status;
}

status_t os_usb_abort_pipes(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	struct os_usb_pipe *pipe;
	int i;
	
	os_dbg(OS_DBG_DEBUG, "URB_FUNCTION_ABORT_PIPE - begins\n");
	
	for (i = 0; i < usb->nr_usb_endpoints; i++) {
		os_usb_abort_pipe(dev, usb->usb_endpoints[i].eid);
	}
	os_dbg(OS_DBG_DEBUG, "URB_FUNCTION_ABORT_PIPE - ends\n");
	return STATUS_SUCCESS;
}

status_t wdm_usbd_ioctl_wait(os_device dev, ULONG code, PVOID arg1)
{
	status_t status;
	os_waiter waiter;
	PIRP irp;
	IO_STATUS_BLOCK iosb;
	PIO_STACK_LOCATION stack;
	os_usbif *usb = dev_priv(dev);
	
	os_waiter_init(&waiter, FALSE);
	irp = IoBuildDeviceIoControlRequest(code,
					    wdm_dev_stacked(dev),
					    NULL, 0, NULL, 0,
					    TRUE, &waiter, &iosb);
	if (!irp) {
		os_dbg(OS_DBG_ERR, "IoBuildDeviceIoControlRequest - failure\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	stack = IoGetNextIrpStackLocation(irp);
	ASSERT(stack != NULL);
	stack->Parameters.Others.Argument1 = arg1;
	status = IoCallDriver(wdm_dev_stacked(dev), irp);
	if (STATUS_PENDING == status) {
		os_waiter_wait(&waiter, OS_WAIT_INFINITE);
	} else {
		iosb.Status = status;
	}
	
	status = iosb.Status;
	if (!NT_SUCCESS(status)) {
		os_dbg(OS_DBG_ERR, "IoCallDriver - failure(%X)\n", status);
	}
	return status;
}

status_t wdm_usb_get_port_status(os_device dev, PULONG port_status)
{
	status_t status;
	*port_status = 0;
	status = wdm_usbd_ioctl_wait(dev, IOCTL_INTERNAL_USB_GET_PORT_STATUS, port_status);
	if (NT_SUCCESS(status)) {
		os_dbg(OS_DBG_DEBUG, "Connected - %s\n",
		       wdm_dev_bool_string((*port_status) & USBD_PORT_CONNECTED));
		os_dbg(OS_DBG_DEBUG, "Enabled - %s\n",
		       wdm_dev_bool_string((*port_status) & USBD_PORT_ENABLED));
	}
	return status;
}

void os_usb_ctrl_device_async(os_device dev)
{
	struct os_usb_pipe *pipe = os_usb_pipe_by_eid(dev, USB_EID_DEFAULT);
	PIO_STACK_LOCATION stack;

	ASSERT(pipe->urb_worker.irp);
	ASSERT(!(pipe->flags & USB_PIPE_MASK));

	stack = IoGetNextIrpStackLocation(pipe->urb_worker.irp);
	ASSERT(stack != NULL);
	stack->Parameters.Others.Argument1 = NULL;
}

void os_usb_reset_device_async(os_device dev)
{
	os_usb_ctrl_device_async(dev);
}

void os_usb_cycle_device_async(os_device dev)
{
	os_usb_ctrl_device_async(dev);
}

status_t os_usb_reset_device(os_device dev)
{
	NTSTATUS status;
	ULONG port_status = 0;
	
	status = wdm_usb_get_port_status(dev, &port_status);
	if ((NT_SUCCESS(status)) &&
	    (port_status & (USBD_PORT_CONNECTED | USBD_PORT_ENABLED))) {
		status = wdm_usbd_ioctl_wait(dev, IOCTL_INTERNAL_USB_RESET_PORT, NULL);
	}
	return status;
}

status_t os_usb_cycle_device(os_device dev)
{
	NTSTATUS status;
	ULONG port_status = 0;
	
	status = wdm_usb_get_port_status(dev, &port_status);
	if ((NT_SUCCESS(status)) &&
	    (port_status & USBD_PORT_CONNECTED)) {
		status = wdm_usbd_ioctl_wait(dev, IOCTL_INTERNAL_USB_CYCLE_PORT, NULL);
	}
	return status;
}

/*=========================================================================
 * selective suspsned
 *=======================================================================*/
void wdm_usb_submit_idle(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	if (usb->idle_timer && usb->ref_users == 0) {
		os_timer_schedule(usb->idle_timer, IDLE_INTERVAL*1000);
	}
}

void wdm_usb_suspend_idle(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	if (usb->idle_enabled) {
		os_timer_discard(usb->idle_timer);
		os_worker_suspend_queue(&usb->idle_worker);
	}
}

void wdm_usb_resume_idle(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	if (usb->idle_enabled) {
		os_worker_resume_queue(&usb->idle_worker);
		wdm_usb_submit_idle(dev);
	}
}

void wdm_usb_user_idle(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	/* If there is not any user file is opened, we can allow the
	 * selective suspend to be activated.
	 */
	InterlockedDecrement((PLONG)&usb->ref_users);
	wdm_usb_resume_idle(dev);
}

void wdm_usb_user_unidle(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	InterlockedIncrement((PLONG)&usb->ref_users);
	wdm_usb_suspend_idle(dev);
	if (wdm_pnp_get_state(dev) != DEV_RUNNING)
		wdm_dev_resume(dev, FALSE, NULL, NULL);
}

status_t wdm_usb_schedule_idle(os_device dev)
{
	struct os_work_item *item;
	os_usbif *usb = dev_priv(dev);
	if (usb->idle_enabled) {
		item = os_worker_alloc_item(&usb->idle_worker,
					    0,
					    "USB_IDLE_INFO");
		if (!item)
			return STATUS_INSUFFICIENT_RESOURCES;
		os_worker_queue_item(&usb->idle_worker, item);
	}
	return STATUS_SUCCESS;
}

VOID __wdm_usb_notify_idle(os_device dev)
{
	if (wdm_pnp_get_state(dev) == DEV_RUNNING)
		wdm_dev_suspend(dev, TRUE, NULL, NULL);
}

status_t __wdm_usb_idle_aval(struct os_work_item *__i,
			     os_device dev)
{
	PIO_STACK_LOCATION stack;
	os_usbif *usb = dev_priv(dev);
	PIRP irp = NULL;
	status_t status = STATUS_SUCCESS;
	struct os_work_queue *__q = __i->work;

	ASSERT(!usb->idle_info);
	ASSERT(__q->irp);

	if (!os_dev_can_stop(dev) || !os_dev_is_powered(dev)) {
		status = STATUS_UNSUCCESSFUL;
		goto fail;
	}

	irp = __q->irp;
	usb->idle_info = os_mem_alloc(sizeof(struct _USB_IDLE_CALLBACK_INFO),
				      "USB_IDLE_CALLBACK_INFO");
	if (!usb->idle_info) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto fail;
	}
	usb->idle_info->IdleCallback = __wdm_usb_notify_idle;
	usb->idle_info->IdleContext = (PVOID)dev;

	stack = IoGetNextIrpStackLocation(irp);
	stack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
	stack->Parameters.DeviceIoControl.IoControlCode =
		IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION;
	stack->Parameters.DeviceIoControl.Type3InputBuffer = usb->idle_info;
	stack->Parameters.DeviceIoControl.InputBufferLength =
		sizeof(struct _USB_IDLE_CALLBACK_INFO);
fail:
	return status;
}

status_t __wdm_usb_idle_cmpl(struct os_work_item *__i,
			     os_device dev, status_t status)
{
	struct os_work_queue *__q = __i->work;
	os_usbif *usb = dev_priv(dev);

	if (!NT_SUCCESS(status) && status != STATUS_NOT_SUPPORTED) {
		if (status == STATUS_POWER_STATE_INVALID)
			goto end;
	}
	wdm_dev_resume(dev, FALSE, NULL, NULL);
end:
	if (usb->idle_info) {
		os_mem_free(usb->idle_info);
		usb->idle_info = NULL;
	}

	return STATUS_MORE_PROCESSING_REQUIRED;
}

void __wdm_usb_idle_timer(os_timer *timer, os_device dev, void *data)
{
	wdm_usb_schedule_idle(dev);
}

void wdm_usb_exit_idle(os_device dev)
{
	os_usbif *usb = dev_priv(dev);

	ASSERT(usb->idle_enabled == FALSE);
	ASSERT(usb->idle_info == NULL);
	ASSERT(usb->idle_timer == NULL);
	usb->idle_registered = 0;
}

void wdm_usb_init_idle(os_device dev)
{
	os_usbif *usb = dev_priv(dev);

	usb->idle_info = NULL;
	usb->idle_registered = FALSE;
	usb->idle_enabled = FALSE;
	usb->idle_timer = NULL;

	if (WinXpOrBetter == wdm_drv_get_version()) {
		if (usb->usb_driver->caps & USB_CAPS_CAN_SUSPEND)
			usb->idle_registered = TRUE;
	}
}

void wdm_usb_stop_idle(os_device dev)
{
	os_usbif *usb = dev_priv(dev);

	if (usb->idle_enabled) {
		usb->idle_enabled = FALSE;
		os_timer_unregister(usb->idle_timer);
		usb->idle_timer = NULL;
		os_worker_delete_queue(&usb->idle_worker);
		os_waiter_wait(&usb->idle_waiter, OS_WAIT_INFINITE);
	}
}

void __wdm_usb_idle_free_notifier(os_device dev, void *data, PIRP irp)
{
	os_usbif *usb = dev_priv(dev);

	if (irp)
		IoFreeIrp(irp);
	os_waiter_wake(&usb->idle_waiter);
}

PIRP __wdm_usb_idle_alloc_notifier(os_device dev, void *unused1,
				   struct os_work_item *unused2)
{
	PIRP irp;
	os_usbif *usb = dev_priv(dev);

	irp = IoAllocateIrp(wdm_io_stack_count(dev), FALSE);
	if (!irp) {
		os_dbg(OS_DBG_ERR, "IoAllocateIrp - failure\n");
		return NULL;
	}
	os_waiter_idle(&usb->idle_waiter);
	return irp;
}

void wdm_usb_start_idle(os_device dev)
{
	os_usbif *usb = dev_priv(dev);

	if (WinXpOrBetter == wdm_drv_get_version())
		usb->idle_enabled = usb->idle_registered;

	if (usb->idle_enabled) {
		os_waiter_init(&usb->idle_waiter, FALSE);
		usb->idle_timer = os_timer_register(dev, __wdm_usb_idle_timer, NULL);
		usb->idle_worker.wdm_alloc_irp = __wdm_usb_idle_alloc_notifier;
		usb->idle_worker.wdm_free_irp = __wdm_usb_idle_free_notifier;
		os_worker_create_queue(dev, &usb->idle_worker,
				       __wdm_usb_idle_aval,
				       __wdm_usb_idle_cmpl,
				       NULL);
	}
	wdm_usb_submit_idle(dev);
}

/*=========================================================================
 * usb context
 *=======================================================================*/
uint8_t os_usb_interface_num(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	return (uint8_t)usb->usb_interface_desc->bInterfaceNumber;
}

int os_usb_is_composite(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	return usb->is_composite;
}

void os_usb_deconfigure_device(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	if (usb->state == USB_STATE_CONFIGURED) {
		usb_set_configuration(dev, 0);
	}
	usb->state = USB_STATE_DEFAULT;
}

void wdm_usb_save_device_desc(os_usbif *usb, struct usb_device_desc *dev_desc)
{
	if (usb->usb_device_desc) {
		os_mem_free(usb->usb_device_desc);
		usb->usb_device_desc = NULL;
	}
	os_dbg(OS_DBG_DEBUG, "----------\n");
	os_dbg(OS_DBG_DEBUG, "Device Desc\n");
	os_dbg(OS_DBG_DEBUG, "bcdUSB = %04X\n",
	       dev_desc->bcdUSB);
	os_dbg(OS_DBG_DEBUG, "bDeviceClass = %02X\n",
	       dev_desc->bDeviceClass);
	os_dbg(OS_DBG_DEBUG, "bDeviceSubClass = %02X\n",
	       dev_desc->bDeviceSubClass);
	os_dbg(OS_DBG_DEBUG, "bDeviceProtocol = %02X\n",
	       dev_desc->bDeviceProtocol);
	os_dbg(OS_DBG_DEBUG, "idVendor = %04X\n",
	       dev_desc->idVendor);
	os_dbg(OS_DBG_DEBUG, "idProduct = %04X\n",
	       dev_desc->idProduct);
	os_dbg(OS_DBG_DEBUG, "bcdDevice = %04X\n",
	       dev_desc->bcdDevice);
	os_dbg(OS_DBG_DEBUG, "bNumConfigurations = %d\n",
	       dev_desc->bNumConfigurations);
	os_dbg(OS_DBG_DEBUG, "----------\n");
	usb->usb_device_desc = dev_desc;
}

void wdm_usb_save_conf_desc(os_usbif *usb, struct usb_conf_desc *conf_desc)
{
	if (usb->usb_config_desc) {
		os_mem_free(usb->usb_config_desc);
		usb->usb_config_desc = NULL;
	}
	os_dbg(OS_DBG_DEBUG, "----------\n");
	os_dbg(OS_DBG_DEBUG, "Config Desc\n");
	os_dbg(OS_DBG_DEBUG, "wTotalLength = %04x\n",
	       conf_desc->wTotalLength);
	os_dbg(OS_DBG_DEBUG, "bNumInterfaces = %d\n",
	       conf_desc->bNumInterfaces);
	os_dbg(OS_DBG_DEBUG, "bConfigurationValue = %d\n",
	       conf_desc->bConfigurationValue);
	os_dbg(OS_DBG_DEBUG, "----------\n");
	usb->usb_config_desc = conf_desc;
}

status_t os_usb_configure_device(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	status_t status = STATUS_NO_SUCH_DEVICE;
	uint8_t conf_val = 0, intf_num = 0, alt_num = 0;
	struct usb_device_desc *dev_desc = NULL;
	struct usb_conf_desc *conf_desc = NULL;
	uint16_t intf_desc_len = 0;

	/* Get device descriptor */
	status = usb_get_device_desc(dev, &dev_desc);
	if (!OS_IS_SUCCESS(status)) goto end;

	wdm_usb_save_device_desc(usb, dev_desc);
	dev_desc = NULL;

	/* Configure device */
	if (os_usb_is_composite(dev)) {
		status = usb_get_configuration(dev, &conf_val);
		if (!OS_IS_SUCCESS(status) || !conf_val) goto end;
		status = usb_get_config_desc(dev, &conf_desc, conf_val-1);
		if (!OS_IS_SUCCESS(status)) goto end;
		ASSERT(conf_desc);
		if (conf_desc->bConfigurationValue != conf_val) goto end;
		wdm_usb_save_conf_desc(usb, conf_desc);
		conf_desc = NULL;
		status = usb_set_configuration(dev, conf_val);
	} else {
		for (conf_val = 1;
		     conf_val <= usb->usb_device_desc->bNumConfigurations;
		     conf_val++) {
			if (conf_desc) os_mem_free(conf_desc);
			status = usb_get_config_desc(dev, &conf_desc, conf_val-1);
			if (!OS_IS_SUCCESS(status)) continue;
			ASSERT(conf_desc);
			if (conf_desc->bConfigurationValue != conf_val)  continue;
			wdm_usb_save_conf_desc(usb, conf_desc);
			conf_desc = NULL;
			status = usb_set_configuration(dev, conf_val);
			if (OS_IS_SUCCESS(status))
				break;
		}
	}
	ASSERT(usb->usb_config_desc);

	/* match interface class */
	usb->usb_interface_desc = usb_match_interface_cls(usb->usb_config_desc,
							  usb->usb_driver->devcls,
							  USB_SUBCLASS_ANY,
							  USB_PROTOCOL_ANY,
							  &usb->usb_interface_desc_length);
	if (!usb->usb_interface_desc) {
		status = STATUS_NO_SUCH_DEVICE;
		goto end;
	}
	os_dbg(OS_DBG_DEBUG, "----------\n");
	os_dbg(OS_DBG_DEBUG, "Interface Desc\n");
	os_dbg(OS_DBG_DEBUG, "bInterfaceNumber = %d\n",
	       usb->usb_interface_desc->bInterfaceNumber);
	os_dbg(OS_DBG_DEBUG, "bAlternateSetting = %d\n",
	       usb->usb_interface_desc->bAlternateSetting);
	os_dbg(OS_DBG_DEBUG, "bNumEndpoints = %d\n",
	       usb->usb_interface_desc->bNumEndpoints);
	os_dbg(OS_DBG_DEBUG, "bInterfaceClass = %02X\n",
	       usb->usb_interface_desc->bInterfaceClass);
	os_dbg(OS_DBG_DEBUG, "bInterfaceSubClass = %02X\n",
	       usb->usb_interface_desc->bInterfaceSubClass);
	os_dbg(OS_DBG_DEBUG, "bInterfaceProtocol = %02X\n",
	       usb->usb_interface_desc->bInterfaceProtocol);
	os_dbg(OS_DBG_DEBUG, "wTotalLength = %0d\n",
	       usb->usb_interface_desc_length);

end:
	if (NT_SUCCESS(status)) {
		if (usb->usb_config_desc->bmAttributes & USB_CONFIG_ATTR_REMOTE_WAKEUP)
			wdm_enable_wait_wake(dev);
		usb->state = USB_STATE_CONFIGURED;
	}
	if (conf_desc) os_mem_free(conf_desc);
	if (dev_desc) os_mem_free(dev_desc);
	return status;
}

static status_t usb_control_pipe_aval(os_device dev, uint8_t eid,
				      struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	if (usb->usb_driver->ctrl_aval)
		return usb->usb_driver->ctrl_aval(usb, urb);
	return STATUS_SUCCESS;
}

static void usb_control_pipe_cmpl(os_device dev, uint8_t eid,
				  struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	if (usb->usb_driver->ctrl_cmpl)
		usb->usb_driver->ctrl_cmpl(usb, urb);
}

status_t os_usb_start_device(os_device dev)
{
	os_usbif *usb = dev_priv(dev);
	status_t status;

	wdm_usb_start_pipe(dev, 0, USB_EID_DEFAULT, NULL);
	os_usb_claim_endpoint(dev, USB_EID_DEFAULT,
			      usb_control_pipe_aval,
			      usb_control_pipe_cmpl);

	/* Asynchronous enumeration process */
	usb->state = USB_STATE_DEFAULT;
	status = os_usb_configure_device(dev);
	if (!NT_SUCCESS(status)) goto end;
	if (usb->state != USB_STATE_CONFIGURED) {
		status = STATUS_NO_SUCH_DEVICE;
		goto end;
	}

	wdm_usb_start_idle(dev);
	status = usb->usb_driver->start(usb);
	if (!NT_SUCCESS(status)) goto end;
end:
	return status;
}

void os_usb_stop_device(os_device dev)
{
	os_usbif *usb = dev_priv(dev);

	usb->usb_driver->stop(usb);
	wdm_usb_stop_idle(dev);

	/* abort user operations */
	os_usb_abort_pipes(dev);

	wdm_usb_state_deconf(dev);
	os_usb_deconfigure_device(dev);

	ASSERT(usb->state = USB_STATE_DEFAULT);
	wdm_usb_stop_pipe(dev, 0);
}

PIRP __wdm_usb_pipe_alloc_urb(os_device dev, void *q_data,
			      struct os_work_item *item)
{
	struct os_usb_pipe *pipe = (struct os_usb_pipe *)q_data;
	struct os_usb_urb *urb = work_priv(item);
	ULONG code;
	if (!urb)
		return NULL;
	switch (urb->action) {
	case OS_USB_RESET_PORT:
		code = IOCTL_INTERNAL_USB_RESET_PORT;
		break;
	case OS_USB_CYCLE_PORT:
		code = IOCTL_INTERNAL_USB_CYCLE_PORT;
		break;
	case OS_USB_SUBMIT_URB:
	default:
		code = IOCTL_INTERNAL_USB_SUBMIT_URB;
		break;
	}
	os_dbg(OS_DBG_DEBUG, "USB_IOCTL - ctrl=%s, action=%d\n",
	       wdm_usb_ioctl_string(code), urb->action);
	return IoBuildDeviceIoControlRequest(code,
					     wdm_dev_stacked(dev),
					     NULL, 0,  NULL, 0, TRUE,
					     &pipe->waiter, &pipe->iosb);
}

void __wdm_usb_pipe_free_urb(os_device dev, void *data, PIRP irp)
{
	/* Nothing should be done for destroying URB IRP.  It is done
	 * automatically by IoCompleteRequest in the port drivers.
	 */
}

status_t __wdm_usb_pipe_aval(struct os_work_item *__i,
			     os_device dev)
{
	struct os_usb_pipe *pipe = os_worker_item_data(__i);
	struct os_usb_urb *urb = work_priv(__i);

	BUG_ON(!pipe->aval);
	return pipe->aval(dev, pipe->eid, urb);
}

status_t __wdm_usb_pipe_cmpl(struct os_work_item *__i,
			     os_device dev, status_t status)
{
	struct os_usb_pipe *pipe = os_worker_item_data(__i);
	struct os_usb_urb *urb = work_priv(__i);

	BUG_ON(!pipe->cmpl);
	if (urb->urb) {
		urb->status = urb->urb->UrbHeader.Status;
		urb->length = urb->urb->UrbControlTransfer.TransferBufferLength;
	} else {
		if (urb->action == OS_USB_SUBMIT_URB) {
			urb->status = USBD_STATUS_ERROR_BUSY;
		} else {
			/* Do not care about RESET/CYCLE result */
			urb->status = USBD_STATUS_SUCCESS;
		}
	}
	pipe->cmpl(dev, pipe->eid, urb);
	os_usb_exit_urb(urb);
	return STATUS_SUCCESS;
}

struct os_usb_pipe *os_usb_pipe_by_eid(os_device dev, uint8_t eid)
{
	os_usbif *usb = dev_priv(dev);
	struct os_usb_pipe *pipe = NULL;
	int i;

	if (eid == USB_EID_DEFAULT)
		return &usb->usb_ctrl_pipe;
	for (i = 0; i < usb->nr_usb_endpoints; i++) {
		if (usb->usb_endpoints[i].eid == eid)
			return &usb->usb_endpoints[i];
	}
	return NULL;
}

status_t os_usb_claim_endpoint(os_device dev, uint8_t eid,
			       os_usb_aval_cb aval, os_usb_cmpl_cb cmpl)
{
	struct os_usb_pipe *pipe = os_usb_pipe_by_eid(dev, eid);
	if (!pipe)
		return STATUS_NO_SUCH_DEVICE;
	BUG_ON(pipe->aval);
	BUG_ON(pipe->cmpl);
	pipe->aval = aval;
	pipe->cmpl = cmpl;
	return STATUS_SUCCESS;
}

void wdm_usb_start_pipe(os_device dev, int id, uint8_t eid,
			USBD_PIPE_INFORMATION *info)
{
	os_usbif *usb = dev_priv(dev);
	struct os_usb_pipe *pipe = NULL;

	if (id == 0) {
		pipe = &usb->usb_ctrl_pipe;
	} else if ((id-1) < usb->nr_usb_endpoints) {
		pipe = &usb->usb_endpoints[id-1];
	}
	if (pipe) {
		/* WDM specific */
		pipe->pipe_id = id;
		pipe->pipe_info = info;
		/* start a work queue for URB serialization */
		os_waiter_init(&pipe->waiter, TRUE);

		/* OS independent */
		pipe->eid = eid;
		pipe->flags = 0;
		pipe->aval = NULL;
		pipe->cmpl = NULL;
		/* URB serialization work queue */
		pipe->urb_worker.wdm_alloc_irp = __wdm_usb_pipe_alloc_urb;
		pipe->urb_worker.wdm_free_irp = __wdm_usb_pipe_free_urb;
		os_worker_create_queue(dev, &pipe->urb_worker,
				       __wdm_usb_pipe_aval,
				       __wdm_usb_pipe_cmpl, pipe);
	}
}

void wdm_usb_stop_pipe(os_device dev, int id)
{
	os_usbif *usb = dev_priv(dev);
	struct os_usb_pipe *pipe = NULL;

	if (id == USB_EID_DEFAULT) {
		pipe = &usb->usb_ctrl_pipe;
	} else if ((id-1) < usb->nr_usb_endpoints) {
		pipe = &usb->usb_endpoints[id-1];
	}
	if (pipe) {
		os_usb_suspend_async(dev, pipe->eid);
		os_worker_delete_queue(&pipe->urb_worker);
		os_waiter_wait(&pipe->waiter, OS_WAIT_INFINITE);
		pipe->aval = NULL;
		pipe->cmpl = NULL;
	}
}

os_device os_usb_device(os_usbif *usb)
{
	return usb->dev;
}

status_t os_usb_init_device(os_device dev)
{
	os_usbif *usb = dev_priv(dev);

	usb->state = USB_STATE_ATTACHED;
	usb->usb_driver = os_usb_driver;
	/* Since os_usbif resides in wdm_device context, so reference
	 * count will not be increased.
	 */
	usb->dev = dev;
	usb->ref_users = 0;
	usb->usb_interface_desc = NULL;
	usb->usb_device_desc = NULL;
	usb->usb_config_desc = NULL;
	usb->usb_interface = NULL;
	usb->usb_endpoints = NULL;
	usb->nr_usb_endpoints = 0;

	usb->is_composite = (NULL != strstr(os_dev_id(dev), "&mi_"));
	wdm_usb_init_idle(dev);
	return usb->usb_driver->init(usb);
}

void os_usb_exit_device(os_device dev)
{
	os_usbif *usb = dev_priv(dev);

	/* !usb_driver means not initialized */
	if (usb->usb_driver) {
		usb->usb_driver->exit(usb);
	}
	wdm_usb_exit_idle(dev);
	ASSERT(usb->usb_device_desc == NULL);
	ASSERT(usb->usb_config_desc == NULL);
	ASSERT(usb->usb_interface == NULL);
	ASSERT(usb->usb_endpoints == NULL);
	usb->state = USB_STATE_ATTACHED;
}

struct os_usb_pipe *wdm_usb_pipe_by_name(PDEVICE_OBJECT dev, PUNICODE_STRING file)
{
	os_usbif *usb = dev_priv(dev);
	LONG ix;
	ULONG uval; 
	ULONG length;
	ULONG umultiplier;
	struct os_usb_pipe *pipe = NULL;
	
	length = (file->Length / sizeof(WCHAR));
	if (length != 0) {
		os_dbg(OS_DBG_DEBUG, "file = %ws length = %d\n", file->Buffer, length);
		/* Parse the pipe#. */
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
			if (uval < 6 && usb->usb_endpoints) {
				pipe = &usb->usb_endpoints[uval];
			}
		}
	}
	return pipe;
}

struct os_usb_file *wdm_usb_open_file(os_usbif *usb,
				      struct os_usb_pipe *pipe, os_file *file)
{
	struct os_usb_file *usb_file = NULL;
	status_t status;

	usb_file = os_mem_alloc(sizeof (struct os_usb_file), "USB_FILE");
	if (!usb_file) {
		return NULL;
	}
	usb_file->file = file;
	usb_file->pipe = pipe;
	usb_file->usb = usb;
	status = usb->usb_driver->open(usb_file);
	if (NT_SUCCESS(status)) {
		file->priv = usb_file;
	} else {
		os_mem_free(usb_file);
		return NULL;
	}
	return usb_file;
}

void wdm_usb_close_file(os_usbif *usb, os_file *file)
{
	struct os_usb_file *usb_file = file_priv(file);
	struct os_usb_pipe *pipe;

	if (usb_file) {
		pipe = usb_file->pipe;
		ASSERT(usb == usb_file->usb);
		ASSERT(file == usb_file->file);
		usb->usb_driver->close(usb_file);
		file->priv = NULL;
		os_mem_free(usb_file);
	}
}

status_t os_usb_open_file(os_device dev, os_file *file)
{
	struct os_usb_pipe *pipe;
	NTSTATUS status = STATUS_SUCCESS;
	os_usbif *usb = dev_priv(dev);
	struct os_usb_file *usb_file = NULL;

	if (!usb->usb_interface) {
		status = STATUS_INVALID_DEVICE_STATE;
		goto end;
	}
	if (0 == file->file_name.Length) {
		/* Control pipe code */
		pipe = &usb->usb_ctrl_pipe;
	} else {
		pipe = wdm_usb_pipe_by_name(dev, &file->file_name);
		if (pipe == NULL) {
			status = STATUS_INVALID_PARAMETER;
			goto end;
		}
	}
	usb_file = wdm_usb_open_file(usb, pipe, file);
	if (!usb_file) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto end;
	}

	wdm_usb_user_unidle(dev);
end:
	return status;
}

void os_usb_close_file(os_device dev, os_file *file)
{
	os_usbif *usb = dev_priv(dev);
	wdm_usb_close_file(usb, file);
	wdm_usb_user_idle(dev);
}

void os_usb_suspend_device(os_device dev)
{
	os_usbif *usb = dev_priv(dev);

	if (usb->usb_driver->suspend)
		usb->usb_driver->suspend(usb);
	wdm_usb_suspend_idle(dev);
	usb->resumed_state = usb->state;
	usb->state = USB_STATE_SUSPENDED;
}

void os_usb_resume_device(os_device dev)
{
	os_usbif *usb = dev_priv(dev);

	wdm_usb_submit_idle(dev);
	if (usb->usb_driver->resume)
		usb->usb_driver->resume(usb);
	usb->state = usb->resumed_state;
}

status_t os_usb_ioctl_file(os_device dev, os_file *file,
			   unsigned long code, void *arg,
			   int *plength)
{
	os_usbif *usb = dev_priv(dev);
	status_t status = STATUS_SUCCESS;
	int length = *plength;
	uint8_t *desc = NULL;
	uint8_t index;

	switch (code) {
	case USBDIORESETEP:
		index = (*(uint8_t *)arg);
		os_dbg(OS_DBG_DEBUG, "USBDIORESETEP - %d %d\n", length, index);
		*plength = 0;
		status = usb_unhalt_endpoint(dev, index);
		break;
	case USBDIORESETIF:
		os_dbg(OS_DBG_DEBUG, "USBDIORESETIF - %d\n", length);
		*plength = 0;
		status = os_usb_reset_device(dev);
		break;
	case USBDIOCYCLEIF:
		os_dbg(OS_DBG_DEBUG, "USBDIOCYCLEIF - %d\n", length);
		*plength = 0;
		status = os_usb_cycle_device(dev);
		break;
	case USBDIOGETIFDESC:
		os_dbg(OS_DBG_DEBUG, "USBDIOGETIFDESC - %d\n", length);
		*plength = usb->usb_interface_desc_length;
		if (length < usb->usb_interface_desc_length) {
			status = STATUS_BUFFER_TOO_SMALL;
			goto end;
		}
                os_mem_move(arg,
                            usb->usb_interface_desc,
                            usb->usb_interface_desc_length);
		break;
	case USBDIOGETSTRING:
		index = (*(uint8_t *)arg);
		os_dbg(OS_DBG_DEBUG, "USBDIOGETSTRING - %d %d\n", length, index);
		status = usb_get_string_desc(dev, (struct usb_string_desc **)(&desc),
					     index);
		if (!NT_SUCCESS(status)) {
			*plength = 0;
			goto end;
		}
		*plength = ((struct usb_string_desc *)desc)->bLength - 2;
		if (length < *plength) {
			status = STATUS_BUFFER_TOO_SMALL;
			goto end;
		}
                os_mem_move(arg, desc+2, *plength);
		break;
	default:
		if (usb->usb_driver->ioctl && file_priv(file)) {
			return usb->usb_driver->ioctl(file_priv(file), code, arg, plength);
		} else {
			*plength = 0;
			status = STATUS_INVALID_PARAMETER;
		}
		break;
	}
end:
	if (desc) os_mem_free(desc);
	return status;
}

status_t os_usb_read_file(os_device dev, os_file *file,
			  char *buf, size_t *count)
{
	os_usbif *usb = dev_priv(dev);
	status_t status = STATUS_SUCCESS;
	if (usb->usb_driver->read && file_priv(file)) {
		return usb->usb_driver->read(file_priv(file), buf, count);
	} else {
		*count = 0;
		return STATUS_INVALID_PARAMETER;
	}
}

status_t os_usb_write_file(os_device dev, os_file *file,
			   const char *buf, size_t *count)
{
	os_usbif *usb = dev_priv(dev);
	status_t status = STATUS_SUCCESS;
	if (usb->usb_driver->read && file_priv(file)) {
		return usb->usb_driver->write(file_priv(file), buf, count);
	} else {
		*count = 0;
		return STATUS_INVALID_PARAMETER;
	}
}

struct os_driver wdm_usb_driver = {
	os_usb_start_device,	/* start device */
	os_usb_stop_device,	/* stop device */
	os_usb_init_device,	/* alloc memory */
	os_usb_exit_device,	/* free memory */
	os_usb_suspend_device,
	os_usb_resume_device,
	os_usb_open_file,
	os_usb_close_file,
	os_usb_ioctl_file,
	os_usb_read_file,
	os_usb_write_file,
};

status_t os_usb_declare_interface(struct os_usb_driver *intf,
				  size_t objsize)
{
	unsigned long flags = OS_DEVICE_REMOVE;

	if (intf->caps & USB_CAPS_CAN_REMOVE)
		flags |= OS_DEVICE_HOTPLUG;
	if (intf->caps & USB_CAPS_CAN_SUSPEND)
		flags |= OS_DEVICE_SUSPEND;
	if (intf->caps & USB_CAPS_CAN_WAKEUP)
		flags |= OS_DEVICE_WAKEUP;

	os_usb_driver = intf;
	return os_dev_register_driver(&wdm_usb_driver,
				      objsize+sizeof(struct os_usb_interface),
				      flags);
}
