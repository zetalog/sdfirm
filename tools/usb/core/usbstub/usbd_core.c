#include "usbstub.h"

typedef struct {
	URB *urb;
	int sequence;
} context_t;

static int sequence = 0;

NTSTATUS DDKAPI transfer_complete(DEVICE_OBJECT *device_object, 
                                  IRP *irp, void *context);

static NTSTATUS create_urb(libusb_device_t *dev, URB **urb, int direction, 
                           int urb_function, int endpoint, int packet_size, 
                           MDL *buffer, int size);

NTSTATUS transfer(libusb_device_t *dev, IRP *irp,
                  int direction, int urb_function, int endpoint, 
                  int packet_size, MDL *buffer, int size)
{
	IO_STACK_LOCATION *stack_location = NULL;
	context_t *context;
	NTSTATUS status = STATUS_SUCCESS;
	
	DEBUG_PRINT_NL();
	
	if (urb_function == URB_FUNCTION_ISOCH_TRANSFER)
		DEBUG_MESSAGE("transfer(): isochronous transfer");
	else
		DEBUG_MESSAGE("transfer(): bulk or interrupt transfer");
	
	if (direction == USBD_TRANSFER_DIRECTION_IN)
		DEBUG_MESSAGE("transfer(): direction in");
	else
		DEBUG_MESSAGE("transfer(): direction out");
	
	DEBUG_MESSAGE("transfer(): endpoint 0x%02x", endpoint);
	
	if (urb_function == URB_FUNCTION_ISOCH_TRANSFER)
		DEBUG_MESSAGE("transfer(): packet_size 0x%x", packet_size);
	
	DEBUG_MESSAGE("transfer(): size %d", size);
	DEBUG_MESSAGE("transfer(): sequence %d", sequence);
	DEBUG_PRINT_NL();
	
	if (!dev->config.value)
	{
		DEBUG_ERROR("transfer(): invalid configuration 0");
		remove_lock_release(dev);
		return complete_irp(irp, STATUS_INVALID_DEVICE_STATE, 0);
	}
	
	context = ExAllocatePool(NonPagedPool, sizeof(context_t));
	
	if (!context)
	{
		remove_lock_release(dev);
		return complete_irp(irp, STATUS_NO_MEMORY, 0);
	}
	
	status = create_urb(dev, &context->urb, direction, urb_function, 
			    endpoint, packet_size, buffer, size);
	
	if (!NT_SUCCESS(status))
	{
		ExFreePool(context);
		remove_lock_release(dev);
		return complete_irp(irp, status, 0);
	}
	
	context->sequence = sequence++;
	
	stack_location = IoGetNextIrpStackLocation(irp);
	
	stack_location->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
	stack_location->Parameters.Others.Argument1 = context->urb;
	stack_location->Parameters.DeviceIoControl.IoControlCode 
		= IOCTL_INTERNAL_USB_SUBMIT_URB;
	
	IoSetCompletionRoutine(irp, transfer_complete, context,
			       TRUE, TRUE, TRUE);
	
	return IoCallDriver(dev->target_device, irp);
}


NTSTATUS DDKAPI transfer_complete(DEVICE_OBJECT *device_object, IRP *irp, 
                                  void *context)
{
	context_t *c = (context_t *)context;
	int transmitted = 0;
	libusb_device_t *dev = device_object->DeviceExtension;
	
	if (irp->PendingReturned)
	{
		IoMarkIrpPending(irp);
	}
	
	if (NT_SUCCESS(irp->IoStatus.Status) &&
	    USBD_SUCCESS(c->urb->UrbHeader.Status))
	{
		if (c->urb->UrbHeader.Function == URB_FUNCTION_ISOCH_TRANSFER)
		{
			transmitted = c->urb->UrbIsochronousTransfer.TransferBufferLength;
		}
		if (c->urb->UrbHeader.Function == URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER)
		{
			transmitted =
				c->urb->UrbBulkOrInterruptTransfer.TransferBufferLength;
		}
		
		DEBUG_MESSAGE("transfer_complete(): sequence %d: %d bytes transmitted", 
			      c->sequence, transmitted);
	}
	else
	{
		if (irp->IoStatus.Status == STATUS_CANCELLED)
		{
			DEBUG_ERROR("transfer_complete(): sequence %d: timeout error",
				    c->sequence);
		}
		else
		{
			DEBUG_ERROR("transfer_complete(): sequence %d: transfer failed: "
				    "status: 0x%x, urb-status: 0x%x", 
				    c->sequence, irp->IoStatus.Status, 
				    c->urb->UrbHeader.Status);
		}
	}
	
	ExFreePool(c->urb);
	ExFreePool(c);
	
	irp->IoStatus.Information = transmitted;
	
	remove_lock_release(dev);
	
	return STATUS_SUCCESS;
}

