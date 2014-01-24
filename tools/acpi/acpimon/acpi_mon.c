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
 * @(#)acpi_mon.c: ACPI monitor driver definitions
 * $Id: acpi_mon.c,v 1.315 2011-10-17 01:40:34 zhenglv Exp $
 */

#include "acpi_mon.h"

os_device acpimon_device(acpimon_dev *acpi)
{
	return acpi->dev;
}

status_t acpimon_init_device(os_device dev)
{
	acpimon_dev *adev = dev_priv(dev);

	return STATUS_SUCCESS;
}

void acpimon_exit_device(os_device dev)
{
	acpimon_dev *adev = dev_priv(dev);
}

void __acpimon_stop_device(acpimon_dev *adev)
{
	adev->stopping = 1;
}

void __acpimon_start_device(acpimon_dev *adev)
{
	adev->stopping = 0;
}

status_t acpimon_start_device(os_device dev)
{
	acpimon_dev *adev = dev_priv(dev);

	__acpimon_start_device(adev);
	return STATUS_SUCCESS;
}

void acpimon_stop_device(os_device dev)
{
	acpimon_dev *adev = dev_priv(dev);

	__acpimon_stop_device(adev);
}

status_t acpimon_open_file(os_device dev, os_file *file)
{
	acpimon_dev *adev = dev_priv(dev);
	acpimon_file *afile = NULL;

	afile = os_mem_alloc(sizeof (acpimon_file), "ACPI_FILE");
	if (!afile) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	afile->file = file;
	afile->state = ACPI_IO_IDLE;
	file->priv = afile;

	return STATUS_SUCCESS;
}

void acpimon_close_file(os_device dev, os_file *file)
{
	acpimon_dev *adev = dev_priv(dev);
	acpimon_file *afile = file_priv(file);

	if (afile) {
		ASSERT(file == afile->file);
		file->priv = NULL;
		os_mem_free(afile);
	}
}

status_t acpimon_ioctl_file(os_device dev, os_file *file,
			    unsigned long code, void *arg,
			    int *plength)
{
	status_t status = STATUS_SUCCESS;
	acpimon_dev *adev = dev_priv(dev);
	acpimon_file *afile = file_priv(file);
	int length = *plength;
	struct acpimon_mapping map;
	PHYSICAL_ADDRESS phys;
	void *mapped_addr;

	switch (code) {
	case ACPIDUMPMAP:
		map = (*(struct acpimon_mapping *)arg);

		if (afile->state != ACPI_IO_IDLE)
			return STATUS_INVALID_DEVICE_STATE;

		phys.QuadPart = map.offset;
                mapped_addr = MmMapIoSpace(phys, map.length, MmNonCached);
		if (!mapped_addr)
			return STATUS_INVALID_PARAMETER;

		afile->mapped_addr = mapped_addr;
		afile->mapped_size = map.length;
		afile->map_rpos = 0;
		afile->state = ACPI_IO_DUMP;
		break;
	case ACPIDUMPUNMAP:
		if (afile->state != ACPI_IO_DUMP)
			return STATUS_INVALID_DEVICE_STATE;

		MmUnmapIoSpace(afile->mapped_addr, afile->mapped_size);
		afile->state = ACPI_IO_IDLE;
		break;
	case ACPISCOPESTART:
	case ACPISCOPESTOP:
	default:
		*plength = 0;
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	return status;
}

status_t acpimon_read_file(os_device dev, os_file *file,
			   char *buf, loff_t offset, size_t *count)
{
	acpimon_dev *adev = dev_priv(dev);
	acpimon_file *afile = file_priv(file);
	size_t bytes = 0;

	if (afile->state = ACPI_IO_DUMP) {
		bytes = min(*count, afile->mapped_size - afile->map_rpos);
		os_mem_copy(buf, (uint8_t *)afile->mapped_addr + afile->map_rpos, bytes);
		afile->map_rpos += bytes;
		*count = bytes;

		return STATUS_SUCCESS;
	}

	return STATUS_INVALID_DEVICE_STATE;
}

status_t acpimon_write_file(os_device dev, os_file *file,
			    const char *buf, loff_t offset, size_t *count)
{
	acpimon_dev *adev = dev_priv(dev);

	return STATUS_INVALID_DEVICE_STATE;
}

void acpimon_suspend_device(os_device dev)
{
	acpimon_dev *adev = dev_priv(dev);

	os_dbg(OS_DBG_DEBUG, "ACPIMON Suspending...\n");
	__acpimon_stop_device(adev);
}

void acpimon_resume_device(os_device dev)
{
	acpimon_dev *adev = dev_priv(dev);

	os_dbg(OS_DBG_DEBUG, "ACPIMON Resuming...\n");
	__acpimon_start_device(adev);
}

struct os_driver wdm_acpimon_driver = {
	acpimon_start_device,	/* start device */
	acpimon_stop_device,	/* stop device */
	acpimon_init_device,	/* alloc memory */
	acpimon_exit_device,	/* free memory */
	acpimon_suspend_device,
	acpimon_resume_device,
	acpimon_open_file,
	acpimon_close_file,
	acpimon_ioctl_file,
	acpimon_read_file,
	acpimon_write_file,
};

status_t os_driver_init(void)
{
	unsigned long flags = OS_DEVICE_REMOVE | OS_DEVICE_HOTPLUG | OS_DEVICE_SUSPEND;

	return os_dev_register_driver(&wdm_acpimon_driver,
				      sizeof(acpimon_dev), flags);
}
