#include "usbstub.h"

NTSTATUS get_descriptor(libusb_device_t *dev,
                        void *buffer, int size, int type, int recipient,
                        int index, int language_id,
			int *received, int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB urb;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("get_descriptor(): buffer size %d", size);
	DEBUG_MESSAGE("get_descriptor(): type %04d", type);
	DEBUG_MESSAGE("get_descriptor(): recipient %04d", recipient);
	DEBUG_MESSAGE("get_descriptor(): index %04d", index);
	DEBUG_MESSAGE("get_descriptor(): language id %04d", language_id);
	DEBUG_MESSAGE("get_descriptor(): timeout %d", timeout);
	
	memset(&urb, 0, sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
	
	switch (recipient)
	{
	case USB_RECP_DEVICE:
		urb.UrbHeader.Function = URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE;
		break;
	case USB_RECP_INTERFACE:
		urb.UrbHeader.Function = URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE;
		break;
	case USB_RECP_ENDPOINT:
		urb.UrbHeader.Function = URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT;
		break;
	default:
		DEBUG_ERROR("get_descriptor(): invalid recipient");
		return STATUS_INVALID_PARAMETER;
	}
	
	urb.UrbHeader.Length = sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST);
	urb.UrbControlDescriptorRequest.TransferBufferLength = size;
	urb.UrbControlDescriptorRequest.TransferBuffer = buffer;
	urb.UrbControlDescriptorRequest.DescriptorType = (UCHAR)type;
	urb.UrbControlDescriptorRequest.Index = (UCHAR)index;
	urb.UrbControlDescriptorRequest.LanguageId = (USHORT)language_id;
	
	status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb.UrbHeader.Status))
	{
		DEBUG_ERROR("get_descriptor(): getting descriptor "
			    "failed: status: 0x%x, urb-status: 0x%x", 
			    status, urb.UrbHeader.Status);
		*received = 0;
	}
	else
	{
		*received = urb.UrbControlDescriptorRequest.TransferBufferLength;
	}
	
	return status;
}

USB_CONFIGURATION_DESCRIPTOR *get_config_descriptor(libusb_device_t *dev,
						    int value, int *size)
{
	NTSTATUS status;
	USB_CONFIGURATION_DESCRIPTOR *desc = NULL;
	USB_DEVICE_DESCRIPTOR device_descriptor;
	int i;
	volatile int desc_size;
	
	status = get_descriptor(dev, &device_descriptor,
				sizeof(USB_DEVICE_DESCRIPTOR), 
				USB_DEVICE_DESCRIPTOR_TYPE,
				USB_RECP_DEVICE,
				0, 0, size, LIBUSB_DEFAULT_TIMEOUT);  
	
	if (!NT_SUCCESS(status) || *size != sizeof (USB_DEVICE_DESCRIPTOR))
	{
		DEBUG_ERROR("get_config_descriptor(): getting device descriptor failed");
		return NULL;
	}
	
	if (!(desc = ExAllocatePool(NonPagedPool, 
				    sizeof(USB_CONFIGURATION_DESCRIPTOR))))
	{
		DEBUG_ERROR("get_config_descriptor(): memory allocation error");
		return NULL;
	}
	
	for (i = 0; i < device_descriptor.bNumConfigurations; i++)
	{
		if (!NT_SUCCESS(get_descriptor(dev, desc, 
					       sizeof(USB_CONFIGURATION_DESCRIPTOR), 
					       USB_CONFIGURATION_DESCRIPTOR_TYPE,
					       USB_RECP_DEVICE,
					       i, 0, size, LIBUSB_DEFAULT_TIMEOUT)))
		{
			DEBUG_ERROR("get_config_descriptor(): getting configuration "
				    "descriptor failed");
			break;
		}
		
		if (desc->bConfigurationValue == value)
		{
			desc_size = desc->wTotalLength;
			ExFreePool(desc);
			
			if (!(desc = ExAllocatePool(NonPagedPool, desc_size)))
			{
				DEBUG_ERROR("get_config_descriptor(): memory allocation error");
				break;
			}
			
			if (!NT_SUCCESS(get_descriptor(dev, desc, desc_size,
						       USB_CONFIGURATION_DESCRIPTOR_TYPE,
						       USB_RECP_DEVICE,
						       i, 0, size, LIBUSB_DEFAULT_TIMEOUT)))
			{
				DEBUG_ERROR("get_config_descriptor(): getting configuration "
					    "descriptor failed");
				break;
			}
			
			return desc;
		}
	}
	
	if (desc)
	{
		ExFreePool(desc);
	}
	
	return NULL;
}

