/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2009
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)acpi.h: advanced configuration and power interfaces interfaces
 * $Id: acpi.h,v 1.279 2011-10-19 10:19:18 zhenglv Exp $
 */

#ifndef __ACPI_H_INCLUDE__
#define __ACPI_H_INCLUDE__

#ifdef __ASSEMBLY__

#define ACPIBIOS_INPUT(align)		\
	__acpi_bios_start = .;		\
	KEEP (*(.acpi.bios))		\
	__acpi_bios_end = .;

#ifndef LINKER_SCRIPT
	.macro define_acpi_table, _name:req
	.pushsection .acpi.bios, "a"
	.align 3
	.type __acpi_bios_\_name, %object
	.quad \_name
	.popsection
	.endm
#endif

#else /* __ASSEMBLY__ */

#define ACPI_MUTEX_TYPE			ACPI_BINARY_SEMAPHORE

#ifdef WIN32
#include <host/acpi.h>
#else
#include <target/generic.h>
#include <target/panic.h>
#include <target/heap.h>
#include <target/page.h>
#include <target/task.h>
#include <target/delay.h>
#include <target/sbi.h>
#include <target/acpi_osl.h>
#endif

#define ACPI_REVISION			0x00000001

#define ACPI_UINT32_MAX			(uint32_t)(~((uint32_t)0)) /* 0xFFFFFFFF         */
#define ACPI_UINT64_MAX			(uint64_t)(~((uint64_t)0)) /* 0xFFFFFFFFFFFFFFFF */

struct acpi_bios_table {
	void *table;
} __packed;
extern struct acpi_bios_table __acpi_bios_start[];
extern struct acpi_bios_table __acpi_bios_end[];

typedef int acpi_status_t;
typedef void *acpi_handle_t;

#if (ACPI_MUTEX_TYPE == ACPI_BINARY_SEMAPHORE)
/*
 * These macros are used if the host OS does not support a mutex object.
 * Map the OSL Mutex interfaces to binary semaphores.
 */
#define acpi_mutex_t				acpi_semaphore_t
#define acpi_os_create_mutex(phandle)		acpi_os_create_semaphore(1, 1, phandle)
#define acpi_os_delete_mutex(handle)		acpi_os_delete_semaphore(handle)
#define acpi_os_acquire_mutex(handle, timeout)	acpi_os_wait_semaphore(handle, 1, timeout)
#define acpi_os_release_mutex(handle)		acpi_os_signal_semaphore(handle, 1)
#endif

#ifndef acpi_semaphore_t
#define acpi_semaphore_t		void *
#endif
#ifndef acpi_mutex_t
#define acpi_mutex_t			void *
#endif
#ifndef acpi_spinlock_t
#define acpi_spinlock_t			void *
#endif
#ifndef acpi_cpuflags_t
#define acpi_cpuflags_t			acpi_size_t
#endif

/* Exception code classes */
#define AE_CODE_ENVIRONMENTAL           0x0000 /* General ACPICA environment */
#define AE_CODE_PROGRAMMER              0x1000 /* External ACPICA interface caller */
#define AE_CODE_ACPI_TABLES             0x2000 /* ACPI tables */
#define AE_CODE_AML                     0x3000 /* From executing AML code */
#define AE_CODE_CONTROL                 0x4000 /* Internal control codes */

#define AE_CODE_MAX                     0x4000
#define AE_CODE_MASK                    0xF000

/* Macros to insert the exception code classes */
#define EXCEP_ENV(code)                 ((acpi_status_t)(code | AE_CODE_ENVIRONMENTAL))
#define EXCEP_PGM(code)                 ((acpi_status_t)(code | AE_CODE_PROGRAMMER))
#define EXCEP_TBL(code)                 ((acpi_status_t)(code | AE_CODE_ACPI_TABLES))
#define EXCEP_AML(code)                 ((acpi_status_t)(code | AE_CODE_AML))
#define EXCEP_CTL(code)                 ((acpi_status_t)(code | AE_CODE_CONTROL))

/* Success is always zero, failure is non-zero */
#define ACPI_SUCCESS(a)                 (!(a))
#define ACPI_FAILURE(a)                 (a)

#define AE_OK                           (acpi_status_t)0x0000

/* Environmental exceptions */
#define AE_ERROR                        EXCEP_ENV(0x0001)
#define AE_NO_ACPI_TABLES               EXCEP_ENV(0x0002)
#define AE_NO_NAMESPACE                 EXCEP_ENV(0x0003)
#define AE_NO_MEMORY                    EXCEP_ENV(0x0004)
#define AE_NOT_FOUND                    EXCEP_ENV(0x0005)
#define AE_NOT_EXIST                    EXCEP_ENV(0x0006)
#define AE_ALREADY_EXISTS               EXCEP_ENV(0x0007)
#define AE_TYPE                         EXCEP_ENV(0x0008)
#define AE_NULL_OBJECT                  EXCEP_ENV(0x0009)
#define AE_NULL_ENTRY                   EXCEP_ENV(0x000A)
#define AE_BUFFER_OVERFLOW              EXCEP_ENV(0x000B)
#define AE_STACK_OVERFLOW               EXCEP_ENV(0x000C)
#define AE_STACK_UNDERFLOW              EXCEP_ENV(0x000D)
#define AE_NOT_IMPLEMENTED              EXCEP_ENV(0x000E)
#define AE_SUPPORT                      EXCEP_ENV(0x000F)
#define AE_LIMIT                        EXCEP_ENV(0x0010)
#define AE_TIME                         EXCEP_ENV(0x0011)
#define AE_ACQUIRE_DEADLOCK             EXCEP_ENV(0x0012)
#define AE_RELEASE_DEADLOCK             EXCEP_ENV(0x0013)
#define AE_NOT_ACQUIRED                 EXCEP_ENV(0x0014)
#define AE_ALREADY_ACQUIRED             EXCEP_ENV(0x0015)
#define AE_NO_HARDWARE_RESPONSE         EXCEP_ENV(0x0016)
#define AE_NO_GLOBAL_LOCK               EXCEP_ENV(0x0017)
#define AE_ABORT_METHOD                 EXCEP_ENV(0x0018)
#define AE_SAME_HANDLER                 EXCEP_ENV(0x0019)
#define AE_NO_HANDLER                   EXCEP_ENV(0x001A)
#define AE_OWNER_ID_LIMIT               EXCEP_ENV(0x001B)
#define AE_NOT_CONFIGURED               EXCEP_ENV(0x001C)
#define AE_ACCESS                       EXCEP_ENV(0x001D)

#define AE_CODE_ENV_MAX                 0x001D

/* Programmer exceptions */
#define AE_BAD_PARAMETER                EXCEP_PGM(0x0001)
#define AE_BAD_CHARACTER                EXCEP_PGM(0x0002)
#define AE_BAD_PATHNAME                 EXCEP_PGM(0x0003)
#define AE_BAD_DATA                     EXCEP_PGM(0x0004)
#define AE_BAD_HEX_CONSTANT             EXCEP_PGM(0x0005)
#define AE_BAD_OCTAL_CONSTANT           EXCEP_PGM(0x0006)
#define AE_BAD_DECIMAL_CONSTANT         EXCEP_PGM(0x0007)
#define AE_MISSING_ARGUMENTS            EXCEP_PGM(0x0008)
#define AE_BAD_ADDRESS                  EXCEP_PGM(0x0009)

