#ifndef __ENDIAN_H_INCLUDE__
#define __ENDIAN_H_INCLUDE__

#ifdef CONFIG_ARCH_IS_BIG_ENDIAN
#define ENDIAN_BE		1
#undef ENDIAN_LE
#elif defined(__BIG_ENDIAN)
#define ENDIAN_BE		1
#undef ENDIAN_LE
#else
#undef ENDIAN_BE
#define ENDIAN_LE		1
#endif

#ifdef ENDIAN_BE
#define ENDIAN_BIG		__BIG_ENDIAN
#define ENDIAN_LITTLE		1234
#include <target/big_endian.h>
#endif
#ifdef ENDIAN_LE
#define ENDIAN_BIG		4321
#define ENDIAN_LITTLE		__LITTLE_ENDIAN
#include <target/little_endian.h>
#endif

#define le64_to_cpu(x)		__le64_to_cpu(x)
#define cpu_to_le64(x)		__cpu_to_le64(x)
#define le32_to_cpu(x)		__le32_to_cpu(x)
#define cpu_to_le32(x)		__cpu_to_le32(x)
#define le16_to_cpu(x)		__le16_to_cpu(x)
#define cpu_to_le16(x)		__cpu_to_le16(x)
#define be64_to_cpu(x)		__be64_to_cpu(x)
#define cpu_to_be64(x)		__cpu_to_be64(x)
#define be32_to_cpu(x)		__be32_to_cpu(x)
#define cpu_to_be32(x)		__cpu_to_be32(x)
#define be16_to_cpu(x)		__be16_to_cpu(x)
#define cpu_to_be16(x)		__cpu_to_be16(x)

#if __riscv_xlen == 64
#define cpu_to_lle		cpu_to_le64
#define lle_to_cpu		le64_to_cpu
#elif __riscv_xlen == 32
#define cpu_to_lle		cpu_to_le32
#define lle_to_cpu		le32_to_cpu
#else
#error "Unknown __riscv_xlen"
#endif

#endif /* __ENDIAN_H_INCLUDE__ */
