#ifndef __SEGGER_RTT_DBG_H_INCLUDE__
#define __SEGGER_RTT_DBG_H_INCLUDE__

#include <target/types.h>

/* Type "int" is assumed to be 32-bits in size. */
typedef uint32_t rtt_int_t;

void SEGGER_RTT_Init(void);

size_t SEGGER_RTT_ReadNoLock(uint8_t index, void *data, size_t size);
size_t SEGGER_RTT_WriteNoLock(uint8_t index, const void *data, size_t size);

int SEGGER_RTT_SetTerminal(uint8_t tid);
int SEGGER_RTT_TerminalOut(uint8_t tid, const char *s);

#endif /* __SEGGER_RTT_DBG_H_INCLUDE__ */
