#include <target/generic.h>
#include <target/arch.h>
#include <target/wdt.h>
#include <target/irq.h>
#include <target/heap.h>

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

void board_init(void)
{
	clk_init();
	mem_init();
	wdt_ctrl_stop();
	DEVICE_ARCH(DEVICE_ARCH_AVR);
}

#include <target/mtd.h>
#include <target/dataflash.h>

#ifdef CONFIG_MTD_DATAFLASH
mtd_t board_flashes[DATAFLASH_MAX_BANKS];
mtd_t board_mtd = INVALID_MTD_ID;
uint8_t nr_board_flashes = 0;

void appl_dataflash_chip(uint8_t chip)
{
	board_flashes[nr_board_flashes] = dataflash_register_bank(chip);
	nr_board_flashes++;
}

void appl_dataflash_init(void)
{
	appl_dataflash_chip(SPI_CHIP_DATAFLASH0);
#if DATAFLASH_MAX_BANKS > 1
	appl_dataflash_chip(SPI_CHIP_DATAFLASH1);
	board_mtd = mtd_concat_devices(board_flashes, nr_board_flashes);
#else
	board_mtd = board_flashes[0];
#endif
	mtd_block_create(board_mtd);
}
#else
#define appl_dataflash_init()
#endif

#ifdef CONFIG_MTD_BLOCK
void appl_block_init(void)
{
	/* board_blkdev = mtd_register_blkdev(board_mtd); */
}
#else
#define appl_block_init()
#endif

#ifdef CONFIG_SCSI_SBC
void appl_scsi_init(void)
{
	/* board_lun = sbc_mount_device(board_blkdev); */
}
#else
#define appl_scsi_init()
#endif

void appl_init(void)
{
	appl_dataflash_init();
	appl_block_init();
	appl_scsi_init();
}