#define AE_CODE_PGM_MAX                 0x0009

/* Acpi table exceptions */
#define AE_BAD_SIGNATURE                EXCEP_TBL(0x0001)
#define AE_BAD_HEADER                   EXCEP_TBL(0x0002)
#define AE_BAD_CHECKSUM                 EXCEP_TBL(0x0003)
#define AE_BAD_VALUE                    EXCEP_TBL(0x0004)
#define AE_INVALID_TABLE_LENGTH         EXCEP_TBL(0x0005)

#define AE_CODE_TBL_MAX                 0x0005

/*
 * AML exceptions. These are caused by problems with
 * the actual AML byte stream
 */
#define AE_AML_UNKNOWN_TERM             EXCEP_AML(0x0001)
#define AE_AML_INCOMPLETE_TERM          EXCEP_AML(0x0002)
#define AE_AML_NO_OPERAND               EXCEP_AML(0x0003)
#define AE_AML_OPERAND_TYPE             EXCEP_AML(0x0004)
#define AE_AML_OPERAND_VALUE            EXCEP_AML(0x0005)
#define AE_AML_UNINITIALIZED_LOCAL      EXCEP_AML(0x0006)
#define AE_AML_UNINITIALIZED_ARG        EXCEP_AML(0x0007)
#define AE_AML_UNINITIALIZED_ELEMENT    EXCEP_AML(0x0008)
#define AE_AML_NUMERIC_OVERFLOW         EXCEP_AML(0x0009)
#define AE_AML_REGION_LIMIT             EXCEP_AML(0x000A)
#define AE_AML_BUFFER_LIMIT             EXCEP_AML(0x000B)
#define AE_AML_PACKAGE_LIMIT            EXCEP_AML(0x000C)
#define AE_AML_DIVIDE_BY_ZERO           EXCEP_AML(0x000D)
#define AE_AML_BAD_NAME                 EXCEP_AML(0x000E)
#define AE_AML_NAME_NOT_FOUND           EXCEP_AML(0x000F)
#define AE_AML_INTERNAL                 EXCEP_AML(0x0010)
#define AE_AML_INVALID_SPACE_ID         EXCEP_AML(0x0011)
#define AE_AML_STRING_LIMIT             EXCEP_AML(0x0012)
#define AE_AML_NO_RETURN_VALUE          EXCEP_AML(0x0013)
#define AE_AML_METHOD_LIMIT             EXCEP_AML(0x0014)
#define AE_AML_NOT_OWNER                EXCEP_AML(0x0015)
#define AE_AML_MUTEX_ORDER              EXCEP_AML(0x0016)
#define AE_AML_MUTEX_NOT_ACQUIRED       EXCEP_AML(0x0017)
#define AE_AML_INVALID_RESOURCE_TYPE    EXCEP_AML(0x0018)
#define AE_AML_INVALID_INDEX            EXCEP_AML(0x0019)
#define AE_AML_REGISTER_LIMIT           EXCEP_AML(0x001A)
#define AE_AML_NO_WHILE                 EXCEP_AML(0x001B)
#define AE_AML_ALIGNMENT                EXCEP_AML(0x001C)
#define AE_AML_NO_RESOURCE_END_TAG      EXCEP_AML(0x001D)
#define AE_AML_BAD_RESOURCE_VALUE       EXCEP_AML(0x001E)
#define AE_AML_CIRCULAR_REFERENCE       EXCEP_AML(0x001F)
#define AE_AML_BAD_RESOURCE_LENGTH      EXCEP_AML(0x0020)
#define AE_AML_ILLEGAL_ADDRESS          EXCEP_AML(0x0021)
#define AE_AML_INFINITE_LOOP            EXCEP_AML(0x0022)

#define AE_CODE_AML_MAX                 0x0022

/* Internal exceptions used for control */
#define AE_CTRL_RETURN_VALUE            EXCEP_CTL(0x0001)
#define AE_CTRL_PENDING                 EXCEP_CTL(0x0002)
#define AE_CTRL_TERMINATE               EXCEP_CTL(0x0003)
#define AE_CTRL_TRUE                    EXCEP_CTL(0x0004)
#define AE_CTRL_FALSE                   EXCEP_CTL(0x0005)
#define AE_CTRL_DEPTH                   EXCEP_CTL(0x0006)
#define AE_CTRL_END                     EXCEP_CTL(0x0007)
#define AE_CTRL_TRANSFER                EXCEP_CTL(0x0008)
#define AE_CTRL_BREAK                   EXCEP_CTL(0x0009)
#define AE_CTRL_CONTINUE                EXCEP_CTL(0x000A)
#define AE_CTRL_SKIP                    EXCEP_CTL(0x000B)
#define AE_CTRL_PARSE_CONTINUE          EXCEP_CTL(0x000C)
#define AE_CTRL_PARSE_PENDING           EXCEP_CTL(0x000D)

#define AE_CODE_CTRL_MAX                0x000D

/* ============================================================ *
 * power of two handling
 * ============================================================ */
#define _ACPI_DIV(value, powerof2)	((uint32_t)((value) >> (powerof2)))
#define _ACPI_MUL(value, powerof2)	((uint32_t)((value) << (powerof2)))
#define _ACPI_MOD(value, divisor)	((uint32_t)((value) & ((divisor) -1)))

#define ACPI_DIV_2(a)			_ACPI_DIV(a, 1)
#define ACPI_MUL_2(a)			_ACPI_MUL(a, 1)
#define ACPI_MOD_2(a)			_ACPI_MOD(a, 2)
#define ACPI_DIV_4(a)			_ACPI_DIV(a, 2)
#define ACPI_MUL_4(a)			_ACPI_MUL(a, 2)
#define ACPI_MOD_4(a)			_ACPI_MOD(a, 4)
#define ACPI_DIV_8(a)			_ACPI_DIV(a, 3)
#define ACPI_MUL_8(a)			_ACPI_MUL(a, 3)
#define ACPI_MOD_8(a)			_ACPI_MOD(a, 8)
#define ACPI_DIV_16(a)			_ACPI_DIV(a, 4)
#define ACPI_MUL_16(a)			_ACPI_MUL(a, 4)
#define ACPI_MOD_16(a)			_ACPI_MOD(a, 16)
#define ACPI_DIV_32(a)			_ACPI_DIV(a, 5)
#define ACPI_MUL_32(a)			_ACPI_MUL(a, 5)
#define ACPI_MOD_32(a)			_ACPI_MOD(a, 32)

#define ACPI_ROUND_DOWN(value, boundary)	\
	(((acpi_size_t)(value)) & (~(((acpi_size_t)boundary)-1)))
#define ACPI_ROUND_UP(value, boundary)		\
	((((acpi_size_t)(value)) + (((acpi_size_t)boundary)-1)) & (~(((acpi_size_t)boundary)-1)))

#define ACPI_GPE_MAX			255

/* ============================================================ *
 * size/addr handling
 * ============================================================ */
typedef uint64_t acpi_uint_t;
typedef int64_t acpi_int_t;
typedef acpi_uint_t acpi_addr_t;
typedef acpi_uint_t acpi_size_t;
typedef void *acpi_uintptr_t;

