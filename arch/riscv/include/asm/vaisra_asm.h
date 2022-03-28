.macro	pmarst
	li	t1, 0x100000000000	# PMAADDR0 is a top address
	srli	t1, t1, 2		# 4-bytes granule
	csrw	CSR_PMAADDR(0), t1
	li	t1, (PMA_AT_DEVICE | PMA_A_TOR)
	csrw	CSR_PMACFG(0), t1
.endm

.macro vaisra_cpu_init
	pmarst
.endm
