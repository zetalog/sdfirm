#ifndef __ENDIAN_LE_H_INCLUDE__
#define __ENDIAN_LE_H_INCLUDE__

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif

#define __cpu_to_le16(x)	((uint16_t)(x))
#define __le16_to_cpu(x)	((uint16_t)(x))
#define __cpu_to_be16(x)	(swab16((uint16_t)(x)))
#define __be16_to_cpu(x)	(swab16((uint16_t)(x)))
#define __cpu_to_le32(x)	((uint32_t)(x))
#define __le32_to_cpu(x)	((uint32_t)(x))
#define __cpu_to_be32(x)	(swab32((uint32_t)(x)))
#define __be32_to_cpu(x)	(swab32((uint32_t)(x)))
#define __cpu_to_le64(x)	((uint64_t)(x))
#define __le64_to_cpu(x)	((uint64_t)(x))
#define __cpu_to_be64(x)	(swab64((uint64_t)(x)))
#define __be64_to_cpu(x)	(swab64((uint64_t)(x)))

#endif /* __ENDIAN_LE_H_INCLUDE__ */