#define ACPI_CAST_PTR(t, p)             ((t *)(acpi_uintptr_t)(p))
#define ACPI_CAST_RSDP(table)		ACPI_CAST_PTR(struct acpi_table_rsdp, table)
#define ACPI_ADD_PTR(t, a, b)           ACPI_CAST_PTR(t, (ACPI_CAST_PTR(uint8_t, (a)) + (acpi_size_t)(b)))
#define ACPI_PTR_DIFF(a, b)             (ACPI_CAST_PTR(uint8_t, (a)) - ACPI_CAST_PTR(uint8_t, (b)))
#define ACPI_TO_POINTER(i)              ACPI_ADD_PTR(void, NULL,(acpi_size_t)i)
#define ACPI_TO_INTEGER(p)              ACPI_PTR_DIFF(p, NULL)
#define ACPI_PHYSADDR_TO_PTR(i)         ACPI_TO_POINTER(i)
#define ACPI_PTR_TO_PHYSADDR(i)         ACPI_TO_INTEGER(i)
#define ACPI_OFFSET(d, f)               ACPI_PTR_DIFF(&(((d *)0)->f), NULL)

typedef uint32_t acpi_ddb_t;

/* Unalignment support */
#ifndef ACPI_MISALIGNMENT_NOT_SUPPORTED
#ifdef ENDIAN_BE
#ifndef ACPI_GET16
#define ACPI_GET16(ptr) ((uint16_t)(						\
	((uint16_t)(*(((uint8_t *)(ptr))+1))) |					\
	((uint16_t)(*(((uint8_t *)(ptr))+0)) << 8)))
#endif
#ifndef ACPI_GET32
#define ACPI_GET32(ptr) ((uint32_t)(						\
	((uint32_t)ACPI_GET16((((uint16_t *)(ptr))+1)) <<  0) |			\
	((uint32_t)ACPI_GET16((((uint16_t *)(ptr))+0)) << 16)))
#endif
#ifndef ACPI_GET64
#define ACPI_GET64(ptr) ((uint64_t)(						\
	((uint64_t)ACPI_GET32((((uint32_t *)(ptr))+1)) <<  0) |			\
	((uint64_t)ACPI_GET32((((uint32_t *)(ptr))+0)) << 32)))
#endif
#ifndef ACPI_PUT16
#define ACPI_PUT16(ptr, val)							\
	(*((uint8_t *)(ptr)+1) = (uint8_t)((uint16_t)(val)),			\
	 *((uint8_t *)(ptr)+0) = (uint8_t)((uint16_t)(val) >> 8))
#endif
#ifndef ACPI_PUT32
#define ACPI_PUT32(ptr, val)							\
	(ACPI_PUT16(((uint16_t *)(ptr))+1, (uint16_t)((uint32_t)(val))),	\
	 ACPI_PUT16(((uint16_t *)(ptr))+0, (uint16_t)((uint32_t)(val) >> 16)))
#endif
#ifndef ACPI_PUT64
#define ACPI_PUT64(ptr, val)							\
	(ACPI_PUT32(((uint32_t *)(ptr))+1, (uint32_t)((uint64_t)(val))),	\
	 ACPI_PUT32(((uint32_t *)(ptr))+0, (uint32_t)((uint64_t)(val) >> 32)))
#endif
#else /* ENDIAN_BE */
#ifndef ACPI_GET16
#define ACPI_GET16(ptr) ((uint16_t)(						\
	((uint16_t)(*(((uint8_t *)(ptr))+0))) |					\
	((uint16_t)(*(((uint8_t *)(ptr))+1)) << 8)))
#endif
#ifndef ACPI_GET32
#define ACPI_GET32(ptr) ((uint32_t)(						\
	((uint32_t)ACPI_GET16((((uint16_t *)(ptr))+0)) <<  0) |			\
	((uint32_t)ACPI_GET16((((uint16_t *)(ptr))+1)) << 16)))
#endif
#ifndef ACPI_GET64
#define ACPI_GET64(ptr) ((uint64_t)(						\
	((uint64_t)ACPI_GET32((((uint32_t *)(ptr))+0)) <<  0) |			\
	((uint64_t)ACPI_GET32((((uint32_t *)(ptr))+1)) << 32)))
#endif
#ifndef ACPI_PUT16
#define ACPI_PUT16(ptr, val)							\
	(*((uint8_t *)(ptr)+0) = (uint8_t)((uint16_t)(val)),			\
	 *((uint8_t *)(ptr)+1) = (uint8_t)((uint16_t)(val) >> 8))
#endif
#ifndef ACPI_PUT32
#define ACPI_PUT32(ptr, val)							\
	(ACPI_PUT16(((uint16_t *)(ptr))+0, (uint16_t)((uint32_t)(val))),	\
	 ACPI_PUT16(((uint16_t *)(ptr))+1, (uint16_t)((uint32_t)(val) >> 16)))
#endif
#ifndef ACPI_PUT64
#define ACPI_PUT64(ptr, val)							\
	(ACPI_PUT32(((uint32_t *)(ptr))+0, (uint32_t)((uint64_t)(val))),	\
	 ACPI_PUT32(((uint32_t *)(ptr))+1, (uint32_t)((uint64_t)(val) >> 32)))
#endif
#endif /* ENDIAN_BE */
#else /* ACPI_MISALIGNMENT_NOT_SUPPORTED */
#ifndef ACPI_GET16
#define ACPI_GET16(ptr)		(*(uint16_t *)(ptr))
#endif
#ifndef ACPI_GET32
#define ACPI_GET32(ptr)		(*(uint32_t *)(ptr))
#endif
#ifndef ACPI_GET64
#define ACPI_GET64(ptr)		(*(uint64_t *)(ptr))
#endif
#ifndef ACPI_PUT16
#define ACPI_PUT16(ptr, val)	(*(uint16_t *)(ptr) = (uint16_t)(val))
#endif
#ifndef ACPI_PUT32
#define ACPI_PUT32(ptr, val)	(*(uint32_t *)(ptr) = (uint32_t)(val))
#endif
#ifndef ACPI_PUT64
#define ACPI_PUT64(ptr, val)	(*(uint64_t *)(ptr) = (uint64_t)(val))
#endif
#endif /* ACPI_MISALIGNMENT_NOT_SUPPORTED */

/* Endianness support */
#ifndef ACPI_SWAB16
#define ACPI_SWAB16(x) ((uint16_t)(					\
	(((uint16_t)(x) & (uint16_t)0x00ffU) << 8) |			\
	(((uint16_t)(x) & (uint16_t)0xff00U) >> 8)))
#endif
#ifndef ACPI_SWAB32
#define ACPI_SWAB32(x) ((uint32_t)(					\
	(((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |		\
	(((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |		\
	(((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |		\
	(((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))
#endif
#ifndef ACPI_SWAB64
#define ACPI_SWAB64(x) ((uint64_t)(					\
	(((uint64_t)(x) & (uint64_t)ULL(0x00000000000000ff)) << 56) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x000000000000ff00)) << 40) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x0000000000ff0000)) << 24) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x00000000ff000000)) <<  8) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x000000ff00000000)) >>  8) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x0000ff0000000000)) >> 24) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x00ff000000000000)) >> 40) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0xff00000000000000)) >> 56)))
#endif

