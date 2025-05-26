#ifndef __CMN600_TRACE_H_INCLUDE__
#define __CMN600_TRACE_H_INCLUDE__

#include <target/noc.h>

/* dtm_control */
#define dtm_control_en				_BV(0)
#define dtm_control_trace_tag_en		_BV(1)

/* dtm_wp_cfg */
#define dtm_wp_cfg_dev_sel			_BV(0)
#define dtm_wp_cfg_chn_sel_OFFSET		1
#define dtm_wp_cfg_chn_sel_MASK			REG_3BIT_MASK
#define dtm_wp_cfg_chn_sel(value)		_SET_FV_ULL(dtm_wp_cfg_chn_sel, value)
#define dtm_wp_REQ				0
#define dtm_wp_RSP				1
#define dtm_wp_SNP				2
#define dtm_wp_DAT				3
#define dtm_wp_cfg_combine			_BV(6)
#define dtm_wp_cfg_pkt_gen			_BV(8)
#define dtm_wp_cfg_pkt_type_OFFSET		9
#define dtm_wp_cfg_pkt_type_MASK		REG_3BIT_MASK
#define dtm_wp_cfg_pkt_type(value)		_SET_FV_ULL(dtm_wp_cfg_pkt_type, value)
#define dtm_wp_cfg_pkt_type_TXNID		0
#define dtm_wp_cfg_pkt_type_TXNID_OP		1
#define dtm_wp_cfg_pkt_type_TXNID_OP_SRC_TGT	2
#define dtm_wp_cfg_pkt_type_CTRL_FLIT		4
#define dtm_wp_cfg_cc_en			_BV(12)
#define dtm_wp_cfg_ctrig_en			_BV(13)
#define dtm_wp_cfg_dbgtrig_en			_BV(14)

/* dtm_wp_val & dtm_wp_mask*/
/* REQ */
#define dtm_wp_REQ_SRCID_OFFSET			0
#define dtm_wp_REQ_SRCID_MASK			REG_11BIT_MASK
#define dtm_wp_REQ_SRCID(value)			_SET_FV_ULL(dtm_wp_REQ_SRCID, value)
#define dtm_wp_REQ_TGTID_OFFSET			0
#define dtm_wp_REQ_TGTID_MASK			REG_11BIT_MASK
#define dtm_wp_REQ_TGTID(value)			_SET_FV_ULL(dtm_wp_REQ_TGTID, value)
#define dtm_wp_REQ_OPCODE_OFFSET		31
#define dtm_wp_REQ_OPCODE_MASK			REG_6BIT_MASK
#define dtm_wp_REQ_OPCODE(value)		_SET_FV_ULL(dtm_wp_REQ_OPCODE, value)

/* RSP */
#define dtm_wp_RSP_SRCID_OFFSET			4
#define dtm_wp_RSP_SRCID_MASK			REG_11BIT_MASK
#define dtm_wp_RSP_SRCID(value)			_SET_FV_ULL(dtm_wp_RSP_SRCID, value)
#define dtm_wp_RSP_TGTID_OFFSET			4
#define dtm_wp_RSP_TGTID_MASK			REG_11BIT_MASK
#define dtm_wp_RSP_TGTID(value)			_SET_FV_ULL(dtm_wp_RSP_TGTID, value)
#define dtm_wp_RSP_OPCODE_OFFSET		15
#define dtm_wp_RSP_OPCODE_MASK			REG_4BIT_MASK
#define dtm_wp_RSP_OPCODE(value)		_SET_FV_ULL(dtm_wp_RSP_OPCODE, value)

/* SNP */
#define dtm_wp_SNP_SRCID_OFFSET			0
#define dtm_wp_SNP_SRCID_MASK			REG_11BIT_MASK
#define dtm_wp_SNP_SRCID(value)			_SET_FV_ULL(dtm_wp_SNP_SRCID, value)
#define dtm_wp_SNP_TGTID_OFFSET			0
#define dtm_wp_SNP_TGTID_MASK			REG_11BIT_MASK
#define dtm_wp_SNP_TGTID(value)			_SET_FV_ULL(dtm_wp_SNP_TGTID, value)
#define dtm_wp_SNP_OPCODE_OFFSET		19
#define dtm_wp_SNP_OPCODE_MASK			REG_5BIT_MASK
#define dtm_wp_SNP_OPCODE(value)		_SET_FV_ULL(dtm_wp_SNP_OPCODE, value)

/* DAT */
#define dtm_wp_DAT_SRCID_OFFSET			4
#define dtm_wp_DAT_SRCID_MASK			REG_11BIT_MASK
#define dtm_wp_DAT_SRCID(value)			_SET_FV_ULL(dtm_wp_DAT_SRCID, value)
#define dtm_wp_DAT_TGTID_OFFSET			4
#define dtm_wp_DAT_TGTID_MASK			REG_11BIT_MASK
#define dtm_wp_DAT_TGTID(value)			_SET_FV_ULL(dtm_wp_DAT_TGTID, value)
#define dtm_wp_DAT_OPCODE_OFFSET		26
#define dtm_wp_DAT_OPCODE_MASK			REG_3BIT_MASK
#define dtm_wp_DAT_OPCODE(value)		_SET_FV_ULL(dtm_wp_DAT_OPCODE, value)

/* dt_dtc_ctl */
#define dt_dtc_ctl_en				_BV(0)
#define dt_dtc_ctl_dbgtrigger_en		_BV(1)
#define dt_dtc_ctl_atbtrigger_en		_BV(2)
#define dt_dtc_ctl_wait_for_trigger		_BV(3)

/* dt_trace_control */
#define dt_trace_control_cc_enable		_BV(8)

#endif /* __CMN600_TRACE_H_INCLUDE__ */