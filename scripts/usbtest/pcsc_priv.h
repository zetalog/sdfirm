#ifndef __PCSC_PRIV_H_INCLUDE__
#define __PCSC_PRIV_H_INCLUDE__

#include <host/missing.h>
#include <host/logger.h>
#include <host/usbapi.h>
#include <host/ccid.h>
#include <host/pcsc.h>

#define PCSC_LOG_CRIT		LOG_ERROR
#define PCSC_LOG_FAIL		LOG_ERROR
#define PCSC_LOG_ERR		LOG_ERROR
#define PCSC_LOG_WARN		LOG_ERROR
#define PCSC_LOG_INFO		LOG_INFO
#define PCSC_LOG_DEBUG		LOG_DEBUG
#define pcsc_log		log_msg

#endif /* __PCSC_PRIV_H_INCLUDE__ */
