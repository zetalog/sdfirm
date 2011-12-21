#ifndef __USB_PRIV_H_INCLUDE__
#define __USB_PRIV_H_INCLUDE__

#include <host/missing.h>
#include <host/logger.h>
#include <host/usbapi.h>
#include <host/pcsc.h>

#define USB_LOG_CRIT		LOG_ERROR
#define USB_LOG_FAIL		LOG_ERROR
#define USB_LOG_ERR		LOG_ERROR
#define USB_LOG_WARN		LOG_ERROR
#define USB_LOG_INFO		LOG_INFO
#define USB_LOG_DEBUG		LOG_DEBUG
#define usb_log			log_msg

#endif /* __USB_PRIV_H_INCLUDE__ */
