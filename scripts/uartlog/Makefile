hostprogs-y	:= uartdbg
always		:= $(hostprogs-y)

uartdbg-objs	:= tuartdbg.o  uartdbg.o

HOSTCFLAGS	+= -fms-extensions -D_CONSOLE
HOSTLDFLAGS	+= -lnsl -lm -lc -lusb

