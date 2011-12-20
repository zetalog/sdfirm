#include <wdm/usb_cmn.h>

void usb_convert_string(struct usb_string_desc *desc,
			char *string, int length)
{
	char i;
	uint8_t *buffer = (uint8_t *)desc;
	
	if (((desc->bLength-2)/2) < length-1)
		length = ((desc->bLength-2)/2);
	else
		length--;

	buffer += sizeof (struct usb_string_desc);
	for (i = 0; i < length; i++) {
		string[i] = buffer[i*2];
	}
	string[i] = '\0';
}
