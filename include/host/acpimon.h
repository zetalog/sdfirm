/*
 * ZETALOG's Personal COPYRIGHT v2
 *
 * Copyright (c) 2013
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 2. Permission of reuse of souce code only granted to ZETALOG and the
 *    developer(s) in the companies ZETALOG worked and has redistributed this
 *    software to.  Permission of redistribution of source code only granted
 *    to ZETALOG.
 * 3. Permission of redistribution and/or reuse of binary fully granted
 *    to ZETALOG and the companies ZETALOG worked and has redistributed this
 *    software to.
 * 4. Any modification of this software in the redistributed companies need
 *    not be published to ZETALOG.
 * 5. All source code modifications linked/included this software and modified
 *    by ZETALOG are of ZETALOG's personal COPYRIGHT unless the above COPYRIGHT
 *    is no long disclaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)acpimon.h: ACPI monitor driver interfaces
 * $Id: acpimon.h,v 1.315 2011-10-17 01:40:34 zhenglv Exp $
 */

#ifndef __ACPIMON_H_INCLUDE__
#define __ACPIMON_H_INCLUDE__

#include <host/types.h>

// {82CE19CD-955C-4ebf-8156-7D794EB04D1B}
DEFINE_GUID(GUID_DEVCLASS_MONITOR, 
0x82ce19cd, 0x955c, 0x4ebf, 0x81, 0x56, 0x7d, 0x79, 0x4e, 0xb0, 0x4d, 0x1b);
// {0AA92844-15CC-4bf9-A109-49F1979FC266}
DEFINE_GUID(MONITOR_WMI_GUID, 
0xaa92844, 0x15cc, 0x4bf9, 0xa1, 0x9, 0x49, 0xf1, 0x97, 0x9f, 0xc2, 0x66);

#define ACPIDUMP_MAP		0
#define ACPIDUMP_UNMAP		1
#define ACPISCOPE_START		2
#define ACPISCOPE_STOP		3

#define ACPI_MON_IOCTL_INDEX	0x0021
#define ACPIDUMPMAP		(ACPI_MON_IOCTL_INDEX+ACPIDUMP_MAP)
#define ACPIDUMPUNMAP		(ACPI_MON_IOCTL_INDEX+ACPIDUMP_UNMAP)
#define ACPISCOPESTART		(ACPI_MON_IOCTL_INDEX+ACPISCOPE_START)
#define ACPISCOPESTOP		(ACPI_MON_IOCTL_INDEX+ACPISCOPE_STOP)

#define ACPI_IOCTL_CODE(code, dir, method)			\
	IOCTL_CODE(code, dir, method)
#define IOCTL_ACPI_DUMP_MAP					\
	ACPI_IOCTL_CODE(ACPIDUMPMAP,				\
			IOCTL_INPUT, METHOD_BUFFERED)
#define IOCTL_ACPI_DUMP_UNMAP					\
	ACPI_IOCTL_CODE(ACPIDUMPUNMAP,				\
			IOCTL_INPUT, METHOD_BUFFERED)
#define IOCTL_ACPI_SCOPE_START					\
	ACPI_IOCTL_CODE(ACPISCOPESTART,				\
			IOCTL_INPUT, METHOD_BUFFERED)
#define IOCTL_ACPI_SCOPE_STOP					\
	ACPI_IOCTL_CODE(ACPISCOPESTOP,				\
			IOCTL_INPUT, METHOD_BUFFERED)

/* for IOCTL_ACPI_DUMP_MAP */
struct acpimon_mapping {
	uint64_t	offset;
	size_t		length;
};

#endif /* __ACPIMON_H_INCLUDE__ */
