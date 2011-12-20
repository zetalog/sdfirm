#include <wdm/os_usb.h>

status_t __usb_get_config_desc(os_device dev,
			       struct usb_conf_desc **pdesc,
			       uint16_t index,
			       uint16_t length)
{
	struct usb_conf_desc *conf_desc = NULL;
	status_t status;
	
	if (length == 0)
		length = sizeof(struct usb_conf_desc);
	conf_desc = os_mem_alloc(length, "USB_CONFIGURATION_DESCRIPTOR");
	if (!conf_desc) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto end;
	}
	status = os_usb_submit_default_sync(dev,
					    USB_MAKECTRL(USB_DIR_IN,
							 USB_TYPE_STANDARD,
							 USB_RECP_DEVICE),
					    USB_REQ_GET_DESCRIPTOR,
					    MAKEWORD(index, USB_DT_CONFIG),
					    0,
					    length,
					    (uint8_t *)conf_desc);
	if (!OS_IS_SUCCESS(status)) goto end;
	*pdesc = conf_desc;
	conf_desc = NULL;
end:
	if (conf_desc) os_mem_free(conf_desc);
	return status;
}

status_t usb_get_config_desc(os_device dev,
			     struct usb_conf_desc **pdesc,
			     uint16_t index)
{
	struct usb_conf_desc *conf_desc = NULL;
	status_t status;
	uint16_t length;

	status = __usb_get_config_desc(dev, &conf_desc,
				       index, 0);
	if (!OS_IS_SUCCESS(status)) goto end;
	length = conf_desc->wTotalLength;
	os_mem_free(conf_desc);
	conf_desc = NULL;
	status = __usb_get_config_desc(dev, &conf_desc,
				       index, length);
	if (!OS_IS_SUCCESS(status)) goto end;
	*pdesc = conf_desc;
	conf_desc = NULL;
	os_dbg(OS_DBG_DEBUG, "USB_CONF_DESC - %d\n", length);
end:
	if (conf_desc) os_mem_free(conf_desc);
	return status;
}

status_t usb_get_device_desc(os_device dev,
			     struct usb_device_desc **pdesc)
{
	uint16_t length;
	os_usbif *usb = dev_priv(dev);

	length = sizeof(struct usb_device_desc);
	(*pdesc) = os_mem_alloc(length, "USB_DEVICE_DESCRIPTOR");
	if (!(*pdesc)) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	return os_usb_submit_default_sync(dev,
					  USB_MAKECTRL(USB_DIR_IN,
						       USB_TYPE_STANDARD,
						       USB_RECP_DEVICE),
					  USB_REQ_GET_DESCRIPTOR,
					  MAKEWORD(0, USB_DT_DEVICE),
					  0,
					  length,
					  (uint8_t *)*pdesc);
}

status_t __usb_get_string_desc(os_device dev,
			       struct usb_string_desc **pdesc,
			       uint8_t index, uint16_t length)
{
	struct usb_string_desc *string_desc = NULL;
	status_t status;
	
	if (length == 0)
		length = sizeof(struct usb_string_desc);
	string_desc = os_mem_alloc(length, "USB_STRING_DESCRIPTOR");
	if (!string_desc) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto end;
	}
	status = os_usb_submit_default_sync(dev,
					    USB_MAKECTRL(USB_DIR_IN,
							 USB_TYPE_STANDARD,
							 USB_RECP_DEVICE),
					    USB_REQ_GET_DESCRIPTOR,
					    MAKEWORD(index, USB_DT_STRING),
					    0,
					    length,
					    (uint8_t *)string_desc);
	if (!OS_IS_SUCCESS(status)) goto end;
	*pdesc = string_desc;
	string_desc = NULL;
end:
	if (string_desc) os_mem_free(string_desc);
	return status;
}

status_t usb_get_string_desc(os_device dev,
			     struct usb_string_desc **pdesc,
			     uint8_t index)
{
	struct usb_string_desc *string_desc = NULL;
	status_t status;
	uint16_t length;

	status = __usb_get_string_desc(dev, &string_desc,
				       index, 0);
	if (!OS_IS_SUCCESS(status)) goto end;
	length = string_desc->bLength;
	os_mem_free(string_desc);
	string_desc = NULL;
	status = __usb_get_string_desc(dev, &string_desc,
				       index, length);
	if (!OS_IS_SUCCESS(status)) goto end;
	*pdesc = string_desc;
	string_desc = NULL;
	os_dbg(OS_DBG_DEBUG, "USB_STRING_DESC - %d\n", length);
end:
	if (string_desc) os_mem_free(string_desc);
	return status;
}

