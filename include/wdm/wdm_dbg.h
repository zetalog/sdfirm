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
 * @(#)wdm_dbg.h: WDM dependent driver debugger interfaces
 * $Id: wdm_dbg.h,v 1.315 2011-10-17 01:40:34 zhenglv Exp $
 */

#ifndef __WDM_DBG_H_INCLUDE__
#define __WDM_DBG_H_INCLUDE__

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

/*
#define OS_DBG_FAIL		DPFLTR_ERROR_LEVEL
#define OS_DBG_ERR		DPFLTR_ERROR_LEVEL
#define OS_DBG_WARN		DPFLTR_WARNING_LEVEL
#define OS_DBG_INFO		DPFLTR_TRACE_LEVEL
#define OS_DBG_DEBUG		DPFLTR_INFO_LEVEL
*/
#define OS_DBG_FAIL		DPFLTR_ERROR_LEVEL
#define OS_DBG_ERR		DPFLTR_ERROR_LEVEL
#define OS_DBG_WARN		DPFLTR_ERROR_LEVEL
#define OS_DBG_INFO		DPFLTR_ERROR_LEVEL
#define OS_DBG_DEBUG		DPFLTR_ERROR_LEVEL

#undef  ExAllocatePool
#define ExAllocatePool(type, size)		\
        ExAllocatePoolWithTag(type, size, OS_DBG_TAG)
#undef  ExFreePool
#define ExFreePool(type)			\
        ExFreePoolWithTag(type, OS_DBG_TAG)

#ifdef CONFIG_DEBUG
#define os_break()				KdBreakPoint()
#define BUG_ON(expr)				ASSERT(!(expr))
#else
#define os_break()
#define BUG_ON(expr)
#endif

const char *wdm_dev_bool_string(unsigned long val);
const char *wdm_dev_irp_string(unsigned long major);
const char *wdm_dev_pnp_string(uint8_t minor);
const char *wdm_dev_pm_string(uint8_t minor);
const char *wdm_dev_wmi_string(uint8_t minor);
const char *wdm_dev_status_string(status_t status);
const char *wdm_dev_state_string(uint8_t state);
const char *wdm_sys_power_string(SYSTEM_POWER_STATE power);
const char *wdm_dev_power_string(DEVICE_POWER_STATE power);
const char *wdm_bus_id_string(BUS_QUERY_ID_TYPE type);
const char *wdm_bus_text_string(DEVICE_TEXT_TYPE type);

#endif /* __WDM_DBG_H_INCLUDE__*/
