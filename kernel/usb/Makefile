# Copyright (C) 2008 by Lv Zheng <lv.zheng@soliton.com.cn>
#
# Licensed under the GPL v2, see the file LICENSE in this tarball.

obj-y:=

obj-$(CONFIG_USB) += usb.rel
obj-$(CONFIG_USB_DEV) += usb_dev.rel
obj-$(CONFIG_USB_HCD) += usb_hcd.rel
obj-$(CONFIG_USB_OTG) += usb_otg.rel

# Standard interfaces
obj-$(CONFIG_USB_HID) += hid/
obj-$(CONFIG_USB_MSD) += msd/
obj-$(CONFIG_USB_SCD) += scd/
obj-$(CONFIG_USB_DFU) += usb_dfu.rel

# Vendor specific interfaces.
obj-$(CONFIG_USB_PN53X) += usb_pn53x.rel
