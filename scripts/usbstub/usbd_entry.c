/* LIBUSB-WIN32, Generic Windows USB Library
 * Copyright (c) 2002-2005 Stephan Meyer <ste_meyer@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#define __LIBUSB_DRIVER_C__

#include "usbstub.h"

extern int debug_level;

static void DDKAPI unload(DRIVER_OBJECT *driver_object);
static NTSTATUS DDKAPI on_usbd_complete(DEVICE_OBJECT *device_object, 
                                        IRP *irp, 
                                        void *context);
static NTSTATUS DDKAPI
on_power_state_complete(DEVICE_OBJECT *device_object,
                        IRP           *irp,
                        void          *context);
static void DDKAPI 
on_power_set_device_state_complete(DEVICE_OBJECT *device_object,
                                   UCHAR minor_function,
                                   POWER_STATE power_state,
                                   void *context,
                                   IO_STATUS_BLOCK *io_status);
static NTSTATUS DDKAPI 
on_start_complete(DEVICE_OBJECT *device_object, IRP *irp, 
                  void *context);
static NTSTATUS DDKAPI
on_device_usage_notification_complete(DEVICE_OBJECT *device_object,
                                      IRP *irp, void *context);
static NTSTATUS DDKAPI 
on_query_capabilities_complete(DEVICE_OBJECT *device_object,
                               IRP *irp, void *context);

NTSTATUS dispatch_pnp(libusb_device_t *dev, IRP *irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	IO_STACK_LOCATION *stack_location = IoGetCurrentIrpStackLocation(irp);
	UNICODE_STRING symbolic_link_name;
	WCHAR tmp_name[128];
	
	status = remove_lock_acquire(dev);
	
	if (!NT_SUCCESS(status))
	{ 
		return complete_irp(irp, status, 0);
	}
	
	DEBUG_PRINT_NL();
	
	switch (stack_location->MinorFunction) 
	{
	case IRP_MN_QUERY_DEVICE_RELATIONS:
		break;
	case IRP_MN_REMOVE_DEVICE:
		
		DEBUG_MESSAGE("dispatch_pnp(): IRP_MN_REMOVE_DEVICE");
		
		dev->is_started = FALSE;
		
		/* wait until all outstanding requests are finished */
		remove_lock_release_and_wait(dev);
		
		status = pass_irp_down(dev, irp, NULL, NULL); 
		
		DEBUG_MESSAGE("dispatch_pnp(): deleting device #%d", dev->id);
		
		_snwprintf(tmp_name, sizeof(tmp_name)/sizeof(WCHAR), L"%s%04d", 
			LIBUSB_SYMBOLIC_LINK_NAME, dev->id);
		
		/* delete the symbolic link */
		RtlInitUnicodeString(&symbolic_link_name, tmp_name);
		IoDeleteSymbolicLink(&symbolic_link_name);
		
		/* delete the device object */
		IoDetachDevice(dev->next_stack_device);
		IoDeleteDevice(dev->self);
		return status;
		
	case IRP_MN_SURPRISE_REMOVAL:
		DEBUG_MESSAGE("dispatch_pnp(): IRP_MN_SURPRISE_REMOVAL");
		dev->is_started = FALSE;
		break;
		
	case IRP_MN_START_DEVICE:
		DEBUG_MESSAGE("dispatch_pnp(): IRP_MN_START_DEVICE");
		
		/*       if(!NT_SUCCESS(set_configuration(dev, 1, 1000))) */
		/*         { */
		/*           DEBUG_ERROR("dispatch_pnp(): IRP_MN_START_DEVICE: selecting " */
		/*                       "configuration failed"); */
		/*         } */
		
		/* report device state to Power Manager */
		/* power_state.DeviceState has been set to D0 by add_device() */
		PoSetPowerState(dev->self, DevicePowerState, dev->power_state);
		return pass_irp_down(dev, irp, on_start_complete, NULL);
		
	case IRP_MN_STOP_DEVICE:
		
		dev->is_started = FALSE;
		DEBUG_MESSAGE("dispatch_pnp(): IRP_MN_STOP_DEVICE");
		break;
		
	case IRP_MN_DEVICE_USAGE_NOTIFICATION:
		DEBUG_MESSAGE("dispatch_pnp(): IRP_MN_DEVICE_USAGE_NOTIFICATION");
		if(!dev->self->AttachedDevice
			|| (dev->self->AttachedDevice->Flags & DO_POWER_PAGABLE))
		{
			dev->self->Flags |= DO_POWER_PAGABLE;
		}
		return pass_irp_down(dev, irp,
				     on_device_usage_notification_complete,
				     NULL);
		
	case IRP_MN_QUERY_CAPABILITIES: 
		DEBUG_MESSAGE("dispatch_pnp(): IRP_MN_QUERY_CAPABILITIES");
		if (!dev->is_filter)
		{
			/* apply registry setting */
			stack_location->Parameters.DeviceCapabilities.Capabilities
				->SurpriseRemovalOK = dev->surprise_removal_ok;
		}
		return pass_irp_down(dev, irp, on_query_capabilities_complete,  NULL);
		
	default:
		;
	}
	
	remove_lock_release(dev);
	return pass_irp_down(dev, irp, NULL, NULL);
}

