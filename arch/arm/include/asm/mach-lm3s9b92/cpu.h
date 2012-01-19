#ifndef __CPU_LM3S9B92_H_INCLUDE__
#define __CPU_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

#define DID0			SYSTEM(0x000)
#define VER_MASK		0x70000000
#define CLASS_MASK		0x00FF0000
#define MAJOR_MASK		0x0000FF00
#define MINOR_MASK		0x000000FF
#define REV_MASK		0x0000FFFF
#define VER			28
#define CLASS			16
#define MAJOR			8
#define MINOR			0
#define REV			0

#define DID1			SYSTEM(0x004)
#define DC0			SYSTEM(0x008)
#define DC1			SYSTEM(0x010)
#define DC2			SYSTEM(0x014)
#define DC3			SYSTEM(0x018)
#define DC4			SYSTEM(0x01C)
#define DC5			SYSTEM(0x020)
#define DC6			SYSTEM(0x024)
#define DC7			SYSTEM(0x028)
#define DC8			SYSTEM(0x02C)

#define DEVICE_CLASS_UNKNOWN	0xFF
#define DEVICE_CLASS_SANDSTORM	0x00
#define DEVICE_CLASS_FURY	0x01
#define DEVICE_CLASS_DUSTDEVIL	0x02
#define DEVICE_CLASS_TEMPEST	0x03

#define DEVICE_REVISION_A0	0x0000
#define DEVICE_REVISION_A1	0x0001
#define DEVICE_REVISION_A2	0x0002
#define DEVICE_REVISION_B0	0x0100
#define DEVICE_REVISION_B1	0x0101
#define DEVICE_REVISION_C0	0x0200
#define DEVICE_REVISION_C1	0x0201
#define DEVICE_REVISION_C2	0x0202
#define DEVICE_REVISION_C3	0x0203

uint8_t cpu_hw_device_class(void);
uint16_t cpu_hw_device_revision(void);

#endif /* __CPU_LM3S9B92_H_INCLUDE__ */
