#ifndef __ESPI_H_INCLUDE__
#define __ESPI_H_INCLUDE__

#include <target/generic.h>
#include <driver/espi.h>

#define ESPI_RESPONSE_MODIFIER_OFFSET		6
#define ESPI_RESPONSE_MODIFIER_MASK		REG_2BIT_MASK
#define ESPI_RSP_NO_RESPONSE			ESPI_RESPONSE_MODIFIER(0x3)
#define ESPI_RSP_RESPONSE			ESPI_RESPONSE_MODIFIER(0x0)
#define ESPI_RSP_ACCEPT(r)			(ESPI_RESPONSE_MODIFIER(r) | 0x08)
#define ESPI_RSP_DEFER				(ESPI_RSP_RESPONSE |  0x01)

#define ESPI_CMD_LEN(opcode)			((opcode & 0x3) + 1)

#endif /* __ESPI_H_INCLUDE__ */