status_t usb_get_configuration(os_device dev, uint8_t *conf)
{
	return os_usb_submit_default_sync(dev,
					  USB_MAKECTRL(USB_DIR_IN,
						       USB_TYPE_STANDARD,
						       USB_RECP_DEVICE),
					  USB_REQ_GET_CONFIGURATION,
					  0,
					  0,
					  1,
					  conf);
}

status_t usb_set_configuration(os_device dev, uint8_t conf)
{
	return os_usb_submit_default_sync(dev,
					  USB_MAKECTRL(USB_DIR_IN,
						       USB_TYPE_STANDARD,
						       USB_RECP_DEVICE),
					  USB_REQ_SET_CONFIGURATION,
					  conf,
					  0,
					  0,
					  NULL);
}

status_t usb_set_interface(os_device dev, uint8_t intf, uint16_t alter)
{
	return os_usb_submit_default_sync(dev,
					  USB_MAKECTRL(USB_DIR_IN,
						       USB_TYPE_STANDARD,
						       USB_RECP_DEVICE),
					  USB_REQ_SET_INTERFACE,
					  alter,
					  intf,
					  0,
					  NULL);
}

status_t usb_halt_endpoint(os_device dev, uint8_t eid)
{
	return os_usb_submit_default_sync(dev,
					  USB_MAKECTRL(USB_DIR_IN,
						       USB_TYPE_STANDARD,
						       USB_RECP_ENDPOINT),
					  USB_REQ_SET_FEATURE,
					  USB_ENDPOINT_HALT,
					  eid,
					  0,
					  NULL);
}

status_t usb_unhalt_endpoint(os_device dev, uint8_t eid)
{
	return os_usb_submit_default_sync(dev,
					  USB_MAKECTRL(USB_DIR_IN,
						       USB_TYPE_STANDARD,
						       USB_RECP_ENDPOINT),
					  USB_REQ_CLEAR_FEATURE,
					  USB_ENDPOINT_HALT,
					  eid,
					  0,
					  NULL);
}

struct usb_intf_desc *usb_match_interface_num(struct usb_conf_desc *conf_desc,
					      uint8_t intf_num,
					      uint16_t alt_num,
					      uint16_t *length)
{
	struct usb_desc *desc = (struct usb_desc *)conf_desc;
	uint8_t *p = (uint8_t *)desc;
	struct usb_intf_desc *intf_desc = NULL, *res = NULL;
	uint16_t size, res_len = 0;

	if (!conf_desc)
		return NULL;

	size = conf_desc->wTotalLength;
	while (size && desc->bLength <= size) {
		if (desc->bDescriptorType == USB_DT_INTERFACE) {
			if (res) break;
			intf_desc = (struct usb_intf_desc *)desc;
			if ((intf_desc->bInterfaceNumber == (UCHAR)intf_num) &&
			    (intf_desc->bAlternateSetting == (UCHAR)alt_num)) {
				res = intf_desc;
			}
		}
		size -= desc->bLength;
		p += desc->bLength;
		if (res)
			res_len += desc->bLength;
		desc = (struct usb_desc *)p;
	}

	if (res) {
		if (length)
			*length = res_len;
	}
	return res;
}

struct usb_intf_desc *usb_match_interface_cls(struct usb_conf_desc *conf_desc,
					      uint8_t devcls, uint8_t subcls, uint8_t proto,
					      uint16_t *length)
{
	struct usb_desc *desc = (struct usb_desc *)conf_desc;
	uint8_t *p = (uint8_t *)desc;
	struct usb_intf_desc *intf_desc = NULL, *res = NULL;
	uint16_t size, res_len = 0;

	if (!conf_desc)
		return NULL;

	size = conf_desc->wTotalLength;
	while (size && desc->bLength <= size) {
		if (desc->bDescriptorType == USB_DT_INTERFACE) {
			if (res) break;
			intf_desc = (struct usb_intf_desc *)desc;
			if (USB_MATCH_CLASS(devcls, intf_desc) &&
			    USB_MATCH_SUBCLASS(subcls, intf_desc) &&
			    USB_MATCH_PROTOCOL(proto, intf_desc)) {
				res = intf_desc;
			}
		}
		size -= desc->bLength;
		p += desc->bLength;
		if (res)
			res_len += desc->bLength;
		desc = (struct usb_desc *)p;
	}

	if (res) {
		if (length)
			*length = res_len;
	}
	return res;
}