static NTSTATUS create_urb(libusb_device_t *dev, URB **urb, int direction, 
                           int urb_function, int endpoint, int packet_size, 
                           MDL *buffer, int size)
{
	USBD_PIPE_HANDLE pipe_handle = NULL;
	int num_packets = 0;
	int i, urb_size;
	
	*urb = NULL;
	
	if (!get_pipe_handle(dev, endpoint, &pipe_handle))
	{
		DEBUG_ERROR("create_urb(): getting endpoint pipe failed");
		return STATUS_INVALID_PARAMETER;
	}
	
	/* isochronous transfer */
	if (urb_function == URB_FUNCTION_ISOCH_TRANSFER)
	{
		num_packets = (size + packet_size - 1) / packet_size;
		
		if (num_packets > 255)
		{
			DEBUG_ERROR("create_urb(): transfer size too large");
			return STATUS_INVALID_PARAMETER;
		}
		
		urb_size = sizeof(struct _URB_ISOCH_TRANSFER) +
			   sizeof(USBD_ISO_PACKET_DESCRIPTOR) * num_packets;
	}
	else /* bulk or interrupt transfer */
	{
		urb_size = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
	}
	
	*urb = ExAllocatePool(NonPagedPool, urb_size);
	
	if (!*urb)
	{
		DEBUG_ERROR("create_urb(): memory allocation error");
		return STATUS_NO_MEMORY;
	}
	
	memset(*urb, 0, urb_size);
	
	(*urb)->UrbHeader.Length = (USHORT)urb_size;
	(*urb)->UrbHeader.Function = (USHORT)urb_function;
	
	/* isochronous transfer */
	if (urb_function == URB_FUNCTION_ISOCH_TRANSFER)
	{
		(*urb)->UrbIsochronousTransfer.PipeHandle = pipe_handle;
		(*urb)->UrbIsochronousTransfer.TransferFlags 
			= direction | USBD_SHORT_TRANSFER_OK | USBD_START_ISO_TRANSFER_ASAP;
		(*urb)->UrbIsochronousTransfer.TransferBufferLength = size;
		(*urb)->UrbIsochronousTransfer.TransferBufferMDL = buffer;
		(*urb)->UrbIsochronousTransfer.NumberOfPackets = num_packets;
		
		for (i = 0; i < num_packets; i++)
		{
			(*urb)->UrbIsochronousTransfer.IsoPacket[i].Offset = i * packet_size;
			(*urb)->UrbIsochronousTransfer.IsoPacket[i].Length = packet_size;
		}
	}
	/* bulk or interrupt transfer */
	else
	{
		(*urb)->UrbBulkOrInterruptTransfer.PipeHandle = pipe_handle;
		(*urb)->UrbBulkOrInterruptTransfer.TransferFlags 
			= direction | USBD_SHORT_TRANSFER_OK;
		(*urb)->UrbBulkOrInterruptTransfer.TransferBufferLength = size;
		(*urb)->UrbBulkOrInterruptTransfer.TransferBufferMDL = buffer;
	}
	
	return STATUS_SUCCESS;
}

NTSTATUS claim_interface(libusb_device_t *dev, int interface)
{
	DEBUG_MESSAGE("claim_interface(): interface %d", interface);
	
	if (!dev->config.value)
	{
		DEBUG_ERROR("claim_interface(): device is not configured"); 
		return STATUS_INVALID_DEVICE_STATE;
	}
	
	if (interface >= LIBUSB_MAX_NUMBER_OF_INTERFACES)
	{
		DEBUG_ERROR("claim_interface(): interface number %d too high", 
			    interface);
		return STATUS_INVALID_PARAMETER;
	}
	
	if (!dev->config.interfaces[interface].valid)
	{
		DEBUG_ERROR("claim_interface(): interface %d does not exist", interface);
		return STATUS_INVALID_PARAMETER;
	}
	
	if (dev->config.interfaces[interface].claimed)
	{
		DEBUG_ERROR("claim_interface(): could not claim interface %d, "
			    "interface is already claimed", interface);
		return STATUS_DEVICE_BUSY;
	}
	
	dev->config.interfaces[interface].claimed = TRUE;
	
	return STATUS_SUCCESS;
}

