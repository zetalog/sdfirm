#include <target/arch.h>

#define BOOT_CLUSTER	CPU_TO_CLUSTER(BOOT_HART)

void k1x_cpu_reset(void)
{
	__raw_writel(0x40000000, (uint32_t *)0xD4282DB0);
	__raw_writel((0x40000000 >> 32) & 0xffffffff, (uint32_t *)0xD4282DB4);
	__raw_writel(1 << 1, 0xD428292C);
	__raw_writel(1 << 2, 0xD428292C);
	__raw_writel(1 << 3, 0xD428292C);

	// __raw_writel(0x40000000, (uint32_t *)(0xD4282C00 + 0x2B0));
	// __raw_writel((0x40000000 >> 32) & 0xffffffff, (uint32_t *)(0xD4282C00 + 0x2B4));
	// __raw_writel(1 << 4, 0xD428292C);
	// __raw_writel(1 << 5, 0xD428292C);
	//__raw_writel(1 << 6, 0xD428292C);
	//__raw_writel(1 << 7, 0xD428292C);
}