NTSTATUS set_descriptor(libusb_device_t *dev,
                        void *buffer, int size, int type, int recipient,
                        int index, int language_id, int *sent, int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB urb;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("set_descriptor(): buffer size %d", size);
	DEBUG_MESSAGE("set_descriptor(): type %04d", type);
	DEBUG_MESSAGE("set_descriptor(): recipient %04d", recipient);
	DEBUG_MESSAGE("set_descriptor(): index %04d", index);
	DEBUG_MESSAGE("set_descriptor(): language id %04d", language_id);
	DEBUG_MESSAGE("set_descriptor(): timeout %d", timeout);
	
	memset(&urb, 0, sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
	
	switch (recipient)
	{
	case USB_RECP_DEVICE:
		urb.UrbHeader.Function = URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE;
		break;
	case USB_RECP_INTERFACE:
		urb.UrbHeader.Function = URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE;
		break;
	case USB_RECP_ENDPOINT:
		urb.UrbHeader.Function = URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT;
		break;
	default:
		DEBUG_ERROR("set_descriptor(): invalid recipient");
		return STATUS_INVALID_PARAMETER;
	}
	
	urb.UrbHeader.Length = sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST);
	urb.UrbControlDescriptorRequest.TransferBufferLength = size;
	urb.UrbControlDescriptorRequest.TransferBuffer = buffer;
	urb.UrbControlDescriptorRequest.DescriptorType = (UCHAR)type;
	urb.UrbControlDescriptorRequest.Index = (UCHAR)index;
	urb.UrbControlDescriptorRequest.LanguageId = (USHORT)language_id;
	
	status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb.UrbHeader.Status))
	{
		DEBUG_ERROR("set_descriptor(): setting descriptor failed: status: "
			    "0x%x, urb-status: 0x%x", status, urb.UrbHeader.Status);
	}
	else
	{
		*sent = urb.UrbControlDescriptorRequest.TransferBufferLength;
	}
	return status;
}

NTSTATUS get_configuration(libusb_device_t *dev,
                           unsigned char *configuration, int *ret, 
                           int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB urb;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("get_configuration(): timeout %d", timeout);
	
	memset(&urb, 0, sizeof(URB));
	
	urb.UrbHeader.Function = URB_FUNCTION_GET_CONFIGURATION;
	urb.UrbHeader.Length = sizeof(struct _URB_CONTROL_GET_CONFIGURATION_REQUEST);
	urb.UrbControlGetConfigurationRequest.TransferBufferLength = 1;
	urb.UrbControlGetConfigurationRequest.TransferBuffer = configuration;
	
	status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb.UrbHeader.Status))
	{
		DEBUG_ERROR("get_configuration(): getting configuration failed: "
			    "status: 0x%x, urb-status: 0x%x",
			    status, urb.UrbHeader.Status);
		*ret = 0;
	}
	else
	{
		DEBUG_MESSAGE("get_configuration(): current configuration is: %d", 
			      *configuration);
		*ret = urb.UrbControlGetConfigurationRequest.TransferBufferLength;
	}
	
	return status;
}

