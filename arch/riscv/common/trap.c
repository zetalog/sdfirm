#include <target/irq.h>
#include <target/panic.h>
#include <stdio.h>

void __attribute__((noreturn))
bad_trap(uintptr_t* regs, uintptr_t dummy, uintptr_t mepc)
{
	printf("machine: unhandlable trap %d @ %p\n",
	       csr_read(CSR_MCAUSE), mepc);
	BUG();
	while (1) wait_irq();
}

#ifndef CONFIG_ARCH_HAS_NOSEE
extern void __redirect_trap();
void redirect_trap(uintptr_t epc, uintptr_t mstatus, uintptr_t badaddr)
{
	uintptr_t new_mstatus;
	uintptr_t mpp_s;

	csr_write(CSR_STVAL, badaddr);
	csr_write(CSR_SEPC, epc);
	csr_write(CSR_SCAUSE, csr_read(MCAUSE));
	csr_write(CSR_MEPC, csr_read(STVEC));

	new_mstatus = mstatus & ~(SR_SPP | SR_SPIE | SR_SIE);
	mpp_s = SR_MPP & (SR_MPP >> 1);
	new_mstatus |= (mstatus * (SR_SPIE / SR_SIE)) & SR_SPIE;
	new_mstatus |= (mstatus / (mpp_s / SR_SPP)) & SR_SPP;
	new_mstatus |= mpp_s;
	csr_write(CSR_MSTATUS, new_mstatus);
	return __redirect_trap();
}
#else
void redirect_trap(uintptr_t epc, uintptr_t mstatus, uintptr_t badaddr)
{
}
#endif

void pmp_trap(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc)
{
	redirect_trap(mepc, csr_read(CSR_MSTATUS), csr_read(CSR_MTVAL));
}

void illegal_insn_trap(uintptr_t epc, uintptr_t mstatus, uintptr_t badaddr)
{
	printf("machine: illegal instruction\n");
	BUG();
	while (1) wait_irq();
}

void mcall_trap(uintptr_t epc, uintptr_t mstatus, uintptr_t badaddr)
{
	printf("machine: mcall\n");
}

void trap_from_machine_mode(uintptr_t epc, uintptr_t mstatus, uintptr_t badaddr)
{
#if 0
	printf("machine: trap from machine mode\n");
	BUG();
	while (1) wait_irq();
#endif
}

union byte_array {
	uint8_t bytes[8];
	uintptr_t intx;
	uint64_t int64;
};

void misaligned_load_trap(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc)
{
#if 0
	union byte_array val;
	uintptr_t mstatus;
	insn_t insn = get_insn(mepc, &mstatus);
	uintptr_t npc = mepc + insn_len(insn);
	uintptr_t addr = read_csr(mbadaddr);
	int shift = 0, fp = 0, len;

	if ((insn & MASK_LW) == MATCH_LW)
		len = 4, shift = 8*(sizeof(uintptr_t) - len);
#if __riscv_xlen == 64
	else if ((insn & MASK_LD) == MATCH_LD)
		len = 8, shift = 8*(sizeof(uintptr_t) - len);
	else if ((insn & MASK_LWU) == MATCH_LWU)
		len = 4;
#endif
	else if ((insn & MASK_LH) == MATCH_LH)
		len = 2, shift = 8*(sizeof(uintptr_t) - len);
	else if ((insn & MASK_LHU) == MATCH_LHU)
		len = 2;
#ifdef __riscv_compressed
#if __riscv_xlen >= 64
	else if ((insn & MASK_C_LD) == MATCH_C_LD)
		len = 8, shift = 8*(sizeof(uintptr_t) - len), insn = RVC_RS2S(insn) << SH_RD;
	else if ((insn & MASK_C_LDSP) == MATCH_C_LDSP && ((insn >> SH_RD) & 0x1f))
		len = 8, shift = 8*(sizeof(uintptr_t) - len);
#endif
	else if ((insn & MASK_C_LW) == MATCH_C_LW)
		len = 4, shift = 8*(sizeof(uintptr_t) - len), insn = RVC_RS2S(insn) << SH_RD;
	else if ((insn & MASK_C_LWSP) == MATCH_C_LWSP && ((insn >> SH_RD) & 0x1f))
		len = 4, shift = 8*(sizeof(uintptr_t) - len);
#endif
	else {
		mcause = CAUSE_LOAD_ACCESS;
		csr_write(CSR_MCAUSE, mcause);
		return truly_illegal_insn(regs, mcause, mepc, mstatus, insn);
	}

	val.int64 = 0;
	for (intptr_t i = 0; i < len; i++)
		val.bytes[i] = load_uint8_t((void *)(addr + i), mepc);

	if (!fp)
		SET_RD(insn, regs, (intptr_t)val.intx << shift >> shift);
	else if (len == 8)
		SET_F64_RD(insn, regs, val.int64);
	else
		SET_F32_RD(insn, regs, val.intx);
	csr_write(CSR_MEPC, npc);
#endif
}

void misaligned_store_trap(uintptr_t *regs, uintptr_t mcause, uintptr_t mepc)
{
#if 0
	union byte_array val;
	uintptr_t mstatus;
	insn_t insn = get_insn(mepc, &mstatus);
	uintptr_t npc = mepc + insn_len(insn);
	int len, i;
	uintptr_t addr;

	val.intx = GET_RS2(insn, regs);
	if ((insn & MASK_SW) == MATCH_SW)
		len = 4;
#if __riscv_xlen == 64
	else if ((insn & MASK_SD) == MATCH_SD)
		len = 8;
#endif
	else if ((insn & MASK_SH) == MATCH_SH)
		len = 2;
#ifdef __riscv_compressed
#if __riscv_xlen >= 64
	else if ((insn & MASK_C_SD) == MATCH_C_SD)
		len = 8, val.intx = GET_RS2S(insn, regs);
	else if ((insn & MASK_C_SDSP) == MATCH_C_SDSP && ((insn >> SH_RD) & 0x1f))
		len = 8, val.intx = GET_RS2C(insn, regs);
#endif
	else if ((insn & MASK_C_SW) == MATCH_C_SW)
		len = 4, val.intx = GET_RS2S(insn, regs);
	else if ((insn & MASK_C_SWSP) == MATCH_C_SWSP && ((insn >> SH_RD) & 0x1f))
		len = 4, val.intx = GET_RS2C(insn, regs);
#endif
	else {
		mcause = CAUSE_STORE_ACCESS;
		write_csr(mcause, mcause);
		return truly_illegal_insn(regs, mcause, mepc, mstatus, insn);
	}

	addr = csr_read_csr(CSR_MTVAL);
	for (i = 0; i < len; i++)
		store_uint8_t((void *)(addr + i), val.bytes[i], mepc);
	csr_write(CSR_MEPC, npc);
#endif
}