static NTSTATUS DDKAPI 
on_start_complete(DEVICE_OBJECT *device_object, IRP *irp, void *context)
{
	libusb_device_t *dev = device_object->DeviceExtension;
	
	if (irp->PendingReturned)
	{
		IoMarkIrpPending(irp);
	}
	
	if (dev->next_stack_device->Characteristics & FILE_REMOVABLE_MEDIA) 
	{
		device_object->Characteristics |= FILE_REMOVABLE_MEDIA;
	}
	
	dev->is_started = TRUE;
	
	remove_lock_release(dev);
	
	return STATUS_SUCCESS;
}

static NTSTATUS DDKAPI 
on_device_usage_notification_complete(DEVICE_OBJECT *device_object,
                                      IRP *irp, void *context)
{
	libusb_device_t *dev = device_object->DeviceExtension;
	
	if (irp->PendingReturned)
	{
		IoMarkIrpPending(irp);
	}
	
	if (!(dev->next_stack_device->Flags & DO_POWER_PAGABLE))
	{
		device_object->Flags &= ~DO_POWER_PAGABLE;
	}
	
	remove_lock_release(dev);
	
	return STATUS_SUCCESS;
}

static NTSTATUS DDKAPI 
on_query_capabilities_complete(DEVICE_OBJECT *device_object,
                               IRP *irp, void *context)
{
	libusb_device_t *dev = device_object->DeviceExtension;
	IO_STACK_LOCATION *stack_location = IoGetCurrentIrpStackLocation(irp);
	
	if (irp->PendingReturned)
	{
		IoMarkIrpPending(irp);
	}
	
	if (NT_SUCCESS(irp->IoStatus.Status))
	{
		if (!dev->is_filter)
		{
			/* apply registry setting */
			stack_location->Parameters.DeviceCapabilities.Capabilities
				      ->SurpriseRemovalOK = dev->surprise_removal_ok;
		}
		
		/* save supported device power states */
		memcpy(dev->device_power_states,
		       stack_location->Parameters.DeviceCapabilities.Capabilities
				     ->DeviceState,
		       sizeof(dev->device_power_states));
	}
	
	remove_lock_release(dev);
	
	return STATUS_SUCCESS;
}

NTSTATUS dispatch_power(libusb_device_t *dev, IRP *irp)
{
	IO_STACK_LOCATION *stack_location = IoGetCurrentIrpStackLocation(irp);
	POWER_STATE power_state;
	NTSTATUS status;
	
	status = remove_lock_acquire(dev);;
	
	if (!NT_SUCCESS(status)) 
	{
		irp->IoStatus.Status = status;
		PoStartNextPowerIrp(irp);
		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return status;
	}
	
	if (stack_location->MinorFunction == IRP_MN_SET_POWER) 
	{     
		power_state = stack_location->Parameters.Power.State;
		
		if (stack_location->Parameters.Power.Type == SystemPowerState)
		{
			DEBUG_MESSAGE("dispatch_power(): IRP_MN_SET_POWER: S%d",
				      power_state.SystemState - PowerSystemWorking);
		}
		else
		{
			DEBUG_MESSAGE("dispatch_power(): IRP_MN_SET_POWER: D%d", 
				      power_state.DeviceState - PowerDeviceD0);
			
			if (power_state.DeviceState > dev->power_state.DeviceState)
			{
				/* device is powered down, report device state to the */
				/* Power Manager before sending the IRP down */
				/* (power up is handled by the completion routine) */
				PoSetPowerState(dev->self, DevicePowerState, power_state);
			}
		}
		
		/* TODO: should PoStartNextPowerIrp() be called here or from the */
		/* completion routine? */
		PoStartNextPowerIrp(irp); 
		
		IoCopyCurrentIrpStackLocationToNext(irp);
		IoSetCompletionRoutine(irp,
				       on_power_state_complete,
				       dev,
				       TRUE, /* on success */
				       TRUE, /* on error   */
				       TRUE);/* on cancel  */
		return PoCallDriver(dev->next_stack_device, irp);
	}
	else
	{  
		/* pass all other power IRPs down without setting a completion routine */
		PoStartNextPowerIrp(irp);
		IoSkipCurrentIrpStackLocation(irp);
		status = PoCallDriver(dev->next_stack_device, irp);
		remove_lock_release(dev);
		
		return status;
	}
}