NTSTATUS set_configuration(libusb_device_t *dev, int configuration, 
                           int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB urb, *urb_ptr = NULL;
	USB_CONFIGURATION_DESCRIPTOR *configuration_descriptor = NULL;
	USB_INTERFACE_DESCRIPTOR *interface_descriptor = NULL;
	USBD_INTERFACE_LIST_ENTRY *interfaces = NULL;
	int i, j, interface_number, desc_size;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("set_configuration(): configuration %d", configuration);
	DEBUG_MESSAGE("set_configuration(): timeout %d", timeout);
	
	if (dev->config.value == configuration)
	{
		return STATUS_SUCCESS;
	}
	
	memset(&urb, 0, sizeof(URB));
	
	if (!configuration)
	{
		urb.UrbHeader.Function = URB_FUNCTION_SELECT_CONFIGURATION;
		urb.UrbHeader.Length = sizeof(struct _URB_SELECT_CONFIGURATION);
		
		status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
		
		if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb.UrbHeader.Status))
		{
			DEBUG_ERROR("set_configuration(): setting configuration %d failed: "
				    "status: 0x%x, urb-status: 0x%x", 
				    configuration, status, urb.UrbHeader.Status);
			return status;
		}
		
		dev->config.handle =  urb.UrbSelectConfiguration.ConfigurationHandle;
		dev->config.value = 0;
		
		clear_pipe_info(dev);
		
		return status;
	}
	
	configuration_descriptor = get_config_descriptor(dev, configuration, 
		&desc_size);
	if (!configuration_descriptor)
	{
		DEBUG_ERROR("set_configuration(): getting configuration descriptor "
			    "failed");
		return STATUS_INVALID_PARAMETER;
	}
	
	interfaces = ExAllocatePool(NonPagedPool,
				    (configuration_descriptor->bNumInterfaces + 1) *
				    sizeof(USBD_INTERFACE_LIST_ENTRY));
	
	if (!interfaces)
	{
		DEBUG_ERROR("set_configuration(): memory allocation failed");
		ExFreePool(configuration_descriptor);
		return STATUS_NO_MEMORY;
	}
	
	memset(interfaces, 0,
	       (configuration_descriptor->bNumInterfaces + 1) *
	       sizeof(USBD_INTERFACE_LIST_ENTRY));
	
	interface_number = 0;
	
	for (i = 0; i < configuration_descriptor->bNumInterfaces; i++)
	{
		for (j = interface_number; j < LIBUSB_MAX_NUMBER_OF_INTERFACES; j++)
		{
			interface_descriptor =
				find_interface_desc(configuration_descriptor, desc_size, j, 0);
			if (interface_descriptor) 
			{
				interface_number = ++j;
				break;
			}
		}
		
		if (!interface_descriptor)
		{
			DEBUG_ERROR("set_configuration(): unable to find interface "
				    "descriptor at index %d", i);
			ExFreePool(interfaces);
			ExFreePool(configuration_descriptor);
			return STATUS_INVALID_PARAMETER;
		}
		else
		{
			DEBUG_MESSAGE("set_configuration(): found interface %d",
				      interface_descriptor->bInterfaceNumber);
			interfaces[i].InterfaceDescriptor = interface_descriptor;
		}
	}
	
	urb_ptr = USBD_CreateConfigurationRequestEx(configuration_descriptor,
		interfaces);
	
	if (!urb_ptr)
	{
		DEBUG_ERROR("set_configuration(): memory allocation failed");
		ExFreePool(interfaces);
		ExFreePool(configuration_descriptor);
		return STATUS_NO_MEMORY;
	}
	
	for (i = 0; i < configuration_descriptor->bNumInterfaces; i++)
	{
		for (j = 0; j < (int)interfaces[i].Interface->NumberOfPipes; j++)
		{
			interfaces[i].Interface->Pipes[j].MaximumTransferSize 
				= LIBUSB_MAX_READ_WRITE;
		}
	}
	
	status = call_usbd(dev, urb_ptr, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb_ptr->UrbHeader.Status))
	{
		DEBUG_ERROR("set_configuration(): setting configuration %d failed: "
			    "status: 0x%x, urb-status: 0x%x", 
			    configuration, status, urb_ptr->UrbHeader.Status);
		ExFreePool(interfaces);
		ExFreePool(configuration_descriptor);
		ExFreePool(urb_ptr);
		return status;
	}
	
	dev->config.handle = urb_ptr->UrbSelectConfiguration.ConfigurationHandle;
	dev->config.value = configuration;
	
	clear_pipe_info(dev);
	
	for (i = 0; i < configuration_descriptor->bNumInterfaces; i++)
	{
		update_pipe_info(dev, interfaces[i].Interface);
	}
	
	ExFreePool(interfaces);
	ExFreePool(urb_ptr);
	ExFreePool(configuration_descriptor);
	
	return status;
}

