#include <target/mtd.h>
#include <asm/mach/flash.h>

#define FLASH_HW_PROTECT_SIZE		0x00000800
#define FLASH_HW_BUFFER_WORDS		32
#define FLASH_HW_BUFFER_SIZE		(FLASH_HW_BUFFER_WORDS*FLASH_HW_WORD_SIZE)

#define __flash_hw_unraise_irq(irq)	__raw_clearl_atomic(irq, FCMISC)
#define __flash_hw_raised_irq(irq)	__raw_testl_atomic(irq, FCRIS)
#define __flash_hw_op_execute(op)	__raw_writel((FLASH_WRITE_KEY<<WRKEY) | _BV(op), FMC)
#define __flash_hw_op_complete(op)	(!__raw_testl_atomic(op, FMC))
#define __flash_hw_op2_execute(op)	__raw_writel((FLASH_WRITE_KEY<<WRKEY) | _BV(op), FMC2)
#define __flash_hw_op2_complete(op)	(!__raw_testl_atomic(op, FMC2))
#define __flash_hw_write_addr(addr)	__raw_writel(addr, FMA)
#define __flash_hw_write_data(data)	__raw_writel(data, FMD)
#define __flash_hw_write_wbuf(n, data)	__raw_writel(data, FWB(n))
#define __flash_hw_has_wbuf()		__raw_testl_atomic(NVFWB, NVMSTAT)
#define __flash_hw_set_usec(clocks)	__raw_writel(clocks - 1, USECRL)
#define __flash_hw_get_usec()		__raw_readl(USECRL)

void flash_hw_op_perform(uint32_t addr, uint8_t op)
{
	__flash_hw_write_addr(addr);
	__flash_hw_op_execute(op);
	while (!__flash_hw_op_complete(op));
}

void flash_hw_erase_page(mtd_addr_t addr)
{
	BUG_ON(addr & (FLASH_HW_PAGE_SIZE-1));
	__flash_hw_unraise_irq(AI);
	flash_hw_op_perform(addr, ERASE);
}

boolean flash_hw_access_failure(void)
{
	return __flash_hw_raised_irq(AI);
}

void flash_hw_program_words(mtd_addr_t addr, mtd_size_t size,
			    flash_word_t *pdata)
{
	BUG_ON(addr & (FLASH_HW_WORD_SIZE-1));
	BUG_ON(size & (FLASH_HW_WORD_SIZE-1));
	
	__flash_hw_unraise_irq(AI);
	
	if (__flash_hw_has_wbuf()) {
		while (size) {
			uint8_t n;

			/* Set the address of this block of words. */
			__flash_hw_write_addr(addr & ~(FLASH_HW_BUFFER_SIZE-1));
			n = ((addr & (FLASH_HW_BUFFER_SIZE-1)) >> FLASH_HW_WORD_WEIGHT);

			/* Loop over the words in this 32-word block. */
			while ((n < FLASH_HW_BUFFER_WORDS) && (size != 0)) {
				__flash_hw_write_wbuf(n, *pdata);
				pdata++;
				addr += FLASH_HW_WORD_SIZE;
				size -= FLASH_HW_WORD_SIZE;
				n++;
			}
			
			__flash_hw_op2_execute(WRBUF);
			while (!__flash_hw_op2_complete(WRBUF));
		}
	} else {
		while (size) {
			__flash_hw_write_addr(addr);
			__flash_hw_write_data(*pdata);

			__flash_hw_op_execute(WRITE);
			while (!__flash_hw_op_complete(WRITE));
			
			pdata++;
			addr += FLASH_HW_WORD_SIZE;
			size -= FLASH_HW_WORD_SIZE;
		}
	}
}

uint32_t flash_hw_get_user(uint8_t n)
{
	return __raw_readl(USER_REG(n));
}

void flash_hw_set_user(uint32_t val)
{
	__raw_writel(val, USER_REG(n));
}

void flash_hw_ctrl_init(void)
{
	/* TODO: What's Flash Programming Timing?
	 *
	 * Following codes are copied from the sample code, but we could
	 * not find any description of USECRL in the datasheet.
	 * Just leave it here for those knowledgeables.
	 */
	/* __flash_hw_set_usec(div32u(mul16u(CLK_MOSC, 1000)+999999, 1000000)); */
}
