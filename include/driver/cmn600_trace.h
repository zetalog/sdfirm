#ifndef __CMN600_TRACE_H_INCLUDE__
#define __CMN600_TRACE_H_INCLUDE__

#include <target/noc.h>

/* dtm_wp_REQ */
#define dtm_wp_REQ_SRCID_OFFSET		0
#define dtm_wp_REQ_SRCID_MASK		REG_11BIT_MASK
#define dtm_wp_REQ_SRCID(value)		_SET_FV_ULL(dtm_wp_REQ_SRCID, value)
#define dtm_wp_REQ_TGTID_OFFSET		0
#define dtm_wp_REQ_TGTID_MASK		REG_11BIT_MASK
#define dtm_wp_REQ_TGTID(value)		_SET_FV_ULL(dtm_wp_REQ_TGTID, value)
#define dtm_wp_REQ_OPCODE_OFFSET	31
#define dtm_wp_REQ_OPCODE_MASK		REG_6BIT_MASK
#define dtm_wp_REQ_OPCODE(value)	_SET_FV_ULL(dtm_wp_REQ_OPCODE, value)

/* dtm_wp_RSP */
#define dtm_wp_RSP_SRCID_OFFSET		4
#define dtm_wp_RSP_SRCID_MASK		REG_11BIT_MASK
#define dtm_wp_RSP_SRCID(value)		_SET_FV_ULL(dtm_wp_RSP_SRCID, value)
#define dtm_wp_RSP_TGTID_OFFSET		4
#define dtm_wp_RSP_TGTID_MASK		REG_11BIT_MASK
#define dtm_wp_RSP_TGTID(value)		_SET_FV_ULL(dtm_wp_RSP_TGTID, value)
#define dtm_wp_RSP_OPCODE_OFFSET	15
#define dtm_wp_RSP_OPCODE_MASK		REG_4BIT_MASK
#define dtm_wp_RSP_OPCODE(value)	_SET_FV_ULL(dtm_wp_RSP_OPCODE, value)

/* dtm_wp_SNP */
#define dtm_wp_SNP_SRCID_OFFSET		0
#define dtm_wp_SNP_SRCID_MASK		REG_11BIT_MASK
#define dtm_wp_SNP_SRCID(value)		_SET_FV_ULL(dtm_wp_SNP_SRCID, value)
#define dtm_wp_SNP_TGTID_OFFSET		0
#define dtm_wp_SNP_TGTID_MASK		REG_11BIT_MASK
#define dtm_wp_SNP_TGTID(value)		_SET_FV_ULL(dtm_wp_SNP_TGTID, value)
#define dtm_wp_SNP_OPCODE_OFFSET	19
#define dtm_wp_SNP_OPCODE_MASK		REG_5BIT_MASK
#define dtm_wp_SNP_OPCODE(value)	_SET_FV_ULL(dtm_wp_SNP_OPCODE, value)

/* dtm_wp_DAT */
#define dtm_wp_DAT_SRCID_OFFSET		4
#define dtm_wp_DAT_SRCID_MASK		REG_11BIT_MASK
#define dtm_wp_DAT_SRCID(value)		_SET_FV_ULL(dtm_wp_DAT_SRCID, value)
#define dtm_wp_DAT_TGTID_OFFSET		4
#define dtm_wp_DAT_TGTID_MASK		REG_11BIT_MASK
#define dtm_wp_DAT_TGTID(value)		_SET_FV_ULL(dtm_wp_DAT_TGTID, value)
#define dtm_wp_DAT_OPCODE_OFFSET	26
#define dtm_wp_DAT_OPCODE_MASK		REG_3BIT_MASK
#define dtm_wp_DAT_OPCODE(value)	_SET_FV_ULL(dtm_wp_DAT_OPCODE, value)

#endif /* __CMN600_TRACE_H_INCLUDE__ */