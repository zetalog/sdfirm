#include <target/perf.h>

void ri5cy_set_count(uint8_t event, uint32_t count)
{
	switch (event) {
	case 0:
		csr_write(CSR_PCCR(0), count);
		break;
	case 1:
		csr_write(CSR_PCCR(1), count);
		break;
	case 2:
		csr_write(CSR_PCCR(2), count);
		break;
	case 3:
		csr_write(CSR_PCCR(3), count);
		break;
	case 4:
		csr_write(CSR_PCCR(4), count);
		break;
	case 5:
		csr_write(CSR_PCCR(5), count);
		break;
	case 6:
		csr_write(CSR_PCCR(6), count);
		break;
	case 7:
		csr_write(CSR_PCCR(7), count);
		break;
	case 8:
		csr_write(CSR_PCCR(8), count);
		break;
	case 9:
		csr_write(CSR_PCCR(9), count);
		break;
	case 10:
		csr_write(CSR_PCCR(10), count);
		break;
	case 11:
		csr_write(CSR_PCCR(11), count);
		break;
	case 12:
		csr_write(CSR_PCCR(12), count);
		break;
	case 13:
		csr_write(CSR_PCCR(13), count);
		break;
	case 14:
		csr_write(CSR_PCCR(14), count);
		break;
	case 15:
		csr_write(CSR_PCCR(15), count);
		break;
	case 16:
		csr_write(CSR_PCCR(16), count);
		break;
	case 17:
		csr_write(CSR_PCCR(17), count);
		break;
	case 18:
		csr_write(CSR_PCCR(18), count);
		break;
	case 19:
		csr_write(CSR_PCCR(19), count);
		break;
	case 20:
		csr_write(CSR_PCCR(20), count);
		break;
	}
}
