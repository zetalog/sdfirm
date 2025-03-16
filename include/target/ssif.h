#ifndef __SSIF_H_INCLUDE__
#define __SSIF_H_INCLUDE__

#ifndef SSIF_BASE
#define SSIF_BASE				0x10
#endif

#define SSIF_REG(base, offset)			(base + offset)

#include <target/i2c.h>

/* SMBus Commands for SSIF */
#define SSIF_SINGLE_WRITE			0x02
#define SSIF_MULTI_WRITE_START			0x06
#define SSIF_MULTI_WRITE_MIDDLE			0x07
#define SSIF_MULTI_WRITE_END			0x08
#define SSIF_SINGLE_READ			0x03
#define SSIF_MULTI_READ_START			0x03
#define SSIF_MULTI_READ_MIDDLE			0x09
#define SSIF_MULTI_READ_END			0x09
#define SSIF_MULTI_READ_RETRY			0x0A

/* SSIF Write Protocol */
#define SSIF_W_SMBUS_CMD			0
#define SSIF_W_LEN				1
#define SSIF_W_NETFN				2
#define SSIF_W_LUN				3
#define SSIF_W_IPMI_CMD				4

/* SSIF Read Protocol */
#define SSIF_R_SMBUS_CMD			0
#define SSIF_R_LEN				0
#define SSIF_R_NETFN				1
#define SSIF_R_LUN				2
#define SSIF_R_IPMI_CMD				3
#define SSIF_R_COMPLETION_CODE			4

#endif