NTSTATUS release_interface(libusb_device_t *dev, int interface)
{
	DEBUG_MESSAGE("release_interface(): interface %d", interface);
	
	if (!dev->config.value)
	{
		DEBUG_ERROR("release_interface(): device is not configured"); 
		return STATUS_INVALID_DEVICE_STATE;
	}
	
	if (interface >= LIBUSB_MAX_NUMBER_OF_INTERFACES)
	{
		DEBUG_ERROR("release_interface(): interface number %d too high", 
			interface);
		return STATUS_INVALID_PARAMETER;
	}
	
	if (!dev->config.interfaces[interface].valid)
	{
		DEBUG_ERROR("release_interface(): invalid interface %02d", interface);
		return STATUS_INVALID_PARAMETER;
	}
	
	if (!dev->config.interfaces[interface].claimed)
	{
		DEBUG_ERROR("claim_interface(): could not release interface %d, "
			"interface is not claimed", interface);
		return STATUS_INVALID_DEVICE_STATE;
	}
	
	dev->config.interfaces[interface].claimed = FALSE;
	return STATUS_SUCCESS;
}

NTSTATUS release_all_interfaces(libusb_device_t *dev)
{
	int i;
	
	for (i = 0; i < LIBUSB_MAX_NUMBER_OF_INTERFACES; i++)
	{
		dev->config.interfaces[i].claimed = FALSE;
	}
	return STATUS_SUCCESS;
}

NTSTATUS reset_device(libusb_device_t *dev, int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	
	DEBUG_MESSAGE("reset_device()");
	
	status = call_usbd(dev, NULL, IOCTL_INTERNAL_USB_RESET_PORT, timeout);
	
	if(!NT_SUCCESS(status))
	{
		DEBUG_ERROR("reset_device(): IOCTL_INTERNAL_USB_RESET_PORT failed: "
			"status: 0x%x", status);
	}
	
	status = call_usbd(dev, NULL, IOCTL_INTERNAL_USB_CYCLE_PORT, timeout);
	
	if (!NT_SUCCESS(status))
	{
		DEBUG_ERROR("reset_device(): IOCTL_INTERNAL_USB_CYCLE_PORT failed: "
			    "status: 0x%x", status);
	}
	
	return status;
}

NTSTATUS reset_endpoint(libusb_device_t *dev, int endpoint, int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB urb;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("reset_endpoint(): endpoint 0x%02x", endpoint);
	DEBUG_MESSAGE("reset_endpoint(): timeout %d", timeout);
	
	if (!dev->config.value)
	{
		DEBUG_ERROR("reset_endpoint(): invalid configuration 0"); 
		return STATUS_INVALID_DEVICE_STATE;
	}
	
	memset(&urb, 0, sizeof(struct _URB_PIPE_REQUEST));
	
	urb.UrbHeader.Length = (USHORT) sizeof(struct _URB_PIPE_REQUEST);
	urb.UrbHeader.Function = URB_FUNCTION_RESET_PIPE;
	
	if (!get_pipe_handle(dev, endpoint, &urb.UrbPipeRequest.PipeHandle))
	{
		DEBUG_ERROR("reset_endpoint(): getting endpoint pipe failed");
		return STATUS_INVALID_PARAMETER;
	}
	
	status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb.UrbHeader.Status))
	{
		DEBUG_ERROR("reset_endpoint(): request failed: status: 0x%x, "
			    "urb-status: 0x%x", status, urb.UrbHeader.Status);
	}
	
	return status;
}

NTSTATUS abort_endpoint(libusb_device_t *dev, int endpoint, int timeout)
{
	NTSTATUS status = STATUS_SUCCESS;
	URB urb;
	
	DEBUG_PRINT_NL();
	DEBUG_MESSAGE("abort_endpoint(): endpoint 0x%02x\n", endpoint);
	DEBUG_MESSAGE("abort_endpoint(): timeout %d\n", timeout);
	
	memset(&urb, 0, sizeof(struct _URB_PIPE_REQUEST));
	
	if (!dev->config.value)
	{
		DEBUG_ERROR("abort_endpoint(): invalid configuration 0");
		return STATUS_INVALID_DEVICE_STATE;
	}
	
	if (!get_pipe_handle(dev, endpoint, &urb.UrbPipeRequest.PipeHandle))
	{
		DEBUG_ERROR("abort_endpoint(): getting endpoint pipe failed");
		return STATUS_INVALID_PARAMETER;
	}
	
	urb.UrbHeader.Length = (USHORT) sizeof(struct _URB_PIPE_REQUEST);
	urb.UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
	
	status = call_usbd(dev, &urb, IOCTL_INTERNAL_USB_SUBMIT_URB, timeout);
	
	if (!NT_SUCCESS(status) || !USBD_SUCCESS(urb.UrbHeader.Status))
	{
		DEBUG_ERROR("abort_endpoint(): request failed: status: 0x%x, "
			    "urb-status: 0x%x", status, urb.UrbHeader.Status);
	}
	
	return status;
}
