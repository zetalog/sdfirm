#include <target/paging.h>
#include <target/gpio.h>
#include <target/clk.h>
#include <target/console.h>

caddr_t sifive_prci_reg_base = PRCI_BASE;
caddr_t sifive_gpio_reg_base = GPIO_BASE;
caddr_t sifive_uart_reg_base[SIFIVE_MAX_UART_PORTS] = {
	__SIFIVE_UART_BASE(0),
	__SIFIVE_UART_BASE(1),
};

void sifive_mmu_dump_maps(void)
{
	if (sifive_prci_reg_base != PRCI_BASE)
		printf("FIXMAP: %016llx -> %016llx: PRCI\n",
		       (uint64_t)PRCI_BASE, fix_to_virt(FIX_PRCI));
	if (sifive_gpio_reg_base != GPIO_BASE)
		printf("FIXMAP: %016llx -> %016llx: GPIO\n",
		       (uint64_t)GPIO_BASE, fix_to_virt(FIX_GPIO));
	if (sifive_uart_reg_base[0] != UART0_BASE)
		printf("FIXMAP: %016llx -> %016llx: UART0\n",
		       (uint64_t)UART0_BASE, fix_to_virt(FIX_UART));
	if (sifive_uart_reg_base[1] != UART1_BASE)
		printf("FIXMAP: %016llx -> %016llx: UART1\n",
		       (uint64_t)UART1_BASE, fix_to_virt(FIX_UART));
}

void sifive_mmu_map_gpio(void)
{
	if (sifive_gpio_reg_base == GPIO_BASE) {
		set_fixmap_io(FIX_GPIO, GPIO_BASE & PAGE_MASK);
		sifive_gpio_reg_base = fix_to_virt(FIX_GPIO);
	}
}

void sifive_mmu_map_prci(void)
{
	if (sifive_prci_reg_base == PRCI_BASE) {
		set_fixmap_io(FIX_PRCI, PRCI_BASE & PAGE_MASK);
		sifive_prci_reg_base = fix_to_virt(FIX_PRCI);
	}
}

void sifive_mmu_map_uart(int n)
{
	if (sifive_uart_reg_base[n] == __SIFIVE_UART_BASE(n)) {
		set_fixmap_io(FIX_UART, __SIFIVE_UART_BASE(n) & PAGE_MASK);
		sifive_uart_reg_base[n] = fix_to_virt(FIX_UART);
	}
}
