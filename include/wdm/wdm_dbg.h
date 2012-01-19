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
