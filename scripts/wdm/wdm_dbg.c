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
 * @(#)wdm_dbg.c: WDM dependent driver debugging functions
 * $Id: wdm_dbg.c,v 1.315 2011-10-17 01:40:34 zhenglv Exp $
 */

#include <wdm/os_dev.h>

int os_dbg_level = 3;

#ifdef CONFIG_DEBUG
int os_dbg_get_level(void)
{
	return os_dbg_level;
}

void os_dbg_set_level(int level)
{
	os_dbg_level = level;
}

void os_dbg(int level, const char *format, ...)
{
	va_list args;

	if (level > os_dbg_level)
		return;
	va_start(args, format);
	vDbgPrintEx(DPFLTR_DEFAULT_ID, level, format, args);
	va_end(args);
}
#else
void os_dbg(int level, const char *format, ...)
{
}

void os_dbg_set_level(int level)
{
}

int os_dbg_get_level(void)
{
	return OS_DBG_DEBUG;
}
#endif

#define VALUE_STRING(mn)		\
	case mn:			\
		return #mn;

const char *wdm_dev_bool_string(ULONG val)
{
	if (val)
		return "TRUE";
	else
		return "FALSE";
}

const char *wdm_dev_pnp_string(uint8_t minor)
{
	switch (minor) {
	VALUE_STRING(IRP_MN_START_DEVICE)
	VALUE_STRING(IRP_MN_QUERY_REMOVE_DEVICE)
	VALUE_STRING(IRP_MN_REMOVE_DEVICE)
	VALUE_STRING(IRP_MN_CANCEL_REMOVE_DEVICE)
	VALUE_STRING(IRP_MN_STOP_DEVICE)
	VALUE_STRING(IRP_MN_QUERY_STOP_DEVICE)
	VALUE_STRING(IRP_MN_CANCEL_STOP_DEVICE)
	VALUE_STRING(IRP_MN_QUERY_DEVICE_RELATIONS)
	VALUE_STRING(IRP_MN_QUERY_INTERFACE)
	VALUE_STRING(IRP_MN_QUERY_CAPABILITIES)
	VALUE_STRING(IRP_MN_QUERY_RESOURCES)
	VALUE_STRING(IRP_MN_QUERY_RESOURCE_REQUIREMENTS)
	VALUE_STRING(IRP_MN_QUERY_DEVICE_TEXT)
	VALUE_STRING(IRP_MN_FILTER_RESOURCE_REQUIREMENTS)
	VALUE_STRING(IRP_MN_READ_CONFIG)
	VALUE_STRING(IRP_MN_WRITE_CONFIG)
	VALUE_STRING(IRP_MN_EJECT)
	VALUE_STRING(IRP_MN_SET_LOCK)
	VALUE_STRING(IRP_MN_QUERY_ID)
	VALUE_STRING(IRP_MN_QUERY_PNP_DEVICE_STATE)
	VALUE_STRING(IRP_MN_QUERY_BUS_INFORMATION)
	VALUE_STRING(IRP_MN_DEVICE_USAGE_NOTIFICATION)
	VALUE_STRING(IRP_MN_SURPRISE_REMOVAL)
	/* VALUE_STRING(IRP_MN_QUERY_LEGACY_BUS_INFORMATION) */
        default:
		return "IRP_MN_PNP_UNKNOWN";
	}
}

const char *wdm_dev_wmi_string(uint8_t minor)
{
	switch (minor) {
	VALUE_STRING(IRP_MN_QUERY_ALL_DATA)
	VALUE_STRING(IRP_MN_QUERY_SINGLE_INSTANCE)
	VALUE_STRING(IRP_MN_CHANGE_SINGLE_INSTANCE)
	VALUE_STRING(IRP_MN_CHANGE_SINGLE_ITEM)
	VALUE_STRING(IRP_MN_ENABLE_EVENTS)
	VALUE_STRING(IRP_MN_DISABLE_EVENTS)
	VALUE_STRING(IRP_MN_ENABLE_COLLECTION)
	VALUE_STRING(IRP_MN_DISABLE_COLLECTION)
	VALUE_STRING(IRP_MN_REGINFO)
	VALUE_STRING(IRP_MN_EXECUTE_METHOD)
        default:
		return "IRP_MN_WMI_UNKNOWN";
	}
}

const char *wdm_dev_irp_string(unsigned long major)
{
	switch (major) {
	VALUE_STRING(IRP_MJ_PNP)
	VALUE_STRING(IRP_MJ_POWER)
	VALUE_STRING(IRP_MJ_CLEANUP)
	VALUE_STRING(IRP_MJ_CREATE)
	VALUE_STRING(IRP_MJ_CLOSE)
	VALUE_STRING(IRP_MJ_READ)
	VALUE_STRING(IRP_MJ_WRITE)
	VALUE_STRING(IRP_MJ_DEVICE_CONTROL)
	VALUE_STRING(IRP_MJ_SYSTEM_CONTROL)
	default:
		return "IRP_MJ_UNKNOWN";
	}
}

const char *wdm_dev_state_string(uint8_t state)
{
	switch (state) {
	VALUE_STRING(DEV_STOPPED)
	VALUE_STRING(DEV_RUNNING)
	VALUE_STRING(DEV_STOPPING)
	VALUE_STRING(DEV_REMOVING)
	VALUE_STRING(DEV_REMOVED)
	VALUE_STRING(DEV_LOWER_REMOVED)
	default:
		return "Unknown";
	}
}

const char *wdm_dev_pm_string(uint8_t minor)
{
	switch (minor) {
	VALUE_STRING(IRP_MN_WAIT_WAKE)
	VALUE_STRING(IRP_MN_POWER_SEQUENCE)
	VALUE_STRING(IRP_MN_SET_POWER)
	VALUE_STRING(IRP_MN_QUERY_POWER)
	default:
		return "IRP_MN_PM_UNKNOWN";
	}
}

const char *wdm_dev_status_string(status_t status)
{
	switch (status) {
	VALUE_STRING(STATUS_INVALID_DEVICE_REQUEST)
	VALUE_STRING(STATUS_CANCELLED)
	VALUE_STRING(STATUS_POWER_STATE_INVALID)
	VALUE_STRING(STATUS_DEVICE_BUSY)
	default:
		return "STATUS_UNKNOWN";
	}
}

const char *wdm_sys_power_string(SYSTEM_POWER_STATE power)
{
	switch (power) {
	VALUE_STRING(PowerSystemUnspecified)
	VALUE_STRING(PowerSystemWorking)
	VALUE_STRING(PowerSystemSleeping1)
	VALUE_STRING(PowerSystemSleeping2)
	VALUE_STRING(PowerSystemSleeping3)
	VALUE_STRING(PowerSystemHibernate)
	VALUE_STRING(PowerSystemShutdown)
	default:
		return "PowerSystemMaximum";
	}
}

const char *wdm_dev_power_string(DEVICE_POWER_STATE power)
{
	switch (power) {
	VALUE_STRING(PowerDeviceUnspecified)
	VALUE_STRING(PowerDeviceD0)
	VALUE_STRING(PowerDeviceD1)
	VALUE_STRING(PowerDeviceD2)
	VALUE_STRING(PowerDeviceD3)
	default:
		return "PowerDeviceMaximum";
	}
}

const char *wdm_bus_id_string(BUS_QUERY_ID_TYPE type)
{
	switch (type) {
	VALUE_STRING(BusQueryDeviceID)
	VALUE_STRING(BusQueryHardwareIDs)
	VALUE_STRING(BusQueryCompatibleIDs)
	VALUE_STRING(BusQueryInstanceID)
	VALUE_STRING(BusQueryDeviceSerialNumber)
#ifdef BusQueryContainerID
	/* VALUE_STRING(BusQueryContainerID) */
#endif
        default:
		return "BusQueryUnknownID";
	}
}

const char *wdm_bus_text_string(DEVICE_TEXT_TYPE type)
{
	switch (type) {
	VALUE_STRING(DeviceTextDescription)
	VALUE_STRING(DeviceTextLocationInformation)
	default:
		return "DeviceTextUnknown";
	}
}
#undef VALUE_STRING