#define ACPI_DECODE8(ptr)	(*(uint8_t *)(ptr))
#define ACPI_ENCODE8(ptr, val)	(*(uint8_t *)(ptr) = (uint8_t)(val))
#ifdef ENDIAN_BE
#define ACPI_DECODE16(ptr)	ACPI_SWAB16(ACPI_GET16(ptr))
#define ACPI_DECODE32(ptr)	ACPI_SWAB32(ACPI_GET32(ptr))
#define ACPI_DECODE64(ptr)	ACPI_SWAB64(ACPI_GET64(ptr))
#define ACPI_ENCODE16(ptr, val)	ACPI_PUT16(ptr, ACPI_SWAB16(val))
#define ACPI_ENCODE32(ptr, val)	ACPI_PUT32(ptr, ACPI_SWAB32(val))
#define ACPI_ENCODE64(ptr, val)	ACPI_PUT64(ptr, ACPI_SWAB64(val))
#else /* ENDIAN_BE */
#define ACPI_DECODE16(ptr)	ACPI_GET16(ptr)
#define ACPI_DECODE32(ptr)	ACPI_GET32(ptr)
#define ACPI_DECODE64(ptr)	ACPI_GET64(ptr)
#define ACPI_ENCODE16(ptr, val)	ACPI_PUT16(ptr, val)
#define ACPI_ENCODE32(ptr, val)	ACPI_PUT32(ptr, val)
#define ACPI_ENCODE64(ptr, val)	ACPI_PUT64(ptr, val)
#endif /* ENDIAN_BE */

/* A name tag is a string in nature, thus needn't be endian converted */
#define ACPI_GET16_PAD(ptr, pad)		(ACPI_GET16(ptr) ? ACPI_GET16(ptr) : (pad))
#define ACPI_GET32_PAD(ptr, pad)		(ACPI_GET32(ptr) ? ACPI_GET32(ptr) : (pad))
#define ACPI_GET64_PAD(ptr, pad)		(ACPI_GET64(ptr) ? ACPI_GET64(ptr) : (pad))
#define ACPI_PUT16_PAD(ptr, val, pad)		((val) ? ACPI_PUT16((ptr), (val)) : ACPI_PUT16((ptr), (pad)))
#define ACPI_PUT32_PAD(ptr, val, pad)		((val) ? ACPI_PUT32((ptr), (val)) : ACPI_PUT32((ptr), (pad)))
#define ACPI_PUT64_PAD(ptr, val, pad)		((val) ? ACPI_PUT64((ptr), (val)) : ACPI_PUT64((ptr), (pad)))

/* ============================================================ *
 * table signature/method name handling
 * ============================================================ */
#define ACPI_NAME_SIZE		4
#define ACPI_NAMESEG_SIZE	ACPI_NAME_SIZE
#define ACPI_AML_PATH_SIZE	1278
#define ACPI_ASL_PATH_SIZE	1530
#define ACPI_MAX_NAME_SEGS	255

typedef uint32_t acpi_tag_t;
typedef char acpi_name_t[ACPI_NAME_SIZE];

/*
 * Maximum AML path size is:
 * 255 * 4 (seg names) + [255 (parents) | 1 (root)] +
 * 1 (multi-name or dual-name) + 1 (seg count) + 1 (trailing null)
 *
 * Maximum ASL path size is:
 * [1 (root) | 255 (parents)] + 255 * 4 (seg names) + 254 (dots) +
 * 1 (trailing null)
 */
typedef uint16_t acpi_path_len_t;

/*
 * The acpi_path_t could be used for both ASL path and AML path. It could
 * contain the trailing null or not. Users may use
 * acpi_path_has_trailing_null() to check if the trailing null is
 * contained.
 */
typedef struct acpi_path {
	acpi_path_len_t length;
	char *names;
} acpi_path_t;

#define ACPI_NAME2TAG(name)				\
	((acpi_tag_t)ACPI_GET32_PAD((name), '_'))
#define ACPI_NAME2RSDPTR(name)				\
	((uint64_t)ACPI_GET64(name))
#define ACPI_TAG_NULL			ACPI_NAME2TAG(ACPI_NULL_NAME)

#define ACPI_ROOT_NAME			"\\___"
#define ACPI_NULL_NAME			"\0\0\0\0"
#define ACPI_ROOT_TAG			ACPI_NAME2TAG(ACPI_ROOT_NAME)

#define ACPI_NAMECMP(sig, name)		((sig) == ACPI_NAME2TAG(name))
#define ACPI_NAMECPY(sig, name)		ACPI_PUT32_PAD((name), (sig), '_')

#define ACPI_RSDP_SIG_CMP(name)		(ACPI_SIG_RSDPTR == ACPI_NAME2RSDPTR(name))
#define ACPI_RSDP_SIG_CPY(name)		ACPI_PUT64((name), ACPI_SIG_RSDPTR)

#define ACPI_OEMCMP(__id1, __id2, __len)		\
	(strncmp((__id1), (__id2), (__len)) == 0)
#define ACPI_OEMCPY(__from, __to, __len)		\
	strncpy((__to), (__from), (__len))

/* ============================================================ *
 * address space handling
 * ============================================================ */
typedef uint8_t acpi_adr_space_type;

#define ACPI_ADR_SPACE_SYSTEM_MEMORY	(acpi_adr_space_type)0
#define ACPI_ADR_SPACE_SYSTEM_IO	(acpi_adr_space_type)1
#define ACPI_ADR_SPACE_PCI_CONFIG	(acpi_adr_space_type)2
#define ACPI_ADR_SPACE_EC		(acpi_adr_space_type)3
#define ACPI_ADR_SPACE_SMBUS		(acpi_adr_space_type)4
#define ACPI_ADR_SPACE_CMOS		(acpi_adr_space_type)5
#define ACPI_ADR_SPACE_PCI_BAR_TARGET	(acpi_adr_space_type)6
#define ACPI_ADR_SPACE_IPMI		(acpi_adr_space_type)7
#define ACPI_ADR_SPACE_GPIO		(acpi_adr_space_type)8
#define ACPI_ADR_SPACE_GSBUS		(acpi_adr_space_type)9
#define ACPI_ADR_SPACE_PLATFORM_COMM	(acpi_adr_space_type)10

#define ACPI_NUM_PREDEFINED_REGIONS     11

struct acpi_reference {
	volatile int count;
};
#define REF_INCREMENT			0
#define REF_DECREMENT			1
#define REF_INCREMENT_FORCE		2

#define ACPI_WAIT_FOREVER		0xFFFF  /* UINT16, as per ACPI spec */
#define ACPI_DO_NOT_WAIT		0

/* ============================================================ *
 * table handling
 * ============================================================ */
#define ACPI_OEM_ID_SIZE	6
#define ACPI_OEM_TABLE_ID_SIZE	8

#define ACPI_SIG_RSDPTR			ACPI_NAME2RSDPTR("RSD PTR ")
#define ACPI_SIG_RSDP			ACPI_NAME2TAG("RSD ")	/* Internal usage */
#define ACPI_SIG_DSDT			ACPI_NAME2TAG("DSDT")
#define ACPI_SIG_FADT			ACPI_NAME2TAG("FACP")
#define ACPI_SIG_RSDT			ACPI_NAME2TAG("RSDT")
#define ACPI_SIG_XSDT			ACPI_NAME2TAG("XSDT")
#define ACPI_SIG_SSDT			ACPI_NAME2TAG("SSDT")
#define ACPI_SIG_PSDT			ACPI_NAME2TAG("PSDT")
#define ACPI_SIG_FACS			ACPI_NAME2TAG("FACS")
#ifndef ACPI_FLEX_ARRAY
#define ACPI_FLEX_ARRAY(TYPE, NAME)     TYPE NAME[0]
#endif

