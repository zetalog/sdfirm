#ifndef __ENDIAN_BE_H_INCLUDE__
#define __ENDIAN_BE_H_INCLUDE__

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN 4321
#endif
#define __constant_cpu_to_le32(x) (___constant_swab32((uint32_t)(x)))
#define __constant_le32_to_cpu(x) (___constant_swab32((uint32_t)(x)))
#define __constant_cpu_to_be32(x) ((uint32_t)(x))
#define __constant_be32_to_cpu(x) ((uint32_t)(x))

#define __cpu_to_le32(x) (__swab32((uint32_t)(x)))
#define __le32_to_cpu(x) (__swab32((uint32_t)(x)))
#define __cpu_to_be32(x) ((uint32_t)(x))
#define __be32_to_cpu(x) ((uint32_t)(x))

#endif /* __ENDIAN_BE_H_INCLUDE__ */
