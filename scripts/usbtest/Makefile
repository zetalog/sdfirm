hostprogs-y	:= usbtest
always		:= $(hostprogs-y)

SCRIPTS_ROOT	:= ${TOPDIR}/scripts/

usbtest-objs	+= logger.o usb.o ccid.o
usbtest-objs	+= pcsc.o pcsc_acos5.o pcsc_tcos.o
usbtest-objs	+= ut_main.o ut_ccid.o

HOSTLDFLAGS	+= -lnsl -lm -lc
HOSTLDFLAGS	+= -lcmn -L$(SCRIPTS_ROOT)/cmn
HOSTLDFLAGS	+= -lusb -L$(SCRIPTS_ROOT)/libusb