NTSTATUS get_interface(libusb_device_t *dev,
                       int interface, unsigned char *altsetting, 
                       int *ret, int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB urb;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("get_interface(): interface %d", interface);
	DEBUG_MESSAGE("get_interface(): timeout %d", timeout);
	
	if (!dev->config.value)
	{
		DEBUG_ERROR("get_interface(): invalid configuration 0"); 
		return STATUS_INVALID_DEVICE_STATE;
	}
	
	memset(&urb, 0, sizeof(URB));
	
	urb.UrbHeader.Function = URB_FUNCTION_GET_INTERFACE;
	urb.UrbHeader.Length = sizeof(struct _URB_CONTROL_GET_INTERFACE_REQUEST);
	urb.UrbControlGetInterfaceRequest.TransferBufferLength = 1;
	urb.UrbControlGetInterfaceRequest.TransferBuffer = altsetting;
	urb.UrbControlGetInterfaceRequest.Interface = (USHORT)interface;
	
	status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb.UrbHeader.Status))
	{
		DEBUG_ERROR("get_interface(): getting interface "
			    "failed: status: 0x%x, urb-status: 0x%x", 
			    status, urb.UrbHeader.Status);
		*ret = 0;
	}
	else
	{
		*ret = urb.UrbControlGetInterfaceRequest.TransferBufferLength;
		DEBUG_MESSAGE("get_interface(): current altsetting is %d", *altsetting); 
	}
	
	return status;
}

NTSTATUS set_interface(libusb_device_t *dev, int interface, int altsetting,
                       int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB *urb;
	int i, config_size, tmp_size;
	
	USB_CONFIGURATION_DESCRIPTOR *configuration_descriptor = NULL;
	USB_INTERFACE_DESCRIPTOR *interface_descriptor = NULL;
	USBD_INTERFACE_INFORMATION *interface_information = NULL;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("set_interface(): interface %d", interface);
	DEBUG_MESSAGE("set_interface(): altsetting %d", altsetting);
	DEBUG_MESSAGE("set_interface(): timeout %d", timeout);
	
	if (!dev->config.value)
	{
		DEBUG_ERROR("release_interface(): device is not configured"); 
		return STATUS_INVALID_DEVICE_STATE;
	}
	
	configuration_descriptor = get_config_descriptor(dev, dev->config.value,
							 &config_size);
	if (!configuration_descriptor)
	{
		DEBUG_ERROR("set_interface(): memory_allocation error");
		return STATUS_NO_MEMORY;
	}
	
	interface_descriptor =
		find_interface_desc(configuration_descriptor, config_size, 
		interface, altsetting);
	
	if (!interface_descriptor)
	{
		DEBUG_ERROR("set_interface(): interface %d or altsetting %d invalid", 
			    interface, altsetting);
		ExFreePool(configuration_descriptor);
		return STATUS_UNSUCCESSFUL;
	}
	
	tmp_size = sizeof(struct _URB_SELECT_INTERFACE)
		+ interface_descriptor->bNumEndpoints
		* sizeof(USBD_PIPE_INFORMATION);
	
	
	urb = ExAllocatePool(NonPagedPool, tmp_size);
	
	if (!urb)
	{
		DEBUG_ERROR("set_interface(): memory_allocation error");
		ExFreePool(configuration_descriptor);
		return STATUS_NO_MEMORY;
	}
	
	memset(urb, 0, tmp_size);
	
	urb->UrbHeader.Function = URB_FUNCTION_SELECT_INTERFACE;
	urb->UrbHeader.Length = (USHORT)tmp_size;
	
	urb->UrbSelectInterface.ConfigurationHandle = dev->config.handle;
	urb->UrbSelectInterface.Interface.Length =
		sizeof(struct _USBD_INTERFACE_INFORMATION);
	urb->UrbSelectInterface.Interface.NumberOfPipes = 
		interface_descriptor->bNumEndpoints;
	urb->UrbSelectInterface.Interface.Length +=
		interface_descriptor->bNumEndpoints 
		* sizeof(struct _USBD_PIPE_INFORMATION);
	
	urb->UrbSelectInterface.Interface.InterfaceNumber = (UCHAR)interface;
	urb->UrbSelectInterface.Interface.AlternateSetting = (UCHAR)altsetting;
	
	interface_information = &urb->UrbSelectInterface.Interface;
	
	for (i = 0; i < interface_descriptor->bNumEndpoints; i++)
	{
		interface_information->Pipes[i].MaximumTransferSize 
			= LIBUSB_MAX_READ_WRITE;
	}
	
	status = call_usbd(dev, urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb->UrbHeader.Status))
	{
		DEBUG_ERROR("set_interface(): setting interface failed: status: 0x%x, "
			    "urb-status: 0x%x", status, urb->UrbHeader.Status);
		ExFreePool(configuration_descriptor);
		ExFreePool(urb);
		return STATUS_UNSUCCESSFUL;
	}
	
	update_pipe_info(dev, interface_information);
	
	ExFreePool(configuration_descriptor);
	ExFreePool(urb);
	
	return status;
}

