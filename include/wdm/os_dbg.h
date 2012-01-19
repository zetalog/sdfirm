#ifndef __OS_DBG_H_INCLUDE__
#define __OS_DBG_H_INCLUDE__

#include <wdm/wdm_dbg.h>

#define BUG()					BUG_ON(0)
void os_dbg_set_level(int level);
int os_dbg_get_level(void);
void os_dbg(int level, const char *format, ...);

#endif /* __OS_DBG_H_INCLUDE__ */
