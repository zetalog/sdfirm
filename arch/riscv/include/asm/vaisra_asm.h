.macro	pmarst
	li	t1, 0x100000000000	# PMAADDR0 is a top address
	srli	t1, t1, 2		# 4-bytes granule
	csrw	CSR_PMAADDR(0), t1
	li	t1, (PMA_AT_DEVICE | PMA_A_TOR)
	csrw	CSR_PMACFG(0), t1
	li	t1, 0
	csrw	CSR_PMACFG(1), t1
	csrw	CSR_PMACFG(2), t1
	csrw	CSR_PMACFG(3), t1
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
.endm

.macro vaisra_cpu_init
	pmarst
.endm
