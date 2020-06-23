#include <target/init.h>
#include <target/arch.h>

ENTRY(vaisra_sim_shutdown)
	j	test_section_end
ENTRY(test_section_end)
#ifdef CONFIG_VAISRA_SPIKE
	li a0, CONFIG_VAISRA_SPIKE_MIMPID
	csrr a1, CSR_MIMPID
	beq a0, a1, htif_poweroff
#endif
END(test_section_end)
#ifdef CONFIG_VAISRA_OVPSIM
ENTRY(write_tohost)
	j	write_tohost
END(write_tohost)
#endif
	ret
ENDPROC(vaisra_sim_shutdown)
