#include <stdio.h>
#include <target/irq.h>

#define ESR_ELx_EC_UNKNOWN  (0x00)
#define ESR_ELx_EC_WFx      (0x01)
/* Unallocated EC: 0x02 */
#define ESR_ELx_EC_CP15_32  (0x03)
#define ESR_ELx_EC_CP15_64  (0x04)
#define ESR_ELx_EC_CP14_MR  (0x05)
#define ESR_ELx_EC_CP14_LS  (0x06)
#define ESR_ELx_EC_FP_ASIMD (0x07)
#define ESR_ELx_EC_CP10_ID  (0x08)
/* Unallocated EC: 0x09 - 0x0B */
#define ESR_ELx_EC_CP14_64  (0x0C)
/* Unallocated EC: 0x0d */
#define ESR_ELx_EC_ILL      (0x0E)
/* Unallocated EC: 0x0F - 0x10 */
#define ESR_ELx_EC_SVC32    (0x11)
#define ESR_ELx_EC_HVC32    (0x12)
#define ESR_ELx_EC_SMC32    (0x13)
/* Unallocated EC: 0x14 */
#define ESR_ELx_EC_SVC64    (0x15)
#define ESR_ELx_EC_HVC64    (0x16)
#define ESR_ELx_EC_SMC64    (0x17)
#define ESR_ELx_EC_SYS64    (0x18)
/* Unallocated EC: 0x19 - 0x1E */
#define ESR_ELx_EC_IMP_DEF  (0x1f)
#define ESR_ELx_EC_IABT_LOW (0x20)
#define ESR_ELx_EC_IABT_CUR (0x21)
#define ESR_ELx_EC_PC_ALIGN (0x22)
/* Unallocated EC: 0x23 */
#define ESR_ELx_EC_DABT_LOW (0x24)
#define ESR_ELx_EC_DABT_CUR (0x25)
#define ESR_ELx_EC_SP_ALIGN (0x26)
/* Unallocated EC: 0x27 */
#define ESR_ELx_EC_FP_EXC32 (0x28)
/* Unallocated EC: 0x29 - 0x2B */
#define ESR_ELx_EC_FP_EXC64 (0x2C)
/* Unallocated EC: 0x2D - 0x2E */
#define ESR_ELx_EC_SERROR   (0x2F)
#define ESR_ELx_EC_BREAKPT_LOW  (0x30)
#define ESR_ELx_EC_BREAKPT_CUR  (0x31)
#define ESR_ELx_EC_SOFTSTP_LOW  (0x32)
#define ESR_ELx_EC_SOFTSTP_CUR  (0x33)
#define ESR_ELx_EC_WATCHPT_LOW  (0x34)
#define ESR_ELx_EC_WATCHPT_CUR  (0x35)
/* Unallocated EC: 0x36 - 0x37 */
#define ESR_ELx_EC_BKPT32   (0x38)
/* Unallocated EC: 0x39 */
#define ESR_ELx_EC_VECTOR32 (0x3A)
/* Unallocted EC: 0x3B */
#define ESR_ELx_EC_BRK64    (0x3C)
/* Unallocated EC: 0x3D - 0x3F */
#define ESR_ELx_EC_MAX      (0x3F)

#define ESR_ELx_EC_SHIFT    (26)
#define ESR_ELx_EC_MASK     (0x3FUL << ESR_ELx_EC_SHIFT)
#define ESR_ELx_EC(esr)     (((esr) & ESR_ELx_EC_MASK) >> ESR_ELx_EC_SHIFT)