NTSTATUS get_status(libusb_device_t *dev, int recipient,
                    int index, char *status, int *ret, int timeout)
{
	NTSTATUS _status = STATUS_SUCCESS;
	URB urb;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("get_status(): recipient %02d", recipient);
	DEBUG_MESSAGE("get_status(): index %04d", index);
	DEBUG_MESSAGE("get_status(): timeout %d", timeout);
	
	memset(&urb, 0, sizeof(URB));
	
	switch (recipient)
	{
	case USB_RECP_DEVICE:
		urb.UrbHeader.Function = URB_FUNCTION_GET_STATUS_FROM_DEVICE;
		break;
	case USB_RECP_INTERFACE:
		urb.UrbHeader.Function = URB_FUNCTION_GET_STATUS_FROM_INTERFACE;
		break;
	case USB_RECP_ENDPOINT:
		urb.UrbHeader.Function = URB_FUNCTION_GET_STATUS_FROM_ENDPOINT;
		break;
	case USB_RECP_OTHER:
		urb.UrbHeader.Function = URB_FUNCTION_GET_STATUS_FROM_OTHER;
		break;
	default:
		DEBUG_ERROR("get_status(): invalid recipient");
		return STATUS_INVALID_PARAMETER;
	}
	
	urb.UrbHeader.Length = sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST);
	urb.UrbControlGetStatusRequest.TransferBufferLength = 2;
	urb.UrbControlGetStatusRequest.TransferBuffer = status; 
	urb.UrbControlGetStatusRequest.Index = (USHORT)index; 
	
	_status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(_status) || !USBD_SUCCESS(urb.UrbHeader.Status))
	{
		DEBUG_ERROR("get_status(): getting status failed: "
			    "status: 0x%x, urb-status: 0x%x", 
			    _status, urb.UrbHeader.Status);
		*ret = 0;
	}
	else
	{
		*ret = urb.UrbControlGetStatusRequest.TransferBufferLength;
	}
	
	return _status;
}

