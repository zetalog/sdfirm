#include "usbstub.h"
#include <stdio.h>
#include <stdarg.h>

int debug_level = LIBUSB_DEBUG_MSG;

void DEBUG_PRINT_NL()
{
#ifdef DBG
	if (debug_level >= LIBUSB_DEBUG_MSG) 
		DbgPrint(("\n"));
#endif
}

void DEBUG_SET_LEVEL(int level)
{
#ifdef DBG
	debug_level = level;
#endif
}

void DEBUG_MESSAGE(const char *format, ...)
{
#ifdef DBG
	
	char tmp[256];
	
	if (debug_level >= LIBUSB_DEBUG_MSG)
	{
		va_list args;
		va_start(args, format);
		_vsnprintf(tmp, sizeof(tmp) - 1, format, args);
		va_end(args);
		
		DbgPrint("LIBUSB-DRIVER - %s", tmp);
	}
#endif
}

void DEBUG_ERROR(const char *format, ...)
{
#ifdef DBG
	
	char tmp[256];
	
	if (debug_level >= LIBUSB_DEBUG_ERR)
	{
		va_list args;
		va_start(args, format);
		_vsnprintf(tmp, sizeof(tmp) - 1, format, args);
		va_end(args);
		
		DbgPrint("LIBUSB-DRIVER - %s", tmp);
	}
#endif
}

/* missing in mingw's ddk headers */
#ifndef PLUGPLAY_REGKEY_DEVICE
#define PLUGPLAY_REGKEY_DEVICE  1
#endif

#define LIBUSB_REG_SURPRISE_REMOVAL_OK L"SurpriseRemovalOK"

static bool_t reg_get_property(DEVICE_OBJECT *physical_device_object, 
                               int property, char *data, int size);

static bool_t reg_get_property(DEVICE_OBJECT *physical_device_object,
                               int property, char *data, int size)
{
	WCHAR tmp[512];
	ULONG ret;
	ULONG i;
	
	if (!physical_device_object || !data || !size)
	{
		return FALSE;
	}
	
	memset(data, 0, size);
	memset(tmp, 0, sizeof(tmp));
	
	if (NT_SUCCESS(IoGetDeviceProperty(physical_device_object,
					   property, sizeof(tmp) - 2,
					   tmp, &ret)) && ret)
	{
		/* convert unicode string to normal character string */
		for (i = 0; (i < ret/2) && (i < ((ULONG)size - 1)); i++)
		{
			data[i] = (char)tmp[i];
		}
		
		_strlwr(data);
		return TRUE;
	}
	return FALSE;
}


bool_t reg_get_properties(libusb_device_t *dev)
{
	HANDLE key = NULL;
	NTSTATUS status;
	UNICODE_STRING name;
	KEY_VALUE_FULL_INFORMATION *info;
	ULONG length;
	
	if (!dev->physical_device_object)
	{
		return FALSE;
	}
	
	/* default settings */
	dev->surprise_removal_ok = FALSE;
	dev->is_filter = TRUE;
	
	status = IoOpenDeviceRegistryKey(dev->physical_device_object,
					 PLUGPLAY_REGKEY_DEVICE,
					 STANDARD_RIGHTS_ALL,
					 &key);
	if (NT_SUCCESS(status)) 
	{
		RtlInitUnicodeString(&name, LIBUSB_REG_SURPRISE_REMOVAL_OK);
		
		length = sizeof(KEY_VALUE_FULL_INFORMATION) + name.MaximumLength +
			 sizeof(ULONG);
		
		info = ExAllocatePool(NonPagedPool, length);
		
		if (info) 
		{
			memset(info, 0, length);
			
			status = ZwQueryValueKey(key, &name, KeyValueFullInformation,
						 info, length, &length);
			
			if (NT_SUCCESS(status) && (info->Type == REG_DWORD))
			{
				ULONG val = *((ULONG *)(((char *)info) + info->DataOffset));
				
				dev->surprise_removal_ok = val ? TRUE : FALSE;
				dev->is_filter = FALSE;
			}
			
			ExFreePool(info);
		}
		
		ZwClose(key);
	}
	return TRUE;
}

bool_t reg_get_hardware_id(DEVICE_OBJECT *physical_device_object, 
                           char *data, int size)
{
	if (!physical_device_object || !data || !size)
	{
		return FALSE;
	}
	
	return reg_get_property(physical_device_object,
				DevicePropertyHardwareID,
				data, size);
}