static const char *esr_class_str[ESR_ELx_EC_MAX + 1] = {
//  [0 ... ESR_ELx_EC_MAX]      = "UNRECOGNIZED EC",
    [ESR_ELx_EC_UNKNOWN]        = "Unknown/Uncategorized",
    [ESR_ELx_EC_WFx]            = "WFI/WFE",
    [ESR_ELx_EC_CP15_32]        = "CP15 MCR/MRC",
    [ESR_ELx_EC_CP15_64]        = "CP15 MCRR/MRRC",
    [ESR_ELx_EC_CP14_MR]        = "CP14 MCR/MRC",
    [ESR_ELx_EC_CP14_LS]        = "CP14 LDC/STC",
    [ESR_ELx_EC_FP_ASIMD]       = "ASIMD",
    [ESR_ELx_EC_CP10_ID]        = "CP10 MRC/VMRS",
    [ESR_ELx_EC_CP14_64]        = "CP14 MCRR/MRRC",
    [ESR_ELx_EC_ILL]            = "PSTATE.IL",
    [ESR_ELx_EC_SVC32]          = "SVC (AArch32)",
    [ESR_ELx_EC_HVC32]          = "HVC (AArch32)",
    [ESR_ELx_EC_SMC32]          = "SMC (AArch32)",
    [ESR_ELx_EC_SVC64]          = "SVC (AArch64)",
    [ESR_ELx_EC_HVC64]          = "HVC (AArch64)",
    [ESR_ELx_EC_SMC64]          = "SMC (AArch64)",
    [ESR_ELx_EC_SYS64]          = "MSR/MRS (AArch64)",
    [ESR_ELx_EC_IMP_DEF]        = "EL3 IMP DEF",
    [ESR_ELx_EC_IABT_LOW]       = "IABT (lower EL)",
    [ESR_ELx_EC_IABT_CUR]       = "IABT (current EL)",
    [ESR_ELx_EC_PC_ALIGN]       = "PC Alignment",
    [ESR_ELx_EC_DABT_LOW]       = "DABT (lower EL)",
    [ESR_ELx_EC_DABT_CUR]       = "DABT (current EL)",
    [ESR_ELx_EC_SP_ALIGN]       = "SP Alignment",
    [ESR_ELx_EC_FP_EXC32]       = "FP (AArch32)",
    [ESR_ELx_EC_FP_EXC64]       = "FP (AArch64)",
    [ESR_ELx_EC_SERROR]         = "SError",
    [ESR_ELx_EC_BREAKPT_LOW]    = "Breakpoint (lower EL)",
    [ESR_ELx_EC_BREAKPT_CUR]    = "Breakpoint (current EL)",
    [ESR_ELx_EC_SOFTSTP_LOW]    = "Software Step (lower EL)",
    [ESR_ELx_EC_SOFTSTP_CUR]    = "Software Step (current EL)",
    [ESR_ELx_EC_WATCHPT_LOW]    = "Watchpoint (lower EL)",
    [ESR_ELx_EC_WATCHPT_CUR]    = "Watchpoint (current EL)",
    [ESR_ELx_EC_BKPT32]         = "BKPT (AArch32)",
    [ESR_ELx_EC_VECTOR32]       = "Vector catch (AArch32)",
    [ESR_ELx_EC_BRK64]          = "BRK (AArch64)",
};

static const char *handler[]= {
	"Synchronous Abort",
	"IRQ",
	"FIQ",
	"Error"
};

static const char *esr_get_class_string(unsigned int esr)
{
	if (esr_class_str[ESR_ELx_EC(esr)])
		return esr_class_str[ESR_ELx_EC(esr)];
	return "UNRECOGNIZED EC";
}

static void show_regs(struct pt_regs *regs)
{
	int i;

	printf("SPSR:    %08lx\n", regs->spsr);
	printf("ELR:     %lx\n", regs->elr);
	printf("LR:      %lx\n", regs->regs[30]);
	for (i = 0; i < 29; i += 2)
		printf("x%-2d: %016lx x%-2d: %016lx\n",
		       i, regs->regs[i], i+1, regs->regs[i+1]);
	printf("\n");
}

static trap_handler _handler = (trap_handler)0;

void trap_register_handler(trap_handler entry)
{
	_handler = entry;
}

uint64_t get_midr(void)
{
	uint64_t midr = 0;

	asm volatile (
	"mrs	%0, MIDR_EL1 \n\t"
	"orr	%0, %0, #0x4000 \n\t"	/* set bit 14 */
	"bfxil	%0, xzr, #0, #4 \n\t"	/* clear bits 3:0 */
        : "=r" (midr)::
	);
	return midr;
}

void bad_mode(struct pt_regs *regs, unsigned int esr, int reason)
{
	unsigned long long midr = get_midr();
	unsigned long cid = 0;

	if (midr == FALKOR_MIDR) {
		if (_handler) {
			_handler(esr_get_class_string(esr));
			return;
		} else {
#if 0
			cid = plat_my_core_pos();
#endif
			printf("\nBad mode in %s handler detected on FALKOR%ld, code 0x%08x -- %s\n",
			       handler[reason], cid, esr, esr_get_class_string(esr));
		}
	} else {
		printf("\nBad mode in %s handler detected on IMC, code 0x%08x -- %s\n",
		       handler[reason], esr, esr_get_class_string(esr));
	}
	show_regs(regs);
}
