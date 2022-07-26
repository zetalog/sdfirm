#ifndef __ARCH_GUANGMU_H_INCLUDE__
#define __ARCH_GUANGMU_H_INCLUDE__

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#ifndef __ASSEMBLY__
#define __GM_TEST_ADDR		0x100000
#ifdef CONFIG_MMU
#define GM_TEST_ADDR		gm_test_reg_addr
extern caddr_t gm_test_reg_addr;
#else
#define GM_TEST_ADDR		__GM_TEST_ADDR
#endif
#define GM_TEST_FINISHER_FAIL	0x3333
#define GM_TEST_FINISHER_PASS	0x5555
#define gm_finish_pass()	__raw_writel(GM_TEST_FINISHER_PASS, GM_TEST_ADDR)
#define gm_finish_fail()	__raw_writel(GM_TEST_FINISHER_FAIL, GM_TEST_ADDR)
#endif

#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
	.macro	boot0_hook
	jal	ra, vaisra_cpu_init
	.endm
#endif

#endif /* __ARCH_GUANGMU_H_INCLUDE__ */
