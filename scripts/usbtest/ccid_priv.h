#ifndef __CCID_PRIV_H_INCLUDE__
#define __CCID_PRIV_H_INCLUDE__

#include <host/missing.h>
#include <host/logger.h>
#include <host/usbapi.h>
#include <host/ccid.h>

#define CCID_LOG_CRIT		LOG_ERROR
#define CCID_LOG_FAIL		LOG_ERROR
#define CCID_LOG_ERR		LOG_ERROR
#define CCID_LOG_WARN		LOG_ERROR
#define CCID_LOG_INFO		LOG_INFO
#define CCID_LOG_DEBUG		LOG_DEBUG
#define ccid_log		log_msg

#endif
