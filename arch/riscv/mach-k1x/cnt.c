#include <target/arch.h>

void cnt_enable_gcounter(void)
{
    #ifdef CONFIG_K1M_K1X
    __raw_writel(1, 0xD5001000);
    #endif
}

int cnt_status_gcounter(void)
{
    #ifdef CONFIG_K1M_K1X
    return __raw_readl(0xD5001000);
    #else
    return 1;
    #endif
}
