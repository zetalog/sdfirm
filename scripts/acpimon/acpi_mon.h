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
 * @(#)acpi_mon.h: ACPI monitor driver declarations
 * $Id: acpi_mon.h,v 1.315 2011-10-17 01:40:34 zhenglv Exp $
 */

#ifndef __ACPI_MON_H_INCLUDE__
#define __ACPI_MON_H_INCLUDE__

#include "os_cfg.h"
#include <wdm/os_dev.h>
#include <host/acpimon.h>

struct acpimon_dev_struct {
	os_device dev;

	int stopping;
};
typedef struct acpimon_dev_struct acpimon_dev;

struct acpimon_file_struct {
	os_file *file;
	int state;
#define ACPI_IO_IDLE		0x00
#define ACPI_IO_DUMP		0x01
#define ACPI_IO_SCOPE		0x02

	void *mapped_addr;
	size_t mapped_size;
	loff_t map_rpos;

	void *priv_data;
};
typedef struct acpimon_file_struct acpimon_file;

#endif /* __ACPI_MON_H_INCLUDE__ */
