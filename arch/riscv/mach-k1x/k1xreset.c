#include <target/arch.h>
#include <target/delay.h>

#define BOOT_CLUSTER	CPU_TO_CLUSTER(BOOT_HART)
#define BOOT_ADDR	CONFIG_BOOT_BASE

void k1x_cpu_reset(void)
{
#if CONFIG_K1X_SMP_CPUS > 1
	__raw_writel(LODWORD(BOOT_ADDR), (caddr_t)0xD4282DB0);
	__raw_writel(HIDWORD(BOOT_ADDR), (caddr_t)0xD4282DB4);
	__raw_writel(1 << 1, 0xD428292C);
	__raw_writel(1 << 2, 0xD428292C);
	__raw_writel(1 << 3, 0xD428292C);
#if CONFIG_K1X_SMP_CPUS > 4
	__raw_writel(LODWORD(BOOT_ADDR), (caddr_t)(0xD4282C00 + 0x2B0));
	__raw_writel(HIDWORD(BOOT_ADDR), (caddr_t)(0xD4282C00 + 0x2B4));
	__raw_writel(1 << 4, 0xD428292C);
	__raw_writel(1 << 5, 0xD428292C);
	__raw_writel(1 << 6, 0xD428292C);
	__raw_writel(1 << 7, 0xD428292C);
#endif
#endif
}