NTSTATUS set_feature(libusb_device_t *dev, int recipient, int index, 
                     int feature, int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB urb;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("set_feature(): recipient %02d", recipient);
	DEBUG_MESSAGE("set_feature(): index %04d", index);
	DEBUG_MESSAGE("set_feature(): feature %04d", feature);
	DEBUG_MESSAGE("set_feature(): timeout %d", timeout);
	
	memset(&urb, 0, sizeof(struct _URB_CONTROL_FEATURE_REQUEST));
	
	switch (recipient)
	{
	case USB_RECP_DEVICE:
		urb.UrbHeader.Function = URB_FUNCTION_SET_FEATURE_TO_DEVICE;
		break;
	case USB_RECP_INTERFACE:
		urb.UrbHeader.Function = URB_FUNCTION_SET_FEATURE_TO_INTERFACE;
		break;
	case USB_RECP_ENDPOINT:
		urb.UrbHeader.Function = URB_FUNCTION_SET_FEATURE_TO_ENDPOINT;
		break;
	case USB_RECP_OTHER:
		urb.UrbHeader.Function = URB_FUNCTION_SET_FEATURE_TO_OTHER;
		urb.UrbControlFeatureRequest.Index = 0; 
		break;
	default:
		DEBUG_ERROR("set_feature(): invalid recipient");
		return STATUS_INVALID_PARAMETER;
	}
	
	urb.UrbHeader.Length = sizeof(struct _URB_CONTROL_FEATURE_REQUEST);
	urb.UrbControlFeatureRequest.FeatureSelector = (USHORT)feature;
	urb.UrbControlFeatureRequest.Index = (USHORT)index; 
	
	status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb.UrbHeader.Status))
	{
		DEBUG_ERROR("set_feature(): setting feature failed: status: 0x%x, "
			    "urb-status: 0x%x", status, urb.UrbHeader.Status);
	}
	
	return status;
}

NTSTATUS clear_feature(libusb_device_t *dev,
                       int recipient, int index,
		       int feature, int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB urb;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("clear_feature(): recipient %02d", recipient);
	DEBUG_MESSAGE("clear_feature(): index %04d", index);
	DEBUG_MESSAGE("clear_feature(): feature %04d", feature);
	DEBUG_MESSAGE("clear_feature(): timeout %d", timeout);
	
	memset(&urb, 0, sizeof(struct _URB_CONTROL_FEATURE_REQUEST));
	
	switch (recipient)
	{
	case USB_RECP_DEVICE:
		urb.UrbHeader.Function = URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE;
		break;
	case USB_RECP_INTERFACE:
		urb.UrbHeader.Function = URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE;
		break;
	case USB_RECP_ENDPOINT:
		urb.UrbHeader.Function = URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT;
		break;
	case USB_RECP_OTHER:
		urb.UrbHeader.Function = URB_FUNCTION_CLEAR_FEATURE_TO_OTHER;
		break;
	default:
		DEBUG_ERROR("clear_feature(): invalid recipient");
		return STATUS_INVALID_PARAMETER;
	}
	
	urb.UrbHeader.Length = sizeof(struct _URB_CONTROL_FEATURE_REQUEST);
	urb.UrbControlFeatureRequest.FeatureSelector = (USHORT)feature;
	urb.UrbControlFeatureRequest.Index = (USHORT)index; 
	
	status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb.UrbHeader.Status))
	{
		DEBUG_ERROR("set_feature(): clearing feature failed: status: 0x%x, "
			    "urb-status: 0x%x", status, urb.UrbHeader.Status);
	}
	
	return status;
}