#pragma pack(1)

struct acpi_generic_address {
	uint8_t space_id;
	uint8_t bit_width;
	uint8_t bit_offset;
	uint8_t access_width;
	uint64_t address;
} __packed;

struct acpi_table_header {
	acpi_name_t signature;
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oem_id[ACPI_OEM_ID_SIZE];
	char oem_table_id[ACPI_OEM_TABLE_ID_SIZE];
	uint32_t oem_revision;
	acpi_name_t asl_compiler_id;
	uint32_t asl_compiler_revision;
} __packed;

struct acpi_table_rsdp {
	char signature[8];	/* "RSD PTR " */
	uint8_t checksum;
	char oem_id[ACPI_OEM_ID_SIZE];
	uint8_t revision;	/* 0 for ACPI 1.0 or 2 for ACPI 2.0+ */
	uint32_t rsdt_physical_address;
	uint32_t length;	/* ACPI 2.0+ */
	uint64_t xsdt_physical_address;
	uint8_t extended_checksum;
	uint8_t reserved[3];
} __packed;
#define ACPI_RSDP_CHECKSUM_LENGTH       20
#define ACPI_RSDP_XCHECKSUM_LENGTH      36

struct acpi_table_rsdt {
	struct acpi_table_header header;
	uint32_t table_offset_entry[1];
} __packed;

struct acpi_table_xsdt {
	struct acpi_table_header header;
	uint64_t table_offset_entry[1];
} __packed;

#define ACPI_RSDT_ENTRY_SIZE		(sizeof (uint32_t))
#define ACPI_XSDT_ENTRY_SIZE		(sizeof (uint64_t))

struct acpi_table_fadt {
	struct acpi_table_header header;
	uint32_t facs;
	uint32_t dsdt;
	uint8_t model;			/* ACPI 1.0 only */
	uint8_t preferred_profile;
	uint16_t sci_interrupt;
	uint32_t smi_command;
	uint8_t acpi_enable;
	uint8_t acpi_disable;
	uint8_t s4_bios_request;
	uint8_t pstate_control;
	uint32_t pm1a_event_block;
	uint32_t pm1b_event_block;
	uint32_t pm1a_control_block;
	uint32_t pm1b_control_block;
	uint32_t pm2_control_block;
	uint32_t pm_timer_block;
	uint32_t gpe0_block;
	uint32_t gpe1_block;
	uint8_t pm1_event_length;
	uint8_t pm1_control_length;
	uint8_t pm2_control_length;
	uint8_t pm_timer_length;
	uint8_t gpe0_block_length;
	uint8_t gpe1_block_length;
	uint8_t gpe1_base;
	uint8_t cst_control;
	uint16_t c2_latency;
	uint16_t c3_latency;
	uint16_t flush_size;
	uint16_t flush_stride;
	uint8_t duty_offset;
	uint8_t duty_width;
	uint8_t day_alarm;
	uint8_t month_alarm;
	uint8_t century;
	uint16_t boot_flags;
	uint8_t reserved;
	uint32_t flags;
	struct acpi_generic_address reset_register;
	uint8_t reset_value;
	uint16_t arm_boot_flags;			/* ACPI 5.1+ */
	uint8_t minor_revision;				/* ACPI 5.1+ */
	uint64_t Xfacs;
	uint64_t Xdsdt;
	struct acpi_generic_address xpm1a_event_block;
	struct acpi_generic_address xpm1b_event_block;
	struct acpi_generic_address xpm1a_control_block;
	struct acpi_generic_address xpm1b_control_block;
	struct acpi_generic_address xpm2_control_block;
	struct acpi_generic_address xpm_timer_block;
	struct acpi_generic_address xgpe0_block;
	struct acpi_generic_address xgpe1_block;
	struct acpi_generic_address sleep_control;	/* ACPI 5.0+ */
	struct acpi_generic_address sleep_status;	/* ACPI 5.0+ */
	uint64_t hypervisor_id;				/* ACPI 6.0+ */
} __packed;

#define ACPI_GPE_REGISTER_WIDTH		8
#define ACPI_PM1_REGISTER_WIDTH		16
#define ACPI_PM2_REGISTER_WIDTH		8
#define ACPI_PM_TIMER_WIDTH		32

#define ACPI_FADT_OFFSET(f)		((uint16_t)ACPI_OFFSET(struct acpi_table_fadt, f))

/* Masks for FADT flags */
#define ACPI_FADT_WBINVD		(1)	/* 00: [V1] The WBINVD instruction works properly */
#define ACPI_FADT_WBINVD_FLUSH		(1<<1)	/* 01: [V1] WBINVD flushes but does not invalidate caches */
#define ACPI_FADT_C1_SUPPORTED		(1<<2)	/* 02: [V1] All processors support C1 state */
#define ACPI_FADT_C2_MP_SUPPORTED	(1<<3)	/* 03: [V1] C2 state works on MP system */
#define ACPI_FADT_POWER_BUTTON		(1<<4)	/* 04: [V1] Power button is handled as a control method device */
#define ACPI_FADT_SLEEP_BUTTON		(1<<5)	/* 05: [V1] Sleep button is handled as a control method device */
#define ACPI_FADT_FIXED_RTC		(1<<6)	/* 06: [V1] RTC wakeup status is not in fixed register space */
#define ACPI_FADT_S4_RTC_WAKE		(1<<7)	/* 07: [V1] RTC alarm can wake system from S4 */
#define ACPI_FADT_32BIT_TIMER		(1<<8)	/* 08: [V1] ACPI timer width is 32-bit (0=24-bit) */
#define ACPI_FADT_DOCKING_SUPPORTED	(1<<9)	/* 09: [V1] Docking supported */
#define ACPI_FADT_RESET_REGISTER	(1<<10)	/* 10: [V2] System reset via the FADT RESET_REG supported */
#define ACPI_FADT_SEALED_CASE		(1<<11)	/* 11: [V3] No internal expansion capabilities and case is sealed */
#define ACPI_FADT_HEADLESS		(1<<12)	/* 12: [V3] No local video capabilities or local input devices */
#define ACPI_FADT_SLEEP_TYPE		(1<<13)	/* 13: [V3] Must execute native instruction after writing  SLP_TYPx register */
#define ACPI_FADT_PCI_EXPRESS_WAKE	(1<<14)	/* 14: [V4] System supports PCIEXP_WAKE (STS/EN) bits (ACPI 3.0) */
#define ACPI_FADT_PLATFORM_CLOCK	(1<<15)	/* 15: [V4] OSPM should use platform-provided timer (ACPI 3.0) */
#define ACPI_FADT_S4_RTC_VALID		(1<<16)	/* 16: [V4] Contents of RTC_STS valid after S4 wake (ACPI 3.0) */
#define ACPI_FADT_REMOTE_POWER_ON	(1<<17)	/* 17: [V4] System is compatible with remote power on (ACPI 3.0) */
#define ACPI_FADT_APIC_CLUSTER		(1<<18)	/* 18: [V4] All local APICs must use cluster model (ACPI 3.0) */
#define ACPI_FADT_APIC_PHYSICAL		(1<<19)	/* 19: [V4] All local xAPICs must use physical dest mode (ACPI 3.0) */
#define ACPI_FADT_HW_REDUCED		(1<<20)	/* 20: [V5] ACPI hardware is not implemented (ACPI 5.0) */
#define ACPI_FADT_LOW_POWER_S0		(1<<21)	/* 21: [V5] S0 power savings are equal or better than S3 (ACPI 5.0) */

