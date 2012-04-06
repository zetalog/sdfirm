#ifndef __DBG_H_INCLUDE__
#define __DBG_H_INCLUDE__

#ifdef WIN32
#include <windows.h>
#else
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#endif
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#ifndef WIN32
#define CloseHandle(x)	close(x)
typedef unsigned char * LPBYTE;
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef void VOID;
typedef char * LPSTR;
typedef const char * LPCSTR;
typedef const LPBYTE LPCBYTE;
typedef char CHAR;
typedef char * LPCTSTR;
#define CONST const
#else
#define strcasecmp	_stricmp
#endif

#ifndef BOOL
typedef int BOOL;
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE (!TRUE)
#endif

typedef int bool;
typedef unsigned char uint8_t;
#ifndef inline
#define inline
#endif

#include <host/list.h>
#include <target/dbg_event.h>

#define DBG_SRC_RAW		NR_DBG_SRCS
#define RAW_EVENT_BASE		(DBG_SRC_RAW<<4)
#define RAW_EVENT_DATA		RAW_EVENT_BASE+0
#define RAW_EVENT_ERROR		RAW_EVENT_BASE+1
#define NR_RAW_EVENTS		(RAW_EVENT_ERROR - RAW_EVENT_BASE + 1)

#define NR_DBG_UI_SRCS		NR_DBG_SRCS+1

void dbg_process_log(void *ctx, uint8_t byte);

typedef unsigned char dbg_cmd_t;
typedef unsigned char dbg_data_t;

#define DBG_STATE_ID(cmd)	((cmd&0xF0) >> 4)
#define DBG_EVENT_ID(cmd)	(cmd&0x0F)

typedef void (*dbg_dump_cb)(void *ctx, dbg_cmd_t cmd, const char *fmt, ...);
typedef void (*dbg_last_cb)(void *ctx, const unsigned char *buf, size_t len);
typedef void (*dbg_storage_cb)(void *ctx, unsigned char buf);

#define DBG_STORE_EVENT		0x00
#define DBG_STORE_RAW		0x01
struct dbg_storage {
	dbg_storage_cb call;
};

struct dbg_parser {
	const char *name;
	unsigned long flags;
	void (*func)(void *ctx, dbg_cmd_t cmd, dbg_data_t data);
};

struct dbg_source {
	const char *name;
	struct dbg_parser *events;
	int nr_events;
};

const char *dbg_state_name(dbg_cmd_t cmd);
const char *dbg_event_name(dbg_cmd_t cmd);
void dbg_register_output(dbg_dump_cb dump, dbg_last_cb last);
void dbg_register_source(dbg_cmd_t state, struct dbg_source *source);

struct dbg_storage *dbg_save_storage(int store,
				     struct dbg_storage *stor);
void dbg_restore_storage(int store,
			 struct dbg_storage *stor);
void dbg_store_event(void *ctx, unsigned char log);
void dbg_store_raw(void *ctx, unsigned char log);

extern dbg_dump_cb dbg_dumper;
extern dbg_last_cb dbg_logger;

void dbg_io_init(void);
void dbg_bulk_init(void);
void dbg_uart_init(void);
void dbg_usb_init(void);
void dbg_scd_init(void);
void dbg_hid_init(void);
void dbg_scs_init(void);
void dbg_msd_init(void);
void dbg_dfu_init(void);
void dbg_scsi_init(void);
void dbg_user_init(void);

void usb_dump_descriptor(void *ctx, dbg_cmd_t cmd, dbg_data_t data);

void dbg_init(void);
void dbg_exit(void);

#endif /* __DBG_H_INCLUDE__ */
