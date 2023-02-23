#include <target/init.h>
#include <target/arch.h>

#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
#define BPRSW	.word 0x9C00802B
#define BPWSW	.word 0x9C10802B
#define DCIALL	.word 0x9C70802B
#define ISB	.word 0x9CD0802B

.macro	csrw_z	csr
	csrw	\csr, zero
.endm
.macro	csrw_i	csr, val
	li	t1, \val
	csrw	\csr, t1
.endm
.macro	csrrs_i	csr, val
	csrr	t1, \csr
	li	t2, \val
	or	t1, t1, t2
	csrw	\csr, t1
.endm
.macro	l2csrw	csr, val
	csrw_i	CSR_SL2CPUSRSEL, \csr
	ISB
	csrw_i	CSR_SL2CPUSRDAT, \val
	ISB
.endm
.macro	l2csrrs	csr, val
	csrw_i	CSR_SL2CPUSRSEL, \csr
	ISB
	csrrs_i	CSR_SL2CPUSRDAT, \val
	ISB
.endm
.macro	dciall	val
	li	x1, \val
	DCIALL
.endm

/* Marking entire 44-bit address space as DEVICE region */
.macro	vaisra_pma_init
	li	t1, 0x100000000000	# PMAADDR0 is a top address
	srli	t1, t1, PMA_SHIFT	# 4-bytes granule
	csrw	CSR_PMAADDR(0), t1
	li	t1, (PMA_AT_DEVICE | PMA_A_TOR)
	csrw	CSR_PMACFG(0), t1
#ifdef CONFIG_VAISRA_INIT_PMA_STRICT
	li	t1, 0
	csrw	CSR_PMAADDR(1), t1
	csrw	CSR_PMAADDR(2), t1
	csrw	CSR_PMAADDR(3), t1
	csrw	CSR_PMAADDR(4), t1
	csrw	CSR_PMAADDR(5), t1
	csrw	CSR_PMAADDR(6), t1
	csrw	CSR_PMAADDR(7), t1
	csrw	CSR_PMAADDR(8), t1
	csrw	CSR_PMAADDR(9), t1
	csrw	CSR_PMAADDR(10), t1
	csrw	CSR_PMAADDR(11), t1
	csrw	CSR_PMAADDR(12), t1
	csrw	CSR_PMAADDR(13), t1
	csrw	CSR_PMAADDR(14), t1
	csrw	CSR_PMAADDR(15), t1
	csrw	CSR_PMACFG(1), t1
	csrw	CSR_PMACFG(2), t1
	csrw	CSR_PMACFG(3), t1
#endif /* CONFIG_VAISRA_INIT_PMA_STRICT */
.endm
.macro	vaisra_ras_init
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
16:	jal	17f
17:	jal	18f
18:	jal	19f
19:	nop
.endm

.macro	vaisra_cpu_init
vaisra_bpu_init:
	csrw_i	CSR_SBPCFG, 0x1070013	# set BPCFG[FBHTD] to allow BPWSW
	csrw_z	CSR_SBPUBTTHR
	csrw_z	CSR_SBPTAG0
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
	csrw_z	CSR_SBPTAG1
	li	x1, 0x8
	li	t2, 264
vaisra_l0_btac_init:
	BPWSW
	addi	x1, x1, 0x10
	BPWSW
	addi	x1, x1, 0x10
	bne	x1, t2, vaisra_l0_btac_init
	csrw_z	CSR_SBPCFG
	vaisra_ras_init

vaisra_l2_init:
	dciall	0x2
	dciall	0x42
	l2csrw	L2CFG_IA, 0x0007E7E5
	l2csrw	L2LCK_IA, 0x0000
	l2csrw	L2BC0_IA, 0x0010
	l2csrw	L2BC1_IA, 0x800642130FE10D14
	l2csrw	L2CPM_IA, 0x00240015
	l2csrw	L2ESTS_IA, -1
	l2csrw	L2ECFG0_IA, 0x0554952549525495
	l2csrw	L2ECFG1_IA, 0x9800000000095999
	l2csrrs	L2VRF0_IA, 0x01000000

vaisra_l1_init:
	dciall	0x0
	dciall	0x40
	csrw_i	CSR_SDCESTS, -1
	csrw_i	CSR_SDCECFG, 0x8000000000000155
	csrw_i	CSR_SICESTS, -1
	csrw_i	CSR_SICECFG, 0x80000055
	csrw_i	CSR_SDCCFG, 0xC03DFC000000C084

vaisra_reset_pre_init:
	csrw_i	CSR_SCPMCFG, 0x00B4
	csrrs_i	CSR_SVRF2F0, 0x01000002
	csrrs_i	CSR_SVRF2F2, 0xffff000000
	csrrs_i	CSR_SVRF2F3, 0xff

vaisra_tlb_init:
	csrw_i	CSR_STLBCFG, 0x0002
	csrw_z	CSR_MCCPCFG
	csrw_z	CSR_SCCPCFG

vaisra_reset_post_init:
	csrw_i	CSR_SCPMCFG, 0x00B4
	csrrs_i	CSR_SVRF2F1, 0x2000000000000000
.endm

.macro vaisra_cache_init
	li	t1, SCR_DC | SCR_IC
	csrw	CSR_MSYSCFG, t1
	csrw	CSR_HSYSCFG, t1
	csrw	CSR_SSYSCFG, t1
vaisra_nmi_init:
	csrsi	CSR_MTRPCFG, 0x10
	csrsi	CSR_NMICFG, 0x1
#ifndef CONFIG_VAISRA_ATOMIC
vaisra_atomic_init:
	csrrs_i	CSR_SVRF2F0, 0x200000000000
#endif /* CONFIG_VAISRA_ATOMIC */
.endm
#endif /* __ASSEMBLY__ && !__DTS__ && !LINKER_SCRIPT */