static NTSTATUS DDKAPI
on_power_state_complete(DEVICE_OBJECT *device_object,
                        IRP           *irp,
                        void          *context)
{
	libusb_device_t *dev = context;
	IO_STACK_LOCATION *stack_location = IoGetCurrentIrpStackLocation(irp);
	POWER_STATE power_state = stack_location->Parameters.Power.State;
	DEVICE_POWER_STATE dev_power_state;
	
	if (irp->PendingReturned)
	{
		IoMarkIrpPending(irp);
	}
	
	if (NT_SUCCESS(irp->IoStatus.Status))
	{
		if (stack_location->Parameters.Power.Type == SystemPowerState)
		{
			DEBUG_MESSAGE("on_power_state_complete(): S%d",
				power_state.SystemState - PowerSystemWorking);
			
			/* save current system state */
			dev->power_state.SystemState = power_state.SystemState;
			
			/* set device power status correctly */
			/* dev_power_state = power_state.SystemState == PowerSystemWorking ? */
			/* PowerDeviceD0 : PowerDeviceD3; */
			
			/* get supported device power state from the array reported by */
			/* IRP_MN_QUERY_CAPABILITIES */
			dev_power_state = dev->device_power_states[power_state.SystemState];
			
			/* set the device power state, but don't block the thread */
			power_set_device_state(dev, dev_power_state, FALSE);
		}
		else /* DevicePowerState */
		{
			DEBUG_MESSAGE("on_power_state_complete(): D%d", 
				      power_state.DeviceState - PowerDeviceD0);
			
			if (power_state.DeviceState <= dev->power_state.DeviceState)
			{
				/* device is powered up, */
				/* report device state to Power Manager */
				PoSetPowerState(dev->self, DevicePowerState, power_state);
			}
			
			/* save current device state */
			dev->power_state.DeviceState = power_state.DeviceState;
		}
	}
	else
	{
		DEBUG_MESSAGE("on_power_state_complete(): failed");
	}
	
	remove_lock_release(dev);
	
	return STATUS_SUCCESS;
}


static void DDKAPI 
on_power_set_device_state_complete(DEVICE_OBJECT *device_object,
                                   UCHAR minor_function,
                                   POWER_STATE power_state,
                                   void *context,
                                   IO_STATUS_BLOCK *io_status)
{
	KeSetEvent((KEVENT *)context, EVENT_INCREMENT, FALSE);
}


void power_set_device_state(libusb_device_t *dev, 
                            DEVICE_POWER_STATE device_state, bool_t block)
{
	NTSTATUS status;
	KEVENT event;
	POWER_STATE power_state;
	
	power_state.DeviceState = device_state;
	
	if (block) /* wait for IRP to complete */
	{
		KeInitializeEvent(&event, NotificationEvent, FALSE);
		
		/* set the device power state and wait for completion */
		status = PoRequestPowerIrp(dev->physical_device_object, 
					   IRP_MN_SET_POWER, 
					   power_state,
					   on_power_set_device_state_complete, 
					   &event, NULL);
		
		if (status == STATUS_PENDING)
		{
			KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
		}
	}
	else
	{
		PoRequestPowerIrp(dev->physical_device_object, IRP_MN_SET_POWER,
				  power_state, NULL, NULL, NULL);
	}
}

