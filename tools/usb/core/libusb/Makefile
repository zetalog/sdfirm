
hostprogs-y := usb_sample
always := $(hostprogs-y)

usb_sample-objs := usb_sample.o libusb.so
libusb-objs := usb_core.o usb_io.o usb_sync.o usb_desc.o usb_linux.o

HOSTCFLAGS += -D_GNU_SOURCE -DLINUX
HOSTLDFLAGS += -lrt
