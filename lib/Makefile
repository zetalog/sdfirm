# Copyright (C) 2008 by Lv Zheng <lv.zheng@soliton.com.cn>
#
# Licensed under the GPL v2, see the file LICENSE in this tarball.

obj-y:=

obj-y += bitops.rel
obj-$(CONFIG_ARCH_IS_BOOT_EXE) += muldiv.rel
obj-$(CONFIG_ARCH_IS_BOOT_EXE) += string.rel
obj-$(CONFIG_ARCH_IS_BOOT_EXE) += errno.rel
obj-$(CONFIG_ARCH_IS_BOOT_EXE) += time.rel
obj-$(CONFIG_PRINT_VSNPRINTF) += vsnprintf.rel
obj-$(CONFIG_CONSOLE_OUTPUT) += printf.rel
obj-$(CONFIG_STRING_HEXDUMP) += hexdump.rel
obj-$(CONFIG_CRC16_CCITT) += crc16_ccitt.rel
obj-$(CONFIG_CRC32) += crc32_table.rel
obj-$(CONFIG_FONTS) += font/
obj-$(CONFIG_STRING_UUID) += uuid.rel
obj-$(CONFIG_STRING_BYTELIST) += byte_list.rel
obj-$(CONFIG_STRING_GETOPT) += getopt.rel
obj-$(CONFIG_CRYPTO_RAND) += rand.rel

#CFLAGS_tmath.o += -fno-builtin
obj-$(CONFIG_MATH_TAYLOR) += tmath.rel