NTSTATUS vendor_class_request(libusb_device_t *dev,
                              int type, int recipient,
                              int request, int value, int index, 
                              void *buffer, int size, int direction,
                              int *ret, int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB urb;
	
	*ret = 0;
	
	DEBUG_PRINT_NL();
	
	memset(&urb, 0, sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));
	
	switch (type)
	{
	case USB_TYPE_CLASS:
		DEBUG_MESSAGE("vendor_class_request(): type: class");
		switch (recipient)
		{
		case USB_RECP_DEVICE:
			DEBUG_MESSAGE("vendor_class_request(): recipient: device");
			urb.UrbHeader.Function = URB_FUNCTION_CLASS_DEVICE;
			break;
		case USB_RECP_INTERFACE:
			DEBUG_MESSAGE("vendor_class_request(): recipient: interface");
			urb.UrbHeader.Function = URB_FUNCTION_CLASS_INTERFACE;
			break;
		case USB_RECP_ENDPOINT:
			DEBUG_MESSAGE("vendor_class_request(): recipient: endpoint");
			urb.UrbHeader.Function = URB_FUNCTION_CLASS_ENDPOINT;
			break;
		case USB_RECP_OTHER:
			DEBUG_MESSAGE("vendor_class_request(): recipient: other");
			urb.UrbHeader.Function = URB_FUNCTION_CLASS_OTHER;
			break;
		default:
			DEBUG_ERROR("vendor_class_request(): invalid recipient");
			return STATUS_INVALID_PARAMETER;
		}
		break;
	case USB_TYPE_VENDOR:
		DEBUG_MESSAGE("vendor_class_request(): type: vendor");
		switch (recipient)
		{
		case USB_RECP_DEVICE:
			DEBUG_MESSAGE("vendor_class_request(): recipient: device");
			urb.UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;
			break;
		case USB_RECP_INTERFACE:
			DEBUG_MESSAGE("vendor_class_request(): recipient: interface");
			urb.UrbHeader.Function = URB_FUNCTION_VENDOR_INTERFACE;
			break;
		case USB_RECP_ENDPOINT:
			DEBUG_MESSAGE("vendor_class_request(): recipient: endpoint");
			urb.UrbHeader.Function = URB_FUNCTION_VENDOR_ENDPOINT;
			break;
		case USB_RECP_OTHER:
			DEBUG_MESSAGE("vendor_class_request(): recipient: other");
			urb.UrbHeader.Function = URB_FUNCTION_VENDOR_OTHER;
			break;
		default:
			DEBUG_ERROR("vendor_class_request(): invalid recipient");
			return STATUS_INVALID_PARAMETER;
		}
		break;
	default:
		DEBUG_ERROR("vendor_class_request(): invalid type");
		return STATUS_INVALID_PARAMETER;
	}
	
	DEBUG_MESSAGE("vendor_class_request(): request: 0x%02x", request);
	DEBUG_MESSAGE("vendor_class_request(): value: 0x%04x", value);
	DEBUG_MESSAGE("vendor_class_request(): index: 0x%04x", index);
	DEBUG_MESSAGE("vendor_class_request(): size: %d", size);
	
	if (direction == USBD_TRANSFER_DIRECTION_IN)
	{
		DEBUG_MESSAGE("vendor_class_request(): direction: in");
	}
	else
	{
		DEBUG_MESSAGE("vendor_class_request(): direction: out");
	}
	
	DEBUG_MESSAGE("vendor_class_request(): timeout: %d", timeout);
	
	urb.UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	urb.UrbControlVendorClassRequest.TransferFlags 
		= direction | USBD_SHORT_TRANSFER_OK ;
	urb.UrbControlVendorClassRequest.TransferBufferLength = size;
	urb.UrbControlVendorClassRequest.TransferBufferMDL = NULL;
	urb.UrbControlVendorClassRequest.TransferBuffer = buffer;
	urb.UrbControlVendorClassRequest.Request = (UCHAR)request;
	urb.UrbControlVendorClassRequest.Value = (USHORT)value;
	urb.UrbControlVendorClassRequest.Index = (USHORT)index;
	
	status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb.UrbHeader.Status))
	{
		DEBUG_ERROR("vendor_class_request(): request failed: status: 0x%x, "
			    "urb-status: 0x%x", status, urb.UrbHeader.Status);
	}
	else
	{
		if (direction == USBD_TRANSFER_DIRECTION_IN)
			*ret = urb.UrbControlVendorClassRequest.TransferBufferLength;
		DEBUG_MESSAGE("vendor_class_request(): %d bytes transmitted", 
			      urb.UrbControlVendorClassRequest.TransferBufferLength);
	}
	
	return status;
}