#define ACPI_FADT_V1_SIZE		((uint32_t)(ACPI_FADT_OFFSET(flags) + 4))
#define ACPI_FADT_V2_SIZE		((uint32_t)(ACPI_FADT_OFFSET(Xfacs)))
#define ACPI_FADT_V3_SIZE		((uint32_t)(ACPI_FADT_OFFSET(sleep_control)))
#define ACPI_FADT_V5_SIZE		((uint32_t)(ACPI_FADT_OFFSET(hypervisor_id)))
#define ACPI_FADT_V6_SIZE		((uint32_t)sizeof (struct acpi_table_fadt))

struct acpi_table_facs {
	acpi_name_t signature;
	uint32_t length;
	uint32_t hardware_signature;
	uint32_t firmware_waking_vector;
	uint32_t global_lock;
	uint32_t flags;
	uint64_t xfirmware_waking_vector;
	uint8_t version;		/* ACPI 2.0+ */
	uint8_t reserved[3];
	uint32_t ospm_flags;		/* ACPI 4.0 */
	uint8_t reserved1[24];
};

/*
 * Get the remaining ACPI tables
 */
#include <acpi/actbl1.h>
#include <acpi/actbl2.h>
#include <acpi/actbl3.h>

#pragma pack()

typedef uint16_t	acpi_table_flags_t;
struct acpi_table_desc {
	acpi_addr_t address;
	uint32_t length;
	acpi_name_t signature;
	char oem_id[ACPI_OEM_ID_SIZE];
	char oem_table_id[ACPI_OEM_TABLE_ID_SIZE];
	uint8_t revision;
	acpi_table_flags_t flags;
#ifdef WIN32
	struct acpi_table_desc *next;
#endif
	struct acpi_reference reference_count;
	struct acpi_table_header *pointer;
};

struct acpi_table {
	acpi_ddb_t ddb;
	struct acpi_table_header *pointer;
	uint32_t length;
	acpi_table_flags_t flags;
};

#define ACPI_TABLE_EXTERNAL_VIRTUAL	((acpi_table_flags_t)0x00)
#define ACPI_TABLE_INTERNAL_PHYSICAL	((acpi_table_flags_t)0x01)
#define ACPI_TABLE_INTERNAL_VIRTUAL	((acpi_table_flags_t)0x02)
#define ACPI_TABLE_ORIGIN_MASK		(0x03)
#define ACPI_TABLE_IS_ACQUIRED		(0x04)
#define ACPI_TABLE_IS_UNINSTALLING	(0x08)
#define ACPI_TABLE_IS_LOADED		(0x10)
#define ACPI_TABLE_IS_UNLOADING		(0x20)
#define ACPI_TABLE_STATE_MASK		(0x3C)

#define ACPI_DDB_HANDLE_DSDT		((acpi_ddb_t)0)
#define ACPI_DDB_HANDLE_FACS		((acpi_ddb_t)1)
#define ACPI_DDB_HANDLE_NON_FIXED	2
#define ACPI_DDB_HANDLE_INVALID		((acpi_ddb_t)0xFFFFFFFF)

/*
 * Types associated with ACPI names and objects. The first group of
 * values (up to ACPI_TYPE_EXTERNAL_MAX) correspond to the definition
 * of the ACPI ObjectType() operator (See the ACPI Spec). Therefore,
 * only add to the first group if the spec changes.
 *
 * NOTE: Types must be kept in sync with the global AcpiNsProperties
 * and AcpiNsTypeNames arrays.
 */
typedef uint8_t acpi_type_t;

#define ACPI_TYPE_ANY			(acpi_type_t)0x00
#define ACPI_TYPE_INTEGER		(acpi_type_t)0x01 /* Byte/Word/Dword/Zero/One/Ones */
#define ACPI_TYPE_STRING		(acpi_type_t)0x02
#define ACPI_TYPE_BUFFER		(acpi_type_t)0x03
#define ACPI_TYPE_PACKAGE		(acpi_type_t)0x04 /* ByteConst, multiple DataTerm/Constant/SuperName */
#define ACPI_TYPE_FIELD_UNIT		(acpi_type_t)0x05
#define ACPI_TYPE_DEVICE		(acpi_type_t)0x06 /* Name, multiple Node */
#define ACPI_TYPE_EVENT			(acpi_type_t)0x07
#define ACPI_TYPE_METHOD		(acpi_type_t)0x08 /* Name, ByteConst, multiple Code */
#define ACPI_TYPE_MUTEX			(acpi_type_t)0x09
#define ACPI_TYPE_REGION		(acpi_type_t)0x0A
#define ACPI_TYPE_POWER			(acpi_type_t)0x0B /* Name,ByteConst,WordConst,multi Node */
#define ACPI_TYPE_PROCESSOR		(acpi_type_t)0x0C /* Name,ByteConst,DWordConst,ByteConst,multi NmO */
#define ACPI_TYPE_THERMAL		(acpi_type_t)0x0D /* Name, multiple Node */
#define ACPI_TYPE_BUFFER_FIELD		(acpi_type_t)0x0E
#define ACPI_TYPE_DDB_HANDLE		(acpi_type_t)0x0F
#define ACPI_TYPE_DEBUG_OBJECT		(acpi_type_t)0x10

#define ACPI_TYPE_EXTERNAL_MAX		(acpi_type_t)0x10

/*
 * These are object types that do not map directly to the ACPI
 * ObjectType() operator. They are used for various internal purposes only.
 * If new predefined ACPI_TYPEs are added (via the ACPI specification), these
 * internal types must move upwards. (There is code that depends on these
 * values being contiguous with the external types above.)
 */
#define ACPI_TYPE_LOCAL_REGION_FIELD	(acpi_type_t)0x11
#define ACPI_TYPE_LOCAL_BANK_FIELD	(acpi_type_t)0x12
#define ACPI_TYPE_LOCAL_INDEX_FIELD	(acpi_type_t)0x13
#define ACPI_TYPE_LOCAL_REFERENCE	(acpi_type_t)0x14 /* Arg#, Local#, Name, Debug, RefOf, Index */
#define ACPI_TYPE_LOCAL_ALIAS		(acpi_type_t)0x15
#define ACPI_TYPE_LOCAL_METHOD_ALIAS	(acpi_type_t)0x16
#define ACPI_TYPE_LOCAL_NOTIFY		(acpi_type_t)0x17
#define ACPI_TYPE_LOCAL_ADDRESS_HANDLER	(acpi_type_t)0x18
#define ACPI_TYPE_LOCAL_RESOURCE	(acpi_type_t)0x19
#define ACPI_TYPE_LOCAL_RESOURCE_FIELD	(acpi_type_t)0x1A
#define ACPI_TYPE_LOCAL_SCOPE		(acpi_type_t)0x1B /* 1 Name, multiple ObjectList Nodes */

#define ACPI_TYPE_NS_NODE_MAX		(acpi_type_t)0x1B /* Last typecode used within a NS Node */

/*
 * These are special object types that never appear in
 * a Namespace node, only in an object of ACPI_OPERAND_OBJECT
 */
