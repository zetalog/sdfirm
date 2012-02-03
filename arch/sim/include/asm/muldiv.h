#ifndef __MULDIV_SIM_H_INCLUDE__
#define __MULDIV_SIM_H_INCLUDE__

#define div16u(x, y)	((uint16_t)((x)/(y)))
#define ARCH_HAVE_DIV16U 1

#define div32u(x, y)	((uint32_t)((x)/(y)))
#define ARCH_HAVE_DIV32U 1

#define mul16u(x, y)	(uint32_t)((((uint32_t)(x))*((uint32_t)(y))))
#define ARCH_HAVE_MUL16U 1

#define mod32u(x, y)	((uint32_t)((x)%(y)))
#define ARCH_HAVE_MOD32U 1

#endif /* __MULDIV_SIM_H_INCLUDE__ */
