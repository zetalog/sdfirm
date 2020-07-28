#include <target/ras.h>
#include <target/sbi.h>

#ifdef CONFIG_SBI
void sbi_process_vaisra_nmi(void)
{
	sbi_printf("IRQ_M_NMI\n");
}
#endif /* CONFIG_SBI */