#define ACPI_TYPE_LOCAL_EXTRA		(acpi_type_t)0x1C
#define ACPI_TYPE_LOCAL_DATA		(acpi_type_t)0x1D

#define ACPI_TYPE_LOCAL_MAX		(acpi_type_t)0x1D

/* All types above here are invalid */

#define ACPI_TYPE_INVALID		(acpi_type_t)0x1E
#define ACPI_TYPE_NOT_FOUND		(acpi_type_t)0xFF

#define ACPI_NUM_NS_TYPES		(ACPI_TYPE_INVALID + 1)

/* Forward declarations */
struct acpi_object;
struct acpi_opcode_info;

/* Function declarations */
typedef void (*acpi_release_cb)(struct acpi_object *);

#define ACPI_DESC_TYPE_NAMED			0x01
#define ACPI_DESC_TYPE_TERM			0x02
#define ACPI_DESC_TYPE_STATE			0x03
#define ACPI_DESC_TYPE_OPERAND			0x04

#define ACPI_OBJECT_HEADER			\
	uint8_t descriptor_type;		\
	boolean closing;			\
	struct acpi_reference reference_count;	\
	acpi_release_cb release;

struct acpi_object {
	ACPI_OBJECT_HEADER
};

#define ACPI_OPERAND_NAMED			0x01

#define ACPI_OPERAND_HEADER			\
	struct acpi_object common;		\
	uint8_t flags;				\
	acpi_type_t object_type;		\
	acpi_release_cb release;

struct acpi_operand {
	ACPI_OPERAND_HEADER
};

struct acpi_method {
	ACPI_OPERAND_HEADER
	acpi_ddb_t ddb;
	uint8_t method_flags;
	uint8_t *aml_start;
	uint32_t aml_length;
};

struct acpi_integer {
	ACPI_OPERAND_HEADER
	uint64_t value;
};

struct acpi_string {
	ACPI_OPERAND_HEADER
	char *value;
};

struct acpi_buffer {
	ACPI_OPERAND_HEADER
	acpi_size_t length;
	char *value;
};

/* Global variables */
extern uint8_t acpi_gbl_integer_bit_width;
extern struct acpi_generic_address acpi_gbl_xpm1a_enable;
extern struct acpi_generic_address acpi_gbl_xpm1b_enable;
extern struct acpi_generic_address acpi_gbl_xpm1a_status;
extern struct acpi_generic_address acpi_gbl_xpm1b_status;
extern boolean acpi_gbl_fadt_use_default_register_width;
extern boolean acpi_gbl_fadt_use_32bit_addresses;
extern struct acpi_namespace_node *acpi_gbl_root_node;
extern boolean acpi_gbl_early_stage;

#define ACPI_FORMAT_UINT64(i)           HIDWORD(i), LODWORD(i)
#if ACPI_MACHINE_WIDTH == 64
#define ACPI_FORMAT_NATIVE_UINT(i)      ACPI_FORMAT_UINT64(i)
#else
#define ACPI_FORMAT_NATIVE_UINT(i)      0, (i)
#endif

#ifdef CONFIG_ACPI_LOGGING
void acpi_warn(const char *fmt, ...);
void acpi_err(const char *fmt, ...);
void acpi_warn_bios(const char *fmt, ...);
void acpi_err_bios(const char *fmt, ...);
#else
static inline void acpi_warn(const char *fmt, ...) {}
static inline void acpi_err(const char *fmt, ...) {}
static inline void acpi_warn_bios(const char *fmt, ...) {}
static inline void acpi_err_bios(const char *fmt, ...) {}
#endif
const char *acpi_error_string(acpi_status_t status, boolean detail);

acpi_addr_t acpi_os_get_root_pointer(void);
#ifndef acpi_os_map_memory
void *acpi_os_map_memory(acpi_addr_t where, acpi_size_t length);
#endif
#ifndef acpi_os_unmap_memory
void acpi_os_unmap_memory(void *where, acpi_size_t length);
#endif
#ifndef acpi_os_sleep
void acpi_os_sleep(uint32_t msecs);
#endif
#ifndef acpi_os_debug_print
void acpi_os_debug_print(const char *fmt, ...);
#endif
#ifndef acpi_os_allocate
void *acpi_os_allocate(acpi_size_t size);
#endif
#ifndef acpi_os_allocate_zeroed
void *acpi_os_allocate_zeroed(acpi_size_t size);
#endif
#ifndef acpi_os_free
void acpi_os_free(void *mem);
#endif
acpi_status_t acpi_os_create_lock(acpi_spinlock_t *phandle);
void acpi_os_delete_lock(acpi_spinlock_t handle);
acpi_cpuflags_t acpi_os_acquire_lock(acpi_spinlock_t handle);
void acpi_os_release_lock(acpi_spinlock_t handle, acpi_cpuflags_t flags);
acpi_status_t acpi_os_create_semaphore(uint32_t max_units,
				       uint32_t initial_units,
				       acpi_handle_t *out_handle);
acpi_status_t acpi_os_delete_semaphore(acpi_handle_t handle);
acpi_status_t acpi_os_wait_semaphore(acpi_handle_t handle,
				     uint32_t units,
				     uint16_t timeout);
acpi_status_t acpi_os_signal_semaphore(acpi_handle_t handle,
				       uint32_t units);
#ifdef CONFIG_ACPI_TABLE_OVERRIDE
#define ACPI_TABLE_OVERRIDE	true
acpi_status_t acpi_os_table_override(struct acpi_table_header *existing_table,
				     acpi_addr_t *address, acpi_table_flags_t *flags);
#else
#define ACPI_TABLE_OVERRIDE	false
#endif

/*=========================================================================
 * Initialization/Finalization
 *=======================================================================*/
acpi_status_t acpi_initialize_tables(struct acpi_table_desc **initial_table_array,
				     uint32_t initial_table_count,
				     boolean allow_resize);
acpi_status_t acpi_reallocate_root_table(void);
acpi_status_t acpi_initialize_subsystem(void);
acpi_status_t acpi_load_tables(void);
acpi_status_t acpi_initialize_events(void);
acpi_status_t acpi_initialize_namespace(void);

void acpi_finalize_tables(void);

/*=========================================================================
 * Utility external
 *=======================================================================*/
void acpi_reference_set(struct acpi_reference *reference, int count);
int acpi_reference_get(struct acpi_reference *reference);
void acpi_reference_inc(struct acpi_reference *reference);
void acpi_reference_dec(struct acpi_reference *reference);
int acpi_reference_dec_and_test(struct acpi_reference *reference);
int acpi_reference_test_and_inc(struct acpi_reference *reference);

const char *acpi_mutex_name(uint32_t mutex_id);

void acpi_encode_generic_address(struct acpi_generic_address *generic_address,
				 uint8_t space_id,
				 uint64_t address64,
				 uint16_t bit_width);

#define acpi_err		acpi_os_debug_print
#define acpi_warn		acpi_os_debug_print
#define acpi_info		acpi_os_debug_print
#define acpi_dbg		acpi_os_debug_print
#define acpi_err_bios		acpi_os_debug_print
#define acpi_warn_bios		acpi_os_debug_print

/*=========================================================================
 * Name externals
 *=======================================================================*/
