#include <target/init.h>
#include <target/arch.h>
#include "vaisra.h"

#ifdef CONFIG_SMP_BOOT
#define SCTLR_IDCACHE		0x1004
#else
#define SCTLR_IDCACHE		0x0
#endif

.macro	csrw_z csr
	csrw	\csr, zero
.endm
.macro	csrw_i csr, val
	li	t1, \val
	csrw	\csr, t1
.endm
.macro	csrrs_i csr, val
	csrr	t1, \csr
	li	t2, \val
	or	t1, t1, t2
	csrw	\csr, t1
.endm
.macro	l2csrw csr, val
	csrw_i	L2CPUSRSELR_EL1, \csr
	ISB
	csrw_i	L2CPUSRDR_EL1, \val
	ISB
.endm
.macro	dciall val
	li	x1, \val
	DCIALL
.endm
#ifdef CONFIG_DPU_PMA
.macro	pmarst
	li	t1, 0x100000000000
	srli	t1, t1, 2
	csrw	CSR_PMAADDR(0), t1
	li	t1, (PMA_AT_DEVICE | PMA_A_TOR)
	csrw	CSR_PMACFG(0), t1
.endm
#else
.macro	pmarst
.endm
#endif

ENTRY(vaisra_cpu_init)
	// Save ra which will be crushed by BPWSW/BPRSW/DCIALL
	// NOTE: Do not use t0 from this point on.
	add	t0, ra, zero

	// Required for DDR boot
	pmarst

vaisra_bp_init:
	csrw_i	BPCR_EL1, 0x1070013	// BPCR[FBHTD] must be set to allow BPWSW below

	csrw_z	BPUBTTR_EL1
	csrw_z	BPTR0_EL1

	li	x1, 0x80000035
	BPWSW
	li	x1, 0x80000000
	BPRSW

	li	x1, 0x0035
	li	t2, 0x4035
vaisra_bht_init:
	BPWSW
	addi	x1, x1, 64
	BPWSW
	addi	x1, x1, 64
	BPWSW
	addi	x1, x1, 64
	BPWSW
	addi	x1, x1, 64
	bne	x1, t2, vaisra_bht_init

skip_vaisra_bht_init:
	csrw_z	BPTR1_EL1

	li	x1, 0x8
	li	t2, 264
vaisra_l0_btac_init:
	BPWSW
	addi	x1, x1, 0x10
	BPWSW
	addi	x1, x1, 0x10
	bne	x1, t2, vaisra_l0_btac_init

	csrw_z	BPCR_EL1

#ifdef CONFIG_VAISRA_RAS_INIT
	jal	1f
1:	jal	2f
2:	jal	3f
3:	jal	4f
4:	jal	5f
5:	jal	6f
6:	jal	7f
7:	jal	8f
8:	jal	9f
9:	jal	10f
10:	jal	11f
11:	jal	12f
12:	jal	13f
13:	jal	14f
14:	jal	15f
15:	jal	16f
16:	nop
#endif

vaisra_l2_init:
	dciall	0x2
	dciall	0x42

	l2csrw	L2CR0_IA, 0x0007e7e5
	l2csrw	L2LKCR_IA, 0x0000
	l2csrw	L2BCR0_IA, 0x0010
	l2csrw	L2BCR1_IA, 0x800642130fe10d14
	l2csrw	L2CPMR_IA, 0x00240015
	l2csrw	L2ESR0_IA, -1
	l2csrw	L2ECR0_IA, 0x0554952549525495
	l2csrw	L2ECR1_IA, 0x9800000000095999

	csrw_i	L2CPUSRSELR_EL1, L2VRF0_IA
	ISB
	csrrs_i	L2CPUSRDR_EL1, 0x01000000
	ISB

vaisra_l1_init:
	// invalidate L1
	dciall	0
	dciall	0x40

	// initialize L1 cache registers
	csrw_i	DCESR_EL1, -1
	csrw_i	DCECR_EL1, 0x8000000000000155
	csrw_i	ICESR_EL1, -1
	csrw_i	ICECR_EL1, 0x80000055
	csrw_i	DCCR0_EL1, 0xC03DFC000000C084

vaisra_reset_init:
	csrw_i	CPMR_EL1, 0x00B4
	csrrs_i	PVR2F0_EL1, 0x01000002
	csrrs_i	PVR2F2_EL1, 0xffff000000
	csrrs_i	PVR2F3_EL1, 0xff

vaisra_tlb_init:
	csrw_i	TLBCR_EL1, 0x0002
	csrw_z	CCPR_EL3
	csrw_z	CCPR_EL1

vaisra_trace_init:
	csrw_i	CPMR_EL1, 0x00B4  // enable trace clk
	csrrs_i	PVR2F1_EL1, 0x2000000000000000

vaisra_sysreg_init:
	// sctlr_el3: 2 - C, 12 - I
	li	t1, SCTLR_IDCACHE
	csrw	SCTLR_EL3, t1
	csrw	SCTLR_EL2, t1
	csrw	SCTLR_EL1, t1
	li	t1, 0x10
	csrs	ACPTR_EL3, t1
	csrsi	NMICSR, 0x1

#ifndef CONFIG_VAISRA_ATOMIC
vaisra_atomic_init:
	csrrs_i	PVR2F0_EL1, 0x200000000000 // set PVR2F0 CSR(0x5e7) bit45
#endif

	// restore ra
	add	ra, t0, zero
	ret
ENDPROC(vaisra_cpu_init)
