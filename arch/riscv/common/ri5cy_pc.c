#include <target/perf.h>

uint32_t ri5cy_get_count(uint8_t event)
{
	switch (event) {
	case PC_CYCLES:
		return csr_read(CSR_PCCR(0));
	case PC_INSTR:
		return csr_read(CSR_PCCR(1));
	case PC_LD_STALL:
		return csr_read(CSR_PCCR(2));
	case PC_JMP_STALL:
		return csr_read(CSR_PCCR(3));
	case PC_IMISS:
		return csr_read(CSR_PCCR(4));
	case PC_LD:
		return csr_read(CSR_PCCR(5));
	case PC_ST:
		return csr_read(CSR_PCCR(6));
	case PC_JUMP:
		return csr_read(CSR_PCCR(7));
	case PC_BRANCH:
		return csr_read(CSR_PCCR(8));
	case PC_BRANCH_TAKEN:
		return csr_read(CSR_PCCR(9));
	case PC_COMP_INSTR:
		return csr_read(CSR_PCCR(10));
	case PC_LD_EXT:
		return csr_read(CSR_PCCR(11));
	case PC_ST_EXT:
		return csr_read(CSR_PCCR(12));
	case PC_LD_EXT_CYC:
		return csr_read(CSR_PCCR(13));
	case PC_ST_EXT_CYC:
		return csr_read(CSR_PCCR(14));
	case PC_TCDM_CONT:
		return csr_read(CSR_PCCR(15));
	case PC_CSR_HAZARD:
		return csr_read(CSR_PCCR(16));
	case PC_FP_TYPE:
		return csr_read(CSR_PCCR(17));
	case PC_FP_CONT:
		return csr_read(CSR_PCCR(18));
	case PC_FP_DEP:
		return csr_read(CSR_PCCR(19));
	case PC_FP_WB:
		return csr_read(CSR_PCCR(20));
	}
	return 0;
}

void ri5cy_set_count(uint8_t event, uint32_t count)
{
	switch (event) {
	case PC_CYCLES:
		csr_write(CSR_PCCR(0), count);
		break;
	case PC_INSTR:
		csr_write(CSR_PCCR(1), count);
		break;
	case PC_LD_STALL:
		csr_write(CSR_PCCR(2), count);
		break;
	case PC_JMP_STALL:
		csr_write(CSR_PCCR(3), count);
		break;
	case PC_IMISS:
		csr_write(CSR_PCCR(4), count);
		break;
	case PC_LD:
		csr_write(CSR_PCCR(5), count);
		break;
	case PC_ST:
		csr_write(CSR_PCCR(6), count);
		break;
	case PC_JUMP:
		csr_write(CSR_PCCR(7), count);
		break;
	case PC_BRANCH:
		csr_write(CSR_PCCR(8), count);
		break;
	case PC_BRANCH_TAKEN:
		csr_write(CSR_PCCR(9), count);
		break;
	case PC_COMP_INSTR:
		csr_write(CSR_PCCR(10), count);
		break;
	case PC_LD_EXT:
		csr_write(CSR_PCCR(11), count);
		break;
	case PC_ST_EXT:
		csr_write(CSR_PCCR(12), count);
		break;
	case PC_LD_EXT_CYC:
		csr_write(CSR_PCCR(13), count);
		break;
	case PC_ST_EXT_CYC:
		csr_write(CSR_PCCR(14), count);
		break;
	case PC_TCDM_CONT:
		csr_write(CSR_PCCR(15), count);
		break;
	case PC_CSR_HAZARD:
		csr_write(CSR_PCCR(16), count);
		break;
	case PC_FP_TYPE:
		csr_write(CSR_PCCR(17), count);
		break;
	case PC_FP_CONT:
		csr_write(CSR_PCCR(18), count);
		break;
	case PC_FP_DEP:
		csr_write(CSR_PCCR(19), count);
		break;
	case PC_FP_WB:
		csr_write(CSR_PCCR(20), count);
		break;
	}
}
