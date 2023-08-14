#include <target/arch.h>

void cnt_enable_gcounter(void)
{
    __raw_writel(1, 0xD5001000);
}

int cnt_status_gcounter(void)
{
    return __raw_readl(0xD5001000);
}