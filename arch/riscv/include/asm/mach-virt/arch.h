#ifndef __ARCH_VIRT_H_INCLUDE__
#define __ARCH_VIRT_H_INCLUDE__

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#ifndef __ASSEMBLY__
#define __VIRT_TEST_ADDR		0x100000
#ifdef CONFIG_MMU
#define VIRT_TEST_ADDR			virt_test_reg_addr
extern caddr_t virt_test_reg_addr;
#else
#define VIRT_TEST_ADDR			__VIRT_TEST_ADDR
#endif
#define VIRT_TEST_FINISHER_FAIL		0x3333
#define VIRT_TEST_FINISHER_PASS		0x5555
#define virt_finish_pass()		__raw_writel(VIRT_TEST_FINISHER_PASS, VIRT_TEST_ADDR)
#define virt_finish_fail()		__raw_writel(VIRT_TEST_FINISHER_FAIL, VIRT_TEST_ADDR)
#endif

#endif /* __ARCH_VIRT_H_INCLUDE__ */