NTSTATUS dispatch_ioctl(libusb_device_t *dev, IRP *irp)
{
	int ret = 0;
	NTSTATUS status = STATUS_SUCCESS;
	
	IO_STACK_LOCATION *stack_location = IoGetCurrentIrpStackLocation(irp);
	ULONG control_code =
		stack_location->Parameters.DeviceIoControl.IoControlCode;
	
	ULONG input_buffer_length =
		stack_location->Parameters.DeviceIoControl.InputBufferLength;
	ULONG output_buffer_length =
		stack_location->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG transfer_buffer_length =
		stack_location->Parameters.DeviceIoControl.OutputBufferLength;
	
	libusb_request *request = (libusb_request *)irp->AssociatedIrp.SystemBuffer;
	char *output_buffer = (char *)irp->AssociatedIrp.SystemBuffer;
	char *input_buffer = (char *)irp->AssociatedIrp.SystemBuffer;
	MDL *transfer_buffer_mdl = irp->MdlAddress;
	
	status = remove_lock_acquire(dev);
	
	if (!NT_SUCCESS(status))
	{ 
		status = complete_irp(irp, status, 0);
		remove_lock_release(dev);
		return status;
	}
	
	if (!request || input_buffer_length < sizeof(libusb_request) ||
	    input_buffer_length > LIBUSB_MAX_READ_WRITE ||
	    output_buffer_length > LIBUSB_MAX_READ_WRITE ||
	    transfer_buffer_length > LIBUSB_MAX_READ_WRITE)
	{ 
		DEBUG_ERROR("dispatch_ioctl(): invalid input or output buffer\n");
		
		status = complete_irp(irp, STATUS_INVALID_PARAMETER, 0);
		remove_lock_release(dev);
		return status;
	}
	
	DEBUG_PRINT_NL();
	
	switch (control_code) 
	{     
	case LIBUSB_IOCTL_SET_CONFIGURATION:
		status = set_configuration(dev,
					   request->configuration.configuration,
					   request->timeout);
		break;
		
	case LIBUSB_IOCTL_GET_CONFIGURATION:
		if (!output_buffer || output_buffer_length < 1)
		{
			DEBUG_ERROR("dispatch_ioctl(), get_configuration: invalid output "
				    "buffer");
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		status = get_configuration(dev, output_buffer, &ret, request->timeout);
		break;
		
	case LIBUSB_IOCTL_SET_INTERFACE:
		status = set_interface(dev,
				       request->interface.interface,
				       request->interface.altsetting,
				       request->timeout);
		break;
		
	case LIBUSB_IOCTL_GET_INTERFACE:
		if (!output_buffer || output_buffer_length < 1)
		{
			DEBUG_ERROR("dispatch_ioctl(), get_interface: invalid output "
				    "buffer");
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		status = get_interface(dev, request->interface.interface,
				       output_buffer, &ret, request->timeout);
		break;
		
	case LIBUSB_IOCTL_SET_FEATURE:
		status = set_feature(dev, request->feature.recipient,
				     request->feature.index,
				     request->feature.feature,
				     request->timeout);
		
		break;
		
	case LIBUSB_IOCTL_CLEAR_FEATURE:
		status = clear_feature(dev, request->feature.recipient,
				       request->feature.index,
				       request->feature.feature,
				       request->timeout);
		
		break;
		
	case LIBUSB_IOCTL_GET_STATUS:
		if (!output_buffer || output_buffer_length < 2)
		{
			DEBUG_ERROR("dispatch_ioctl(), get_status: invalid output buffer");
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		status = get_status(dev, request->status.recipient,
				    request->status.index, output_buffer,
				    &ret, request->timeout);
		break;
		
	case LIBUSB_IOCTL_SET_DESCRIPTOR:
		if (input_buffer_length <= sizeof(libusb_request))
		{
			DEBUG_ERROR("dispatch_ioctl(), set_descriptor: invalid input "
				"buffer");
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		status = set_descriptor(dev, 
					input_buffer + sizeof(libusb_request), 
					input_buffer_length - sizeof(libusb_request), 
					request->descriptor.type,
					request->descriptor.recipient,
					request->descriptor.index,
					request->descriptor.language_id, 
					&ret, request->timeout);
		break;
		
	case LIBUSB_IOCTL_GET_DESCRIPTOR:
		
		if (!output_buffer || !output_buffer_length)
		{
			DEBUG_ERROR("dispatch_ioctl(), get_descriptor: invalid output "
				    "buffer");
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		status = get_descriptor(dev, output_buffer, 
					output_buffer_length,
					request->descriptor.type,
					request->descriptor.recipient,
					request->descriptor.index,
					request->descriptor.language_id, 
					&ret, request->timeout);
		break;
		
	case LIBUSB_IOCTL_INTERRUPT_OR_BULK_READ:
		if (!transfer_buffer_mdl)
		{
			DEBUG_ERROR("dispatch_ioctl(), bulk_int_read: invalid transfer "
				    "buffer");
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		return transfer(dev, irp,
				USBD_TRANSFER_DIRECTION_IN,
				URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER,
				request->endpoint.endpoint,
				request->endpoint.packet_size,
				transfer_buffer_mdl, 
				transfer_buffer_length);
		
	case LIBUSB_IOCTL_INTERRUPT_OR_BULK_WRITE:
		/* we don't check 'transfer_buffer_mdl' here because it might be NULL */
		/* if the DLL requests to send a zero-length packet */
		return transfer(dev, irp,
				USBD_TRANSFER_DIRECTION_OUT,
				URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER,
				request->endpoint.endpoint,
				request->endpoint.packet_size,
				transfer_buffer_mdl, 
				transfer_buffer_length);
		
	case LIBUSB_IOCTL_VENDOR_READ:
		
		if(output_buffer_length && !output_buffer)
		{
			DEBUG_ERROR("dispatch_ioctl(), vendor_read: invalid output buffer");
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		status = vendor_class_request(dev,
			request->vendor.type, 
			request->vendor.recipient,
			request->vendor.request,
			request->vendor.value,
			request->vendor.index,
			output_buffer,
			output_buffer_length,
			USBD_TRANSFER_DIRECTION_IN,
			&ret, request->timeout);
		break;
		
	case LIBUSB_IOCTL_VENDOR_WRITE:
		status = vendor_class_request(dev,
					      request->vendor.type, 
					      request->vendor.recipient,
					      request->vendor.request,
					      request->vendor.value,
					      request->vendor.index,
					      input_buffer_length == sizeof(libusb_request) ? NULL : input_buffer + sizeof(libusb_request),
					      input_buffer_length - sizeof(libusb_request),
					      USBD_TRANSFER_DIRECTION_OUT, 
					      &ret, request->timeout);
		break;
		
	case LIBUSB_IOCTL_RESET_ENDPOINT:
		status = reset_endpoint(dev, request->endpoint.endpoint,
					request->timeout);
		break;
		
	case LIBUSB_IOCTL_ABORT_ENDPOINT:
		status = abort_endpoint(dev, request->endpoint.endpoint,
					request->timeout);
		break;
		
	case LIBUSB_IOCTL_RESET_DEVICE: 
		status = reset_device(dev, request->timeout);
		break;
		
	case LIBUSB_IOCTL_SET_DEBUG_LEVEL:
		DEBUG_SET_LEVEL(request->debug.level);
		break;
		
	case LIBUSB_IOCTL_GET_VERSION:
		if (!request || output_buffer_length < sizeof(libusb_request))
		{
			DEBUG_ERROR("dispatch_ioctl(), get_version: invalid output buffer");
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		request->version.major = VERSION_MAJOR;
		request->version.minor = VERSION_MINOR;
		request->version.micro = VERSION_MICRO;
		request->version.nano  = VERSION_NANO;
		
		ret = sizeof(libusb_request);
		break;
		
	case LIBUSB_IOCTL_CLAIM_INTERFACE:
		status = claim_interface(dev, request->interface.interface);
		break;
		
	case LIBUSB_IOCTL_RELEASE_INTERFACE:
		status = release_interface(dev, request->interface.interface);
		break;
		
	case LIBUSB_IOCTL_ISOCHRONOUS_READ:
		if (!transfer_buffer_mdl)
		{
			DEBUG_ERROR("dispatch_ioctl(), isochronous_read: invalid transfer "
				    "buffer");
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		return transfer(dev, irp, USBD_TRANSFER_DIRECTION_IN,
				URB_FUNCTION_ISOCH_TRANSFER,
				request->endpoint.endpoint,
				request->endpoint.packet_size,
				transfer_buffer_mdl, 
				transfer_buffer_length);
		
	case LIBUSB_IOCTL_ISOCHRONOUS_WRITE:
		if (!transfer_buffer_mdl)
		{
			DEBUG_ERROR("dispatch_ioctl(), isochronous_write: invalid transfer "
				    "buffer");
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		
		return transfer(dev, irp, USBD_TRANSFER_DIRECTION_OUT,
				URB_FUNCTION_ISOCH_TRANSFER,
				request->endpoint.endpoint,
				request->endpoint.packet_size,
				transfer_buffer_mdl, 
				transfer_buffer_length);
	default:
		
		status = STATUS_INVALID_PARAMETER;
	}
	
	status = complete_irp(irp, status, ret);  
	remove_lock_release(dev);
	
	return status;
}

NTSTATUS DDKAPI dispatch(DEVICE_OBJECT *device_object, IRP *irp)
{
	libusb_device_t *dev = device_object->DeviceExtension;
	
	switch (IoGetCurrentIrpStackLocation(irp)->MajorFunction) 
	{
	case IRP_MJ_PNP:
		return dispatch_pnp(dev, irp);
		
	case IRP_MJ_POWER:
		return dispatch_power(dev, irp);
	}
	
	/* since this driver may run as an upper filter we have to check whether */
	/* the IRP is sent to this device object or to the lower one */
	if (accept_irp(dev, irp))
	{
		switch (IoGetCurrentIrpStackLocation(irp)->MajorFunction) 
		{
		case IRP_MJ_DEVICE_CONTROL:
			if (dev->is_started)
			{
				return dispatch_ioctl(dev, irp);
			}
			else /* not started yet */
			{
				return complete_irp(irp, STATUS_INVALID_DEVICE_STATE, 0);
			}
		case IRP_MJ_CREATE:
			if (dev->is_started)
			{
				if (InterlockedIncrement(&dev->ref_count) == 1)
				{
					if(dev->power_state.DeviceState != PowerDeviceD0)
					{
						/* power up the device, block until the call */
						/* completes */
						power_set_device_state(dev, PowerDeviceD0, TRUE);
					}
				}
				
				return complete_irp(irp, STATUS_SUCCESS, 0);
			}
			else /* not started yet */
			{
				return complete_irp(irp, STATUS_INVALID_DEVICE_STATE, 0);
			}
		case IRP_MJ_CLOSE:
			if (!InterlockedDecrement(&dev->ref_count))
			{
				/* release all interfaces when the last handle is closed */
				release_all_interfaces(dev);
			}
			return complete_irp(irp, STATUS_SUCCESS, 0);
			
		case IRP_MJ_CLEANUP:
			return complete_irp(irp, STATUS_SUCCESS, 0);
			
		default:
			return complete_irp(irp, STATUS_NOT_SUPPORTED, 0);
		}
	}
	else /* the IRP is for the lower device object */
	{
		return pass_irp_down(dev, irp, NULL, NULL);
	}
}

NTSTATUS DDKAPI DriverEntry(DRIVER_OBJECT *driver_object,
                            UNICODE_STRING *registry_path)
{
	int i;
	
	DEBUG_MESSAGE("DriverEntry(): loading driver");
	
	/* initialize global variables */
	debug_level = LIBUSB_DEBUG_MSG;
	
	/* initialize the driver object's dispatch table */
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) 
	{
		driver_object->MajorFunction[i] = dispatch;
	}
	
	driver_object->DriverExtension->AddDevice = add_device;
	driver_object->DriverUnload = unload;
	
	return STATUS_SUCCESS;
}

NTSTATUS DDKAPI add_device(DRIVER_OBJECT *driver_object, 
                           DEVICE_OBJECT *physical_device_object)
{
	NTSTATUS status;
	DEVICE_OBJECT *device_object = NULL;
	libusb_device_t *dev;
	ULONG device_type;
	
	UNICODE_STRING nt_device_name;
	UNICODE_STRING symbolic_link_name;
	WCHAR tmp_name_0[128];
	WCHAR tmp_name_1[128];
	char id[256];
	int i;
	
	/* get the hardware ID from the registry */
	if (!reg_get_hardware_id(physical_device_object, id, sizeof(id)))
	{
		DEBUG_ERROR("add_device(): unable to read registry");
		return STATUS_SUCCESS;
	}
	
	/* only attach the (filter) driver to USB devices, skip hubs */
	/* and interfaces of composite devices */
	if (!strstr(id, "usb\\") || strstr(id, "hub") || strstr(id, "&mi_"))
	{
		return STATUS_SUCCESS;
	}
	
	/* retrieve the type of the lower device object */
	device_object = IoGetAttachedDeviceReference(physical_device_object);
	
	if (device_object)
	{
		device_type = device_object->DeviceType;
		ObDereferenceObject(device_object);
	}
	else
	{
		device_type = FILE_DEVICE_UNKNOWN;
	}
	
	/* try to create a new device object */
	for (i = 1; i < LIBUSB_MAX_NUMBER_OF_DEVICES; i++)
	{
		/* initialize some unicode strings */
		_snwprintf(tmp_name_0, sizeof(tmp_name_0)/sizeof(WCHAR),
			   L"%s%04d", LIBUSB_NT_DEVICE_NAME, i);
		_snwprintf(tmp_name_1, sizeof(tmp_name_1)/sizeof(WCHAR),
			   L"%s%04d", LIBUSB_SYMBOLIC_LINK_NAME, i);
		
		RtlInitUnicodeString(&nt_device_name, tmp_name_0);  
		RtlInitUnicodeString(&symbolic_link_name, tmp_name_1);
		
		/* create the object */
		status = IoCreateDevice(driver_object,
					sizeof(libusb_device_t), 
					&nt_device_name, device_type, 0,
					FALSE, &device_object);
		
		if (NT_SUCCESS(status))
		{
			DEBUG_MESSAGE("add_device(): device #%d created", i);
			break;
		}
		
		device_object = NULL;
		
		/* continue until an unused device name is found */
	}
	
	if (!device_object)
	{
		DEBUG_ERROR("add_device(): creating device failed");
		return status;
	}
	
	status = IoCreateSymbolicLink(&symbolic_link_name, &nt_device_name);
	
	if (!NT_SUCCESS(status))
	{
		DEBUG_ERROR("add_device(): creating symbolic link failed");
		IoDeleteDevice(device_object);
		return status;
	}
	
	/* setup the "device object" */
	dev = device_object->DeviceExtension;
	
	memset(dev, 0, sizeof(libusb_device_t));
	
	
	/* attach the newly created device object to the stack */
	dev->next_stack_device = 
		IoAttachDeviceToDeviceStack(device_object, physical_device_object);
	
	if (!dev->next_stack_device)
	{
		DEBUG_ERROR("add_device(): attaching to device stack failed");
		IoDeleteSymbolicLink(&symbolic_link_name);
		IoDeleteDevice(device_object);
		return STATUS_NO_SUCH_DEVICE;
	}
	
	dev->self = device_object;
	dev->physical_device_object = physical_device_object;
	dev->id = i;
	
	/* set initial power states */
	dev->power_state.DeviceState = PowerDeviceD0;
	dev->power_state.SystemState = PowerSystemWorking;
	
	/* get device properties from the registry */
	reg_get_properties(dev);
	
	if (dev->is_filter)
	{
		/* send all USB requests to the PDO in filter driver mode */
		dev->target_device = dev->physical_device_object;
		
		/* use the same flags as the underlying object */
		device_object->Flags |= dev->next_stack_device->Flags 
			& (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE);
	}
	else
	{
		/* send all USB requests to the lower object in device driver mode */
		dev->target_device = dev->next_stack_device;
		
		device_object->Flags |= DO_DIRECT_IO | DO_POWER_PAGABLE;
	}
	
	clear_pipe_info(dev);
	
	remove_lock_initialize(dev);
	
	device_object->Flags &= ~DO_DEVICE_INITIALIZING;
	
	return status;
}


VOID DDKAPI unload(DRIVER_OBJECT *driver_object)
{
	DEBUG_MESSAGE("unload(): unloading driver");
}

NTSTATUS complete_irp(IRP *irp, NTSTATUS status, ULONG info)
{
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = info;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	
	return status;
}

NTSTATUS call_usbd(libusb_device_t *dev, void *urb,
		   ULONG control_code, int timeout)
{
	KEVENT event;
	NTSTATUS status;
	IRP *irp;
	IO_STACK_LOCATION *next_irp_stack;
	LARGE_INTEGER _timeout;
	IO_STATUS_BLOCK io_status;
	
	if (timeout > LIBUSB_MAX_CONTROL_TRANSFER_TIMEOUT)
	{
		timeout = LIBUSB_MAX_CONTROL_TRANSFER_TIMEOUT;
	}
	
	KeInitializeEvent(&event, NotificationEvent, FALSE);
	
	irp = IoBuildDeviceIoControlRequest(control_code,
					    dev->target_device,
					    NULL, 0, NULL, 0, TRUE,
					    NULL, &io_status);
	
	if (!irp)
	{
		return STATUS_NO_MEMORY;
	}
	
	next_irp_stack = IoGetNextIrpStackLocation(irp);
	next_irp_stack->Parameters.Others.Argument1 = urb;
	next_irp_stack->Parameters.Others.Argument2 = NULL;
	
	IoSetCompletionRoutine(irp, on_usbd_complete, &event, TRUE, TRUE, TRUE); 
	
	status = IoCallDriver(dev->target_device, irp);
	
	if (status == STATUS_PENDING)
	{
		_timeout.QuadPart = -(timeout * 10000);
		
		if (KeWaitForSingleObject(&event, Executive, KernelMode,
					  FALSE, &_timeout) == STATUS_TIMEOUT)
		{
			DEBUG_ERROR("call_usbd(): request timed out");
			IoCancelIrp(irp);
		}
	}
	
	/* wait until completion routine is called */
	KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
	
	status = irp->IoStatus.Status;
	
	/* complete the request */
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	
	return status;
}


static NTSTATUS DDKAPI on_usbd_complete(DEVICE_OBJECT *device_object, 
                                        IRP *irp, void *context)
{
	KeSetEvent((KEVENT *) context, IO_NO_INCREMENT, FALSE);
	
	return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS pass_irp_down(libusb_device_t *dev, IRP *irp, 
                       PIO_COMPLETION_ROUTINE completion_routine, 
                       void *context)
{
	if (completion_routine)
	{
		IoCopyCurrentIrpStackLocationToNext(irp);
		IoSetCompletionRoutine(irp, completion_routine, context,
			TRUE, TRUE, TRUE);
	}
	else
	{
		IoSkipCurrentIrpStackLocation(irp);
	}
	
	return IoCallDriver(dev->next_stack_device, irp);
}

bool_t accept_irp(libusb_device_t *dev, IRP *irp)
{
	/* check if the IRP is sent to libusb's device object or to */
	/* the lower one. This check is neccassary since the device object */
	/* might be a filter */
	if (irp->Tail.Overlay.OriginalFileObject)
	{
		return irp->Tail.Overlay.OriginalFileObject->DeviceObject
			== dev->self ? TRUE : FALSE;
	}
	
	return FALSE;
}

bool_t get_pipe_handle(libusb_device_t *dev, int endpoint_address, 
                       USBD_PIPE_HANDLE *pipe_handle)
{
	int i, j;
	
	*pipe_handle = NULL;
	
	for (i = 0; i < LIBUSB_MAX_NUMBER_OF_INTERFACES; i++)
	{
		if (dev->config.interfaces[i].valid)
		{
			for (j = 0; j < LIBUSB_MAX_NUMBER_OF_ENDPOINTS; j++)
			{
				if (dev->config.interfaces[i].endpoints[j].address 
					== endpoint_address)
				{
					*pipe_handle = dev->config.interfaces[i].endpoints[j].handle;
					
					return !*pipe_handle ? FALSE : TRUE;
				}
			}
		}
	}
	
	return FALSE;
}

void clear_pipe_info(libusb_device_t *dev)
{
	memset(dev->config.interfaces, 0 , sizeof(dev->config.interfaces));
}

bool_t update_pipe_info(libusb_device_t *dev,
                        USBD_INTERFACE_INFORMATION *interface_info)
{
	int i;
	int number;
	
	if (!interface_info)
	{
		return FALSE;
	}
	
	number = interface_info->InterfaceNumber;
	
	if (interface_info->InterfaceNumber >= LIBUSB_MAX_NUMBER_OF_INTERFACES)
	{
		return FALSE;
	}
	
	DEBUG_MESSAGE("update_pipe_info(): interface %d", number);
	
	dev->config.interfaces[number].valid = TRUE;
	
	for (i = 0; i < LIBUSB_MAX_NUMBER_OF_ENDPOINTS; i++)
	{
		dev->config.interfaces[number].endpoints[i].address = 0;
		dev->config.interfaces[number].endpoints[i].handle = NULL;
	} 
	
	if (interface_info)
	{
		for (i = 0; i < (int)interface_info->NumberOfPipes &&
		     i < LIBUSB_MAX_NUMBER_OF_ENDPOINTS; i++) 
		{
			DEBUG_MESSAGE("update_pipe_info(): endpoint address 0x%02x",
				interface_info->Pipes[i].EndpointAddress);	  
			
			dev->config.interfaces[number].endpoints[i].handle
				= interface_info->Pipes[i].PipeHandle;	
			dev->config.interfaces[number].endpoints[i].address = 
				interface_info->Pipes[i].EndpointAddress;
		}
	}
	
	return TRUE;
}


void remove_lock_initialize(libusb_device_t *dev)
{
	KeInitializeEvent(&dev->remove_lock.event, NotificationEvent, FALSE);
	dev->remove_lock.usage_count = 1;
	dev->remove_lock.remove_pending = FALSE;
}


NTSTATUS remove_lock_acquire(libusb_device_t *dev)
{
	InterlockedIncrement(&dev->remove_lock.usage_count);
	
	if (dev->remove_lock.remove_pending)
	{
		if (InterlockedDecrement(&dev->remove_lock.usage_count) == 0)
		{
			KeSetEvent(&dev->remove_lock.event, 0, FALSE);
		}      
		return STATUS_DELETE_PENDING;
	}
	return STATUS_SUCCESS;
}


void remove_lock_release(libusb_device_t *dev)
{
	if (InterlockedDecrement(&dev->remove_lock.usage_count) == 0)
	{
		KeSetEvent(&dev->remove_lock.event, 0, FALSE);
	}
}


void remove_lock_release_and_wait(libusb_device_t *dev)
{
	dev->remove_lock.remove_pending = TRUE;
	remove_lock_release(dev);
	remove_lock_release(dev);
	KeWaitForSingleObject(&dev->remove_lock.event, Executive,
			      KernelMode, FALSE, NULL);
}


USB_INTERFACE_DESCRIPTOR *
find_interface_desc(USB_CONFIGURATION_DESCRIPTOR *config_desc,
                    unsigned int size, int interface_number, int altsetting)
{
	usb_descriptor_header_t *desc = (usb_descriptor_header_t *)config_desc;
	char *p = (char *)desc;
	USB_INTERFACE_DESCRIPTOR *if_desc = NULL;
	
	if (!config_desc || (size < config_desc->wTotalLength))
		return NULL;
	
	while (size && desc->length <= size)
	{
		if (desc->type == USB_INTERFACE_DESCRIPTOR_TYPE)
		{
			if_desc = (USB_INTERFACE_DESCRIPTOR *)desc;
			
			if ((if_desc->bInterfaceNumber == (UCHAR)interface_number) &&
			    (if_desc->bAlternateSetting == (UCHAR)altsetting))
			{
				return if_desc;
			}
		}
		
		size -= desc->length;
		p += desc->length;
		desc = (usb_descriptor_header_t *)p;
	}
	
	return NULL;
}