int acpi_compare_name(acpi_name_t name1, acpi_name_t name2);
int acpi_compare_sig_name(acpi_tag_t sig, acpi_name_t name);
boolean acpi_path_has_trailing_null(acpi_path_t *path);
acpi_path_len_t acpi_path_encode(const char *name, acpi_path_t *path);
acpi_path_len_t acpi_path_decode(acpi_path_t *path,
				 char *name, acpi_path_len_t size);
acpi_path_len_t acpi_path_split(acpi_path_t *path,
				acpi_path_t *parent, acpi_name_t name);
acpi_path_len_t acpi_path_encode_alloc(const char *name, acpi_path_t *path);

/*=========================================================================
 * Table externals
 *=======================================================================*/
acpi_status_t acpi_install_and_load_table(struct acpi_table_header *table,
					  acpi_table_flags_t flags,
					  boolean versioning,
					  acpi_ddb_t *ddb_handle);
void acpi_uninstall_table(acpi_ddb_t ddb);
/* references with mappings */
acpi_status_t acpi_get_table_by_inst(acpi_tag_t sig, uint32_t instance,
				     struct acpi_table *out_table);
acpi_status_t acpi_get_table_by_name(acpi_tag_t sig, char *oem_id, char *oem_table_id,
				     struct acpi_table *out_table);
acpi_status_t acpi_get_table(acpi_ddb_t ddb, struct acpi_table *out_table);
void acpi_put_table(struct acpi_table *table);
/* flag testing */
boolean acpi_table_contains_aml(struct acpi_table_header *table);
boolean acpi_table_has_header(acpi_name_t signature);
/* FADT flag testing */
uint32_t acpi_fadt_flag_is_set(uint32_t mask);

/*=========================================================================
 * Namespace externals
 *=======================================================================*/
typedef boolean (*acpi_space_cb)(struct acpi_namespace_node *scope,
				 void *context);

#define ACPI_SPACE_DEPTH_INFINITE	0XFFFFFFFF

#define ACPI_SPACE_OPEN_EXIST		0x00
#define ACPI_SPACE_OPEN_CREATE		0x01
#define ACPI_SPACE_OPEN_REPLACE		0x02

void acpi_space_walk_depth_first(acpi_handle_t scope,
				 acpi_type_t object_type,
				 uint32_t max_depth,
				 acpi_space_cb descending_callback,
				 acpi_space_cb ascending_callback,
				 void *context);
acpi_handle_t acpi_space_open(acpi_ddb_t ddb, acpi_handle_t scope,
			      const char *name, uint32_t length,
			      acpi_type_t object_type, uint8_t open_type);
void acpi_space_close(acpi_handle_t node, boolean delete_node);
acpi_handle_t acpi_space_open_exist(acpi_handle_t scope,
				    const char *name, uint32_t length);
void acpi_space_close_exist(acpi_handle_t node);
void acpi_space_increment(acpi_handle_t node);
void acpi_space_decrement(acpi_handle_t node);
acpi_path_len_t acpi_space_get_full_path(acpi_handle_t node,
					 char *path, acpi_path_len_t size);

/*=========================================================================
 * Interpreter externals
 *=======================================================================*/
#ifdef CONFIG_ACPI_AML
struct acpi_operand *acpi_operand_get(struct acpi_operand *operand,
				      const char *hint);
void acpi_operand_put(struct acpi_operand *operand, const char *hint);
#else
#define acpi_operand_get(operand, hint)		NULL
#define acpi_operand_put(operand, hint)		do { } while (0)
#endif
struct acpi_integer *acpi_integer_open(uint64_t value);
struct acpi_string *acpi_string_open(const char *value);
struct acpi_buffer *acpi_buffer_open(const uint8_t *value, acpi_size_t length);

acpi_status_t acpi_evaluate_object(acpi_handle_t handle, char *path,
				   uint8_t nr_arguments,
				   struct acpi_operand **arguments,
				   struct acpi_operand **result);

/*=========================================================================
 * Checksum validations
 *=======================================================================*/
uint8_t acpi_checksum_calc(uint8_t *buffer, uint32_t length);
/* checksum validation for all types of tables */
boolean __acpi_table_checksum_valid(struct acpi_table_header *table);
void acpi_table_calc_checksum(struct acpi_table_header *table);
/* checksum validation for non RSDP tables */
boolean acpi_table_checksum_valid(struct acpi_table_header *table);
uint32_t acpi_table_get_length(struct acpi_table_header *table);
/* checksum validation for RSDP tables */
void acpi_rsdp_calc_checksum(struct acpi_table_rsdp *rsdp);
boolean acpi_rsdp_checksum_valid(struct acpi_table_rsdp *rsdp);

/*=========================================================================
 * Event (callback handler) externals
 *=======================================================================*/
#define ACPI_EVENT_TABLE_INSTALL	0x0
#define ACPI_EVENT_TABLE_UNINSTALL	0x1
#define ACPI_EVENT_TABLE_LOAD		0x2
#define ACPI_EVENT_TABLE_UNLOAD		0x3
#define ACPI_EVENT_TABLE_MAX		0x3
#define ACPI_NR_TABLE_EVENTS		(ACPI_EVENT_TABLE_MAX+1)

#define ACPI_EVENT_SPACE_CREATE		0x0
#define ACPI_EVENT_SPACE_DELETE		0x1
#define ACPI_EVENT_SPACE_MAX		0x1
#define ACPI_NR_SPACE_EVENTS		(ACPI_EVENT_SPACE_MAX+1)

typedef acpi_status_t (*acpi_event_table_cb)(struct acpi_table_desc *table,
					     acpi_ddb_t ddb,
					     uint32_t event,
					     void *context);
typedef acpi_status_t (*acpi_event_space_cb)(struct acpi_namespace_node *node,
					     uint32_t event,
					     void *context);

acpi_status_t acpi_event_register_table_handler(acpi_event_table_cb handler,
						void *context);
void acpi_event_unregister_table_handler(acpi_event_table_cb handler);
void acpi_event_table_notify(struct acpi_table_desc *table_desc,
			     acpi_ddb_t ddb, uint32_t event);

acpi_status_t acpi_event_register_space_handler(acpi_event_space_cb handler,
						void *context);
void acpi_event_unregister_space_handler(acpi_event_space_cb handler);
void acpi_event_space_notify(struct acpi_namespace_node *node, uint32_t event);

#ifdef CONFIG_ACPI_DEBUG
void acpi_debug_opcode_info(const struct acpi_opcode_info *op_info,
			    const char *hint);
void acpi_dump_opcode_info(void);
void acpi_space_test_nodes(void);
#else
static inline void acpi_debug_opcode_info(const struct acpi_opcode_info *op_info,
					  const char *hint)
{
}
#define acpi_dump_opcode_info()
#define acpi_space_test_nodes()
#endif

#ifdef CONFIG_ACPI_BIOS
void acpi_bios_init(void);
void acpi_bios_install_table(void *table);
#else
#define acpi_bios_init()		do { } while (0)
#define acpi_bios_install_table(table)	do { } while (0)
#endif
#ifdef CONFIG_SBI_EFI
void acpi_fixups(char *oem, char *oem_table);
char *acpi_get_vid(void);
char *acpi_get_pid(void);
#else
#define acpi_fixups(oem, oem_table)	do { } while (0)
#define acpi_get_vid()			NULL
#define acpi_get_pid()			NULL
#endif
#endif /* __ASSEMBLY__ */

#include <target/acpi_sdt.h>

#endif /* __ACPI_H_INCLUDE__ */

