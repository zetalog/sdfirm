#include <target/generic.h>
#include <target/arch.h>
#include <target/wdt.h>
#include <target/irq.h>
#include <target/heap.h>
#include <target/mtd.h>
#include <target/dataflash.h>

extern caddr_t __bss_end[];

void clk_hw_set_cpu(void)
{
	irq_flags_t flags;

	irq_local_save(flags);
	__clk_hw_set_prescale(CLK_DIVISOR);
	irq_local_restore(flags);
}

static void clk_init(void)
{
	clk_hw_set_cpu();
}

void mem_init(void)
{
	heap_range_init((caddr_t)__bss_end);
}

#ifdef CONFIG_MTD_DATAFLASH
mtd_t board_flashes[DATAFLASH_MAX_BANKS];
uint8_t nr_board_flashes = 0;
mtd_t board_flash = INVALID_MTD_ID;

void board_flash_chip(uint8_t chip)
{
	board_flashes[nr_board_flashes] = dataflash_register_bank(chip);
	nr_board_flashes++;
}

void board_flash_init(void)
{
	board_flash_chip(SPI_CS_0);
#if DATAFLASH_MAX_BANKS > 1
	board_flash_chip(SPI_CS_1);
	board_flash = mtd_concat_devices(board_flashes, nr_board_flashes);
#else
	board_flash = board_flashes[0];
#endif
	mtd_block_create(board_flash);
}
#else
#define board_flash_init()
#endif

void board_early_init(void)
{
	clk_init();
	mem_init();
	wdt_ctrl_stop();
	DEVICE_ARCH(DEVICE_ARCH_AVR);
}

void board_late_init(void)
{
	board_flash_init();
	/* board_blkdev = mtd_register_blkdev(board_flash); */
	/* board_lun = sbc_mount_device(board_blkdev); */
}
