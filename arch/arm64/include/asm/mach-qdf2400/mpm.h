#ifndef __MPM_QDF2400_H_INCLUDE__
#define __MPM_QDF2400_H_INCLUDE__

#include <target/types.h>

#define MPM_START		0xFF26A00000
#define MPM_SIZE		0x0000100000
#define MPM_END			(MPM_START + MPM_SIZE)

/* =================================================================
 * 69: Global Counter Control registers
 * ================================================================= */
#define GBLCT_CTL_START			(MPM_START + 0x00010000)

#define MPM_CONTROL_CNTCR		(GBLCT_CTL_START + 0x00000000)
#define MPM_CONTROL_CNTCR_FCREQ		_BV(8) /* Select counter clock source */
#define MPM_CONTROL_CNTCR_FCREQ_XO	0
#define MPM_CONTROL_CNTCR_HDBG		_BV(1) /* Halt On Debug */
#define MPM_CONTROL_CNTCR_EN		_BV(0) /* Counter Enable */
#define MPM_CONTROL_CNTCR_RES0			\
	(_BV(31)|_BV(30)|_BV(29)|_BV(28)|	\
	 _BV(27)|_BV(26)|_BV(25)|_BV(24)|	\
	 _BV(23)|_BV(22)|_BV(21)|_BV(20)|	\
	 _BV(19)|_BV(18)|_BV(17)|_BV(16)|	\
	 _BV(15)|_BV(14)|_BV(13)|_BV(12)|	\
	 _BV(11)|_BV(10)|_BV(9)|_BV(7)|		\
	 _BV(6)|_BV(5)|_BV(4)|_BV(3)|_BV(2))
#define MPM_CONTROL_CNTCR_RESET		0x00000000

#define MPM_CONTROL_CNTSR		(GBLCT_CTL_START + 0x00000004)
#define MPM_CONTROL_CNTSR_FCACK		_BV(8) /* Ack of Frequency Change */
#define MPM_CONTROL_CNTSR_HDBG		_BV(1) /* Status of HDBG */
#define MPM_CONTROL_CNTSR_RES0			\
	(_BV(31)|_BV(30)|_BV(29)|_BV(28)|	\
	 _BV(27)|_BV(26)|_BV(25)|_BV(24)|	\
	 _BV(23)|_BV(22)|_BV(21)|_BV(20)|	\
	 _BV(19)|_BV(18)|_BV(17)|_BV(16)|	\
	 _BV(15)|_BV(14)|_BV(13)|_BV(12)|	\
	 _BV(11)|_BV(10)|_BV(9)|_BV(7)|		\
	 _BV(6)|_BV(5)|_BV(4)|_BV(3)|		\
	 _BV(2)|_BV(0))
#define MPM_CONTROL_CNTSR_RESET		0x00000000

#define MPM_CONTROL_CNTCV_LO		(GBLCT_CTL_START + 0x00000008)
#define MPM_CONTROL_CNTCV_LO_RESET	0x00000000

#define MPM_CONTROL_CNTCV_HI		(GBLCT_CTL_START + 0x0000000c)
#define MPM_CONTROL_CNTCV_HI_RESET	0x00000000

#define MPM_CONTROL_CNTFID0		(GBLCT_CTL_START + 0x00000020)
#define MPM_CONTROL_CNTFID0_RESET	0x01312D00	/* 20.0MHz XO */

#endif /* __MPM_QDF2400_H_INCLUDE__ */
