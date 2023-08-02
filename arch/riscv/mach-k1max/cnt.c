void g_counter_enable(void){
    __raw_writel(1, 0xD5001000);
}