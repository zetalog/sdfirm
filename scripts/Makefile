###
# scripts contains sources for various helper programs used throughout
# the kernel for the build process.
# ---------------------------------------------------------------------------

always		:= $(hostprogs-y)

subdir-$(CONFIG_DEBUG_PRINT) += uartlog

# Let clean descend into subdirs
subdir-	+= basic kconfig cmn libusb usbtest
